#ifndef KLIB_CYPRESS_MB9BF560L_USB_HPP
#define KLIB_CYPRESS_MB9BF560L_USB_HPP

#include <span>

#include <klib/klib.hpp>
#include <klib/usb/usb/usb.hpp>
#include <klib/math.hpp>
#include <klib/irq_helper.hpp>
#include <klib/usb/usb/setup.hpp>

#include <io/port.hpp>

namespace klib::core::mb9bf560l::io::detail::usb {
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

namespace klib::core::mb9bf560l::io {
    template <typename Usb, typename Device>
    class usb {
    public:
        // amount of endpoints supported by the lpc1756
        constexpr static uint8_t endpoint_count = 6;

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
        // struct with information about the state of a endpoint
        static inline volatile detail::usb::state state[endpoint_count] = {};

        // helper for 6 interrupt bits
        using irq_helper = klib::irq_helper<6>;

        // create a instance of the helper
        static inline auto helper = irq_helper();

        /**
         * @brief Convert a mode to the correct raw value for the usb hardware
         *
         * @param mode
         * @return uint8_t
         */
        constexpr static uint8_t transfer_type_to_raw(const klib::usb::descriptor::transfer_type type) {
            // convert the type to raw data
            switch (type) {
                case klib::usb::descriptor::transfer_type::isochronous:
                    return 0b01;
                case klib::usb::descriptor::transfer_type::bulk:
                    return 0b10;
                case klib::usb::descriptor::transfer_type::interrupt:
                    return 0b11;
                default:
                    return 0b00;
            }
        }

