#ifndef KLIB_LPC1756_USB_HPP
#define KLIB_LPC1756_USB_HPP

#include <klib/stream.hpp>

#include <klib/usb/usb.hpp>
#include <klib/math.hpp>

#include <lpc1756.hpp>

#include "pins.hpp"
#include "io/power.hpp"
#include "io/clocks.hpp"

namespace klib::lpc1756::io::periph::detail::usb {
    enum class mode {
        dplus,
        dminus,
        vbus,
        connect,
        led,
    };

    template <typename Pin, mode Type, typename Periph>
    struct usb {
        // pin of the peripheral
        using pin = Pin;

        // type of the pin
        constexpr static mode type = Type;

        // alternate function
        using periph = Periph;
    };
}

namespace klib::lpc1756::io::periph::lqfp_80 {
    struct usb0 {
        // peripheral id (e.g usb0, usb1)
        constexpr static uint32_t id = 0;

        // peripheral clock bit position
        constexpr static uint32_t clock_id = 31;

        // interrupt id (including the arm vector table)
        constexpr static uint32_t interrupt_id = 40;

        // port to the usb hardware
        static inline USB_Type *const port = USB;

        // configuration of the pins 
        using connect = detail::usb::usb<pins::package::lqfp_80::p49, detail::usb::mode::connect, io::detail::alternate::func_1>;
        using led = detail::usb::usb<pins::package::lqfp_80::p25, detail::usb::mode::led, io::detail::alternate::func_1>;
        using vbus = detail::usb::usb<pins::package::lqfp_80::p18, detail::usb::mode::vbus, io::detail::alternate::func_2>;
        using dplus = detail::usb::usb<pins::package::lqfp_80::p22, detail::usb::mode::dplus, io::detail::alternate::func_1>;
        using dminus = detail::usb::usb<pins::package::lqfp_80::p23, detail::usb::mode::dminus, io::detail::alternate::func_1>;
    };
}

namespace klib::lpc1756::io::detail::usb {
    // TODO: decide if we want memory efficiency or flash 
    // efficiency here. (adds 32 bytes ram or 164 bytes rom)
    // // change the packing and push the old packing
    // #pragma pack(push, 1)

    /**
     * @brief Struct to store the state of a endpoint
     * 
     */
    struct state {
        // flag if the endpoint is busy
        volatile bool is_busy;

        // max size of the endpoint
        uint8_t max_size;

        // pointer to the data
        uint8_t *data;
    
        // requested size of the current endpoint
        uint32_t requested_size;

        // transmitted/received amount of data.
        volatile uint32_t transferred_size; 

        // callback function
        klib::usb::usb::usb_callback callback;
    };

    // // restore the old packing
    // #pragma pack(pop)
}

namespace klib::lpc1756::io {
    template <typename Usb, typename Device, typename Irq>
    class usb {
    public:
        // amount of endpoints supported by the lpc1756
        constexpr static uint8_t endpoint_count = 16;

        // max size in a single endpoint
        constexpr static uint8_t max_endpoint_size = 64;
        
    protected:
        // physical endpoint count. 2x the amount of the logical 
        // endpoint count as we have a in and out endpoint
        constexpr static uint8_t physical_endpoint_count = endpoint_count * 2;

        // struct with information about the state of a endpoint
        static inline detail::usb::state state[endpoint_count] = {};

        // control endpoint buffer
        static inline uint8_t endpoint0_buffer[max_endpoint_size] = {};

        // variable to store the device address. Used to buffer the
        // address before setting it in the hardware
        static inline uint8_t device_address = 0x00;

        /**
         * @brief Command phase
         * 
         */
        enum class command_phase: uint8_t {
            write = 0x01,
            read = 0x02,
            command = 0x05,
        };

