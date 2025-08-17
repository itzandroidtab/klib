#ifndef KLIB_ATMEL_ATSAM4S_USB_HPP
#define KLIB_ATMEL_ATSAM4S_USB_HPP

#include <span>

#include <klib/klib.hpp>
#include <klib/usb/usb/usb.hpp>
#include <klib/usb/usb/size.hpp>
#include <klib/math.hpp>

#include <io/power.hpp>
#include <io/port.hpp>
#include <io/clocks.hpp>

namespace klib::core::atsam4s::io::detail::usb {
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
        uint16_t max_size;

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

namespace klib::core::atsam4s::io {
    template <typename Usb, typename Device>
    class usb {
    public:
        // amount of endpoints supported by the atsam4s
        constexpr static uint8_t endpoint_count = 8;

        // maximum endpoint sizes
        constexpr static klib::usb::endpoint_size_endpoint<endpoint_count, 
            64, 64, 64, 64, 512, 512, 64, 64
        > max_endpoint_size = {};

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
        // struct with information about the state of a endpoint
        static inline volatile detail::usb::state state[endpoint_count] = {};

        // all the iso endpoints
        constexpr static uint32_t iso_endpoints = 0b11110110;

        // address to change to if not zero.
        static inline volatile uint8_t device_address = 0;

        /**
         * @brief Endpoint modes the hardware supports
         *
         */
        enum class endpoint_mode {
            control = 0x0,
            iso_out = 0x1,
            bulk_out = 0x2,
            int_out = 0x3,
            iso_in = 0x5,
            bulk_in = 0x6,
            int_in = 0x7,
        };

        /**
         * @brief Convert a mode to the correct raw value for the usb hardware
         *
         * @param mode
         * @return uint8_t
         */
        constexpr static uint8_t transfer_type_to_raw(const klib::usb::usb::endpoint_mode mode,
            const klib::usb::descriptor::transfer_type type)
        {
            // special case for control endpoints
            if (mode == klib::usb::usb::endpoint_mode::control) {
                return 0;
            }

            // bit 3 sets if we have a in or out.
            return static_cast<uint8_t>(type) | ((mode != klib::usb::usb::endpoint_mode::out) << 2);
        }

        constexpr static klib::usb::usb::endpoint_mode raw_to_endpoint_mode(const endpoint_mode mode) {
            switch (mode) {
                case endpoint_mode::control:
                    return klib::usb::usb::endpoint_mode::control;
                case endpoint_mode::bulk_out:
                case endpoint_mode::iso_out:
                case endpoint_mode::int_out:
                    return klib::usb::usb::endpoint_mode::out;
                case endpoint_mode::bulk_in:
                case endpoint_mode::iso_in:
                case endpoint_mode::int_in:
                default:
                    return klib::usb::usb::endpoint_mode::in;
            }
        }


        /**
         * @brief Reset the bus
         *
         */
        static void reset() {
            // clear the device address
            set_device_address_impl(0x00);

            // configure endpoint 0 so we can receive
            // a setup packet (also enable it)
            Usb::port->CSR[klib::usb::usb::control_endpoint] = (
                (0x1 << 15) | (transfer_type_to_raw(
                    klib::usb::usb::endpoint_mode::control,
                    klib::usb::descriptor::transfer_type::control
                ) << 8)
            );

            // enable all the enpoint interrupts
            Usb::port->IER = 0xff;

            if constexpr (has_bus_reset_callback) {
                // call the device bus reset
                device::template bus_reset<usb_type>();
            }
        }

        static void write_impl(const uint8_t endpoint, const klib::usb::usb::endpoint_mode mode,
                               const uint8_t* data, const uint32_t size)
        {
            // convert the current mode to a klib endpoint mode
            const auto m = static_cast<endpoint_mode>((Usb::port->CSR[endpoint] >> 8) & 0x7);

            // set the endpoint direction for control endpoints
            if (raw_to_endpoint_mode(m) == klib::usb::usb::endpoint_mode::control) {
                // enable the DATA in for the control endpoint
                Usb::port->CSR[endpoint] |= (0x1 << 7);
            }

            // write the data
            for (uint32_t i = 0; i < size; i++) {
                // write 4 bytes at the time
                Usb::port->FDR[endpoint] = data[i];
            }

            // mark we have a transfer
            Usb::port->CSR[endpoint] = (Usb::port->CSR[endpoint] | (0x1 << 4));
        }