        /**
         * @brief Convert a mode to the correct raw value for the usb hardware
         *
         * @param mode
         * @return uint8_t
         */
        constexpr static bool endpoint_mode_to_raw(const klib::usb::usb::endpoint_mode mode) {
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

        static void clear_endpoint_state(const uint8_t endpoint) {
            state[endpoint].is_busy = false;
            state[endpoint].requested_size = 0;
            state[endpoint].transferred_size = 0;
            state[endpoint].callback = nullptr;
            state[endpoint].data = nullptr;
        }

        /**
         * @brief Reset the bus
         *
         */
        static void reset() {
            // reset the usb 
            Usb::port->UDCC |= (0x1 << 7);

            // change the usb to device mode and reset usb host
            Usb::port->HCNT = 0x1 << 1;

            // start the usb clock, clear the resume flag and 
            // clear the data toggle flag
            Usb::port->UDCC &= ~((0x1 << 6) | (0x1 << 4) | (0x1 << 1));

            // change the power mode to self powered
            Usb::port->UDCC |= 0x1;

            // configure enpoint 0 endpoint size.
            Usb::port->EP0C = max_endpoint_size;

            // clear the reset flag
            Usb::port->UDCC &= ~(0x1 << 7);

            // enable all the interrupts except the sof and setup
            Usb::port->UDCIE = 0b101101;

            // clear all the endpoint buffers (+ 1 as ep 0 has a seperate in and out register)
            for (uint32_t endpoint = 0; endpoint < (endpoint_count + 1); endpoint++) {
                // get the enpoint status register
                volatile uint16_t *const ep = &(&Usb::port->EP0IS)[endpoint * 2];

                // clear the buffer for the endpoint
                (*ep) &= (~(0x1 << 15));
            }

            // enable the packet transfer interrupt for endpoint 0
            (*get_endpoint_status(0, klib::usb::usb::endpoint_mode::out)) |= (0x1 << 14);

            // disable the endpoint 0 in interrupt. We turn this on when needed
            (*get_endpoint_status(0, klib::usb::usb::endpoint_mode::in)) &= ~(0x1 << 14);

            // check if we need to call the reset callback
            if constexpr (has_bus_reset_callback) {
                // call the device bus reset
                device::template bus_reset<usb_type>();
            }
        }

        static void write_impl(const uint8_t endpoint, const klib::usb::usb::endpoint_mode mode,
                               const uint8_t* data, const uint32_t size)
        {
            // get the endpoint status register
            volatile uint16_t* ep_status = get_endpoint_status(endpoint, mode);

            // enable the drqi interrupt
            (*ep_status) |= (0x1 << 14);

            // wait until the buffer is empty by checking the drqi flag
            while (!((*ep_status) & (0x1 << 10))) {
                // do nothing
            }

            // get the endpoint data register
            volatile uint16_t* ep_data = &(&Usb::port->EP0DT)[endpoint * 2];

            // write the data in pairs
            for (uint32_t i = 0; i < (size & (~0x1)); i += 2) {
                // write 2 bytes at the time
                (*ep_data) = *reinterpret_cast<const uint16_t*>(&data[i]);
            }

            // write the remainder of the data
            if (size & 0x1) {
                // write using a single byte write
                (*((volatile uint8_t*)ep_data)) = data[size - 1];
            }

            // set the flag we have data to send
            (*ep_status) &= (~(0x1 << 10));
        }

        static uint32_t get_endpoint_byte_count(const uint8_t endpoint, const klib::usb::usb::endpoint_mode mode) {
            // get the endpoint status register
            volatile uint16_t* ep_status = get_endpoint_status(endpoint, mode);

            // get the amount of bytes we have in memory from the status register
            const uint16_t status = (*ep_status) & 0x7f;

            // get the byte count from the status
            return status & (endpoint == 1 ? 0x1ff : 0x7f);
        }

        static uint32_t read_impl(const uint8_t endpoint, const klib::usb::usb::endpoint_mode mode,
                                  uint8_t* data, const uint32_t size)
        {
            // get the byte count from the status
            const uint32_t count = get_endpoint_byte_count(endpoint, mode);
            
            // get the register where to read from
            volatile uint16_t* ep = &(&Usb::port->EP0DT)[endpoint * 2];

            // check if we have received a zlp (zero length packet)
            if (count) [[likely]] {
                // read the data in pairs
                for (uint32_t i = 0; i < (count & (~0x1)); i += 2) {
                    // read the data 2 bytes at the time
                    *reinterpret_cast<uint16_t*>(&data[i]) = (*ep);
                }

                // read the remainder of the data
                if (count & 0x1) {
                    // write using a single byte write
                    data[size - 1] = *reinterpret_cast<volatile uint8_t*>(ep);
                }
            }
            else {
                // received a zlp. Do a dummy read on the data
                (void)(*ep);
            }

            volatile uint16_t* ep_status = get_endpoint_status(endpoint, mode);
         
            // clear the endpoint received data flag on the data we just read
            (*ep_status) &= ~(0x1 << 10);

            // return the amount of data we have read
            return count;
        }

        static void suspend_irq() {
            // check if we have a handler
            if constexpr (has_sleep_callback) {
                // call the device sleep function
                device::template sleep<usb_type>();
            }
        }

        static void sof_irq() {
            // TODO: do something here
        }

        static void busreset_irq() {
            // reset every endpoint. Call all the enabled callbacks with a error
            for (uint32_t endpoint = 0; endpoint < endpoint_count; endpoint++) {
                // get the endpoint control register
                volatile uint16_t *const ep_control = &(&Usb::port->EP0C)[endpoint * 2];

                // check if the endpoint is enabled
                if (!((*ep_control) & (0x1 << 15))) [[likely]] {
                    continue;
                }

                // check if for endpoint 0. Special case for ep 0
                if (endpoint == 0) [[unlikely]] {
                    // clear both the in and out buffers
                    (*get_endpoint_status(0, klib::usb::usb::endpoint_mode::out)) |= (0x1 << 15);
                    (*get_endpoint_status(0, klib::usb::usb::endpoint_mode::out)) &= ~(0x1 << 15);
                    (*get_endpoint_status(0, klib::usb::usb::endpoint_mode::in)) |= (0x1 << 15);
                    (*get_endpoint_status(0, klib::usb::usb::endpoint_mode::in)) &= ~(0x1 << 15);

                    // setup the interrupts for endpoint 0, enable the packet transfer interrupt for endpoint 0
                    (*get_endpoint_status(0, klib::usb::usb::endpoint_mode::out)) |= (0x1 << 14);

                    // disable the endpoint 0 in interrupt. We turn this on when needed
                    (*get_endpoint_status(0, klib::usb::usb::endpoint_mode::in)) &= ~(0x1 << 14);
                }
                else {
                    // get the endpoint status register (for all the 
                    // endpoints except endpoint 0 we dont need the
                    // endpoint mode. No need to add instructions to
                    // check for it)
                    volatile uint16_t* const ep = get_endpoint_status(
                        endpoint, klib::usb::usb::endpoint_mode::out
                    );

                    // clear the buffer for the endpoint
                    (*ep) |= (0x1 << 15);
                    (*ep) &= (~(0x1 << 15));
                }

                // get the callback
                const auto callback = state[endpoint].callback;

                // get the amount of data we have transferred
                const auto transferred = state[endpoint].transferred_size;

                // clear the state
                clear_endpoint_state(endpoint);

                if (callback) [[likely]] {
                    // call with a reset. Endpoint mode as in but we do not know what it is
                    callback(endpoint, klib::usb::usb::endpoint_mode::in, klib::usb::usb::error::reset, transferred);
                }
            }
        }

        static void wakeup_irq() {
            // check if we have a handler
            if constexpr (has_wakeup_callback) {
                // call the device wakeup function
                device::template wakeup<usb_type>();
            }
        }

        static void setup_packet_irq() {
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

            // handle a bug where the hardware does not ack a
            // set idle request. Get the request type from the 
            // packet
            const auto type = static_cast<klib::usb::setup::request_type>((packet.bmRequestType >> 5) & 0x3);

            // check if we have a uclass request
            if (type == klib::usb::setup::request_type::uclass) {
                if (packet.bRequest == 0x0a) {
                    // mark the set_idle as received
                    Usb::port->EP0IS &= (~(0x1 << 10));
                }
            }
        }

        static void configured_irq() {
            // check what configuration we have
            const uint8_t configured = Usb::port->UDCIE & (0x1 << 1);

            // check if we are configured
            if (configured) {
                // we need to do a little bit of a hack here. The 
                // USB device checks if we have a matching configuration.
                // As the hardware handles this we need to fetch the 
                // configuration from the device here and pass it to
                // itself
                const uint8_t config = device::template get_configuration<usb_type>();

                // fake the setup packet here for the device
                const klib::usb::setup_packet packet = {
                    .wValue = config
                };

                // call set config manually with our payload. This is
                // normally done by the klib usb library
                device::template set_config<usb_type>(packet);
            }
            else {
                // fake the setup packet here for the device
                const klib::usb::setup_packet packet = {
                    .wValue = 0x00
                };

                // call set config manually with our payload. This is
                // normally done by the klib usb library
                device::template set_config<usb_type>(packet);
            }
        }

        static volatile uint16_t *const get_endpoint_status(const uint8_t endpoint, const klib::usb::usb::endpoint_mode mode) {
            // check if we have endpoint 0. Special case here
            if (endpoint == 0) {
                // return either the in or out register
                if (mode == klib::usb::usb::endpoint_mode::out) {
                    return &Usb::port->EP0OS;
                }
                else {
                    return &Usb::port->EP0IS;
                }
            }
            else {
                // for the other ones we only have one 
                // register per endpoint.
                return &((&Usb::port->EP0OS)[endpoint * 2]);
            }
        }

        static void endpoint_in_callback(const uint8_t endpoint) {
            // check if we are busy transmitting data
            if (!state[endpoint].is_busy) [[unlikely]] {
                return;
            }

            // check if we are done
            if (state[endpoint].transferred_size >= state[endpoint].requested_size) {
                // we are done. clear the flag and call the callback
                const auto callback = state[endpoint].callback;

                // get the amount of data we have transferred
                const auto transferred = state[endpoint].transferred_size;

                // turn off the drqi interrupt
                (*get_endpoint_status(endpoint, klib::usb::usb::endpoint_mode::in)) &= ~(0x1 << 14);

                // clear the state
                clear_endpoint_state(endpoint);

                // check for any callbacks
                if (callback) [[likely]] {
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

        static void endpoint_out_callback(const uint8_t endpoint) {
            // check if we are busy.
            if (!state[endpoint].is_busy) {
                // set the flag we have a out interrupt pending
                state[endpoint].interrupt_pending = true;

                // disable the endpoint interrupt to prevent
                // continuous triggers on the endpoint
                (*get_endpoint_status(endpoint, klib::usb::usb::endpoint_mode::out)) &= ~(0x1 << 14);

                // do not read anything
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

                // get the amount of data we have transferred
                const auto transferred = state[endpoint].transferred_size;

                // clear the state
                clear_endpoint_state(endpoint);

                // check for any callbacks
                if (callback) [[likely]] {
                    // call the callback
                    callback(
                        endpoint, klib::usb::usb::endpoint_mode::out,
                        klib::usb::usb::error::no_error, transferred
                    );
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
        static void irq_ep0_handler() {
            // get the interrupt status 
            const uint8_t status = Usb::port->UDCS;

            // clear the interrupt status so we dont miss any
            // interrupts while the user code is running. We
            // clear a interrupt by setting it to zero. Invert 
            // the mask to get this result
            Usb::port->UDCS = (~(status));

            // check if endpoint 0 is stalled. Manually unstall it here
            if (is_stalled(0, klib::usb::usb::endpoint_mode::control)) [[unlikely]] {
                // unstall the endpoint
                un_stall(0, klib::usb::usb::endpoint_mode::control);
            }

            // check what kind of interrupt we have (in or out)
            if (Usb::port->EP0OS & (0x1 << 10)) [[likely]]  {
                // check if we have a setup packet
                if (status & (0x1 << 1)) [[likely]] {
                    // we have a setup packet
                    setup_packet_irq();
                }
                else {
                    // get the amount of bytes we received
                    const uint32_t count = get_endpoint_byte_count(0, klib::usb::usb::endpoint_mode::out);

                    // check if we have a zero length packet (zlp)
                    if (!count) {
                        // get the register where to read from
                        volatile uint16_t* ep = &Usb::port->EP0DT;

                        // do a read and discard the data
                        (void)(*ep);

                        // clear the endpoint interrupt
                        Usb::port->EP0OS &= (~(0x1 << 10));
                    }
                    else {
                        // no zlp. Process the endpoint normally
                        endpoint_out_callback(0);
                    }
                }
            }

            // check if we have a in interrupt
            if ((Usb::port->EP0IS & (0x1 << 10))) [[likely]] {                
                // we have a in interrupt.
                endpoint_in_callback(0);
            }

            // handle the other flags
            helper.handle_irq(status, 0xff);
        }
 
        static void irq_handler() {
            // check all the endpoints for data
            for (uint32_t ep = 1; ep < endpoint_count; ep++) {
                // get the endpoint control register
                volatile uint16_t *const ep_control = &(&Usb::port->EP0C)[ep * 2];

                // check if the endpoint is enabled
                if (!((*ep_control) & (0x1 << 15))) [[likely]] {
                    continue;
                }

                // check if we have a in or a out endpoint
                if ((((*ep_control) >> 12) & 0x1) == endpoint_mode_to_raw(klib::usb::usb::endpoint_mode::out)) {
                    // we have a out endpoint.
                    endpoint_out_callback(ep);
                }
                else {
                    // we have a in endpoint
                    endpoint_in_callback(ep);
                }
            }
        }

    public:
        /**
         * @brief Initialize the usb hardware. This requires the usb pll to be enabled beforehand using
         *
         * target::io::system::clock::set_usb<external_crystal_frequency, etc, etc, etc>();
         *
         * @tparam UsbConnect
         */
        template <bool UsbConnect = true>
        static void init() {            
            // we only support up to 2 channels. Make sure this is valid
            static_assert(Usb::id < 2, "Invalid USB id");

            // check what usb channel to enable
            if constexpr (Usb::id == 0) {
                // enable the usb clock
                USBCLK->USBEN0 = 0x1;
            }
            else {
                // enable the usb clock
                USBCLK->USBEN1 = 0x1;
            }

            // TODO: change this to a clock manager
            // enable power to the usb clock
            CLK_GATING->CKEN2 |= 0x1 << Usb::id;

            // setup the io 
            GPIO->SPSR = GPIO->SPSR | (0x1 << (Usb::id + 4));

            // check if we need to enable the USB connect pin
            if constexpr (UsbConnect) {
                // setup the usb connect pin
                target::io::detail::pins::set_peripheral<typename Usb::connect::pin, typename Usb::connect::periph>();
            }

            // disconnect from the host while we are configuring ourselfs
            usb::disconnect();

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

            // register the interrupt handlers
            helper.register_irq<0>(configured_irq);
            helper.register_irq<2>(wakeup_irq);
            helper.register_irq<3>(busreset_irq);
            helper.register_irq<4>(sof_irq);
            helper.register_irq<5>(suspend_irq);

            // do a partial reset
            reset();

            // register ourselfs with the interrupt controller
            // (both interrupts)
            target::irq::register_irq<Usb::interrupt_ep0_id>(irq_ep0_handler);
            target::irq::register_irq<Usb::interrupt_id>(irq_handler);

            // enable the usb interrupt (both interrupts)
            target::enable_irq<Usb::interrupt_ep0_id>();
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
            if (endpoint >= endpoint_count) {
                return false;
            }

            // this chip has some weird rules. We can enable a Bulk/Interrupt 
            // on every channel except endpoint 0. 
            // We can only enable a isochronous endpoint on endpoint 2 and 4.
            // If we enable isochronous mode on a endpoint the previous 
            // endpoint becomes unusable. And if we enable isochronous mode
            // on endpoint 4 endpoint 2 needs to be isochronous mode as well
            
            // check if the endpoint supports the transfer type
            if constexpr (type == transfer_type::control) {
                return (endpoint == 0);
            }
            else if constexpr (type == transfer_type::interrupt || type == transfer_type::bulk) {
                return (endpoint != 0);
            }
            else {
                return ((endpoint == 2) || (endpoint == 4));
            }
        }

        /**
         * @brief Function that gets called to notify the driver
         * the devices is configured
         *
         * @param cfg
         */
        static void configured(const bool cfg) {
            // Nothing here. We call the set_config in 
            // our interrupt. If we need to set something
            // we do it there.

            // This is done because the hardware handles most 
            // of the setup requests
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
            // get the register we should write to (skip every other half word)
            volatile uint16_t *const ep_control = &(&Usb::port->EP0C)[endpoint * 2];

            // limit the size based on the endpoint number
            const uint16_t s = size & (endpoint == 1 ? 0x1ff : 0x7f);

            // set the new endpoint size
            state[endpoint].max_size = s;

            // set the endpoint configuration
            (*ep_control) = (
                s | (endpoint_mode_to_raw(mode) << 12) |
                (transfer_type_to_raw(type) << 13) | (0x1 << 15)
            );

            // check if we need to enable the interrupt
            if (endpoint != 0) [[likely]] {
                // get the enpoint status register
                volatile uint16_t *const ep_status = get_endpoint_status(endpoint, mode);

                // clear the buffer for the endpoint
                (*ep_status) |= (0x1 << 15);
                (*ep_status) &= (~(0x1 << 15));

                // enable the interrupt flag for the endpoint
                if (mode != klib::usb::usb::endpoint_mode::out) {
                    // enable the packet transfer interrupt for the endpoint
                    (*ep_status) |= 0x1 << 14;
                }
            }
        }

        /**
         * @brief Set the device address. Handled in hardware
         *
         * @param address
         */
        static klib::usb::usb::handshake set_device_address(const uint8_t address) {
            // This is handled by the usb hardware on the chip

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
            // TODO: add support for this

            // get the callback
            const auto callback = state[endpoint].callback;

            // get the amount of data we have transferred
            const auto transferred = state[endpoint].transferred_size;

            // clear the state of the endpoint
            clear_endpoint_state(endpoint);

            if (callback) [[likely]] {
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
            // connect the pull-up to d+ by clearing the hconx flag
            Usb::port->UDCC &= (~(0x1 << 5));
        }

        /**
         * @brief Disconnect the device from the host by disabling
         * the pullup
         *
         */
        static void disconnect() {
            // disconnect the pull-up from d+ by setting the register
            Usb::port->UDCC |= (0x1 << 5);
        }

        /**
         * @brief Ack a endpoint
         *
         * @param endpoint
         * @param mode
         */
        static void ack(const uint8_t endpoint, const klib::usb::usb::endpoint_mode mode) {
            // This is handled in hardware. No need to send a ack again. In some 
            // cases the hardware does not send a ack. For example the SET_IDLE.
            // We send this manually when we detect it.
        }

        /**
         * @brief Stall a endpoint
         *
         * @param endpoint
         * @param mode
         */
        static void stall(const uint8_t endpoint, const klib::usb::usb::endpoint_mode mode) {
            // get the register we should write to (skip every other half word)
            volatile uint16_t *const ep = &(&Usb::port->EP0C)[endpoint * 2];

            // set the stall bit
            (*ep) |= 0x1 << 9;

            // get the callback
            const auto callback = state[endpoint].callback;

            // get the amount of data we have transferred
            const auto transferred = state[endpoint].transferred_size;

            // clear the state of the endpoint
            clear_endpoint_state(endpoint);

            if (callback) [[likely]] {
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

            // TODO: EP0 needs more stuff here as we need to 
            // unstall both the in and out endpoint

            // get the register we should write to (skip every other half word)
            volatile uint16_t *const ep = &(&Usb::port->EP0C)[endpoint * 2];

            // clear the stall bit
            (*ep) = (*ep) & (~(0x1 << 9));

            // get the callback
            const auto callback = state[endpoint].callback;

            // get the amount of data we have transferred
            const auto transferred = state[endpoint].transferred_size;

            // clear the state of the endpoint
            clear_endpoint_state(endpoint);

            if (callback) [[likely]] {
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
            // get the register we should write to (skip every other half word)
            volatile uint16_t *const ep = &(&Usb::port->EP0C)[endpoint * 2];

            // return the endpoint stall bit
            return (*ep) & (0x1 << 9);
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
                (*get_endpoint_status(endpoint, klib::usb::usb::endpoint_mode::out)) |= (0x1 << 14);
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
            // TODO: check if we need to do stuff here

            // get the callback
            const auto callback = state[endpoint].callback;

            // get the amount of data we have transferred
            const auto transferred = state[endpoint].transferred_size;

            // clear the state of the endpoint
            clear_endpoint_state(endpoint);

            // check if the callback is valid
            if (callback) [[likely]] {
                // call the callback
                callback(endpoint, mode, klib::usb::usb::error::cancel, transferred);
            }
        }
    };
}

#endif
