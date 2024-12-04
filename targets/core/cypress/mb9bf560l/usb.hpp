#ifndef KLIB_CYPRESS_MB9BF560L_USB_HPP
#define KLIB_CYPRESS_MB9BF560L_USB_HPP

#include <span>

#include <klib/klib.hpp>
#include <klib/usb/usb/usb.hpp>
#include <klib/math.hpp>

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

    public:
        /**
         * @brief Interrupt handler for the usb driver
         *
         * @warning Should not be called by the user
         *
         */
        static void irq_handler() {

        }

    public:
        /**
         * @brief Initialize the usb hardware. This requires the usb pll to be enabled beforehand using
         *
         * target::io::system::clock::set_usb<external_crystal_frequency>();
         *
         * @tparam UsbConnect
         */
        template <bool UsbConnect = true>
        static void init() {
            // TODO: enable the usb clock
            
            // we only support up to 2 channels. Make sure this is valid
            static_assert(Usb::id < 2, "Invalid USB id");

            // setup the io 
            GPIO->SPSR = GPIO->SPSR | (0x1 << (Usb::id + 4));

            // check if we need to enable the USB connect pin
            if constexpr (UsbConnect) {
                // setup the usb connect pin
                target::io::detail::pins::set_peripheral<typename Usb::connect::pin, typename Usb::connect::periph>();
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

            // register ourselfs with the interrupt controller
            target::irq::register_irq<Usb::interrupt_id>(irq_handler);

            // enable the usb interrupt
            target::enable_irq<Usb::interrupt_id>();
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
            static_assert(endpoint < endpoint_count, "Invalid endpoint provided");

            // this chip has some weird rules. We can enable a Bulk/Interrupt 
            // on every channel except endpoint 0. 
            // We can only enable a isochronous endpoint on endpoint 2 and 4.
            // If we enable isochronous mode on a endpoint the previous 
            // endpoint becomes unusable. And if we enable isochronous mode
            // on endpoint 4 endpoint 2 needs to be isochronous mode as well
            
            // check if the endpoint supports the transfer type
            if constexpr (type == transfer_type::control) {
                static_assert(endpoint == 0,
                    "Endpoint does not support control mode"
                );
            }
            else if constexpr (type == transfer_type::interrupt || type == transfer_type::bulk) {
                static_assert(endpoint != 0,
                    "Endpoint does not support interrupt/bulk mode"
                );
            }
            else {
                static_assert((endpoint == 2) || (endpoint == 4),
                    "Endpoint does not support isochronous mode"
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
            
        }

        /**
         * @brief Set the device address. Handled in hardware
         *
         * @param address
         */
        static klib::usb::usb::handshake set_device_address(const uint8_t address) {
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

        }

        /**
         * @brief Connect the device to the host by enabling the
         * pullup
         *
         */
        static void connect() {

        }

        /**
         * @brief Disconnect the device from the host by disabling
         * the pullup
         *
         */
        static void disconnect() {
            
        }

        /**
         * @brief Ack a endpoint
         *
         * @param endpoint
         * @param mode
         */
        static void ack(const uint8_t endpoint, const klib::usb::usb::endpoint_mode mode) {

        }

        /**
         * @brief Stall a endpoint
         *
         * @param endpoint
         * @param mode
         */
        static void stall(const uint8_t endpoint, const klib::usb::usb::endpoint_mode mode) {

        }

        /**
         * @brief Unstall a endpoint
         *
         * @param endpoint
         * @param mode
         */
        static void un_stall(const uint8_t endpoint, const klib::usb::usb::endpoint_mode mode) {

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
            return true;
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
            return false;
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
            return false;
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

        }
    };
}

#endif