        /**
         * @brief Usb device commands
         * 
         */
        enum class device_command: uint8_t {
            set_address = 0xd0,
            configure = 0xd8,
            set_mode = 0xf3,
            read_frame_number = 0xf5,
            read_test_register = 0xfd,
            set_status = 0xfe,
            get_status = 0xfe,
            get_error = 0xff,
            get_error_status = 0xfb
        };

        /**
         * @brief Usb endpoint commands
         * 
         */
        enum class endpoint_command: uint8_t {
            // select endpoint. to select 
            // a different endpoint than 0
            // add the endpoint number to 
            // this value (e.g.)
            // endpoint 0 = 0x00 + 0x01
            // endpoint 1 = 0x00 + 0x01
            // endpoint 2 = 0x00 + 0x02
            select_endpoint = 0x00,
            // same notes as select endpoint 
            // command
            clear_endpoint = 0x40,
            // same notes as select endpoint
            // command
            set_status = 0x40,
            clear_buffer = 0xf2,
            validate_buffer = 0xfa,
        };

        /**
         * @brief Defice status bits
         * 
         */
        enum class device_status: uint8_t {
            connect = 0,
            connect_change = 1,
            suspend = 2,
            suspend_change = 3,
            reset = 4
        };

        /**
         * @brief Set enpoint status bits
         * 
         */
        enum class endpoint_status: uint8_t {
            stall = 0,
            disable = 5,
            rate_feedback = 6,
            conditional_stall = 7
        };

        static void reset() {
            Usb::port->EPIND = 0;
            Usb::port->MAXPSIZE = max_endpoint_size;
            Usb::port->EPIND = 1;
            Usb::port->MAXPSIZE = max_endpoint_size;

            while ((Usb::port->DEVINTST & 0x100) == 0) {
                // do nothing
            }

            Usb::port->EPINTCLR = 0xFFFFFFFF;
            Usb::port->EPINTEN = 0xFFFFFFFF;
            Usb::port->DEVINTCLR = 0xFFFFFFFF;

            // enable interrupt endpoint slow and stat interrupt
            Usb::port->DEVINTEN = 0x8 | 0x4;

            // call the device bus reset
            device::template bus_reset<usb_type>();
        }

        static bool endpoint_mode_to_raw(const klib::usb::usb::endpoint_mode mode) {
            switch (mode) {
                case klib::usb::usb::endpoint_mode::out:
                    return false;
                case klib::usb::usb::endpoint_mode::in:
                case klib::usb::usb::endpoint_mode::control:
                case klib::usb::usb::endpoint_mode::disabled:
                default:
                    return true;
            }
        }

        /**
         * @brief Write a command without data
         * 
         * @tparam T 
         * @param phase 
         * @param command 
         */
        template <typename T>
        static void write_command(const command_phase phase, const T command) {
            Usb::port->DEVINTCLR = 0x10;
            Usb::port->CMDCODE = ((static_cast<uint8_t>(phase) << 8) | (static_cast<uint32_t>(command) << 16));

            while ((Usb::port->DEVINTST & 0x10) == 0) {
                // wait
            }
        }

        /**
         * @brief Write a command with data
         * 
         * @tparam T 
         * @param phase 
         * @param command 
         * @param value 
         */
        template <typename T>
        static void write_command(const command_phase phase, const T command, uint8_t value) {
            // write the command
            write_command(phase, command);

            // write the value
            Usb::port->DEVINTCLR = 0x10;
            Usb::port->CMDCODE = 0x100 | (value << 16);

            while ((Usb::port->DEVINTST & 0x10) == 0) {
                // wait
            }
        }

        template <typename T>
        static uint32_t read_result(const command_phase phase, const T command) {
            Usb::port->DEVINTCLR = 0x10 | 0x20;
            Usb::port->CMDCODE = ((static_cast<uint8_t>(phase) << 8) | (static_cast<uint32_t>(command) << 16));

            while ((Usb::port->DEVINTST & (0x20 | 0x10)) == 0) {
                // wait
            }

            return (Usb::port->CMDDATA);
        }

