#ifndef KLIB_USB_MOUSE_HPP
#define KLIB_USB_MOUSE_HPP

#include <algorithm>

#include <klib/string.hpp>
#include <klib/usb/device.hpp>
#include <klib/usb/descriptor.hpp>
#include <klib/usb/hid/descriptor.hpp>

namespace klib::usb::device {
    class mouse_hid {
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

        /**
         * @brief Class specific descriptors
         * 
         */
        enum class class_type {
            hid = 0x21,
            report = 0x22
        };

        /**
         * @brief Class specific requests for the handle class packet function
         * 
         */
        enum class class_request {
            get_report = 0x01,
            get_idle = 0x02,
            get_protocol = 0x03,
            set_report = 0x09,
            set_idle = 0x0a,
            set_protocol = 0x0b,
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

            // hid descriptor
            hid::descriptor hid;

            // endpoint descriptor
            descriptor::endpoint endpoint;
        };

        // release the old pack so the rest of the structs are not 
        // affected by the pack(1)
        #pragma pack(pop)

        // device descriptor for the hid keyboard
        const __attribute__((aligned(4))) static inline descriptor::device device = {
            .bcdUSB = 0x0110,
            .bDeviceClass = 0x00,
            .bDeviceSubClass = 0x00,
            .bDeviceProtocol = 0x00,
            .bMaxPacketSize = 0x40,
            .idVendor = 0x0B6A,
            .idProduct = 0x003F,
            .bcdDevice = 0x0100,
            .iManufacturer = 0x01,
            .iProduct = 0x02,
            .iSerialNumber = 0x03,
            .bNumConfigurations = 0x1
        };

        // report descriptor for the hid mouse with 2 axis and 8 buttons
        const __attribute__((aligned(4))) static inline uint8_t report[] = {
            0x05, 0x01, // USAGE_PAGE (Generic Desktop) 
            0x09, 0x02, // USAGE (Mouse)
            0xA1, 0x01, // COLLECTION (Application)
            0x09, 0x01, //   USAGE (Pointer)
            0xA1, 0x00, //   COLLECTION (Physical)
            0x05, 0x09, //     USAGE_PAGE (Button)
            0x19, 0x01, //       USAGE_MINIMUM (Button 1)
            0x29, 0x08, //       USAGE_MAXIMUM (Button 8)
            0x15, 0x00, //       LOGICAL_MINIMUM (0)
            0x25, 0x01, //       LOGICAL_MAXIMUM (1)
            0x95, 0x08, //       REPORT_COUNT (8)
            0x75, 0x01, //       REPORT_SIZE (1)
            0x81, 0x02, //     INPUT (Data,Var,Abs)
            0x05, 0x01, //     USAGE_PAGE (Generic Desktop)
            0x09, 0x30, //       USAGE (X)
            0x09, 0x31, //       USAGE (Y)
            0x15, 0x81, //       LOGICAL_MINIMUM (-127)
            0x25, 0x7F, //       LOGICAL_MAXIMUM (127)
            0x75, 0x08, //       REPORT_SIZE (8)
            0x95, 0x02, //       REPORT_COUNT (2)
            0x81, 0x06, //     INPUT (Data,Var,Rel)
            0xC0, //         END_COLLECTION
            0xC0, //       END_COLLECTION
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
                .bNumEndpoints = 0x01,
                .bInterfaceClass = 0x03,
                .bInterfaceSubClass = 0x00,
                .bInterfaceProtocol = 0x00,
                .iInterface = 0x00
            },
            {
                .bDescriptorType = 0x21,
                .bcdHID = 0x0110,
                .bCountryCode = 0x00,
                .bNumDescriptors = 0x01,
                .bClassDescriptorType = static_cast<uint8_t>(class_type::report),
                .wDescriptorLength = sizeof(report)
            },
            {
                .bEndpointAddress = 0x84,
                .bmAttributes = 0x03,
                .wMaxPacketSize = 0x0040,
                .bInterval = 0x01
            }
        };

        // language descriptor for the keyboard
        const __attribute__((aligned(4))) static inline descriptor::string<1> language = {
            .bString = {0x0409}
        };

