#ifndef KLIB_NXP_LPC17XX_USB_HPP
#define KLIB_NXP_LPC17XX_USB_HPP

#include <span>

#include <klib/klib.hpp>
#include <klib/usb/usb.hpp>
#include <klib/math.hpp>

#include <io/power.hpp>
#include <io/port.hpp>
#include <io/clocks.hpp>

namespace klib::core::lpc17xx::io::detail::usb {
    /**
     * @brief Struct to store the state of a endpoint
     * 
     */
    struct state {
        // flag if the endpoint is busy
        bool is_busy;

        // flag if a endpoint out interrupt is pending
        // (only used on out endpoints)
        bool interrupt_pending;

        // max size of the endpoint
        uint8_t max_size;

        // pointer to the data
        uint8_t *data;
    
        // requested size of the current endpoint
        uint32_t requested_size;

        // maximum requested size. (only used on out endpoints)
        uint32_t max_requested_size;

        // transmitted/received amount of data.
        uint32_t transferred_size; 

        // callback function
        klib::usb::usb::usb_callback callback;
    };
}

namespace klib::core::lpc17xx::io {
    template <typename Usb, typename Device>
    class usb {
    public:
        // amount of endpoints supported by the lpc1756
        constexpr static uint8_t endpoint_count = 16;

        // max size in a single endpoint
        constexpr static uint8_t max_endpoint_size = 64;
        
        // type to use in device functions
        using usb_type = usb<Usb, Device>;

        // type so the klib usb driver can comunicate to the device
        using device = Device;

    protected:
        /**
         * @brief Flags about the usb device we are configured with.
         * 
         */

        // check if the device has the usb wakeup callback
        constexpr static bool has_wakeup_callback = requires() {
            device::template wakeup<usb_type>();
        };

        // check if the device has the usb sleep callback
        constexpr static bool has_sleep_callback = requires() {
            device::template sleep<usb_type>();
        };

        // check if the device has the usb disconnected callback
        constexpr static bool has_disconnected_callback = requires() {
            device::template disconnected<usb_type>();
        };

        // check if the device has the usb connected callback
        constexpr static bool has_connected_callback = requires() {
            device::template connected<usb_type>();
        };

        // check if the device has the usb activity callback
        constexpr static bool has_activity_callback = requires() {
            device::template activity<usb_type>();
        };

        // check if the device has the usb bus reset callback
        constexpr static bool has_bus_reset_callback = requires() {
            device::template bus_reset<usb_type>();
        };

    protected:
        // physical endpoint count. 2x the amount of the logical 
        // endpoint count as we have a in and out endpoint
        constexpr static uint8_t physical_endpoint_count = endpoint_count * 2;

        // struct with information about the state of a endpoint
        static inline volatile detail::usb::state state[endpoint_count] = {};

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

            // enable the interrupts for all the endpoints
            Usb::port->EPINTCLR = 0xffffffff;
            Usb::port->EPINTEN = 0xffffffff;

            // enable interrupt endpoint slow and stat interrupt
            Usb::port->DEVINTCLR = 0xffffffff;
            Usb::port->DEVINTEN = 0x8 | 0x4;

            if constexpr (has_bus_reset_callback) {
                // call the device bus reset
                device::template bus_reset<usb_type>();
            }
        }

        /**
         * @brief Convert a mode to the correct raw value for the usb hardware
         * 
         * @param mode 
         * @return uint8_t 
         */
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
            read_impl(klib::usb::usb::control_endpoint, 
                klib::usb::usb::endpoint_mode::out, 
                reinterpret_cast<uint8_t*>(&packet), sizeof(packet)
            );

            // clear the pending flag on the first endpoint when 
            // we receive a setup packet. This is to prevent us from
            // reading the setup packet again
            state[0].interrupt_pending = false;

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

