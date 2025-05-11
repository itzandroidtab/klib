#ifndef KLIB_USB_MASS_STORAGE_HPP
#define KLIB_USB_MASS_STORAGE_HPP

#include <klib/usb/usb/device.hpp>
#include <klib/usb/usb/capability.hpp>
#include <klib/usb/usb/msc/msc.hpp>
#include <klib/usb/usb/msc/bulk_only_transfer.hpp>

namespace klib::usb::device {
    template <typename Memory, uint8_t InEndpoint = 0x02, uint8_t OutEndpoint = 0x05>
    class mass_storage {
    protected:
        // mass storage bot handler (set the in endpoint bit)
        using bot = msc::bot::handler<Memory, (0x80 | InEndpoint), OutEndpoint>;

        /**
         * @brief Enum with the string descriptor indexes
         *
         */
        enum class string_index {
            language = 0,
            manufacturer = 1,
            product = 2,
            serial = 3,
        };

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

        // device descriptor for the mass storage class
        const __attribute__((aligned(4))) static inline descriptor::device device = {
            .bcdUSB = static_cast<uint16_t>(setup::usb_version::usb_v2_0),
            .bDeviceClass = descriptor::class_type::use_class_info,
            .bDeviceSubClass = 0x00,
            .bDeviceProtocol = 0x00,
            .bMaxPacketSize = 0x40,
            .idVendor = 0x6666,
            .idProduct = 0xBEEF,
            .bcdDevice = 0x0100,
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
                .iConfiguration = 0x04,
                .bmAttributes = 0x80,
                .bMaxPower = 0x32,
            },
            {
                .bInterfaceNumber = 0x00,
                .bAlternateSetting = 0x00,
                .bNumEndpoints = 0x02,
                .bInterfaceClass = 0x08,
                .bInterfaceSubClass = 0x06,
                .bInterfaceProtocol = 0x50,
                .iInterface = 0x00,
            },
            {
                .bEndpointAddress = 0x80 | InEndpoint,
                .bmAttributes = static_cast<uint8_t>(klib::usb::descriptor::transfer_type::bulk),
                .wMaxPacketSize = 0x0040,
                .bInterval = 0x00
            },
            {
                .bEndpointAddress = OutEndpoint,
                .bmAttributes = static_cast<uint8_t>(klib::usb::descriptor::transfer_type::bulk),
                .wMaxPacketSize = 0x0040,
                .bInterval = 0x00
            }
        };

        // device qualifier
        const __attribute__((aligned(4))) static inline descriptor::qualifier qualifier = {
            .bcdUSB = static_cast<uint16_t>(setup::usb_version::usb_v2_0),
            .bDeviceClass = descriptor::class_type::use_class_info,
            .bDeviceSubClass = 0x00,
            .bDeviceProtocol = 0x00,
            .bMaxPacketSize0 = 0x40,
            .bNumConfigurations = 0x1
        };

        // language descriptor for the keyboard
        const __attribute__((aligned(4))) static inline descriptor::string<1> language = {
            .bString = {0x0409}
        };

        // manufacturer string descriptor
        const __attribute__((aligned(4))) static inline auto manufacturer = string_descriptor("KLIB");

        // product string descriptor
        const __attribute__((aligned(4))) static inline auto product = string_descriptor("KLIB Mass Storage");

        // serial number string descriptor
        const __attribute__((aligned(4))) static inline auto serial = string_descriptor("00001337");

        // configuration value. Value is set in the set config function
        static inline uint8_t configuration = 0x00;

    public:
        /**
         * @brief public functions for the user. This is not required
         * but is used in all the other drivers and should be provided
         * as a default
         *
         */

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
         * This part is for the optional functions
         */

        /**
         * @brief Called when the usb tranceiver wakes up
         *
         * @tparam Usb
         */
        template <typename Usb>
        static void wakeup() {
            // if not needed this function can be removed
        };

        /**
         * @brief Called when the usb tranceiver goes to sleep
         *
         * @tparam Usb
         */
        template <typename Usb>
        static void sleep() {
            // if not needed this function can be removed
        };

        /**
         * @brief Called when the host is disconnected
         *
         * @tparam Usb
         */
        template <typename Usb>
        static void disconnected() {
            configuration = 0x00;
        };

        /**
         * @brief Called when the usb has connected to a host
         *
         * @tparam Usb
         */
        template <typename Usb>
        static void connected() {
            // if not needed this function can be removed
        };

        /**
         * @brief Called when the usb tranceiver has detected activity
         *
         * @warning not all hardware supports this
         *
         * @tparam Usb
         */
        template <typename Usb>
        static void activity() {
            // if not needed this function can be removed
        };

        /**
         * @brief Called when a bus reset has occured
         *
         * @tparam Usb
         */
        template <typename Usb>
        static void bus_reset() {
            configuration = 0x00;
        };

        /**
         * @brief Called when get interface is received
         *
         * @tparam Usb
         * @param packet
         * @return usb::handshake
         */
        template <typename Usb>
        static usb::handshake get_interface(const klib::usb::setup_packet &packet) {
            return bot::template get_interface<Usb>(packet);
        }

        /**
         * @brief Called when set interface is received
         *
         * @tparam Usb
         * @param packet
         * @return usb::handshake
         */
        template <typename Usb>
        static usb::handshake set_interface(const klib::usb::setup_packet &packet) {
            return bot::template set_interface<Usb>(packet);
        }

        /**
         * @brief Called when a class specific packet is received
         *
         * @tparam Usb
         * @param packet
         */
        template <typename Usb>
        static usb::handshake handle_class_packet(const klib::usb::setup_packet &packet) {
            return bot::template handle_class_packet<Usb>(packet);
        }

    public:
        /**
         * @brief static functions needed for the usb stack. Should not
         * be called manually
         *
         * This part is is required for the usb stack to work. (it will
         * give compile errors when one of the function is missing)
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

            // make sure the endpoints support the bulk endpoint type
            static_assert(Usb::template is_valid_endpoint<
                usb::get_endpoint(InEndpoint),
                klib::usb::descriptor::transfer_type::bulk
            >(), "invalid bulk endpoint selected");

            static_assert(Usb::template is_valid_endpoint<
                usb::get_endpoint(OutEndpoint),
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
         * @brief Clear a feature on the device
         *
         * @tparam Usb
         * @param feature
         * @param packet
         */
        template <typename Usb>
        static usb::handshake clear_feature(const klib::usb::setup::feature feature, const klib::usb::setup_packet &packet) {
            // no features are supported. Stall to notify this
            return usb::handshake::stall;
        }

        /**
         * @brief Set a feature on the device
         *
         * @tparam Usb
         * @param feature
         * @param packet
         */
        template <typename Usb>
        static usb::handshake set_feature(const klib::usb::setup::feature feature, const klib::usb::setup_packet &packet) {
            // no features are supported. Stall to notify this
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
                case descriptor::descriptor_type::device_qualifier:
                    return to_description(qualifier, qualifier.bLength);
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
                    usb::get_transfer_type(config.endpoint0.bmAttributes),
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

                // init the bulk only transfer driver
                bot::template init<Usb>();

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

                // de-init the bulk only transfer driver
                bot::template de_init<Usb>();

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
            return 0;
        }
    };
}


#endif