        // manufacturer string descriptor
        const __attribute__((aligned(4))) static inline descriptor::string<4> manufacturer = {
            .bString = {
                'k', 'l', 'i', 'b'
            }
        };

        // product string descriptor
        const __attribute__((aligned(4))) static inline descriptor::string<10> product = {
            .bString = {
                'k', 'l', 'i', 'b', ' ', 'm', 'o', 'u', 's', 'e'
            }
        };

        // serial number string descriptor
        const __attribute__((aligned(4))) static inline descriptor::string<8> serial = {
            .bString = {
                '0', '0', '0', '0', '1', '3', '3', '7'
            }
        };

        // used endpoint for the keyboard
        static inline uint8_t used_endpoint = 0;

        // configuration value. Value is set in the set config function
        static inline uint8_t configuration_value = 0x00;

        // mouse report structure
        struct mouse_report {
            uint8_t buttons;
            int8_t x;
            int8_t y;
        };

        // storage for the keyboard hid messages
        static inline mouse_report report_data = {};

        // flag for if we have data updating
        static inline volatile bool data_updating = false;

        /**
         * @brief Callback that can send more keypresses or will send no keys
         * 
         * @tparam Usb 
         * @param data 
         */
        template <typename Usb>
        static void hid_callback(const uint8_t endpoint, const usb::endpoint_mode mode, const usb::error error_code) {
            // only continue if we do not have any errors
            if (error_code != usb::error::no_error) {
                // we have a error
                return;
            }
            
            // check if we are configured
            if (!configuration_value) {
                return;
            }

            data_updating = false;
        }

    public:
        /**
         * @brief Init function. Called when the usb stack is initalized
         * 
         * @tparam Usb 
         */
        template <typename Usb>
        static void init() {

        }

        template <typename Usb, bool Async = true>
        static bool write(uint8_t buttons, int8_t x, int8_t y) {
            if (!configuration_value) {
                return false;
            }

            if constexpr (Async) {
                while (is_busy<Usb>()) {
                    // wait until we are done processing
                }
            }

            report_data.buttons = buttons;
            report_data.x = x;
            report_data.x = x;

            data_updating = true;

            // send the report to the host
            Usb::write(hid_callback<Usb>, used_endpoint, usb::endpoint_mode::in, 
                reinterpret_cast<const uint8_t*>(&report_data), sizeof(report_data)
            );

            if constexpr (!Async) {
                while (is_busy<Usb>()) {
                    // wait until we are done processing
                }
            }            

            return true;
        }

        /**
         * @brief Returns if a previous request is still busy.
         * 
         * @tparam Usb 
         * @return true 
         * @return false 
         */
        template <typename Usb>
        static bool is_busy() {
            // return if the endpoint is still pending
            return data_updating;
        }

    public:
        /**
         * @brief static functions needed for the usb stack. Should not 
         * be called manually
         * 
         */

        /**
         * @brief Called when the usb has connected to a host
         * 
         * @tparam Usb 
         */
        template <typename Usb>
        static void connected() {
            // do nothing on a connect event
        }

        /**
         * @brief Called when the host is disconnected
         * 
         * @tparam Usb 
         */
        template <typename Usb>
        static void disconnected() {

        }

        /**
         * @brief Called when a bus reset has occured
         * 
         * @tparam Usb 
         */
        template <typename Usb>
        static void bus_reset() {

        }

        /**
         * @brief Called when the usb tranceiver goes to sleep
         * 
         * @tparam Usb 
         */
        template <typename Usb>
        static void sleep() {
            
        }

        /**
         * @brief Called when the usb tranceiver wakes up
         * 
         * @tparam Usb 
         */
        template <typename Usb>
        static void wakeup() {

        }

