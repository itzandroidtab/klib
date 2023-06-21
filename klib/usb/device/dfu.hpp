#ifndef KLIB_USB_DFU_HPP
#define KLIB_USB_DFU_HPP

#include <algorithm>

#include <klib/string.hpp>
#include <klib/usb/device.hpp>
#include <klib/usb/descriptor.hpp>
#include <klib/usb/dfu/descriptor.hpp>
#include <klib/usb/dfu/request.hpp>

namespace klib::usb::device {
    class dfu {
    protected:
        constexpr static uint32_t max_transfer_size = 64;

        /**
         * @brief Enum with the string descriptor indexes
         * 
         */
        enum class string_index {
            language = 0,
            manufacturer = 1,
            product = 2,
            serial = 3,
            flash_information = 4
        };

        // buffer for the firmware
        static inline uint8_t buffer[max_transfer_size] = {};

        // length of the current buffer
        static inline uint8_t length = 0;

        // flag if we are done with the manifestation stage
        static inline bool manifestation_complete = false;

        // Push the current pack to the stack and set the pack to 1
        // as all these structs have specific sizes
        #pragma pack(push, 1)

        /**
         * @brief Config descriptor for the hid keyboard
         * 
         * @details packed so we can write this whole descriptor 
         * to the usb hardware in one go.
         * 
         */
        struct runtime_config_descriptor {
            // configuration descriptor
            descriptor::configuration configuration;

            // interface descriptor
            descriptor::interface interface;

            // functional descriptor
            klib::usb::dfu::functional functional;
        };

        // release the old pack so the rest of the structs are not 
        // affected by the pack(1)
        #pragma pack(pop)

        // device descriptor for the hid keyboard
        const __attribute__((aligned(4))) static inline descriptor::device device = {
            .bcdUSB = static_cast<uint16_t>(setup::usb_version::usb_v1_1),
            .bDeviceClass = 0x00,
            .bDeviceSubClass = 0x00,
            .bDeviceProtocol = 0x00,
            .bMaxPacketSize = 0x40,
            .idVendor = 0x0483,
            .idProduct = 0xDF11,
            .bcdDevice = 0x0002,
            .iManufacturer = 0x01,
            .iProduct = 0x02,
            .iSerialNumber = 0x03,
            .bNumConfigurations = 0x1
        };

        // configuration descriptor
        __attribute__((aligned(4))) static inline runtime_config_descriptor runtime_config = {
            {
                .wTotalLength = sizeof(runtime_config_descriptor),
                .bNumInterfaces = 0x01,
                .bConfigurationValue = 0x01,
                .iConfiguration = 0x00,
                .bmAttributes = 0xA0,
                .bMaxPower = 0x32,
            },
            {
                .bInterfaceNumber = 0x00,
                .bAlternateSetting = 0x00,
                .bNumEndpoints = 0x00,
                .bInterfaceClass = 0xfe,
                .bInterfaceSubClass = 0x01,
                .bInterfaceProtocol = 0x01, // 0x01 = runtime, 0x02 = boot mode
                .iInterface = 0x04
            },
            {
                .bmAttributes = 0b00000101,
                .wDetachTimeOut = 255,
                .wTransferSize = max_transfer_size,
                // .bcdDFUVersion = 0x011a, // 0x011a for STM
            }
        };

        // language descriptor for the keyboard
        const __attribute__((aligned(4))) static inline descriptor::string<1> language = {
            .bString = {0x0409}
        };

        // manufacturer string descriptor
        const __attribute__((alligned(4))) static inline auto manufacturer = string_descriptor("KLIB");

        // product string descriptor
        const __attribute__((aligned(4))) static inline auto product = string_descriptor("KLIB DFU");

        // serial number string descriptor
        const __attribute__((aligned(4))) static inline auto serial = string_descriptor("00001337");

        // flash information
        const __attribute__((aligned(4))) static inline auto flash_information = string_descriptor("@Internal Flash /0x08000000/03*016Ka,01*016Kg,01*064Kg,07*128Kg");
    
        // configuration value. Value is set in the set config function
        static inline uint8_t configuration = 0x00;
 