        /**
         * @brief Write a command for a endpoint. Uses logical endpoints with a 
         * flag for in endpoints
         * 
         * @param endpoint 
         * @param in_endpoint 
         * @param cmd 
         */
        template <typename T>
        static void write_ep_command(const uint8_t endpoint, bool in_endpoint, const command_phase phase, const T command) {
            // write the select endpoint command
            write_command(command_phase::command, endpoint << 1 | in_endpoint);

            // write the command
            write_command(phase, command);
        }

        static uint32_t read_ep_command(const uint8_t endpoint, bool in_endpoint) {
            // write the select endpoint command
            write_command(command_phase::command, endpoint << 1 | in_endpoint);    

            // return the result
            return read_result(command_phase::read, endpoint << 1 | in_endpoint);     
        }

        static void set_device_address_impl(const uint8_t address) {
            // set the address 2x for some reason. TODO: check if this is needed
            // set the address enabled bit with the address
            write_command(command_phase::command, device_command::set_address, 0x80 | address);
            write_command(command_phase::command, device_command::set_address, 0x80 | address);
        }

        static void write_impl(const uint8_t endpoint, const klib::usb::usb::endpoint_mode mode, 
                               const uint8_t* data, const uint32_t size) 
        {
            // set the current endpoint (and enable the write)
            Usb::port->CTRL = (endpoint & 0xf) << 2 | (0x1 << 1);

            // set the length we want to write
            Usb::port->TXPLEN = size;

            // write the data
            for (uint32_t i = 0; i < size; i += 4) {
                // write 4 bytes at the time
                Usb::port->TXDATA = *reinterpret_cast<const uint32_t*>(&data[i]);
            }

            // clear the write flag
            Usb::port->CTRL = 0x00;

            // validate the buffer
            write_ep_command(endpoint, endpoint_mode_to_raw(mode), command_phase::command, endpoint_command::validate_buffer);
        }

        static uint32_t read_impl(const uint8_t endpoint, const klib::usb::usb::endpoint_mode mode, 
                                  uint8_t* data, const uint32_t size) 
        {
            // set the current endpoint (and enable read mode)
            Usb::port->CTRL = (endpoint << 2) | 0x1;

            // get the inital value
            uint32_t status = Usb::port->RXPLEN;

            // wait until the data is ready
            while ((status & 0x800) == 0x00) {
                // update the status
                status = Usb::port->RXPLEN;
            }

            // get the byte count from the status
            const uint32_t count = status & 0x3ff;

            // read all the available data
            for (uint32_t i = 0; (i < count) && (i < size); i += 4) {
                // read 4 bytes at the time
                const uint32_t value = Usb::port->RXDATA;

                // write into the buffer 4 bytes at the time
                *reinterpret_cast<uint32_t*>(&data[i]) = value;
            }

            // clear the read flag
            Usb::port->CTRL = 0;

            // only clear the buffers if we have a non isochronous endpoint
            if (((0x1 << endpoint) & 0x1248) == 0) {
                // clear the buffer
                write_ep_command(endpoint, endpoint_mode_to_raw(mode), command_phase::command, endpoint_command::clear_buffer);
            }

            // return the amount of data we have read
            return count;
        }

        static void setup_packet() {
            // struct to store the setup packet in
            klib::usb::setup_packet packet;

            // read the setup packet
            read(nullptr, klib::usb::usb::control_endpoint, 
                klib::usb::usb::endpoint_mode::out, 
                reinterpret_cast<uint8_t*>(&packet), sizeof(packet)
            );

            // handle the setup packet in the klib library
            klib::usb::usb::handle_setup_packet<usb_type>(packet);
        }

        static void clear_endpoint_state(const uint8_t endpoint) {
            state[endpoint].is_busy = false;
            state[endpoint].requested_size = 0;
            state[endpoint].transferred_size = 0;
            state[endpoint].callback = nullptr;
            state[endpoint].data = nullptr;
        }

