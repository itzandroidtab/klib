#ifndef KLIB_MAX32625_USB_HPP
#define KLIB_MAX32625_USB_HPP

#include <klib/irq_helper.hpp>
#include <klib/usb/usb.hpp>
#include <klib/math.hpp>

#include <max32625.hpp>
#include "clocks.hpp"

#include <klib/stream.hpp>

namespace klib::max32625::io::periph {
    struct usb0 {
        // peripheral id (e.g usb0, usb1)
        constexpr static uint32_t id = 0;

        // interrupt id (including the arm vector table)
        constexpr static uint32_t interrupt_id = 24;
    };
}

namespace klib::max32625::io::detail::usb {
    // default type when using the port
    template<uint32_t Usb>
    USB_Type *const port = nullptr;

    // port when using the pio0
    template<>
    USB_Type *const port<0> = USB;

    // change the packing and push the old packing
    #pragma pack(push, 1)

    /**
     * @brief Struct for the endpoint buffers
     * 
     */
    struct endpoint_buffer {
        volatile uint32_t buf0_desc;
        volatile uint32_t buf0_address;
        volatile uint32_t buf1_desc;
        volatile uint32_t buf1_address;
    };

    /**
     * @brief Struct for endpoint 0
     * 
     */
    struct endpoint0_buffer {
        endpoint_buffer out_buffer;
        endpoint_buffer in_buffer;
    };

    /**
     * @brief Struct for storing all the endpoints. Needs to be 512 byte alligned for the max32625
     * 
     */
    template <uint32_t EndpointAmount>
    struct ep_buffer_descriptor {
        // endpoint 0
        endpoint0_buffer ep0;

        // all the other endpoints
        endpoint_buffer ep[EndpointAmount - 1];
    };

    /**
     * @brief Struct to store the state of a endpoint
     * 
     */
    struct state {
        // flag if the endpoint is busy
        volatile bool is_busy;

        // max size of the endpoint
        uint8_t max_size;

        // requested size of the current endpoint
        uint32_t requested_size;

        // transmitted/received amount of data.
        volatile uint32_t transferred_size; 

        // callback function
        klib::usb::usb::usb_callback callback;
    };

    // restore the old packing
    #pragma pack(pop)
}

namespace klib::max32625::io {
    template <typename Usb, typename Device, typename Irq>
    class usb {
    public:
        // amount of endpoints supported by the max32625
        constexpr static uint8_t endpoint_count = 8;

        // max size in a single endpoint
        constexpr static uint8_t max_endpoint_size = 64;
        
    protected:
        // flag to signal the endpoint is busy and cannot handle a new request
        static inline detail::usb::state state[endpoint_count] = {};

        // descriptor for all the endpoints
        alignas(512) static inline io::detail::usb::ep_buffer_descriptor<endpoint_count> descriptor = {};

        // port to the usb peripheral
        static inline USB_Type *const port = io::detail::usb::port<Usb::id>;

        using irq_helper = klib::irq_helper<32>;

        // create a helper to call all the specific functions when 
        // certain bits are set in the status register
        static inline auto helper = irq_helper();

        /**
         * @brief Different events supported by the hardware
         * 
         */
        enum class event: uint8_t {
            dplus_activity = 0,
            remote_wakeup_done = 1, 
            bus_active = 2,
            bus_reset = 3,
            suspend = 4,
            no_vbus = 5,
            vbus = 6,
            bus_reset_done = 7,
            setup_data_available = 8,
            endpoint_in = 9,
            endpoint_out = 10
        };

        template <event Event>
        constexpr static uint32_t event_mask() {
            return (0x1 << static_cast<uint8_t>(Event));
        }

        template <event Num>
        static void enable_irq() {
            port->DEV_INTEN |= (0x1 << static_cast<uint8_t>(Num));
        }

        template <event Num>
        static void disable_irq() {
            port->DEV_INTEN &= ~(0x1 << static_cast<uint8_t>(Num));
        }

        template <event Num>
        static void clear_irq() {
            port->DEV_INTFL |= (0x1 << static_cast<uint8_t>(Num));
        }

        template <event Num>
        static void setup_event_irq(const irq_helper::interrupt_callback &irq) {
            // register the interrupt is the helper
            helper.register_irq<static_cast<uint8_t>(Num)>(irq);

            // enable the interrupt in the enable register
            enable_irq<Num>();
        }

