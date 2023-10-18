#ifndef KLIB_USB_BULK_ONLY_TRANSPORT_HPP
#define KLIB_USB_BULK_ONLY_TRANSPORT_HPP

#include <cstdint>
#include <algorithm>

#include <klib/usb/usb.hpp>
#include <klib/string.hpp>

#include "bot.hpp"
#include "msc.hpp"

namespace klib::usb::msc::bot {
    template <typename Memory, uint8_t InEndpoint, uint8_t OutEndpoint>
    class handler {
    protected:
        // max supported luns. (- 1 as 0 is 1 lun)
        constexpr static uint8_t max_lun = (1 - 1);

        // max supported packet size
        constexpr static uint32_t max_packet_size = 512;
        constexpr static uint32_t formatted_media = 0x02000000;

        // variable used for the interface configuration
        static inline uint8_t interface = 0;

        // command block and command status
        static inline command_block_wrapper cbw = {};
        static inline command_status_wrapper csw = {};

        struct block {
            uint32_t address;
            uint32_t count;
        }; 

        // information for the the transfers
        static inline volatile block transfer_block;

        // buffer to store one packet for the usb stack
        static inline uint8_t block_buffer[max_packet_size] = {};

        // available states for the callback handler
        enum class state {
            wait_for_cbw,
            send_csw,
            receive_cbw,
            memory_read,
            memory_write,
        };

    public:
        template <typename Usb>
        static void init() {
            // init the memory
            Memory::init();
        }

        template <typename Usb>
        static void de_init() {
            // stop the memory
            Memory::stop();
        }        

        template <typename Usb>
        static usb::handshake get_interface(const klib::usb::setup_packet &packet) {
            if (!Usb::write(usb::status_callback<Usb>,
                usb::control_endpoint, usb::endpoint_mode::in, 
                &interface, sizeof(interface)))
            {
                // something went wrong stall now
                return usb::handshake::stall;
            }            

            // wait for now until the callback is called
            return usb::handshake::wait;
        } 

        template <typename Usb>
        static usb::handshake set_interface(const klib::usb::setup_packet &packet) {
            // set the interface
            interface = static_cast<uint8_t>(packet.wValue);

            // ack the packet
            return usb::handshake::ack;
        }

        template <typename Usb>
        static usb::handshake handle_class_packet(const klib::usb::setup_packet &packet) {
            // get the class specific request
            const msc::requests request = static_cast<msc::requests>(packet.bRequest);

            // check what we should do
            switch (request) {
                case msc::requests::get_max_lun:
                    // send the maximum amount of supported luns
                    if (!Usb::write(callback_handler<Usb, state::wait_for_cbw>,
                        usb::control_endpoint, usb::endpoint_mode::in,
                        &max_lun, sizeof(max_lun)))
                    {
                        // something went wrong stall now
                        return usb::handshake::stall;
                    }

                    // we do not ack here as the status callback 
                    // will handle this for us
                    return usb::handshake::wait;
                case msc::requests::bulk_only_reset:
                    // cancel any open requests
                    Usb::cancel(
                        usb::get_endpoint(OutEndpoint), 
                        usb::get_endpoint_mode(OutEndpoint)
                    );

                    // request a new cbw
                    wait_for_cbw<Usb>();

                    return usb::handshake::ack;
                default:
                    break;
            }

            // unknown packet. stall
            return usb::handshake::stall;            
        }