        /**
         * @brief Called when the usb tranceiver has detected activity
         * 
         * @tparam Usb 
         */
        template <typename Usb>
        static void activity() {
            // do nothing on a activity packet
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

            // check if we have a class specific descriptor
            switch (static_cast<class_type>(type)) {
                case class_type::hid:
                    // return the hid descriptor only
                    return to_description(config.hid, config.hid.bLength);
                case class_type::report:
                    // return the raw report.
                    return to_description(report, sizeof(report));
                default:
                    // unkown class descriptor
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
                usb::endpoint_mode::in, &configuration_value, 
                sizeof(configuration_value)
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
                // get the endpoint we should configure
                used_endpoint = config.endpoint.bEndpointAddress & 0x0f;

                // configure the endpoint for our report data
                Usb::configure(used_endpoint, usb::endpoint_mode::in, sizeof(report_data));

                // store the configuration value
                configuration_value = packet.wValue;

                // notify the usb driver we are configured
                Usb::configured(true);

                // prepare a inital report with no keys pressed and no mouse movement
                report_data = {};

                // write the inital report
                if (Usb::write(hid_callback<Usb>, used_endpoint, usb::endpoint_mode::in, 
                    reinterpret_cast<const uint8_t*>(&report_data), sizeof(report_data))) 
                {
                    // no issue for now ack
                    return usb::handshake::ack;
                }
                else {
                    // something went wrong stall for now
                    return usb::handshake::stall;
                }
            }
            else if (packet.wValue == 0) {
                // clear the configuration value
                configuration_value = 0x00;

                // notify the usb driver we are not configured anymore
                Usb::configured(false);

                // reset the used endpoint if we have one
                if (used_endpoint != 0) {
                    // reset the endpoint
                    Usb::reset(used_endpoint, usb::endpoint_mode::in);

                    // clear the used endpoint
                    used_endpoint = 0;
                } 

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

        /**
         * @brief Called when a class specific packet is received
         * 
         * @warning Function needs to stall and ack
         * 
         * @tparam Usb 
         * @param packet 
         */
        template <typename Usb>
        static usb::handshake handle_class_packet(const klib::usb::setup_packet &packet) {
            // check if we have a valid class packet
            if (usb::get_recipient(packet) != setup::recipient_code::interface || (packet.wIndex != 0)) {
                // packet is invalid. stall the packet
                return usb::handshake::stall;
            }

            // convert the request to a class request
            const auto& request = static_cast<class_request>(packet.bRequest);

            // check what requets we got
            switch (request) {
                case class_request::get_report:
                    // the host should not use this as a substitute for the Interrupt EP
                    // but send the data we have anyway

                    // write the data to the control endpoint
                    if (Usb::write(
                            nullptr, usb::control_endpoint, usb::endpoint_mode::in, 
                            reinterpret_cast<const uint8_t*>(&report_data), 
                            sizeof(report_data))) 
                    {
                        // no issue for now ack
                        return usb::handshake::ack;
                    }
                    else {
                        // something went wrong stall for now
                        return usb::handshake::stall;
                    }
                    break;
                case class_request::get_idle:
                    // TODO: add support for report id != 0
                    // for now we only support report id == 0
                    if ((packet.wValue & 0xff) != 0x00) {
                        // not supported for now
                        return usb::handshake::stall;
                    }
                    else {
                        // send 0 to the host
                        const uint8_t idle = 0;

                        // write the data to the control endpoint
                        if (Usb::write(
                            nullptr, usb::control_endpoint, usb::endpoint_mode::in, 
                            reinterpret_cast<const uint8_t*>(&report_data),
                            sizeof(report_data))) 
                        {
                            // no issue for now ack
                            return usb::handshake::ack;
                        }
                        else {
                            // something went wrong stall for now
                            return usb::handshake::stall;
                        }
                    }   
                    break;
                case class_request::set_report:
                    // check if the packet length is not above the max endpoint size
                    if (packet.wLength > Usb::max_endpoint_size) {
                        // invalid length
                        return usb::handshake::stall;
                    }
                    else {
                        // accept and ignore the report
                        return usb::handshake::ack;
                    }
                    break;
                case class_request::set_idle:
                    // TODO: add support for report id != 0
                    // for now we only support report id == 0 and we do not 
                    // support changing the idle
                    if ((packet.wValue & 0xff) != 0 || (packet.wValue >> 8) != 0) {
                        return usb::handshake::stall;
                    }
                    else {
                        // ack and ignore
                        return usb::handshake::ack;
                    }
                    break;
                case class_request::get_protocol:
                case class_request::set_protocol:
                default:
                    // not supported. stall
                    return usb::handshake::stall;
            }
        }
    };
}


#endif