#ifndef KLIB_USB_KEYBOARD_HPP
#define KLIB_USB_KEYBOARD_HPP

#include <algorithm>

#include <klib/string.hpp>
#include <klib/usb/usb/device.hpp>
#include <klib/usb/usb/descriptor.hpp>
#include <klib/usb/usb/hid/descriptor.hpp>

namespace klib::usb::device {
    template <uint32_t Endpoint = 6>
    class keyboard_hid {
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
            .bcdUSB = static_cast<uint16_t>(setup::usb_version::usb_v1_1),
            .bDeviceClass = descriptor::class_type::use_class_info,
            .bDeviceSubClass = 0x00,
            .bDeviceProtocol = 0x00,
            .bMaxPacketSize = 0x40,
            .idVendor = 0x0B6A,
            .idProduct = 0x003D,
            .bcdDevice = 0x0100,
            .iManufacturer = static_cast<uint8_t>(string_index::manufacturer),
            .iProduct = static_cast<uint8_t>(string_index::product),
            .iSerialNumber = static_cast<uint8_t>(string_index::serial),
            .bNumConfigurations = 0x1
        };

        // report descriptor for the hid keyboard
        const __attribute__((aligned(4))) static inline uint8_t report[] = {
            0x05, 0x01,   //  Usage Page (generic desktop)
            0x09, 0x06,   //  Usage (keyboard)
            0xa1, 0x01,   //  Collection
            0x05, 0x07,   //    Usage Page 7 (keyboard/keypad)
            0x19, 0xe0,   //    Usage Minimum = 224
            0x29, 0xe7,   //    Usage Maximum = 231
            0x15, 0x00,   //    Logical Minimum = 0
            0x25, 0x01,   //    Logical Maximum = 1
            0x75, 0x01,   //    Report Size = 1
            0x95, 0x08,   //    Report Count = 8
            0x81, 0x02,   //   Input(Data,Variable,Absolute)
            0x95, 0x01,   //    Report Count = 1
            0x75, 0x08,   //    Report Size = 8
            0x81, 0x01,   //   Input(Constant)
            0x19, 0x00,   //    Usage Minimum = 0
            0x29, 0x65,   //    Usage Maximum = 101
            0x15, 0x00,   //    Logical Minimum = 0
            0x25, 0x65,   //    Logical Maximum = 101
            0x75, 0x08,   //    Report Size = 8
            0x95, 0x01,   //    Report Count = 1
            0x81, 0x00,   //   Input(Data,Variable,Array)
            0xc0          //  End Collection
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
                .bDescriptorType = hid::class_type::hid,
                .bcdHID = 0x0110,
                .bCountryCode = 0x00,
                .bNumDescriptors = 0x01,
                .bClassDescriptorType = hid::class_type::report,
                .wDescriptorLength = sizeof(report)
            },
            {
                .bEndpointAddress = 0x80 | Endpoint,
                .bmAttributes = static_cast<uint8_t>(klib::usb::descriptor::transfer_type::interrupt),
                .wMaxPacketSize = 0x0040,
                .bInterval = 0x01
            }
        };

        // language descriptor for the keyboard
        const __attribute__((aligned(4))) static inline descriptor::string<1> language = {
            .bString = {0x0409}
        };

        // manufacturer string descriptor
        const __attribute__((aligned(4))) static inline auto manufacturer = string_descriptor("KLIB");

        // product string descriptor
        const __attribute__((aligned(4))) static inline auto product = string_descriptor("KLIB Keyboard");

        // serial number string descriptor
        const __attribute__((aligned(4))) static inline auto serial = string_descriptor("00001337");

        // configuration value. Value is set in the set config function
        static inline uint8_t configuration = 0x00;

        // flag if remote wakeup is supported
        static inline bool remote_wakeup = false;

        // storage for the keyboard hid messages
        static inline uint8_t report_data[3] = {};

        // static parameters with data to write to the host
        static inline const char *volatile irq_data = nullptr;
        static inline volatile uint32_t irq_size = 0;
        static inline bool repeated_key = false;

        /**
         * @brief Callback that can send more keypresses or will send no keys
         *
         * @tparam Usb
         * @param data
         */
        template <typename Usb>
        static void hid_callback(const uint8_t endpoint, const usb::endpoint_mode mode, const usb::error error_code, const uint32_t transferred) {
            // only continue if we do not have any errors
            if (error_code != usb::error::no_error) {
                // we have a error
                return;
            }

            // check if we are configured
            if (!configuration) {
                return;
            }

            // check if we still have a valid pointer and no data left to send
            if ((!irq_size) && (irq_data != nullptr)) {
                // we have nothing more to send. Send a empty report
                // with no keypresses
                std::fill_n(report_data, sizeof(report_data), 0x00);

                // send the no key pressed to the host
                Usb::write(
                    hid_callback<Usb>, usb::get_endpoint(config.endpoint.bEndpointAddress),
                    mode, report_data
                );

                // clear the data to notify we are done with the string
                irq_data = nullptr;

                // exit the callback
                return;
            }

            // do not continue if we do not have any valid data or
            // if we do not have any size left to send
            if ((irq_data == nullptr) || (!irq_size)) {
                // exit as we do not have anything left to send
                return;
            }

            // check if the next character is the same as the last send character
            if (irq_size > 0 && irq_data[0] == irq_data[1] && repeated_key == false) {
                // we have detected a double character. Send a no keys
                // pressed in between
                std::fill_n(report_data, sizeof(report_data), 0x00);

                // send the report to the host
                Usb::write(
                    hid_callback<Usb>, usb::get_endpoint(config.endpoint.bEndpointAddress),
                    mode, report_data
                );

                // mark we have send a no keys between two repeated keys
                repeated_key = true;

                // exit for now
                return;
            }

            // we still have data to send. Decrease the size and move
            // the pointer
            irq_size--;
            irq_data++;

            // encode the next character
            encode_report(irq_data[0], report_data);

            // clear we have transmitted a no keys in between two repeated keys
            repeated_key = false;

            // send the report to the host
            Usb::write(
                hid_callback<Usb>, usb::get_endpoint(config.endpoint.bEndpointAddress),
                mode, report_data
            );

            // return we are done
            return;
        }

        static void encode_report(const char ch, uint8_t report[sizeof(report_data)]) {
            // check what data we have
            if (klib::string::is_character(ch)) {
                // we have a character. re-align to hid codes
                if (klib::string::is_upper(ch)) {
                    report[0] = 0x02;
                    report[1] = 0x00;
                    report[2] = ch - 0x3d;
                }
                else {
                    report[0] = 0x00;
                    report[1] = 0x00;
                    report[2] = klib::string::to_upper(ch) - 0x3d;
                }

                return;
            }

            if (klib::string::is_digit(ch)) {
                // set the first two bytes of the report
                report[0] = 0x00;
                report[1] = 0x00;

                // we have a number
                if (ch == '0') {
                    report[2] = 0x27;
                }
                else {
                    report[2] = ch - 0x13;
                }

                return;
            }

            // temporary variables to store the code and the shift
            uint8_t code;
            uint8_t shift = 0x00;

            // handle some of the other characters
            switch (ch) {
                case '\r':
                case '\n':
                    code = 0x28;
                    break;
                case '\b':
                    code = 0x2a;
                    break;
                case '\e':
                    code = 0x29;
                    break;
                case '\t':
                    code = 0x2b;
                    break;
                case ' ':
                    code = 0x2c;
                    break;
                case ',':
                    code = 0x36;
                    break;
                case '.':
                    code = 0x37;
                case '!':
                    shift = 0x02;
                    code = 0x1e;
                default:
                    // all others are mapped to a question mark
                    shift = 0x02;
                    code = 0x38;
                    break;
            }

            // set the code and the shift
            report[0] = shift;
            report[2] = code;

            // always clear the 2nd byte
            report[1] = 0x00;
        }

    public:
        template <typename Usb, bool Async = false>
        static bool write(const char *const data, const uint32_t size) {
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
            if (is_busy<Usb>()) {
                // a other request is already pending exit
                return false;
            }

            // encode the first report
            encode_report(data[0], report_data);

            // set the data in the interrupt
            irq_size = size - 1;
            irq_data = data;

            // write the first report to the endpoint
            if (!Usb::write(hid_callback<Usb>, usb::get_endpoint(config.endpoint.bEndpointAddress),
                usb::endpoint_mode::in, report_data))
            {
                return false;
            }

            // check if we should exit
            if constexpr (Async) {
                return true;
            }

            // wait until we are done transmitting all characters
            while (is_busy<Usb>()) {
                // do nothing
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
            return irq_data != nullptr;
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
            remote_wakeup = false;

            // clear the strings we are sending
            irq_data = nullptr;
            irq_size = 0;

            // make sure the endpoint supports interrupt
            // endpoints
            Usb::template is_valid_endpoint<
                Endpoint,
                klib::usb::descriptor::transfer_type::interrupt
            >();
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

            // check if we have a class specific descriptor
            switch (static_cast<hid::class_type>(type)) {
                case hid::class_type::hid:
                    // return the hid descriptor only
                    return to_description(config.hid, config.hid.bLength);
                case hid::class_type::report:
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
                // configure the endpoint for our report data
                Usb::configure(
                    usb::get_endpoint(config.endpoint.bEndpointAddress),
                    usb::endpoint_mode::in,
                    usb::get_transfer_type(config.endpoint.bmAttributes),
                    sizeof(report_data)
                );

                // store the configuration value
                configuration = packet.wValue;

                // notify the usb driver we are configured
                Usb::configured(true);

                // prepare a inital report with no keys pressed
                std::fill_n(report_data, sizeof(report_data), 0x00);

                // write the inital report
                if (Usb::write(hid_callback<Usb>, usb::get_endpoint(config.endpoint.bEndpointAddress),
                    usb::endpoint_mode::in, report_data))
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
                // notify the usb driver we are not configured anymore
                Usb::configured(false);

                // reset the used endpoint if we have one
                if (configuration != 0) {
                    // reset the endpoint
                    Usb::reset(
                        usb::get_endpoint(config.endpoint.bEndpointAddress),
                        usb::endpoint_mode::in
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

        /**
         * @brief Called when a class specific packet is received
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
            const auto& request = static_cast<hid::class_request>(packet.bRequest);

            // check what requets we got
            switch (request) {
                case hid::class_request::get_report:
                    // the host should not use this as a substitute for the Interrupt EP
                    // we simply send a "no keys" report to the host
                    std::fill_n(report_data, sizeof(report_data), 0x00);

                    // write the data to the control endpoint
                    if (Usb::write(nullptr, usb::control_endpoint,
                        usb::endpoint_mode::in, report_data))
                    {
                        // no issue for now ack
                        return usb::handshake::ack;
                    }
                    else {
                        // something went wrong stall for now
                        return usb::handshake::stall;
                    }
                    break;
                case hid::class_request::get_idle:
                    // TODO: add support for report id != 0
                    // for now we only support report id == 0
                    if ((packet.wValue & 0xff) != 0x00) {
                        // not supported for now
                        return usb::handshake::stall;
                    }
                    else {
                        // write the data to the control endpoint
                        if (Usb::write(nullptr, usb::control_endpoint, usb::endpoint_mode::in,
                            report_data))
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
                case hid::class_request::set_report:
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
                case hid::class_request::set_idle:
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
                case hid::class_request::get_protocol:
                case hid::class_request::set_protocol:
                default:
                    // not supported. stall
                    return usb::handshake::stall;
            }
        }
    };
}

#endif