        template <typename Usb, state State>
        static void callback_handler(const uint8_t endpoint, const usb::endpoint_mode mode, const usb::error error_code) {
            // check what we should do
            switch (error_code) {
                case usb::error::stall:
                case usb::error::reset:
                    return Usb::stall(endpoint, mode);
                case usb::error::un_stall:
                case usb::error::cancel:
                    return;
                case usb::error::nak:
                    // do not continue if we have a nak except in the write
                    if (State != state::memory_write) {
                        return;
                    }
                    break;
                case usb::error::no_error:
                    break;
                default:
                    break;
            }

            // only send acks on the control endpoint
            if (endpoint == 0) {
                // no error send a ack
                Usb::ack(endpoint, mode);
            }

            // handle all the specific states
            switch (State) {
                case state::wait_for_cbw:
                    // start a read for new data
                    wait_for_cbw<Usb>();
                    break;
                case state::send_csw:
                    // send the current csw
                    send_csw<Usb>();
                    break;
                case state::receive_cbw:
                    // check if the received cbw is correct
                    if (!receive_cbw<Usb>()) {
                        // stall the endpoint
                        Usb::stall(endpoint, mode);
                    }
                    break;
                case state::memory_read:
                    read_memory<Usb>();
                    break;
                case state::memory_write:
                    write_memory<Usb>();
                    break;
                default:
                    break;                    
            }
        }

        template <typename Usb>
        static void wait_for_cbw() {
            // Start a read request for new cbw data
            Usb::read(callback_handler<Usb, state::receive_cbw>, 
                usb::get_endpoint(OutEndpoint), 
                usb::get_endpoint_mode(OutEndpoint), 
                reinterpret_cast<uint8_t*>(&cbw), 
                sizeof(cbw)
            );
        }

        template <typename Usb>
        static bool receive_cbw() {
            // make sure we do not have a error code
            if (cbw.dCBWSignature != 0x43425355 || cbw.bCBWCBLength < 1 || 
                cbw.bCBWCBLength > sizeof(cbw.CBWCB)) 
            {
                // something is wrong with the cbw. Return we should stall
                return false;
            }

            // set the tag to the one we received from the host
            csw.dCBWTag = cbw.dCBWTag;

            // set the status to passed for now
            csw.bCSWStatus = static_cast<uint8_t>(status::command::passed);

            // get the scsi command
            const auto scsi = static_cast<msc::scsi::command>(cbw.CBWCB[0]);

            // process the scsi command. 
            switch (scsi) {
                case msc::scsi::command::test_unit_ready:
                    // check if the memory is ready
                    csw.bCSWStatus = (Memory::ready() ? 
                        static_cast<uint8_t>(status::command::passed) : 
                        static_cast<uint8_t>(status::command::failed)
                    );

                    // send the status response directly
                    send_csw<Usb>();
                    break;
                case msc::scsi::command::request_sense:
                    // respond to the sense request and send a status response
                    // after sending the sense
                    request_sense<Usb>();
                    break;
                case msc::scsi::command::inquiry:
                    // respond to the inquiry and send a status response after 
                    // sending the sense
                    send_inquiry<Usb>();
                    break;
                case msc::scsi::command::start_stop_unit:
                    // check if we should stop or start the memory
                    if (cbw.CBWCB[3] & 0x2) {
                        // flush any pending transactions and report the result
                        csw.bCSWStatus = (Memory::stop() ? 
                            static_cast<uint8_t>(status::command::passed) : 
                            static_cast<uint8_t>(status::command::failed)
                        );
                    }
                    else {
                        // have the memory start
                        Memory::start();

                        // check if the memory is ready
                        csw.bCSWStatus = (Memory::ready() ? 
                            static_cast<uint8_t>(status::command::passed) : 
                            static_cast<uint8_t>(status::command::failed)
                        );
                    }

                    // send the status response directly
                    send_csw<Usb>();
                    break;
                case msc::scsi::command::mode_sense6:
                case msc::scsi::command::mode_sense10:
                    // send the result of the sense
                    send_sense<Usb>();
                    break;
                case msc::scsi::command::allow_medium_removal:
                    // check if the memory is ready for removal
                    csw.bCSWStatus = (Memory::can_remove() ? 
                        static_cast<uint8_t>(status::command::passed) : 
                        static_cast<uint8_t>(status::command::failed)
                    );

                    // send the status response directly
                    send_csw<Usb>();
                    break;
                case msc::scsi::command::receive_diagnostic_result:
                    send_receive_diagnostic_result<Usb>();
                    break;
                case msc::scsi::command::read_format_capacities:
                    send_format_capacity<Usb>();
                    break;
                case msc::scsi::command::read_capacity10:
                    send_capacity10<Usb>();
                    break;
                case msc::scsi::command::read_capacity16:
                    send_capacity16<Usb>();
                    break;
                case msc::scsi::command::read10:
                case msc::scsi::command::write10:
                    // get the address we should use
                    transfer_block.address = (
                        cbw.CBWCB[2] << 24 | cbw.CBWCB[3] << 16 | 
                        cbw.CBWCB[4] << 8 | cbw.CBWCB[5]
                    );
                    
                    // get the block count
                    transfer_block.count = cbw.CBWCB[7] << 8 | cbw.CBWCB[8];

                    // start a read or write based on the scsi command we 
                    // received
                    if (scsi == msc::scsi::command::read10) {
                        read_memory<Usb>();
                    }
                    else {
                        request_data_write<Usb>();
                    }
                    break;
                default:
                    // unknown command. Return a failure
                    csw.bCSWStatus = static_cast<uint8_t>(status::command::failed);
                    send_csw<Usb>();
                    break;
            }

            // return we do not need a stall
            return true;
        }