        static void endpoint_callback(const uint8_t endpoint, const klib::usb::usb::endpoint_mode mode) {
            switch (mode) {
                case klib::usb::usb::endpoint_mode::in:
                    // check if we need to update the device address. This cannot be 
                    // done in the set_device_address callback. 
                    if (device_address) {
                        // change the device address
                        set_device_address_impl(device_address);

                        // clear the device address value to prevent it from updating
                        // again
                        device_address = 0;
                    }

                    // check if we are busy transmitting data
                    if (state[endpoint].is_busy) {
                        // get the maximum size we can transmit
                        const uint32_t s = klib::min(
                            (state[endpoint].requested_size - state[endpoint].transferred_size),
                            state[endpoint].max_size
                        );

                        // check if we are done
                        if (s) {
                            write_impl(
                                endpoint, klib::usb::usb::endpoint_mode::in, 
                                (state[endpoint].data + state[endpoint].transferred_size), s
                            );

                            // update the amount we have transferred
                            state[endpoint].transferred_size += s;
                        }

                        // check if we are done
                        if (state[endpoint].transferred_size >= state[endpoint].requested_size) {
                            // we are done. clear the flag and call the callback
                            const auto callback = state[endpoint].callback;

                            // clear the state
                            clear_endpoint_state(endpoint);

                            // check for any callbacks
                            if (callback) {
                                // call the callback
                                callback(
                                    endpoint, klib::usb::usb::endpoint_mode::in, 
                                    klib::usb::usb::error::no_error
                                );
                            }
                        }
                    }
                    break;

                case klib::usb::usb::endpoint_mode::out:
                default:
                    // check if we should receive more data
                    if (state[endpoint].is_busy) {
                        // receive more data
                        state[endpoint].transferred_size += read_impl(
                            endpoint, klib::usb::usb::endpoint_mode::out, 
                            (state[endpoint].data + state[endpoint].transferred_size),
                            state[endpoint].requested_size - state[endpoint].transferred_size
                        );

                        // check if we are done
                        if (state[endpoint].transferred_size >= state[endpoint].requested_size) {
                            // we are done. clear the flag and call the callback
                            const auto callback = state[endpoint].callback;

                            // clear the state
                            clear_endpoint_state(endpoint);

                            // check for any callbacks
                            if (callback) {
                                // call the callback
                                callback(
                                    endpoint, klib::usb::usb::endpoint_mode::out, 
                                    klib::usb::usb::error::no_error
                                );
                            }
                        }
                    }
                    break;
            }

            // check if the device has the vendor handler
            constexpr bool has_endpoint_callback = requires(const uint8_t endpoint, const klib::usb::usb::endpoint_mode mode) {
                device::template endpoint_callback<usb_type>(endpoint, mode);
            };

            // check if the device has the endpoint callback
            if constexpr (has_endpoint_callback) {
                // call the device endpoint callback
                device::template endpoint_callback<usb_type>(endpoint, mode);
            }
        }

        static void data_irq_handler() {
            const uint32_t status = Usb::port->EPINTST;
            const uint32_t mask = Usb::port->EPINTEN;

            // get the masked status
            uint32_t masked_status = status & mask;

            // amount of trailing zeros in the status register
            uint8_t trailing_zeros = 0;

            // check what endpoint has triggered the interrupt (32 == zero's)
            while ((trailing_zeros = klib::ctz(masked_status)) < 32) {
                // get the current endpoint
                const uint8_t endpoint = trailing_zeros;

                // clear the interrupt of the current endpoint
                Usb::port->EPINTCLR = (0x1 << endpoint);

                // wait until the CDFULL bit is cleared so we can read 
                // the status of the endpoint in the cmddata field
                while ((Usb::port->DEVINTST & (0x1 << 5)) == 0) {
                    // wait and do nothing
                }

                // clear the bit from the masked_status interrupts
                masked_status &= ~(0x1 << endpoint);

                // get the endpoint status
                const uint32_t value = Usb::port->CMDDATA;

                // check if we have a in or out endpoint
                if ((endpoint & 0x1) == 0) {
                    // check if we have a setup packet
                    if (endpoint == 0 && value & (0x1 << 2)) {
                        // we have a setup packet. Handle it
                        setup_packet();
                    }
                    else {
                        // we have a out endpoint. 
                        endpoint_callback(endpoint >> 1, klib::usb::usb::endpoint_mode::out);
                    }
                }
                else {
                    // we have a in endpoint
                    endpoint_callback(endpoint >> 1, klib::usb::usb::endpoint_mode::in);
                }
            }
        }