        // current status and state
        static inline volatile auto current_status = klib::usb::dfu::device_status::ok;
        static __attribute__((aligned(4))) inline volatile auto current_state = klib::usb::dfu::device_state::app_idle;

        // offset in the current download
        static inline uint32_t offset = 0;

    public:
        /**
         * @brief Returns if the device is configured
         * 
         * @tparam Usb 
         * @return true 
         * @return false 
         */
        template <typename Usb>
        static bool is_configured() {
            return static_cast<volatile uint8_t>(configuration) != 0;
        }

        /**
         * @brief Poll function that calls the write/verity callback. Should be called from a 
         * context with a lower priority than the usb interrupt.
         * 
         * @tparam Usb 
         * @tparam T 
         * @param func 
         */
        template <typename Usb, typename T>
        static void poll(const T func) {
            // check if we have the data and are waiting to write to the flash
            if (current_state != klib::usb::dfu::device_state::download_busy && 
                current_state != klib::usb::dfu::device_state::manifset) 
            {
                // nothing to do. Do a early exit
                return;
            }

            // check if we are in download or manifest mode
            const bool download = (current_state == klib::usb::dfu::device_state::download_busy);

            // call the write function
            const bool status = func(download, offset, buffer, length);

            // check what we should do with the result
            if (download) {
                if (status) {
                    // change the state to idle
                    current_state = klib::usb::dfu::device_state::download_idle;

                    // add the length to the offset
                    offset += length;
                }
                else {
                    // set we have a error
                    current_status = klib::usb::dfu::device_status::verify_error;
                }
            }
            else {
                // set the manifestation as the current state
                manifestation_complete = status;

                if (status) {
                    // change the state to idle
                    current_state = klib::usb::dfu::device_state::manifest_sync;                    
                }
                else {
                    // set we have a error
                    current_status = klib::usb::dfu::device_status::firmware_error;
                }  
            }
        }

    public:
        /**
         * @brief static functions needed for the usb stack. Should not 
         * be called manually
         * 
         */

        /**
         * @brief Init function. Called when the usb stack is initalized
         * 
         * @tparam Usb 
         */
        template <typename Usb>
        static void init() {
            // init all the variables to default
            configuration = 0x00;
        }