        template <typename Usb>
        static void send_csw() {
            // write the current csw to the host and request a new cbw 
            // in the status callback
            Usb::write(callback_handler<Usb, state::wait_for_cbw>, 
                usb::get_endpoint(InEndpoint),
                usb::get_endpoint_mode(InEndpoint),
                reinterpret_cast<uint8_t*>(&csw),
                sizeof(csw)
            );
        }

        template <typename Usb>
        static void request_sense() {
            // create the struct with the additional sense data
            struct data {
                uint8_t bytes[10];
            };

            // get a reference to the response
            auto &response = *reinterpret_cast<scsi::parameters::sense<data>*>(block_buffer);

            response.response_code = 0x70;
            response.sense_key = 0x00;
            response.sense_code_qualifier = 0x00;
            response.reserved[0] = 0x00;
            response.reserved[1] = 0x00;
            response.reserved[2] = 0x00;

            // set if the device is ready
            response.sense_code = Memory::ready() ? 0x00 : 0x02;

            // set the length of the additional sense
            response.length = sizeof(data);

            // command specfic info (4 bytes)
            response.data.bytes[0] = 0x00;
            response.data.bytes[1] = 0x00;
            response.data.bytes[2] = 0x00;
            response.data.bytes[3] = 0x00;

            // additional sense code
            response.data.bytes[4] = 0x3A;

            // additional send qualifier
            response.data.bytes[5] = 0x00;

            // reserved (4 bytes)
            response.data.bytes[6] = 0x00;
            response.data.bytes[7] = 0x00;
            response.data.bytes[8] = 0x00;
            response.data.bytes[9] = 0x00;

            // send the data and send a csw after we have transmitted the data
            Usb::write(callback_handler<Usb, state::send_csw>, 
                usb::get_endpoint(InEndpoint),
                usb::get_endpoint_mode(InEndpoint),
                block_buffer, sizeof(response)
            );
        }