        static uint32_t read_impl(const uint8_t endpoint, const klib::usb::usb::endpoint_mode mode,
                                  uint8_t* data, const uint32_t size)
        {
            // get the amount of bytes in the endpoint
            const uint32_t count = (Usb::port->CSR[endpoint] >> 16) & 0xff;

            // read all the available data
            for (uint32_t i = 0; (i < count) && (i < size); i++) {
                // read into the buffer
                data[i] = Usb::port->FDR[endpoint];
            }

            return count;
        }

        static void clear_endpoint_state(const uint8_t endpoint) {
            state[endpoint].is_busy = false;
            state[endpoint].requested_size = 0;
            state[endpoint].transferred_size = 0;
            state[endpoint].callback = nullptr;
            state[endpoint].data = nullptr;
        }

        static void setup_packet(const uint8_t endpoint, const endpoint_mode mode, const uint8_t bytes) {
            // make sure we have the correct amount of bytes
            if (bytes != sizeof(klib::usb::setup_packet)) {
                // clear the setup packet flag
                Usb::port->CSR[endpoint] &= ~(0x1 << 2);

                // stall the current endpoint
                stall(endpoint, raw_to_endpoint_mode(mode));

                // exit
                return;
            }

            // struct to store the setup packet in
            klib::usb::setup_packet packet;

            // read the setup packet
            read_impl(
                endpoint, raw_to_endpoint_mode(mode),
                reinterpret_cast<uint8_t*>(&packet), sizeof(packet)
            );

            // clear the setup packet flag
            Usb::port->CSR[endpoint] &= ~(0x1 << 2);

            // clear the pending flag on the control endpoint when
            // we receive a setup packet. This is to prevent us from
            // reading the setup packet again
            state[endpoint].interrupt_pending = false;

            // handle the setup packet in the klib library
            klib::usb::usb::handle_setup_packet<usb_type>(packet);
        }

