#ifndef KLIB_USB_BULK_HPP
#define KLIB_USB_BULK_HPP

#include <algorithm>

#include <klib/string.hpp>
#include <klib/usb/usb/device.hpp>
#include <klib/usb/usb/descriptor.hpp>

namespace klib::usb::device {
    template <uint8_t InEndpoint = 0x2, uint8_t OutEndpoint = 0x05>
    class bulk {
    public:
        // using for the array of callbacks
        using interrupt_callback = void(*)();

    protected:
        /**
         * @brief Enum with the string descriptor indexes
         *
         */
        enum class string_index {
            language = 0,
            manufacturer = 1,
            product = 2,
            serial = 3
        };

        // Push the current pack to the stack and set the pack to 1
        // as all these structs have specific sizes
        #pragma pack(push, 1)

        /**
         * @brief Config descriptor for the bulk device
         *
         * @details packed so we can write this whole descriptor
         * to the usb hardware in one go.
         *
         */
        struct config_descriptor {
            // configuration descriptor
            descriptor::configuration configuration;

            // interface descriptor
            descriptor::interface interface;

            // endpoint descriptor
            descriptor::endpoint endpoint0;

            // endpoint descriptor
            descriptor::endpoint endpoint1;
        };

        // release the old pack so the rest of the structs are not
        // affected by the pack(1)
        #pragma pack(pop)

        // device descriptor for the bulk device
        const __attribute__((aligned(4))) static inline descriptor::device device = {
            .bcdUSB = static_cast<uint16_t>(setup::usb_version::usb_v1_1),
            .bDeviceClass = descriptor::class_type::vendor_specific,
            .bDeviceSubClass = 0x00,
            .bDeviceProtocol = 0x00,
            .bMaxPacketSize = 0x40,
            .idVendor = 0x6666,
            .idProduct = 0xdead,
            .bcdDevice = 0x0200,
            .iManufacturer = static_cast<uint8_t>(string_index::manufacturer),
            .iProduct = static_cast<uint8_t>(string_index::product),
            .iSerialNumber = static_cast<uint8_t>(string_index::serial),
            .bNumConfigurations = 0x1
        };

        // configuration descriptor
        const __attribute__((aligned(4))) static inline config_descriptor config = {
            {
                .wTotalLength = sizeof(config_descriptor),
                .bNumInterfaces = 0x01,
                .bConfigurationValue = 0x01,
                .iConfiguration = 0x00,
                .bmAttributes = 0xA0,
                .bMaxPower = 0x32,
            },
            {
                .bInterfaceNumber = 0x00,
                .bAlternateSetting = 0x00,
                .bNumEndpoints = 0x02,
                .bInterfaceClass = 0xff,
                .bInterfaceSubClass = 0x00,
                .bInterfaceProtocol = 0x00,
                .iInterface = 0x00
            },
            {
                .bEndpointAddress = 0x80 | InEndpoint,
                .bmAttributes = static_cast<uint8_t>(klib::usb::descriptor::transfer_type::bulk),
                .wMaxPacketSize = 0x0040,
                .bInterval = 0x01
            },
            {
                .bEndpointAddress = OutEndpoint,
                .bmAttributes = static_cast<uint8_t>(klib::usb::descriptor::transfer_type::bulk),
                .wMaxPacketSize = 0x0040,
                .bInterval = 0x01
            }
        };

        // language descriptor for the bulk device
        const __attribute__((aligned(4))) static inline descriptor::string<1> language = {
            .bString = {0x0409}
        };

        // manufacturer string descriptor
        const __attribute__((aligned(4))) static inline auto manufacturer = string_descriptor("KLIB");

        // product string descriptor
        const __attribute__((aligned(4))) static inline auto product = string_descriptor("KLIB Bulk USB");

        // serial number string descriptor
        const __attribute__((aligned(4))) static inline auto serial = string_descriptor("00001337");

        // configuration value. Value is set in the set config function
        static inline uint8_t configuration = 0x00;

        // flag if remote wakeup is supported
        static inline bool remote_wakeup = false;

        // callbacks
        static inline interrupt_callback transmit_callback = nullptr;
        static inline interrupt_callback receive_callback = nullptr;

        /**
         * @brief Callback for when a write or read is
         * finished. Calls the user callbacks if available
         *
         * @tparam Usb
         * @param endpoint
         * @param mode
         * @param error_code
         */
        template <typename Usb>
        static void callback(const uint8_t endpoint, const usb::endpoint_mode mode, const usb::error error_code, const uint32_t transferred) {
            // only continue if we do not have any errors
            if (error_code != usb::error::no_error) {
                // we have a error
                return;
            }

            // check if we are configured
            if (!configuration) {
                return;
            }

            // check if we should read or write
            if (mode == usb::endpoint_mode::in) {
                if (transmit_callback) {
                    transmit_callback();
                }
            }
            else {
                if (receive_callback) {
                    receive_callback();
                }
            }
        }

    public:
        /**
         * @brief Register the transmit and receive callbacks
         *
         * @param transmit
         * @param receive
         */
        static void register_callback(const interrupt_callback& transmit = nullptr, const interrupt_callback& receive = nullptr) {
            // set the transmit callback
            transmit_callback = transmit;

            // set the receive callback
            receive_callback = receive;
        }