        template <typename Usb>
        static void send_inquiry() {
            // TODO: move this to somewhere global
            static uint8_t buffer[36] = {};

            buffer[0]  = 0x00;               // Peripheral Device Type (PDT) - SBC Direct-access device
            buffer[1]  = 0x80;               // Removable Medium Bit is Set
            buffer[2]  = 0x02;               // Version
            buffer[3]  = 0x02;               // Obsolete[7:6],NORMACA[5],HISUP[4],Response Data Format[3:0]
            buffer[4]  = 0x1f;               // Additional Length
            buffer[5]  = 0x73;               // SCCS[7],ACC[6],TPGS[5:4],3PC[3],Reserved[2:1],PROTECT[0]
            buffer[6]  = 0x6d;               // BQUE[7],ENCSERV[6],VS[5],MULTIP[4],MCHNGR[3],Obsolete[2:1],ADDR16[0]
            buffer[7]  = 0x69;               // Obsolete[7:6],WBUS116[5],SYNC[4],LINKED[3],Obsolete[2],CMDQUE[1],VS[0]

            // TODO: make this configurable
            klib::string::strcpy(reinterpret_cast<char*>(buffer) + 8, "KLIB");
            klib::string::strcpy(reinterpret_cast<char*>(buffer) + 16, "MASS Storage");
            klib::string::strcpy(reinterpret_cast<char*>(buffer) + 32, "1.0");

            // send the data and send a csw after we have transmitted the data
            Usb::write(callback_handler<Usb, state::send_csw>, 
                usb::get_endpoint(InEndpoint),
                usb::get_endpoint_mode(InEndpoint),
                buffer, sizeof(buffer)
            );
        }

        template <typename Usb>
        static void send_sense() {
            // TODO: move this to somewhere global
            static uint8_t buffer[4] = {};

            buffer[0] = 0x04;
            buffer[1] = 0x00;
            buffer[2] = Memory::is_writable() ? 0x10 : 0x90;
            buffer[3] = 0x00;

            // send the data and send a csw after we have transmitted the data
            Usb::write(callback_handler<Usb, state::send_csw>, 
                usb::get_endpoint(InEndpoint),
                usb::get_endpoint_mode(InEndpoint),
                buffer, sizeof(buffer)
            );   
        }

        template <typename Usb>
        static void send_receive_diagnostic_result() {
            // TODO: move this to somewhere global
            static uint8_t buffer[32] = {};

            // send the data and send a csw after we have transmitted the data
            Usb::write(callback_handler<Usb, state::send_csw>, 
                usb::get_endpoint(InEndpoint),
                usb::get_endpoint_mode(InEndpoint),
                buffer, sizeof(buffer)
            );   
        }
        
        template <typename Usb>
        static void send_format_capacity() {
            // Get the total number of blocks on the "disk".
            const uint32_t size = (Memory::size() / max_packet_size) - 1;

            // TODO: move this to somewhere global
            static uint8_t buffer[12] = {};

            // Prep the response and send it followed by sending the CSW
            buffer[0] = 0x00;
            buffer[1] = 0x00;
            buffer[2] = 0x00;
            buffer[3] = 0x08;

            buffer[4] = static_cast<uint8_t>(size >> 24);
            buffer[5] = static_cast<uint8_t>(size >> 16);
            buffer[6] = static_cast<uint8_t>(size >> 8);
            buffer[7] = static_cast<uint8_t>(size);

            buffer[8]  = static_cast<uint8_t>((max_packet_size | formatted_media) >> 24);
            buffer[9]  = static_cast<uint8_t>((max_packet_size | formatted_media) >> 16);
            buffer[10] = static_cast<uint8_t>((max_packet_size | formatted_media) >> 8);
            buffer[11] = static_cast<uint8_t>(max_packet_size | formatted_media);

            // send the data and send a csw after we have transmitted the data
            Usb::write(callback_handler<Usb, state::send_csw>, 
                usb::get_endpoint(InEndpoint),
                usb::get_endpoint_mode(InEndpoint),
                buffer, sizeof(buffer)
            );            
        }

        template <typename Usb>
        static void send_capacity10() {
            // Get the total number of blocks on the "disk".
            const uint32_t size = (Memory::size() / max_packet_size) - 1;

            // get a reference to the response
            auto &response = *reinterpret_cast<scsi::parameters::read_capacity10*>(block_buffer);

            // set the response data
            response.block_address = klib::to_big_endian(size);

            // set the packet length in bytes
            response.length = klib::to_big_endian(max_packet_size);

            // send the data and send a csw after we have transmitted the data
            Usb::write(callback_handler<Usb, state::send_csw>, 
                usb::get_endpoint(InEndpoint),
                usb::get_endpoint_mode(InEndpoint),
                block_buffer, sizeof(response)
            );            
        }