        /**
         * @brief Called when a class specific packet is received
         * 
         * @tparam Usb 
         * @param packet 
         */
        template <typename Usb>
        static usb::handshake handle_class_packet(const klib::usb::setup_packet &packet) {
            // get the dfu request
            const auto request = static_cast<klib::usb::dfu::dfu_request>(packet.bRequest);

            // check what request we have
            switch (request) {
                case klib::usb::dfu::dfu_request::get_state:
                    // wValue = zero
                    // wIndex = interface
                    // wLength = 1
                    // data = state

                    // write the status to the host (data we are writing
                    // should be 4 byte alligned as some devices requre 
                    // this. Data should also be static to make sure it
                    // is still allocated when the dma is sending it)
                    if (Usb::write(klib::usb::usb::status_callback<Usb>, klib::usb::usb::control_endpoint, 
                        usb::endpoint_mode::in, const_cast<const uint8_t *>(reinterpret_cast<const volatile uint8_t *>(&current_state)), 
                        sizeof(current_state)))
                    {
                        // return we should wait on the ack from the write
                        return usb::handshake::wait;
                    }

                    // we could not write. Stall
                    return usb::handshake::stall;

                case klib::usb::dfu::dfu_request::get_status: {
                    // wValue = zero
                    // wIndex = interface
                    // wLength = 6
                    // data = status

                    // timeout value we are sending back to the host
                    uint32_t timeout = 0;

                    // check what we should do
                    switch (current_state) {
                        case klib::usb::dfu::device_state::download_sync:
                            current_state = klib::usb::dfu::device_state::download_busy;
                            timeout = 2;
                            // poll should call on download now
                            break;
                        case klib::usb::dfu::device_state::manifest_sync:
                            if (manifestation_complete) {
                                current_state = klib::usb::dfu::device_state::dfu_idle;

                                // reset the state back to runtime mode
                                runtime_config.interface.bInterfaceProtocol = 1;

                                // the cpu should be reset at this moment to change to the 
                                // new user appication
                            }
                            else {
                                current_state = klib::usb::dfu::device_state::manifset;
                                // poll should call manifestion now
                            }
                            break;
                        default:
                            break;
                    }

                    // create the status payload. Data needs to be 4 
                    // byte alligned on some devices. So always allign it.
                    // Data needs to be static to make sure it is still
                    // allocated when the dma is sending it.
                    alignas(4) static klib::usb::dfu::status status;
                    
                    // set the status message
                    status = {
                        .bStatus = static_cast<uint8_t>(current_status),
                        .bwPollTimout = timeout,
                        .bState = static_cast<uint8_t>(current_state),
                        .iString = 0
                    };

                    // write the status to the host
                    if (Usb::write(klib::usb::usb::status_callback<Usb>, klib::usb::usb::control_endpoint, 
                         usb::endpoint_mode::in, reinterpret_cast<const uint8_t *const>(&status), sizeof(status)))
                    {
                        // return we should wait on the ack from the write
                        return usb::handshake::wait;
                    }

                    return usb::handshake::stall;                    
                }
                case klib::usb::dfu::dfu_request::clear_status:
                    // wValue = zero
                    // wIndex = interface
                    // wLength = zero
                    // data = none

                    // clear the current status to ok
                    current_status = klib::usb::dfu::device_status::ok;

                    return usb::handshake::ack;

                case klib::usb::dfu::dfu_request::abort:
                    // wValue = zero
                    // wIndex = interface
                    // wLength = zero
                    // data = none

                    // abort and change to idle
                    current_state = klib::usb::dfu::device_state::dfu_idle;

                    return usb::handshake::ack;

                case klib::usb::dfu::dfu_request::download:
                    // wValue = wBlockNum
                    // wIndex = interface
                    // wLength = length
                    // data = firmware

                    // check if we are in the correct state to accept a download request
                    if (current_state != klib::usb::dfu::device_state::dfu_idle &&
                        current_state != klib::usb::dfu::device_state::download_idle)
                    {
                        return usb::handshake::stall;
                    }

                    // check if we got a 0 length packet
                    if (packet.wLength == 0) {
                        // check if the 0 length packet is correct
                        if (current_state == klib::usb::dfu::device_state::dfu_idle) {
                            // we do not expect a 0 length packet. Give a error and stall.
                            current_status = klib::usb::dfu::device_status::stall_error;

                            // return we should stall
                            return usb::handshake::stall;
                        }
                        else {
                            // we have received the last packet for the download. Change to
                            // the manifest sync state
                            current_state = klib::usb::dfu::device_state::manifest_sync;

                            return usb::handshake::ack;
                        }
                    }

                    // we have a non 0 length packet. Read the data
                    Usb::read(nullptr, klib::usb::usb::control_endpoint, 
                        usb::endpoint_mode::in, buffer, length
                    );

                    // check if this is a new download
                    if (current_state == klib::usb::dfu::device_state::dfu_idle) {
                        // new download. Clear the offset and clear the manifestion progress
                        offset = 0;

                        manifestation_complete = false;
                    }

                    // change the state to sync
                    current_state = klib::usb::dfu::device_state::download_sync;
            
                    // set the packet length
                    length = packet.wLength;

                    return usb::handshake::ack;

                case klib::usb::dfu::dfu_request::detach:
                    // wValue = wTimeout
                    // wIndex = interface
                    // wLength = zero
                    // data = none

                    // change to boot mode
                    runtime_config.interface.bInterfaceProtocol = 2;
                    current_state = klib::usb::dfu::device_state::dfu_idle;

                    return usb::handshake::ack;
                
                default:
                    return usb::handshake::stall;
            }

        }        

        /**
         * @brief Called when the host is disconnected
         * 
         * @tparam Usb 
         */
        template <typename Usb>
        static void disconnected() {
            configuration = 0x00;
        }

        /**
         * @brief Called when a bus reset has occured
         * 
         * @tparam Usb 
         */
        template <typename Usb>
        static void bus_reset() {
            // clear all the variables to default
            configuration = 0x00;
        }