        static void device_status_irq() {
            // get the status
            write_command(command_phase::read, device_command::get_status);
            const uint32_t status = read_result(command_phase::read, device_command::get_status);

            // check for the different flag
            if (status & 0x10) {
                // reset event. Reset the usb driver
                reset();

                // call all the callbacks with a error
                for (uint32_t i = 0; i < endpoint_count; i++) {
                    const auto callback = state[i].callback;

                    // clear the state
                    clear_endpoint_state(i);

                    if (callback) {
                        // call with a reset. Endpoint mode as in but we do not know what it is
                        callback(i, klib::usb::usb::endpoint_mode::in, klib::usb::usb::error::reset);
                    }
                }
            }

            // check for the connect flag
            if (status & 0x2) {
                // we have a connect change event. Notify the user
                if (status & 0x1) {
                    // call the user function to signal we have connected to a host
                    Device::template connected<usb_type>();
                }
                else {
                    // call the user function to signal we have disconnected
                    Device::template disconnected<usb_type>();
                }
            }

            // check for a suspend/resume
            if (status & (0x1 << 3)) {
                // we have a suspend/resume event
                if (status & (0x1 << 2)) {
                    // call the device sleep function
                    device::template sleep<usb_type>();
                }
                else {
                    // call the device wakeup function
                    device::template wakeup<usb_type>();
                }
            }
        }

        /**
         * @brief Interrupt handler for the usb driver
         * 
         */
        static void irq_handler() {
            // get the status and the mask
            const uint32_t status = Usb::port->DEVINTST;
            const uint32_t mask = Usb::port->DEVINTEN;

            // create the masked status
            const uint32_t masked_status = status & mask;

            // check for a status interrupt
            if (masked_status & (0x1 << 4)) {
                // we have a device status interrupt
                device_status_irq();
            }
            else if (masked_status & (0x1 << 2)) {
                // we have a endpoint interrupt. Handle it in the data irq
                data_irq_handler();
            }

            // clear the interrupt status after handling all 
            // the interrupts
            Usb::port->DEVINTCLR = masked_status;
        }

    public:
        // type to use in device functions
        using usb_type = usb<Usb, Device, Irq>;

        // type so the klib usb driver can comunicate to the device
        using device = Device;