        template <typename Usb>
        static void send_capacity16() {
            // Get the total number of blocks on the "disk".
            const uint32_t size = (Memory::size() / max_packet_size) - 1;

            // TODO: move this to somewhere global
            static uint8_t buffer[32] = {};

            /* Prep the response and send it followed by sending the CSW */
            buffer[4] = static_cast<uint8_t>(size >> 24);
            buffer[5] = static_cast<uint8_t>(size >> 16);
            buffer[6] = static_cast<uint8_t>(size >> 8);
            buffer[7] = static_cast<uint8_t>(size);

            buffer[8] = static_cast<uint8_t>((max_packet_size) >> 24);
            buffer[9] = static_cast<uint8_t>((max_packet_size) >> 16);
            buffer[10] = static_cast<uint8_t>((max_packet_size) >> 8);
            buffer[11] = static_cast<uint8_t>(max_packet_size);

            // send the data and send a csw after we have transmitted the data
            Usb::write(callback_handler<Usb, state::send_csw>, 
                usb::get_endpoint(InEndpoint),
                usb::get_endpoint_mode(InEndpoint),
                buffer, sizeof(buffer)
            );            
        }

        template <typename Usb>
        static void read_memory() {
            // check if we have more data
            if (transfer_block.count) {
                // read the memory and update the status
                csw.bCSWStatus |= (
                    Memory::read(block_buffer, transfer_block.address * max_packet_size, sizeof(block_buffer)) ? 
                        static_cast<uint8_t>(status::command::passed) : 
                        static_cast<uint8_t>(status::command::failed)
                );

                // update the addresses
                transfer_block.count--;
                transfer_block.address++;

                // write the data and have it call this function again to write more data
                Usb::write(callback_handler<Usb, state::memory_read>, 
                    usb::get_endpoint(InEndpoint),
                    usb::get_endpoint_mode(InEndpoint),
                    block_buffer, sizeof(block_buffer)
                );                    
            }
            else {
                // send the status after we are done with sending all the memory
                send_csw<Usb>();
            }
        }

        template <typename Usb>
        static void request_data_write() {
            // check if we have more data to receive
            if (transfer_block.count) {
                Usb::read(
                    callback_handler<Usb, state::memory_write>, 
                    usb::get_endpoint(OutEndpoint),
                    usb::get_endpoint_mode(OutEndpoint),
                    block_buffer, sizeof(block_buffer)
                );
            }
            else {
                // we are done send the status
                send_csw<Usb>();                
            }
        }

        template <typename Usb>
        static void write_memory() {
            // read the memory and update the status
            csw.bCSWStatus |= (
                Memory::write(block_buffer, transfer_block.address * max_packet_size, sizeof(block_buffer)) ? 
                    static_cast<uint8_t>(status::command::passed) : 
                    static_cast<uint8_t>(status::command::failed)
            );

            // update the addresses
            transfer_block.count--;
            transfer_block.address++;

            // request more data if needed
            request_data_write<Usb>();
        }     
    };

    /**
     * @brief Helper class that alows a memory device to be accessed by the bulk only transfer driver
     * 
     * @tparam Memory 
     * @tparam Size 
     * @tparam SectorSize 
     * @tparam PageSize 
     */
    template <typename Memory, uint32_t Size, uint32_t SectorSize, uint32_t PageSize>
    class helper {
    protected:
        constexpr static uint32_t invalid_sector = 0xffffffff;

        // current sector
        static inline uint32_t current_sector = invalid_sector;

        // buffer to store a whole sector for writing to the memory
        static inline uint8_t buffer[SectorSize] = {};

        // flag if we wrote something to the sector buffer
        static inline bool dirty = false;