        static void usb_wakeup() {
            // wakeup the usb
            disable_irq<event::dplus_activity>();

            PWRMAN->PWR_RST_CTRL |= (0x1 << 4);

            port->DEV_CN &= ~(0x1 << 4);
        }

        /**
         * @brief Reset the usb controller
         * 
         */
        static void reset() {
            // disable the device
            port->CN = 0;

            // reenable the device again
            port->CN = 1;

            // disable all the interrupts
            port->DEV_INTEN = 0;

            // clear the device control register
            port->DEV_CN = 0;

            // force a reset on the usb controller
            port->DEV_CN = 0x1 << 5;

            // clear the usb controller reset
            port->DEV_CN = 0;
        }

        static volatile uint32_t *const get_endpoint_ptr(const uint32_t endpoint) {
            // get the stall flag 
            switch (endpoint) {
                case 1:
                    return &port->EP1;
                case 2:
                    return &port->EP2;
                case 3:
                    return &port->EP3;
                case 4:
                    return &port->EP4;
                case 5:
                    return &port->EP5;
                case 6:
                    return &port->EP6;
                case 7:
                    return &port->EP7;
                case 0:
                default:
                    // always return a valid pointer
                    // even if it is not correct.
                    return &port->EP0;
            }
        }

        static void connect() {
            // enable interrupts for setup, endpoint in, endpoint out and dma erros
            port->DEV_INTEN |= (0x1 << 8) | (0x1 << 9) | (0x1 << 10) | (0x1 << 12);

            // allow interrupts on enpoint 0
            port->EP0 |= (0x1 << 4);

            // enable the pullups on dplus to signal the host we have connected (and enable the fifo bit
            // to respond to a incomming in request as soon as the device controllers fifo becomes 
            // non-empty, instead of wiating the full pakcet to be received by the fifo. reduces the 
            // likelihood of returning a nak)
            port->DEV_CN |= (0x1 << 3) | (0x1 << 9);
        }

        static void disconnect() {
            // disable the pullups to disconnect the device
            port->DEV_CN &= ~(1 << 3);
        }

        static uint8_t endpoint_mode_to_raw(const klib::usb::usb::endpoint_mode mode) {
            switch (mode) {
                case klib::usb::usb::endpoint_mode::out:
                    return 1;
                case klib::usb::usb::endpoint_mode::in:
                    return 2;
                case klib::usb::usb::endpoint_mode::control:
                    return 3;
                case klib::usb::usb::endpoint_mode::disabled:
                default:
                    return 0;
            }
        }

        static void clear_endpoint_state(const uint8_t endpoint) {
            state[endpoint].is_busy = false;
            state[endpoint].requested_size = 0;
            state[endpoint].transferred_size = 0;
            state[endpoint].callback = nullptr;
        }

        static void setup_packet_irq() {
            // create a copy of the setup packet
            const uint32_t buffer[2] = {
                port->SETUP0,
                port->SETUP1
            };

            // get the setup packet we have received
            const klib::usb::setup_packet& packet = (*(reinterpret_cast<const klib::usb::setup_packet*>(buffer)));

            // handle the setup packet in the klib library
            klib::usb::usb::handle_setup_packet<usb_type>(packet);
        }

        static void novbus_irq() {
            // disable interrupts that make only sense when we are connected
            disable_irq<event::bus_reset>();
            disable_irq<event::suspend>();
            disable_irq<event::dplus_activity>();

            // clear the pullup resistor to disconnect
            disconnect();

            // check if the device has the usb disconnected callback
            constexpr bool has_disconnected_callback = requires() {
                device::template connected<usb_type>();
            };

            if constexpr (has_disconnected_callback) {
                // call the user function to signal we have disconnected
                Device::template disconnected<usb_type>();
            }

            // put the usb controller in low power mode
            sleep();
        }

        static void vbus_irq() {
            // clear any bus reset interrupt that has not been processed yet
            clear_irq<event::bus_reset>();
            // setup and enable the busreset interrupt
            enable_irq<event::bus_reset>();

            // clear any suspend interrupt that has not been processed yet
            clear_irq<event::suspend>();
            // setup and enable the suspend interrupt
            setup_event_irq<event::suspend>(suspend_irq);

            // connect to the usb host
            connect();

            // check if the device has the usb connected callback
            constexpr bool has_connected_callback = requires() {
                device::template connected<usb_type>();
            };

            if constexpr (has_connected_callback) {
                // call the user function to signal we have connected to a host
                Device::template connected<usb_type>();
            }

            // put the usb controller in a low power state until we get 
            // activity from the host
            sleep();
        }