        /**
         * @brief Clear a feature on the device
         * 
         * @warning only remote wakeup is supported
         * 
         * @tparam Usb 
         * @param feature 
         * @param packet 
         */
        template <typename Usb>
        static usb::handshake clear_feature(const klib::usb::setup::feature feature, const klib::usb::setup_packet &packet) {
            // we do not support any features
            return usb::handshake::stall;
        }

        /**
         * @brief Set a feature on the device
         * 
         * @warning only remote wakeup is supported
         * 
         * @tparam Usb 
         * @param feature 
         * @param packet 
         */
        template <typename Usb>
        static usb::handshake set_feature(const klib::usb::setup::feature feature, const klib::usb::setup_packet &packet) {
            // we do not support any features
            return usb::handshake::stall;
        }

        /**
         * @brief Get the descriptor for the descriptor type and index
         * 
         * @tparam Usb 
         * @param packet 
         * @param type 
         * @param index 
         * @return usb::description 
         */
        template <typename Usb>
        static usb::description get_descriptor(const setup_packet &packet, descriptor::descriptor_type type, const uint8_t index) {
            // check if we have a default usb descriptor
            switch (type) {
                case descriptor::descriptor_type::device:
                    // return the device descriptor
                    return to_description(device, device.bLength);
                case descriptor::descriptor_type::configuration:
                    // return the whole configuration descriptor (total size is in 
                    // wTotalLength of the configuration)
                    return to_description(runtime_config, runtime_config.configuration.wTotalLength);
                case descriptor::descriptor_type::string:
                    // check what string descriptor to send
                    switch (static_cast<string_index>(index)) {
                        case string_index::language:
                            return to_description(language, language.bLength);
                        case string_index::manufacturer:
                            return to_description(manufacturer, manufacturer.bLength);
                        case string_index::product:
                            return to_description(product, product.bLength);
                        case string_index::serial:
                            return to_description(serial, serial.bLength);
                        case string_index::flash_information:
                            return to_description(flash_information, flash_information.bLength);
                        default:
                            // unknown string descriptor
                            break;
                    }
                default:
                    // unkown default descriptor. Might be a class descriptor
                    break;
            }

            // unkown get descriptor call. return a nullptr and 0 size
            return {nullptr, 0};
        }

        /**
         * @brief Get the configuration value set in the set config call
         * 
         * @tparam Usb 
         * @param packet 
         */
        template <typename Usb>
        static usb::handshake get_config(const klib::usb::setup_packet &packet) {
            // send the configuration back to the host
            const auto result = Usb::write(
                usb::status_callback<Usb>, usb::control_endpoint, 
                usb::endpoint_mode::in, &configuration, 
                sizeof(configuration)
            );

            // check if something went wrong already
            if (!result) {
                // something went wrong stall
                return usb::handshake::stall;
            }

            // we do not ack here as the status callback 
            // will handle this for us
            return usb::handshake::wait;
        }

        /**
         * @brief Set a configuration value
         * 
         * @tparam Usb 
         * @param packet 
         */
        template <typename Usb>
        static usb::handshake set_config(const klib::usb::setup_packet &packet) {
            // check if the set is the same as the configuration we have stored
            if (packet.wValue == runtime_config.configuration.bConfigurationValue) {
                // store the configuration value
                configuration = packet.wValue;

                // notify the usb driver we are configured
                Usb::configured(true);

                // return everything is oke
                return usb::handshake::ack;
            }
            else if (packet.wValue == 0) {
                // notify the usb driver we are not configured anymore
                Usb::configured(false);

                // clear the configuration value
                configuration = 0x00;

                // ack the packet
                return usb::handshake::ack;
            }
            else {
                // not sure what to do, stall
                return usb::handshake::stall;
            }
        }

        /**
         * @brief Get the device status. Called when the status is requested
         * 
         * @tparam Usb 
         * @return uint8_t 
         */
        template <typename Usb>
        static uint8_t get_device_status() {
            // TODO: implement the correct get device status using the remote wakeup and self powered flags
            return 0;
        }
    };
}

#endif