        /**
         * @brief Write or read from the sector
         * 
         * @param sector 
         * @return true 
         * @return false 
         */
        static bool get_sector(const uint32_t sector) {
            // check if a new sector requested
            if (current_sector == sector) {
                return true;
            }

            // check if we should flush data
            if (current_sector != invalid_sector && dirty) {
                // get the starting address of the sector
                const uint32_t address = (current_sector * SectorSize);

                // do a sector erase
                Memory::erase(Memory::erase_mode::sector, address);

                // wait until the device is not busy
                while (Memory::is_busy()) {
                    // do nothing until the memory is not busy anymore
                }

                // write the data in pagesize chunks (some memories do not 
                // support writing big chunks at once)
                for (uint32_t i = 0; i < sizeof(buffer); i += PageSize) {
                    // write the buffer
                    Memory::write(address + i, &buffer[i], PageSize);

                    // wait until the device is not busy
                    while (Memory::is_busy()) {
                        // do nothing until the memory is not busy anymore
                    }
                }

                dirty = false;
            }

            // check if we should read a new sector
            if (sector != invalid_sector) {
                // get the start address of the next sector
                const uint32_t address = (sector * SectorSize);

                // read the data from the sector
                Memory::read(address, buffer, sizeof(buffer));

                // update the current sector and clear the dirty flag
                current_sector = sector;
                dirty = false;            
            }
            
            return true;
        }

    public:
        /**
         * @brief Init the memory
         * 
         */
        static void init() {
            // mark the current sector as invalid
            current_sector = invalid_sector;

            // clear the dirty flag as we did not do anything yet
            dirty = false;
            
            // initialize the memory
            Memory::init();
        }

        /**
         * @brief Start the memory
         * 
         */
        static void start() {
            // nothing to do here
        }

        /**
         * @brief Stop any pending transaction and flush to disk
         * 
         * @return true 
         * @return false 
         */
        static bool stop() {
            // stop any pending transactions and make sure to write the 
            // last sector we wrote to if it is dirty
            get_sector(invalid_sector);

            return true;
        }

        /**
         * @brief Return if the memory is ready
         * 
         * @return true 
         * @return false 
         */
        static bool ready() {
            // return if the device is busy
            return !Memory::is_busy();
        }

        /**
         * @brief Returns if the device can be removed from the host
         * 
         * @return true 
         * @return false 
         */
        static bool can_remove() {
            // return the device can be removed
            return true;
        }

        /**
         * @brief Returns the size of the memory device in bytes
         * 
         * @return uint32_t 
         */
        static uint32_t size() {
            return Size;
        }

        /**
         * @brief Returns if the drive is writable
         * 
         * @return true 
         * @return false 
         */
        static bool is_writable() {
            return true;
        }

        /**
         * @brief Read a sector. Can read up to SectorSize of bytes 
         * at once (without crossing a sector boundry)
         * 
         * @param data 
         * @param address 
         * @param size 
         * @return true 
         * @return false 
         */
        static bool read(uint8_t *const data, const uint32_t address, const uint16_t size) {
            // get the sector from memory
            if (!get_sector(address / SectorSize)) {
                return false;
            }

            // copy the data from the buffer to the destination
            std::copy_n(&buffer[address & (SectorSize - 1)], size, data);

            // return we are good
            return true;
        }

        /**
         * @brief Write to a sector. Can write up to SectorSize of bytes 
         * at once (without crossing a sector boundry)
         * 
         * @param data 
         * @param address 
         * @param size 
         * @return true 
         * @return false 
         */
        static bool write(uint8_t *const data, const uint32_t address, const uint16_t size) {
            // get the sector
            if (!get_sector(address / SectorSize)) {
                return false;
            }

            // copy the data from the buffer to the destination
            std::copy_n(data, size, &buffer[address & (SectorSize - 1)]);

            // mark the sector as dirty so we will write it when 
            // we change sectors
            dirty = true;

            // return we are good
            return true;
        }
    };    
}

#endif