        static void suspend_irq() {
            // put the usb controller in low power mode
            sleep();
        }

        static void activity_irq() {
            // wakeup the usb transceiver 
            wakeup();

            // call the user function to signal have detected activity on the bus
            Device::template activity<usb_type>();
        }

        static void bus_reset_irq() {
            // wakeup the usb hardware
            wakeup();

            // call the device bus reset
            device::template bus_reset<usb_type>();
        }

        static void sleep() {
            // put the tranceiver into a low power state
            port->DEV_CN |= (0x1 << 4);

            PWRMAN->PWR_RST_CTRL &= ~(0x1 << 4);

            // check what state we should go to. If we are connected
            // we want to wait for the activity interrupt. If we are
            // not we disable the interrupt
            if (port->DEV_CN & (0x1 << 3)) {
                // clear the interrupt
                clear_irq<event::dplus_activity>();

                // enable the activity event
                setup_event_irq<event::dplus_activity>(activity_irq);
            }
            else {
                // disable the event
                disable_irq<event::dplus_activity>();
            }

            // call the device sleep function
            device::template sleep<usb_type>();
        }

        static void wakeup() {
            // disable the interrupt
            disable_irq<event::dplus_activity>();

            // enable thje power
            PWRMAN->PWR_RST_CTRL |= (0x1 << 4);
            
            // power on the usb tranceiver
            port->DEV_CN &= ~(0x1 << 4);

            // call the device wakeup function
            device::template wakeup<usb_type>();
        }

        /**
         * @brief This function is called when we get device -> host
         * interrupt. (IN is from the host side)
         * 
         * @param status_register 
         * @param interrupt_mask 
         */
        static void data_in_irq() {
            // get the in interrupts
            const uint32_t in_irq = port->IN_INT;

            // clear the in interrupt
            port->IN_INT = in_irq;

            // process every endpoint
            for (uint8_t ep = 0; ep < endpoint_count; ep++) {
                // check if the interrupt is for the current endpoint
                if (!(in_irq & (0x1 << ep))) {
                    // out is not for this endpoint
                    continue;
                }

                // check if the endpoint is busy
                if (!state[ep].is_busy) {
                    // endpoint is not busy. Not sure how this
                    // can happen but catch it anyway
                    continue;
                }

                // what is the size we are transmitting
                const uint32_t data_left = (state[ep].requested_size - state[ep].transferred_size);

                // get the endpoint buffers
                detail::usb::endpoint_buffer& buffer = (
                    (ep == 0) ? (descriptor.ep0.in_buffer) : (descriptor.ep[ep - 1])
                );

                if (buffer.buf0_desc == 0) {
                    // get the callback
                    const auto callback = state[ep].callback;

                    // clear the state of the endpoint. (we clear it first as the 
                    // callback may reissue a request)
                    clear_endpoint_state(ep);

                    // must have sent the ZLP, mark done
                    if (callback) {
                        // call the callback
                        callback(ep, klib::usb::usb::endpoint_mode::in, klib::usb::usb::error::no_error);
                    }

                    // skip the remainder
                    continue;
                }

                // check if we have data left to transmit
                if (data_left) {
                    // get the amount of data we can transmit in a single go
                    const uint32_t transmit_size = klib::min(data_left, state[ep].max_size);

                    // increment the transmitted length. When we receive the interrupt 
                    // again the data is already transmitted
                    state[ep].transferred_size += transmit_size;

                    // update the data pointer
                    buffer.buf0_desc += state[ep].max_size;

                    // start the dma to send the data
                    port->IN_OWNER = (0x1 << ep);
                }
                else {
                    // all done sending data, either send ZLP or we are done. Get size mask
                    // of the endpoint max size. Used to determine if the last packet was the
                    // full size of the endpoint. 
                    const uint32_t size_mask = state[ep].max_size ? (state[ep].max_size - 1) : 0;

                    // check if the last message was full sized
                    if ((state[ep].transferred_size & size_mask) == 0) {
                        // send ZLP following the spec. the last packet was full sized and nothing left to send    
                        buffer.buf0_desc = 0;
                        port->IN_OWNER = (0x1 << ep);
                    }
                    else {
                        // get the callback
                        const auto callback = state[ep].callback;

                        // clear the state of the endpoint
                        clear_endpoint_state(ep);

                        // we are done with the transaction. call the callback
                        if (callback) {
                            // send we are done. Do not care about the return value
                            // as we are always clearing the endpoint after the 
                            // transaction is done
                            callback(ep, klib::usb::usb::endpoint_mode::in, klib::usb::usb::error::no_error);
                        }
                    }
                }
            }
        }