        static void endpoint_in_callback(const uint8_t endpoint) {
            // check if we are busy transmitting data
            if (!state[endpoint].is_busy) {
                return;
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
            else {
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
            }            
        }

        static void endpoint_out_callback(const uint8_t endpoint) {
            // check if we are busy.
            if (!state[endpoint].is_busy) {
                // set the flag we have a out interrupt pending
                state[endpoint].interrupt_pending = true;

                return;
            }

            // receive more data
            state[endpoint].transferred_size += read_impl(
                endpoint, klib::usb::usb::endpoint_mode::out, 
                (state[endpoint].data + state[endpoint].transferred_size),
                state[endpoint].max_requested_size - state[endpoint].transferred_size
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

        static void data_irq_handler() {
            // get the endpoints we should handle
            uint32_t status = Usb::port->EPINTST;

            // amount of trailing zeros in the status register
            uint8_t trailing_zeros = 0;

            // check what endpoint has triggered the interrupt (32 == zero's)
            while ((trailing_zeros = klib::ctz(status)) < 32) {
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
                status &= ~(0x1 << endpoint);

                // get the endpoint status
                const uint32_t value = Usb::port->CMDDATA;

                // get the endpoint mode
                const auto mode = (
                    (endpoint & 0x1) ? 
                    klib::usb::usb::endpoint_mode::in : 
                    klib::usb::usb::endpoint_mode::out
                );

                const auto ep = endpoint >> 1;

                // check if we have a control endpoint
                if (ep == 0 && (value & (0x1 << 2))) {
                    // we have a control endpoint
                    setup_packet();
                }
                else {
                    // check if we have a ack or nak. If we have a nak we will
                    // send it to the callback if available
                    if (value & (0x1 << 4)) {
                        // get the callback
                        const auto callback = state[ep].callback;

                        // check if the callback is valid
                        if (callback) {
                            // call the callback
                            callback(ep, mode, klib::usb::usb::error::nak);
                        }
                    }
                    else {
                        // handle the endpoint request
                        switch (mode) {
                            case klib::usb::usb::endpoint_mode::in:
                                endpoint_in_callback(ep);
                                break;
                            case klib::usb::usb::endpoint_mode::out:
                                endpoint_out_callback(ep);
                                break;
                        }
                    }
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
                    if constexpr (has_connected_callback) {
                        // call the user function to signal we have connected to a host
                        device::template connected<usb_type>();
                    }
                }
                else {
                    if constexpr (has_disconnected_callback) {
                        // call the user function to signal we have disconnected
                        device::template disconnected<usb_type>();
                    }
                }
            }

            // check for a suspend/resume
            if (status & (0x1 << 3)) {
                // we have a suspend/resume event
                if (status & (0x1 << 2)) {
                    if constexpr (has_sleep_callback) {
                        // call the device sleep function
                        device::template sleep<usb_type>();
                    }
                }
                else {
                    if constexpr (has_wakeup_callback) {                    
                        // call the device wakeup function
                        device::template wakeup<usb_type>();
                    }
                }
            }
        }

    public:
        /**
         * @brief Interrupt handler for the usb driver
         * 
         * @warning Should not be called by the user
         * 
         */
        static void irq_handler() {
            // get the status and the mask
            const uint32_t status = Usb::port->DEVINTST;
            const uint32_t mask = Usb::port->DEVINTEN;

            // create the masked status
            const uint32_t masked_status = status & mask;

            // clear the interrupt status so we dont miss any
            // interrupts while the user code is running
            Usb::port->DEVINTCLR = masked_status;

            // check for a status interrupt
            if (masked_status & (0x1 << 4)) {
                // we have a device status interrupt
                device_status_irq();
            }
            else if (masked_status & (0x1 << 2)) {
                // we have a endpoint interrupt. Handle it in the data irq
                data_irq_handler();
            }
        }

    public:
        /**
         * @brief Initialize the usb hardware. This requires the usb pll to be enabled beforehand using
         * 
         * target::io::system::clock::set_usb<external_crystal_frequency>();
         * 
         * @tparam UsbLed 
         * @tparam UsbConnect 
         */
        template <bool UsbLed = true, bool UsbConnect = true, bool NakIrq = false>
        static void init() {
            // enable the usb power
            target::io::power_control::enable<Usb>();

            // clock register
            constexpr uint32_t ctrl = (0x1 << 4) | (0x1 << 1);

            // enable the device clock and AHB clock for the usb
            Usb::port->OTGCLKCTRL = ctrl;

            // wait until the data is set
            while ((Usb::port->OTGCLKST & ctrl) != ctrl) {
                // wait and do nothing
            }

            // setup the io
            target::io::detail::pins::set_peripheral<typename Usb::vbus::pin, typename Usb::vbus::periph>();
            target::io::detail::pins::set_peripheral<typename Usb::dplus::pin, typename Usb::dplus::periph>();
            target::io::detail::pins::set_peripheral<typename Usb::dminus::pin, typename Usb::dminus::periph>();

            // check if we need to enable the USB connect pin
            if constexpr (UsbConnect) {
                // setup the usb connect pin
                target::io::detail::pins::set_peripheral<typename Usb::connect::pin, typename Usb::connect::periph>();
            }

            // check if we need to enable the usb led
            if constexpr (UsbLed) {
                target::io::detail::pins::set_peripheral<typename Usb::led::pin, typename Usb::led::periph>();
            }

            // reset all the info stored about the endpoints
            for (uint32_t i = 0; i < endpoint_count; i++) {
                // set the endpoint to a known state
                state[i].is_busy = false;
                state[i].max_size = static_cast<uint8_t>((i == 0) ? max_endpoint_size : 0);
                state[i].data = nullptr;
                state[i].requested_size = 0;
                state[i].transferred_size = 0;
                state[i].callback = nullptr;
            }

            // check if we should enable naks
            if constexpr (NakIrq) {
                // enable nak interrupts on all endpoint types except control 
                // (interrupt in/out, bulk in/out)
                write_command(command_phase::command, device_command::set_mode, 0b1111 << 3);
            }

            // register ourselfs with the interrupt controller
            target::irq::register_irq<Usb::interrupt_id>(irq_handler);

            // enable the usb interrupt
            target::enable_irq<Usb::interrupt_id>();

            // do a partial manual reset since automatic bus 
            // reset is not working.
            reset();

            // clear the device addess for the manual reset
            set_device_address(0);

            // init the device
            device::template init<usb_type>();

            // check if we should enable the usb connect feature
            if constexpr (UsbConnect) {
                // enable the usb connect pin to allow connections
                connect();
            }
        }

        /**
         * @brief Function to check if a endpoint with type is supported at compile time
         * 
         * @tparam endpoint 
         * @tparam type 
         */
        template <uint8_t endpoint, klib::usb::descriptor::transfer_type type>
        constexpr static void is_valid_endpoint() {
            using transfer_type = klib::usb::descriptor::transfer_type;

            // make sure the endpoint is valid
            static_assert(endpoint <= 0xf, "Invalid endpoint provided");

            // check if the endpoint supports the transfer type
            if constexpr (type == transfer_type::control) {
                static_assert(endpoint == 0, 
                    "Endpoint does not support control mode"
                );
            }
            else if constexpr (type == transfer_type::interrupt) {
                static_assert((0x1 << endpoint) & 0b10010010010010, 
                    "Endpoint does not support interrupt mode"
                );
            }
            else if constexpr (type == transfer_type::isochronous) {
                static_assert((0x1 << endpoint) & 0b1001001001000, 
                    "Endpoint does not support isochronous mode"
                );
            }
            else {
                static_assert(
                    (0x1 << endpoint) & 0b110100100100100, 
                    "Endpoint does not support bulk mode"
                );
            }
        }

        /**
         * @brief Function that gets called to notify the driver 
         * the devices is configured
         * 
         * @param cfg 
         */
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

        /**
         * @brief Configure a endpoint
         * 
         * @param endpoint 
         * @param mode 
         * @param type
         * @param size 
         */
        static void configure(
            const uint8_t endpoint, const klib::usb::usb::endpoint_mode mode, 
            const klib::usb::descriptor::transfer_type type, const uint32_t size) 
        {
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

        /**
         * @brief Set the device address. Handled in hardware
         * 
         * @param address 
         */
        static klib::usb::usb::handshake set_device_address(const uint8_t address) {
            write_command(command_phase::command, device_command::set_address, 0x80 | address);

            // ack the set device address
            return klib::usb::usb::handshake::ack;
        }

        /**
         * @brief Reset a endpoint
         * 
         * @param endpoint 
         * @param mode
         */
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

        /**
         * @brief Connect the device to the host by enabling the
         * pullup
         * 
         */
        static void connect() {
            // connect the pullup
            write_command(
                command_phase::command, device_command::set_status, 
                0x1 << static_cast<uint8_t>(device_status::connect)
            );
        }

        /**
         * @brief Disconnect the device from the host by disabling
         * the pullup
         * 
         */
        static void disconnect() {
            // disconnect the pullup to disconnect from the host
            write_command(command_phase::command, device_command::set_status, 0);
        }

        /**
         * @brief Ack a endpoint
         * 
         * @param endpoint 
         * @param mode
         */
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

        /**
         * @brief Stall a endpoint
         * 
         * @param endpoint 
         * @param mode
         */
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

        /**
         * @brief Unstall a endpoint
         * 
         * @param endpoint 
         * @param mode
         */
        static void un_stall(const uint8_t endpoint, const klib::usb::usb::endpoint_mode mode) {
            // check if we are stalled
            if (!is_stalled(endpoint, mode)) {
                // we are not stalled return
                return;
            }

            write_command(command_phase::command, 
                static_cast<uint8_t>(endpoint_command::set_status) | (endpoint << 1 | endpoint_mode_to_raw(mode)), 
                0x0
            );

            // get the callback
            const auto callback = state[endpoint].callback;

            // clear the state of the endpoint
            clear_endpoint_state(endpoint);

            if (callback) {
                // send a error to the callback
                callback(endpoint, mode, klib::usb::usb::error::un_stall);
            }
        }

        /**
         * @brief returns if a endpoint is stalled
         * 
         * @param endpoint 
         * @param mode
         * @return true 
         * @return false 
         */
        static bool is_stalled(const uint8_t endpoint, const klib::usb::usb::endpoint_mode mode) {
            // return the stalled endpoint flag
            return read_ep_command(endpoint, endpoint_mode_to_raw(mode)) & (0x1 << 1);
        }

        /**
         * @brief Write data to an endpoint.
         * 
         * @warning Buffers should be valid until the callback function is called
         * 
         * @param callback 
         * @param endpoint 
         * @param mode
         * @param data
         * @return true 
         * @return false 
         */
        static bool write(const klib::usb::usb::usb_callback callback, const uint8_t endpoint, 
                          const klib::usb::usb::endpoint_mode mode, const std::span<const uint8_t>& data) 
        {
            
            // get the size we can write in a single transmission
            const uint32_t s = klib::min(data.size(), state[endpoint].max_size);

            // set the endpoint callback and mark the endpoint as busy
            state[endpoint].is_busy = true;
            state[endpoint].callback = callback;

            // we remove the const here as we know we dont write to it.
            // the state cannot have the const as the read does write to 
            // the array
            state[endpoint].data = const_cast<uint8_t*>(data.data());
            
            // set the endpoint data
            state[endpoint].requested_size = data.size();
            state[endpoint].transferred_size = s;

            // call the write implementation
            write_impl(endpoint, mode, data.data(), s);

            // notify everything is correct
            return true;
        }

        /**
         * @brief Read data from a endpoint. Data is only valid when the callback is called.
         * 
         * @warning Buffers should be valid until the callback function is called
         * 
         * @param callback 
         * @param endpoint 
         * @param mode
         * @param data 
         * @return true 
         * @return false 
         */
        static bool read(const klib::usb::usb::usb_callback callback, const uint8_t endpoint, 
                         const klib::usb::usb::endpoint_mode mode, const std::span<uint8_t>& data)
        {
            // call read with a fixed size
            return read(callback, endpoint, mode, data.data(), data.size(), data.size());
        }

        /**
         * @brief Read data from a endpoint. Data is only valid when the callback is called. Has a 
         * min size and max size for dynamic data length. The difference between min and max size is
         * determined by the endpoint size. For example if the endpoint size is 8 bytes, min size is 
         * 2 bytes, max is 32 bytes the maximum amount of data that will be received is 8 bytes.
         * 
         * @warning Buffers should be valid until the callback function is called
         * 
         * @param callback 
         * @param endpoint 
         * @param mode 
         * @param data 
         * @param min_size 
         * @param max_size 
         * @return true 
         * @return false 
         */
        static bool read(const klib::usb::usb::usb_callback callback, const uint8_t endpoint, 
                         const klib::usb::usb::endpoint_mode mode, uint8_t *const data, 
                         const uint32_t min_size, const uint32_t max_size) 
        {
            // set the endpoint callback
            state[endpoint].callback = callback;
            state[endpoint].data = data;
            
            // set the endpoint data
            state[endpoint].requested_size = min_size;
            state[endpoint].max_requested_size = max_size;
            state[endpoint].transferred_size = 0;

            // mark the endpoint as busy
            state[endpoint].is_busy = true;
            
            // check if we already received data while we were not reading
            if (state[endpoint].interrupt_pending) {
                // clear the pending interrupt flag
                state[endpoint].interrupt_pending = false;

                // process the data we already received by forcing a interrupt
                // on the endpoint we are reading
                Usb::port->EPINTSET = (0x1 << (endpoint << 1) | endpoint_mode_to_raw(mode));
            }

            // notify everything is correct
            return true;
        }

        /**
         * @brief Returns if a endpoint has a pending transmission
         * 
         * @param endpoint 
         * @param mode
         * @return true 
         * @return false 
         */
        static bool is_pending(const uint8_t endpoint, const klib::usb::usb::endpoint_mode mode) {
            return state[endpoint].is_busy;
        }

        /**
         * @brief Cancel a pending transaction
         * 
         * @param endpoint 
         * @param mode 
         */
        static void cancel(const uint8_t endpoint, const klib::usb::usb::endpoint_mode mode) {
            // get the callback
            const auto callback = state[endpoint].callback;

            // clear the state of the endpoint
            clear_endpoint_state(endpoint);

            // check if the callback is valid
            if (callback) {
                // call the callback
                callback(endpoint, mode, klib::usb::usb::error::cancel);
            }
        }
    };
}

#endif