        /**
         * @brief Setup a read. Calls the receive callback when
         * all the data is send to the host
         *
         * @tparam Usb
         * @tparam Async
         * @param data
         * @param size
         * @return true
         * @return false
         */
        template <typename Usb, bool Async = false>
        static bool setup(uint8_t *const data, const uint32_t size) {
            if (data == nullptr || !size || is_busy<Usb, true>()) {
                // we are still transmitting
                return false;
            }

            // start a read on the endpoint
            Usb::read(callback<Usb>, usb::get_endpoint(config.endpoint1.bEndpointAddress),
                usb::endpoint_mode::out, {data, size}
            );

            // check if we should exit
            if constexpr (Async) {
                return true;
            }

            // wait until we are done transmitting all characters
            while (is_busy<Usb, true>()) {
                // wait
            }

            // return success
            return true;
        }

        /**
         * @brief write data to the host. Calls the transmit
         * callback when all the data is send
         *
         * @tparam Usb
         * @tparam Async
         * @param data
         * @param size
         * @return true
         * @return false
         */
        template <typename Usb, bool Async = false>
        static bool write(const uint8_t *const data, const uint32_t size) {
            // check if we are configured
            if (!configuration) {
                return false;
            }

            // check if we have a valid size
            if (size == 0) {
                // return we are done. Nothing to do here
                return true;
            }

            // check if a request is already pending
            if (is_busy<Usb, false>()) {
                // a other request is already pending exit
                return false;
            }

            // start to the write to the endpoint
            if (!Usb::write(callback<Usb>, usb::get_endpoint(config.endpoint0.bEndpointAddress),
                usb::get_endpoint_mode(config.endpoint0.bEndpointAddress), {data, size}))
            {
                return false;
            }

            // check if we should exit
            if constexpr (Async) {
                return true;
            }

            // wait until we are done transmitting all characters
            while (is_busy<Usb, false>()) {
                // do nothing
            }

            return true;
        }

        /**
         * @brief Returns if the read or write is busy based
         * on the template parameter
         *
         * @tparam Usb
         * @tparam Read
         * @return true
         * @return false
         */
        template <typename Usb, bool Read>
        static bool is_busy() {
            if constexpr (Read) {
                // return if the read is still pending
                return Usb::is_pending(
                    usb::get_endpoint(config.endpoint1.bEndpointAddress),
                    usb::get_endpoint_mode(config.endpoint1.bEndpointAddress)
                );
            }
            else {
                // return if the transmit is still pending
                return Usb::is_pending(
                    usb::get_endpoint(config.endpoint0.bEndpointAddress),
                    usb::get_endpoint_mode(config.endpoint0.bEndpointAddress)
                );
            }
        }

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
            remote_wakeup = false;

            // make sure the endpoints support bulk
            // endpoints
            static_assert(Usb::template is_valid_endpoint<
                InEndpoint,
                klib::usb::descriptor::transfer_type::bulk
            >(), "invalid bulk endpoint selected");

            static_assert(Usb::template is_valid_endpoint<
                OutEndpoint,
                klib::usb::descriptor::transfer_type::bulk
            >(), "invalid bulk endpoint selected");
        }

        /**
         * @brief Get the configuration of the config. Needed
         * for some hardware
         * 
         * @tparam Usb
         * @return uint16_t 
         */
        template <typename Usb>
        static uint8_t get_configuration() {
            return config.configuration.bConfigurationValue;
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
            // check if we support the feature
            if (feature != klib::usb::setup::feature::remote_wake) {
                // we only support remote wakeup
                return usb::handshake::stall;
            }

            // disable remote wakeup
            remote_wakeup = false;

            // ack the packet
            return usb::handshake::ack;
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
            // check if we support the feature
            if (feature != klib::usb::setup::feature::remote_wake) {
                // we only support remote wakeup
                return usb::handshake::stall;
            }

            // enable remote wakeup
            remote_wakeup = true;

            // ack the packet
            return usb::handshake::ack;
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
                    return to_description(config, config.configuration.wTotalLength);
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
                usb::endpoint_mode::in,
                {&configuration, sizeof(configuration)}
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
            if (packet.wValue == config.configuration.bConfigurationValue) {
                // configure the endpoints
                Usb::configure(
                    usb::get_endpoint(config.endpoint0.bEndpointAddress),
                    usb::get_endpoint_mode(config.endpoint0.bEndpointAddress),
                    usb::get_transfer_type(config.endpoint1.bmAttributes),
                    config.endpoint0.wMaxPacketSize
                );
                Usb::configure(
                    usb::get_endpoint(config.endpoint1.bEndpointAddress),
                    usb::get_endpoint_mode(config.endpoint1.bEndpointAddress),
                    usb::get_transfer_type(config.endpoint1.bmAttributes),
                    config.endpoint1.wMaxPacketSize
                );

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

                // check if we were configured already
                if (configuration) {
                    // reset the endpoint
                    Usb::reset(
                        usb::get_endpoint(config.endpoint0.bEndpointAddress),
                        usb::get_endpoint_mode(config.endpoint0.bEndpointAddress)
                    );

                    // reset the endpoint
                    Usb::reset(
                        usb::get_endpoint(config.endpoint1.bEndpointAddress),
                        usb::get_endpoint_mode(config.endpoint1.bEndpointAddress)
                    );
                }

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