        /**
         * @brief This function is called when we get host -> device 
         * interrupt. (OUT is from the host side)
         * 
         * @param status_register 
         * @param interrupt_mask 
         */
        static void data_out_irq() {
            // get the out interrupts
            const uint32_t out_irq = port->OUT_INT;

            // clear the out interrupt
            port->OUT_INT = out_irq;

            // process every endpoint
            for (uint8_t ep = 0; ep < endpoint_count; ep++) {
                /// check if the interrupt is for the current endpoint
                if (!(out_irq & (0x1 << ep))) {
                    // out is not for this endpoint
                    continue;
                }

                // check if the endpoint is busy
                if (!state[ep].is_busy) {
                    // endpoint is not busy. This can happen if the 
                    // callback was called then ZLP received. (ignore
                    // the callback must have been called and it has 
                    // cleared the endpoint)
                    continue;
                }

                // what was the size we have requested
                const uint32_t request_size = klib::min(state[ep].max_size, 
                    (state[ep].requested_size - state[ep].transferred_size)
                );

                // get the endpoint buffers
                detail::usb::endpoint_buffer& buffer = (
                    (ep == 0) ? (descriptor.ep0.in_buffer) : (descriptor.ep[ep - 1])
                );

                // get the actual size of the data written to the buffer. 
                // This will be lesser of the packet size and the 
                // requested size
                const uint32_t rx_size = klib::min(request_size, buffer.buf0_desc);

                // update the amount of data we have received
                state[ep].transferred_size += rx_size;

                // check if we have more data to receive. If we receive less
                // data than the endpoint size or if we receive a zero length
                // packet we know the transmission is finished
                if ((rx_size < state[ep].max_size) || (rx_size == 0)) {
                    // get the callback
                    const auto callback = state[ep].callback;

                    // clear the state of the endpoint
                    clear_endpoint_state(ep);

                    if (callback) {
                        // send we are done. Do not care about the return value
                        // as we are always clearing the endpoint after the 
                        // transaction is done
                        callback(ep, klib::usb::usb::endpoint_mode::out, klib::usb::usb::error::no_error);
                    }
                }
                else {
                    // we are not done yet with the transaction. Push the new
                    // pointers and update the descriptor.
                    buffer.buf0_address += state[ep].max_size;
                    buffer.buf0_desc = request_size;

                    // transfer buffer back to the dma controller
                    port->IN_OWNER = (0x1 << ep);
                }
            }
        }

        /**
         * @brief Interrupt handler for the usb driver
         * 
         */
        static void irq_handler() {
            // get the interrupt status from the hardware
            const uint32_t status = port->DEV_INTFL;

            // get the mask of the enabled interrupts
            const uint32_t mask = port->DEV_INTEN;

            // calculate the masked status
            const uint32_t masked_status = status & mask;

            // clear only the enabled interrupts
            port->DEV_INTFL = masked_status;

            // check if we have a bus reset event
            if (masked_status & event_mask<event::bus_reset>()) {
                // reset every endpoint
                for (uint32_t i = 0; i < endpoint_count; i++) {
                    // we do not know what endpoint mode we have
                    // as the driver ignores this field anyway. Just
                    // keep it as disabled
                    reset(i, klib::usb::usb::endpoint_mode::disabled);
                }

                // call the bus reset
                bus_reset_irq();
            }
            else {
                // do not process any in/out request after the bus reset

                // check for data in requests
                if (masked_status & event_mask<event::endpoint_in>()) {
                    data_in_irq();
                }

                // check for data out requests
                if (masked_status & event_mask<event::endpoint_out>()) {
                    data_out_irq();
                }
            }

            // use the interrupt helper to handle the other individual bits
            helper.handle_irq(status, mask);
        }

    public:
        // type to use in device functions
        using usb_type = usb<Usb, Device, Irq>;

        // type so the klib usb driver can comunicate to the device
        using device = Device;