        static void endpoint_in_callback(const uint8_t endpoint) {
            // check if we need to update the device address. This cannot be
            // done in the set_device_address callback so we do it here. This
            // gets called after we have send the ack for the set device
            // address call
            if (device_address) {
                // change the device address
                set_device_address_impl(device_address);

                // clear the device address value to prevent it from updating
                // again
                device_address = 0;
            }

            // check if we are busy transmitting data
            if (!state[endpoint].is_busy) {
                return;
            }

            // check if we are done
            if (state[endpoint].transferred_size >= state[endpoint].requested_size) {
                // we are done. clear the flag and call the callback
                const auto callback = state[endpoint].callback;

                // get the amount of data we have transferred
                const auto transferred = state[endpoint].transferred_size;

                // clear the state
                clear_endpoint_state(endpoint);

                // check for any callbacks
                if (callback) {
                    // call the callback
                    callback(
                        endpoint, klib::usb::usb::endpoint_mode::in,
                        klib::usb::usb::error::no_error, transferred
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

        template <bool Isochronous>
        static void endpoint_out_callback(const uint8_t endpoint) {
            // only check if we are busy if we are not a iso endpoint
            if constexpr (!Isochronous) {
                // check if we are busy.
                if (!state[endpoint].is_busy) {
                    // set the flag we have a out interrupt pending
                    state[endpoint].interrupt_pending = true;

                    // disable the endpoint interrupt to prevent
                    // continuous triggers on the endpoint
                    Usb::port->IDR = (0x1 << endpoint);

                    // do not read anything
                    return;
                }
            }

            // check both banks if the endpoint supports dual banks
            for (uint32_t i = 0; i < get_endpoint_bank_count(endpoint); i++) {
                // check if the current bank has data
                if (!(Usb::port->CSR[endpoint] & (0x1 << (1 + (i * 5))))) {
                    continue;
                }

                // get the offset we are into the transfer
                const auto offset = state[endpoint].transferred_size;

                // receive more data
                state[endpoint].transferred_size = (offset + read_impl(
                    endpoint, klib::usb::usb::endpoint_mode::out,
                    (state[endpoint].data + offset),
                    state[endpoint].max_requested_size - offset
                ));

                // clear the flag to notify we have read the data
                Usb::port->CSR[endpoint] = Usb::port->CSR[endpoint] & ~(0x1 << (1 + (i * 5)));

                // check if we are done
                if (state[endpoint].transferred_size >= state[endpoint].requested_size) {
                    // we are done. clear the flag and call the callback
                    const auto callback = state[endpoint].callback;

                    // get the amount of data we have transferred
                    const auto transferred = state[endpoint].transferred_size;

                    // clear the state
                    clear_endpoint_state(endpoint);

                    // check for any callbacks
                    if (callback) {
                        // call the callback
                        callback(
                            endpoint, klib::usb::usb::endpoint_mode::out,
                            klib::usb::usb::error::no_error, transferred
                        );
                    }
                }
            }
        }

        static void data_irq_handler(uint32_t endpoints) {
            // amount of trailing zeros in the status register
            uint8_t trailing_zeros = 0;

            // check what endpoint has triggered the interrupt (32 == zero's)
            while ((trailing_zeros = klib::ctz(endpoints)) < 32) {
                // get the current endpoint
                const uint8_t endpoint = trailing_zeros;

                // clear the bit from the masked_status interrupts
                endpoints &= ~(0x1 << endpoint);

                // check what kind of packet we have
                const uint32_t value = Usb::port->CSR[endpoint];

                // get the amount of data we have in the fifo
                const uint32_t count = (value >> 16) & 0x7ff;

                // get the endpoint mode
                const auto mode = static_cast<endpoint_mode>((value >> 8) & 0x7);

                // check for a control endpoint
                if ((mode == endpoint_mode::control) && (value & (0x1 << 2))) {
                    // we have a setup packet handle it
                    setup_packet(endpoint, mode, count);
                }
                else if (value & 0x1) {
                    // clear the flag we have transmitted a packet
                    Usb::port->CSR[endpoint] &= ~(0x1);

                    // call the in callback
                    endpoint_in_callback(endpoint);
                }
                else if (value & ((0x1 << 1) | (0x1 << 6))) {
                    if ((mode == endpoint_mode::control) && ((value >> 7) & 0x1)) {
                        // do nothing if we are here in a control + in direction
                        Usb::port->CSR[endpoint] = Usb::port->CSR[endpoint] & ~(0x1 << 1);
                    }
                    else {
                        // call the out callback
                        endpoint_out_callback<false>(endpoint);
                    }
                }
                else if (value & (0x1 << 3)) {
                    // for isochronous it is a crc error flag. For the others it
                    // is the stall send flag. Clear it anyway
                    Usb::port->CSR[endpoint] &= ~(0x1 << 3);
                }
            }
        }

        static void set_device_address_impl(const uint8_t address) {
            // set the device address
            if (address) {
                // set the address flag if we have a non zero address
                Usb::port->GLB_STAT = (Usb::port->GLB_STAT & ~(0x1 << 1)) | 0x1;
            }
            else {
                // clear the addressed state
                Usb::port->GLB_STAT = (Usb::port->GLB_STAT & ~(0x3));
            }

            // set the received address
            Usb::port->FADDR = (address & 0x7f) | (0x1 << 8);
        }

        static void reset_impl(const uint8_t endpoint, const klib::usb::usb::endpoint_mode mode) {
            // get the endpoint mask
            const uint8_t ep = (0x1 << endpoint);

            // reset the endpoint
            Usb::port->RST_EP |= ep;

            // wait until the bit is set
            while (!(Usb::port->RST_EP & ep)) {
                // do nothing
            }

            // clear the endpoint reset bit
            Usb::port->RST_EP &= ~ep;
        }

        constexpr static uint8_t get_endpoint_bank_count(const uint8_t endpoint) {
            // return the amount of banks for the endpoint
            if ((endpoint == 0) || (endpoint == 3)) {
                return 1;
            }
            else {
                return 2;
            }
        }

        static void isochronous_irq_handler() {
            // get all the isochronous endpoints
            uint32_t endpoints = iso_endpoints;

            // amount of trailing zeros in the status register
            uint8_t trailing_zeros = 0;

            // we have a frame interrupt. Mark all the isochronous
            // endpoints as not busy
            while ((trailing_zeros = klib::ctz(endpoints)) < 32) {
                // get the current endpoint
                const uint8_t endpoint = trailing_zeros;

                // clear the bit from the endpoints
                endpoints &= ~(0x1 << endpoint);

                // check if we are busy transmitting or receiving data
                if (!state[endpoint].is_busy) {
                    continue;
                }

                // check what kind of packet we have
                const uint32_t value = Usb::port->CSR[endpoint];

                // check if the endpoint is enabled
                if (!(value & (0x1 << 15))) {
                    continue;
                }

                // get the endpoint mode
                const auto mode = static_cast<endpoint_mode>((value >> 8) & 0x7);

                // check if the endpoint is isochronous
                if (mode != endpoint_mode::iso_in && mode != endpoint_mode::iso_out) {
                    continue;
                }

                // check how we need to handle the endpoint
                if (raw_to_endpoint_mode(mode) == klib::usb::usb::endpoint_mode::out) {
                    endpoint_out_callback<true>(endpoint);
                }
                else {
                    endpoint_in_callback(endpoint);
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
            const uint32_t status = Usb::port->ISR;
            const uint32_t mask = Usb::port->IMR;

            // create the masked status
            const uint32_t masked_status = status & mask;

            // clear the interrupt status so we dont miss any
            // interrupts while the user code is running
            Usb::port->ICR = masked_status;

            // check what kind of interrupt we have
            if (masked_status & (0x1 << 12)) {
                // reset event. Reset the usb driver
                reset();

                // call all the callbacks with a error
                for (uint32_t i = 0; i < endpoint_count; i++) {
                    const auto callback = state[i].callback;

                    // clear the state
                    clear_endpoint_state(i);

                    if (callback) {
                        // call with a reset. Endpoint mode as in but we do not know what it is
                        callback(i, klib::usb::usb::endpoint_mode::in, klib::usb::usb::error::reset, 0);
                    }
                }

                // if we have a reset do not handle anything else
                return;
            }

            // check if we have any endpoint interrupts
            if (masked_status & 0xff) {
                // handle the data irq
                data_irq_handler(masked_status & 0xff);
            }

            if (masked_status & (0x1 << 8)) {
                if constexpr (has_sleep_callback) {
                    // call the device sleep function
                    device::template sleep<usb_type>();
                }

                // enable the wakeup and resume interrupts 
                Usb::port->IER = ((0x1 << 13) | (0x1 << 9));

                // disable the suspend interrupt
                Usb::port->IDR = (0x1 << 8);
            }

            if (masked_status & ((0x1 << 13) | (0x1 << 9))) {
                if constexpr (has_wakeup_callback) {
                    // call the device wakeup function
                    device::template wakeup<usb_type>();
                }

                // enable the suspend interrupt
                Usb::port->IER = (0x1 << 8);

                // disable the wakeup and resume interrupts
                Usb::port->IDR = ((0x1 << 13) | (0x1 << 9));
            }

            // check for a frame interrupt
            if (masked_status & (0x1 << 11)) {
                // we have a frame interrupt. Handle it in the iso irq
                isochronous_irq_handler();
            }
        }

    public:
        /**
         * @brief Initialize the usb hardware. This requires the usb pll to be enabled beforehand using
         *
         * target::io::system::clock::set_usb<external_crystal_frequency>();
         *
         * @tparam UsbConnect
         * @tparam NakIrq
         */
        template <bool UsbConnect = true, bool NakIrq = false>
        static void init() {
            // enable the clock to the usb peripheral
            target::io::clocks::enable<target::io::clocks::id::udp>();

            // enable the usb power
            target::io::power_control::enable<Usb>();

            // clear the d-plus and d-minus bits in the matrix
            // to setup the io
            MATRIX->CCFG_SYSIO = (MATRIX->CCFG_SYSIO & ~(0x3 << 10));

            // reset all the info stored about the endpoints
            for (uint32_t i = 0; i < endpoint_count; i++) {
                // set the endpoint to a known state
                state[i].is_busy = false;
                state[i].max_size = static_cast<uint8_t>(
                    (i == 0) ? max_endpoint_size.size(0, klib::usb::descriptor::transfer_type::control) : 0
                );
                state[i].data = nullptr;
                state[i].requested_size = 0;
                state[i].transferred_size = 0;
                state[i].callback = nullptr;
            }

            // disable all interrupts (except the
            // busreset interrupt we cannot turn it off)
            Usb::port->IDR = 0xffffffff;
            Usb::port->ICR = 0xffffffff;

            // register ourselfs with the interrupt controller
            target::irq::register_irq<Usb::interrupt_id>(irq_handler);

            // enable the usb interrupt
            target::enable_irq<Usb::interrupt_id>();

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
        constexpr static bool is_valid_endpoint() {
            using transfer_type = klib::usb::descriptor::transfer_type;

            // make sure the endpoint is valid
            static_assert(endpoint <= 8, "Invalid endpoint provided");

            // check if the endpoint supports the transfer type
            if constexpr (type == transfer_type::control) {
                return (endpoint == 0) || (endpoint == 3);
            }
            else if constexpr (type == transfer_type::isochronous) {
                return ((0x1 << endpoint) & iso_endpoints);
            }

            // every endpoint supports bulk and interrupt. Dont
            // bother checking them
            return true;
        }

        /**
         * @brief Function that gets called to notify the driver
         * the devices is configured
         *
         * @param cfg
         */
        static void configured(const bool cfg) {
            // check if we need to set or clear the configured flag
            if (cfg) {
                // set the configured flag
                Usb::port->GLB_STAT |= (0x1 << 1);
            }
            else {
                // clear the configured flag
                Usb::port->GLB_STAT &= ~(0x1 << 1);
            }
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
            // set the new endpoint size
            state[endpoint].max_size = klib::min(size, max_endpoint_size.size(endpoint, type));

            // reset the endpoint
            reset(endpoint, mode);

            // configure the selected endpoint
            Usb::port->CSR[endpoint] = (
                (0x1 << 15) | (transfer_type_to_raw(mode, type) << 8)
            );

            // check if we need to enable the iso interrupt
            if (type == klib::usb::descriptor::transfer_type::isochronous) {
                Usb::port->IER = (0x1 << 11);
            }
        }

        /**
         * @brief Set the device address. Handled in hardware
         *
         * @param address
         */
        static klib::usb::usb::handshake set_device_address(const uint8_t address) {
            // store the device address for later. We need to change it
            // after the handshake.
            device_address = address;

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
            // call the reset implementation
            reset_impl(endpoint, mode);

            // get the callback
            const auto callback = state[endpoint].callback;

            // get the amount of data we have transferred
            const auto transferred = state[endpoint].transferred_size;

            // clear the state of the endpoint
            clear_endpoint_state(endpoint);

            if (callback) {
                // send a error to the callback. Do not care about the
                // return value as we are always clearing the endpoint
                // in a reset event
                callback(endpoint, mode, klib::usb::usb::error::reset, transferred);
            }
        }

        /**
         * @brief Connect the device to the host by enabling the
         * pullup
         *
         */
        static void connect() {
            // enable the 1.5k pullup
            Usb::port->TXVC = (0x1 << 9);
        }

        /**
         * @brief Disconnect the device from the host by disabling
         * the pullup
         *
         */
        static void disconnect() {
            // disconnect the pullup to disconnect from the host
            Usb::port->TXVC = 0x00;
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
                // mark we have a transfer
                Usb::port->CSR[endpoint] = (Usb::port->CSR[endpoint] | (0x1 << 4));
            }
            else {
                // clear the flag to notify we have read the data
                Usb::port->CSR[endpoint] = Usb::port->CSR[endpoint] & ~(0x1 << 1);
            }
        }

        /**
         * @brief Stall a endpoint
         *
         * @param endpoint
         * @param mode
         */
        static void stall(const uint8_t endpoint, const klib::usb::usb::endpoint_mode mode) {
            // set the force stall flag for the endpoint
            Usb::port->CSR[endpoint] |= (0x1 << 5);

            // get the callback
            const auto callback = state[endpoint].callback;

            // get the amount of data we have transferred
            const auto transferred = state[endpoint].transferred_size;

            // clear the state of the endpoint
            clear_endpoint_state(endpoint);

            if (callback) {
                // send a error to the callback
                callback(endpoint, mode, klib::usb::usb::error::stall, transferred);
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

            // clear the stall and the stall send bit
            Usb::port->CSR[endpoint] &= ~((0x1 << 5) | (0x1 << 3));

            // reset the endpoint
            reset_impl(endpoint, mode);

            // get the callback
            const auto callback = state[endpoint].callback;

            // get the amount of data we have transferred
            const auto transferred = state[endpoint].transferred_size;

            // clear the state of the endpoint
            clear_endpoint_state(endpoint);

            if (callback) {
                // send a error to the callback
                callback(endpoint, mode, klib::usb::usb::error::un_stall, transferred);
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
            return Usb::port->CSR[endpoint] & ((0x1 << 5) | (0x1 << 3));;
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
                Usb::port->IER = (0x1 << endpoint);
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
            // disable the endpoint interrupt while we modify the fifo
            const bool irq_enabled = Usb::port->IMR & (0x1 << endpoint);

            if (irq_enabled) {
                Usb::port->IDR = (0x1 << endpoint);
            }

            // check for a in or out
            if (mode == klib::usb::usb::endpoint_mode::out) {
                // clear both banks
                Usb::port->CSR[endpoint] &= ~((0x1 << 1) | (0x1 << 6));
            }
            else {
                // check if we have a packet ready
                if (Usb::port->CSR[endpoint] & (0x1 << 4)) {
                    if (get_endpoint_bank_count(endpoint) > 1) {
                        // clear the packet ready flag for the other bank
                        Usb::port->CSR[endpoint] &= ~(0x1 << 4);

                        // wait until it is cleared
                        while (Usb::port->CSR[endpoint] & (0x1 << 4)) {
                            // do nothing
                        }

                        // set the packet ready flag for the other bank
                        Usb::port->CSR[endpoint] &= ~(0x1 << 4);

                        // wait until it is set
                        while (!Usb::port->CSR[endpoint] & (0x1 << 4)) {
                            // do nothing
                        }
                    }

                    // clear the packet ready flag
                    Usb::port->CSR[endpoint] &= ~(0x1 << 4);
                }

                // clear the flag we have transmitted a packet to
                // prevent us from entering the entry in the
                // interrupt
                Usb::port->CSR[endpoint] &= ~(0x1);
            }

            // enable the interrupts again if they were on
            if (irq_enabled) {
                Usb::port->IER = (0x1 << endpoint);
            }

            // reset the endpoint
            reset_impl(endpoint, mode);

            // get the callback
            const auto callback = state[endpoint].callback;

            // get the amount of data we have transferred
            const auto transferred = state[endpoint].transferred_size;

            // clear the state of the endpoint
            clear_endpoint_state(endpoint);

            // check if the callback is valid
            if (callback) {
                // call the callback
                callback(endpoint, mode, klib::usb::usb::error::cancel, transferred);
            }
        }
    };
}

#endif