        template <bool UsbLed = true, bool UsbConnect = true>
        static void init() {
            // clock register
            constexpr uint32_t ctrl = (0x1 << 4) | (0x1 << 3) | (0x1 << 1);

            // enable the device clock and AHB clock for the usb
            // TODO: determine if we also need the OTG clock
            Usb::port->OTGCLKCTRL = ctrl;

            // wait until the data is set
            while ((Usb::port->OTGCLKST & ctrl) != ctrl) {
                // wait and do nothing
            }

            // setup the io
            io::detail::pins::set_peripheral<typename Usb::vbus::pin, typename Usb::vbus::periph>();
            io::detail::pins::set_peripheral<typename Usb::dplus::pin, typename Usb::dplus::periph>();
            io::detail::pins::set_peripheral<typename Usb::dminus::pin, typename Usb::dminus::periph>();

            // check if we need to enable the USB connect pin
            if constexpr (UsbConnect) {
                // setup the usb connect pin
                io::detail::pins::set_peripheral<typename Usb::connect::pin, typename Usb::connect::periph>();
            }

            // check if we need to enable the usb led
            if constexpr (UsbLed) {
                io::detail::pins::set_peripheral<typename Usb::led::pin, typename Usb::led::periph>();
            }

            // reset all the info stored about the endpoints
            for (uint32_t i = 0; i < endpoint_count; i++) {
                // set the endpoint to a known state
                state[i] = {
                    .is_busy = false,
                    .max_size = static_cast<uint8_t>((i == 0) ? max_endpoint_size : 0),
                    .data = nullptr,
                    .requested_size = 0,
                    .transferred_size = 0, 
                    .callback = nullptr,
                };
            }

            // register ourselfs with the interrupt controller
            irq::register_irq<Usb::interrupt_id>(irq_handler);

            // enable the usb interrupt
            enable_irq<Usb::interrupt_id>();

            // do a partial manual reset since automatic bus 
            // reset is not working.
            reset();

            // clear the device addess for the manual reset
            set_device_address_impl(0);

            // init the device
            device::template init<usb_type>();

            // check if we should enable the usb connect feature
            if constexpr (UsbConnect) {
                // enable the usb connect pin to allow connections
                connect();
            }
        }

        static void configured(const bool cfg) {
            // mark the endpoint as configured
            write_command(command_phase::command, device_command::configure, cfg ? 0x1 : 0);

            // mark endpoint 0 (in and out) as realized
            if (cfg) {
                // do not clear the other realized endpoints
                Usb::port->REEP |= 0x00000003;
            }
            else {
                Usb::port->REEP = 0x00000003;
            }

            // wait until the flag is updated
            while ((Usb::port->DEVINTST & 0x100) == 0) {
                // do nothing and wait
            }

            // clear the interrupt flag
            Usb::port->DEVINTCLR = 0x100;
        }

        static void configure(const uint8_t endpoint, const klib::usb::usb::endpoint_mode mode, const uint32_t size) {
            // get the physical endpoint number
            const uint32_t ep = (endpoint << 1) | endpoint_mode_to_raw(mode);

            // mark the endpoint as realized
            Usb::port->REEP |= (1 << ep);

            // select the endpoint
            Usb::port->EPIND = ep;

            // set the endpoint max size
            Usb::port->MAXPSIZE = size;

            // set the new endpoint size
            state[endpoint].max_size = size;

            // wait until the flag is updated
            while ((Usb::port->DEVINTST & 0x100) == 0) {
                // do nothing and wait
            }

            // clear the interrupt flag
            Usb::port->DEVINTCLR = 0x100;

            // enable the endpoint
            write_command(command_phase::command, 
                static_cast<uint8_t>(endpoint_command::select_endpoint) | ep, 0
            );

            // reset the endpoint
            reset(endpoint, mode);
        }   

        static klib::usb::usb::handshake set_device_address(const uint8_t address) {
            // set the address in the variable. We cannot update the device address
            // here as it would change immediately and will not respond to previous
            // packets
            device_address = address;

            // ack the set device address
            return klib::usb::usb::handshake::ack;
        }

        static void reset(const uint8_t endpoint, const klib::usb::usb::endpoint_mode mode) {
            // reset the endpoint
            write_command(command_phase::command, 
                static_cast<uint8_t>(endpoint_command::set_status) | (endpoint << 1 | endpoint_mode_to_raw(mode)), 
                0x0
            );

            // get the callback
            const auto callback = state[endpoint].callback;

            // clear the state of the endpoint
            clear_endpoint_state(endpoint);

            if (callback) {
                // send a error to the callback. Do not care about the 
                // return value as we are always clearing the endpoint
                // in a reset event
                callback(endpoint, mode, klib::usb::usb::error::reset);
            }
        }

        static void connect() {
            // connect the pullup
            write_command(
                command_phase::command, device_command::set_status, 
                0x1 << static_cast<uint8_t>(device_status::connect)
            );
        }