        /**
         * @brief Init the usb hardware
         * 
         * @tparam Irq 
         */
        static void init() {
            // enable the usb clock
            // TODO: change to clock functions
            CLKMAN->CLK_CTRL |= (0x1 << 4);

            // enable the usb power
            // TODO: change to power functions
            PWRMAN->PWR_RST_CTRL |= (0x1 << 4);

            // reset all the info stored about the endpoints
            for (uint32_t i = 0; i < endpoint_count; i++) {
                // set the endpoint to a known state
                state[i] = {
                    .is_busy = false,
                    .max_size = static_cast<uint8_t>((i == 0) ? max_endpoint_size : 0),
                    .requested_size = 0,
                    .transferred_size = 0, 
                    .callback = nullptr
                };
            }

            // reset the usb peripheral
            reset();

            // set the endpoint descriptor location. Needs to be 512 byte alligned
            port->EP_BASE = reinterpret_cast<uint32_t>(&descriptor);

            // register the irq handler
            Irq::template register_irq<Usb::interrupt_id>(irq_handler);

            // enable the interrupt for a setup packet
            setup_event_irq<event::setup_data_available>(setup_packet_irq);

            // init the device
            device::template init<usb_type>();

            // register vbus events
            setup_event_irq<event::no_vbus>(novbus_irq);
            setup_event_irq<event::vbus>(vbus_irq);

            // set the usb hardware in sleep mode
            sleep();
        }

        /**
         * @brief Shutdown the usb peripheral
         * 
         */
        static void shutdown() {
            port->DEV_CN = 1 << 5;
            port->DEV_CN = 0;
            port->CN = 0;
        }

        /**
         * @brief Configure a endpoint
         * 
         * @param endpoint 
         * @param mode 
         * @param size 
         */
        static void configure(const uint8_t endpoint, const klib::usb::usb::endpoint_mode mode, const uint32_t size) {
            // get the endpoint pointer
            auto *const ep = get_endpoint_ptr(endpoint);

            // prepare the endpoint register
            const uint32_t ctrl = (
                endpoint_mode_to_raw(mode) | (1 << 6) | 
                ((mode == klib::usb::usb::endpoint_mode::disabled) ? 0 : (1 << 4))
            );

            // set the new endpoint size
            state[endpoint].max_size = klib::min(max_endpoint_size, size);

            // store the ctrl data in the endpoint
            (*ep) = ctrl;
        }

        /**
         * @brief Returns if a endpoint is configured (not disabled)
         * 
         * @param endpoint 
         * @param mode
         * @return true 
         * @return false 
         */
        static bool is_configured(const uint8_t endpoint, const klib::usb::usb::endpoint_mode mode) {
            // get the endpoint pointer
            auto *const ep = get_endpoint_ptr(endpoint);

            // return if the endpoint is not disabled
            return ((*ep) & 0x3) != endpoint_mode_to_raw(klib::usb::usb::endpoint_mode::disabled);
        }

        /**
         * @brief Stall a endpoint
         * 
         * @param endpoint 
         * @param mode
         */
        static void stall(const uint8_t endpoint, const klib::usb::usb::endpoint_mode mode) {
            // get the pointer to the endpoint
            auto *const ep = get_endpoint_ptr(endpoint);

            // check for endpoint 0
            if (endpoint == 0) {
                // stall the control transfer
                (*ep) |= (0x1 << 9);
            }

            // set the stall flag 
            (*ep) |= (0x1 << 8);

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
            // get the pointer to the endpoint
            auto *const ep = get_endpoint_ptr(endpoint);

            // check if the endpoint is stalled. The host can unstall 
            // us even if we are not stalled
            if (!((*ep) & (0x1 << 8))) {
                // ack the request
                ack(endpoint, mode);

                // we are not stalled return
                return;
            }

            // reset the data0 and buffer of the endpoint
            (*ep) |= (0x1 << 6);

            // clear the stall flag
            (*ep) &= (0x1 << 8);

            // get the callback
            const auto callback = state[endpoint].callback;

            // clear the state of the endpoint
            clear_endpoint_state(endpoint);

            if (callback) {
                // send a error to the callback
                callback(endpoint, mode, klib::usb::usb::error::un_stall);
            }

            // ack the endpoint
            ack(endpoint, mode);
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
            // get the pointer to the endpoint
            const auto *const ep = get_endpoint_ptr(endpoint);

            // get the stall flag 
            return (*ep) & (0x1 << 8);
        }

        /**
         * @brief Reset a endpoint
         * 
         * @param endpoint 
         * @param mode
         */
        static void reset(const uint8_t endpoint, const klib::usb::usb::endpoint_mode mode) {
            // get the pointer to the endpoint
            auto *const ep = get_endpoint_ptr(endpoint);

            // disable the endpoint
            (*ep) &= ~(0x3);

            // clear the data toggle
            (*ep) |= (0x1 << 6);

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
         * @brief Ack a endpoint
         * 
         * @param endpoint 
         * @param mode
         */
        static void ack(const uint8_t endpoint, const klib::usb::usb::endpoint_mode mode) {
            // get the pointer to the endpoint
            auto *const ep = get_endpoint_ptr(endpoint);

            // set the ack
            (*ep) |= (0x1 << 10);
        }

        /**
         * @brief Set the device address. Handled in hardware
         * 
         * @param address 
         */
        static klib::usb::usb::handshake set_device_address(const uint8_t address) {
            // ack the response. The hardware handles this
            return klib::usb::usb::handshake::ack;
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
         * @param size 
         * @return true 
         * @return false 
         */
        static bool write(const klib::usb::usb::usb_callback callback, const uint8_t endpoint, 
                          const klib::usb::usb::endpoint_mode mode, const uint8_t* data, 
                          const uint32_t size) 
        {
            // check if the endpoint is configured
            if (!is_configured(endpoint, mode)) {
                // endpoint is not configured stop
                return false;
            }

            // check if we have a pending request
            if (state[endpoint].is_busy || (port->IN_OWNER & (0x1 << endpoint))) {
                // we have a pending request stop
                return false;
            }

            // get the size we can write in a single transmission
            const uint32_t s = klib::min(size, state[endpoint].max_size);

            // set the endpoint callback and mark the endpoint as busy
            state[endpoint].is_busy = true;
            state[endpoint].callback = callback;
            
            // set the endpoint data (we prefill the transferred size. When we get a 
            // interrupt for it is already transmitted)
            state[endpoint].requested_size = size;
            state[endpoint].transferred_size = s;

            // check what buffer to write to
            if (endpoint == 0) {
                // write to the first endpoint
                descriptor.ep0.in_buffer.buf0_desc = s;
                descriptor.ep0.in_buffer.buf0_address = reinterpret_cast<const uint32_t>(data);
            }
            else {
                // write to the other endpoints
                descriptor.ep[endpoint - 1].buf0_desc = s;
                descriptor.ep[endpoint - 1].buf0_address = reinterpret_cast<const uint32_t>(data);
            }

            // start the dma
            port->IN_OWNER = (0x1 << endpoint);

            // return we have no errors at the moment
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
         * @param size 
         * @return true 
         * @return false 
         */
        static bool read(const klib::usb::usb::usb_callback callback, const uint8_t endpoint, 
                         const klib::usb::usb::endpoint_mode mode, uint8_t* data, 
                         const uint32_t size) 
        {
            // check if the endpoint is configured
            if (!is_configured(endpoint, mode) || is_stalled(endpoint, mode)) {
                // endpoint is not configured stop
                return false;
            }

            // check if we have a pending request
            if (state[endpoint].is_busy || (port->IN_OWNER & (0x1 << endpoint))) {
                // we have a pending request stop
                return false;
            }

            // get the size we can write in a single transmission
            const uint32_t s = klib::min(size, state[endpoint].max_size);

            // set the endpoint callback and mark the endpoint as busy
            state[endpoint].is_busy = true;
            state[endpoint].callback = callback;
            
            // set the endpoint data
            state[endpoint].requested_size = size;
            state[endpoint].transferred_size = 0;

            // check what buffer to write to
            if (endpoint == 0) {
                // write to the first endpoint
                descriptor.ep0.out_buffer.buf0_desc = s;
                descriptor.ep0.out_buffer.buf0_address = reinterpret_cast<const uint32_t>(data);
            }
            else {
                // write to the other endpoints
                descriptor.ep[endpoint - 1].buf0_desc = s;
                descriptor.ep[endpoint - 1].buf0_address = reinterpret_cast<const uint32_t>(data);
            }

            // start the dma
            port->OUT_OWNER = (0x1 << endpoint);

            // return we have no errors at the moment
            return true;
        }

        /**
         * @brief Send a remote wakeup signal to the host
         * 
         */
        static void remote_wakeup() {
            // wakup the usb tranceiver
            wakeup();

            // send a remote wakeup signal to the host
            port->DEV_CN |= (0x1 << 2);
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
         * @brief Function that gets called to notify the driver 
         * the devices is configured
         * 
         * @param cfg 
         */
        static void configured(const bool cfg) {
            // do nothing
        }
    };
}

#endif