        static void disconnect() {
            // disconnect the pullup to disconnect from the host
            write_command(command_phase::command, device_command::set_status, 0);
        }

        static void ack(const uint8_t endpoint, const klib::usb::usb::endpoint_mode mode) {
            // check how we should ack
            if (mode != klib::usb::usb::endpoint_mode::out) {
                write_impl(endpoint, klib::usb::usb::endpoint_mode::in, nullptr, 0);
            }
            else {
                // read data from the endpoint (we discard everything)
                read_impl(endpoint, klib::usb::usb::endpoint_mode::out, nullptr, 0);
            }
        }

        static void stall(const uint8_t endpoint, const klib::usb::usb::endpoint_mode mode) {
            write_command(command_phase::command, 
                static_cast<uint8_t>(endpoint_command::set_status) | (endpoint << 1 | endpoint_mode_to_raw(mode)), 
                0x1 << static_cast<uint8_t>(endpoint_status::stall)
            );

            // get the callback
            const auto callback = state[endpoint].callback;

            // clear the state of the endpoint
            clear_endpoint_state(endpoint);

            if (callback) {
                // send a error to the callback
                callback(endpoint, mode, klib::usb::usb::error::stall);
            }
        }

        static void un_stall(const uint8_t endpoint, const klib::usb::usb::endpoint_mode mode) {
            write_command(command_phase::command, 
                static_cast<uint8_t>(endpoint_command::set_status) | (endpoint << 1 | endpoint_mode_to_raw(mode)), 
                0x0
            );

            // check if we are stalled
            if (!is_stalled(endpoint, mode)) {
                // we are not stalled return
                return;
            }

            // get the callback
            const auto callback = state[endpoint].callback;

            // clear the state of the endpoint
            clear_endpoint_state(endpoint);

            if (callback) {
                // send a error to the callback
                callback(endpoint, mode, klib::usb::usb::error::un_stall);
            }
        }

        static bool is_stalled(const uint8_t endpoint, const klib::usb::usb::endpoint_mode mode) {
            // return the stalled endpoint flag
            return read_ep_command(endpoint, endpoint_mode_to_raw(mode)) & (0x1 << 1);
        }

        static bool write(const klib::usb::usb::usb_callback callback, const uint8_t endpoint, 
                          const klib::usb::usb::endpoint_mode mode, const uint8_t* data, 
                          const uint32_t size) 
        {
            
            // get the size we can write in a single transmission
            const uint32_t s = klib::min(size, state[endpoint].max_size);

            // set the endpoint callback and mark the endpoint as busy
            state[endpoint].is_busy = true;
            state[endpoint].callback = callback;

            // we remove the const here as we know we dont write to it.
            // the state cannot have the const as the read does write to 
            // the array
            state[endpoint].data = const_cast<uint8_t*>(data);
            
            // set the endpoint data
            state[endpoint].requested_size = size;
            state[endpoint].transferred_size = s;

            // call the write implementation
            write_impl(endpoint, mode, data, s);

            // notify everything is correct
            return true;
        }

        static bool read(const klib::usb::usb::usb_callback callback, const uint8_t endpoint, 
                         const klib::usb::usb::endpoint_mode mode, uint8_t* data, 
                         const uint32_t size) 
        {
            // set the endpoint callback and mark the endpoint as busy
            state[endpoint].is_busy = true;
            state[endpoint].callback = callback;
            state[endpoint].data = data;
            
            // set the endpoint data
            state[endpoint].requested_size = size;
            state[endpoint].transferred_size = 0;

            // call the read and discard the result
            (void)read_impl(endpoint, mode, data, size);

            // notify everything is correct
            return true;
        }

        static bool is_pending(const uint8_t endpoint, const klib::usb::usb::endpoint_mode mode) {
            return state[endpoint].is_busy;
        }
    };
}

#endif