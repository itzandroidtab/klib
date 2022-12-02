#ifndef KLIB_USB_KEYBOARD_HPP
#define KLIB_USB_KEYBOARD_HPP

#include <algorithm>

#include <klib/string.hpp>
#include <klib/usb/device.hpp>
#include <klib/usb/descriptor.hpp>
#include <klib/usb/hid/descriptor.hpp>

namespace klib::usb::device {
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

        /**
         * @brief Config descriptor for the hid keyboard
         * 
         * @details packed so we can write this whole descriptor 
         * to the usb hardware in one go.
         * 
         */
        struct __attribute__((packed)) config_descriptor {
            // configuration descriptor
            descriptor::configuration configuration;

            // interface descriptor
            descriptor::interface interface;

            // hid descriptor
            hid::descriptor hid;

            // endpoint descriptor
            descriptor::endpoint endpoint;
        };

        // device descriptor for the hid keyboard
        const __attribute__((aligned(4))) static inline descriptor::device device = {
            .bcdUSB = 0x0110,
            .bDeviceClass = 0x00,
            .bDeviceSubClass = 0x00,
            .bDeviceProtocol = 0x00,
            .bMaxPacketSize = 0x40,
            .idVendor = 0x0B6A,
            .idProduct = 0x003D,
            .bcdDevice = 0x0100,
            .iManufacturer = 0x01,
            .iProduct = 0x02,
            .iSerialNumber = 0x03,
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
        __attribute__((aligned(4)))
        const static inline config_descriptor config = {
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
                .bEndpointAddress = 0x83,
                .bmAttributes = 0x03,
                .wMaxPacketSize = 0x0040,
                .bInterval = 0x01
            }
        };

        // language descriptor for the keyboard
        const __attribute__((aligned(4))) static inline descriptor::string<1> language = {
            .bString = {0x0904}
        };

        // manufacturer string descriptor
        const __attribute__((aligned(4))) static inline descriptor::string<4> manufacturer = {
            .bString = {
                klib::bswap16('k'), klib::bswap16('l'), klib::bswap16('i'), klib::bswap16('b')
            }
        };

        // product string descriptor
        const __attribute__((aligned(4))) static inline descriptor::string<13> product = {
            .bString = {
                klib::bswap16('k'), klib::bswap16('l'), klib::bswap16('i'), klib::bswap16('b'), 
                klib::bswap16(' '), klib::bswap16('k'), klib::bswap16('e'), klib::bswap16('y'), 
                klib::bswap16('b'), klib::bswap16('o'), klib::bswap16('a'), klib::bswap16('r'), 
                klib::bswap16('d')
            }
        };

        // serial number string descriptor
        const __attribute__((aligned(4))) static inline descriptor::string<8> serial = {
            .bString = {
                klib::bswap16('0'), klib::bswap16('0'), klib::bswap16('0'), klib::bswap16('0'), 
                klib::bswap16('1'), klib::bswap16('3'), klib::bswap16('3'), klib::bswap16('7')
            }
        };

        // used endpoint for the keyboard
        static inline uint8_t used_endpoint = 0;

        // configuration value. Value is set in the set config function
        static inline uint8_t configuration_value = 0x00;

        // flag if we are configured
        static inline bool configured = false;

        // flag if the device is suspended
        static inline bool suspended = false;

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
        static void hid_callback(const uint8_t endpoint, const usb::error error_code) {
            // check if we are configured
            if (!configuration_value) {
                return;
            }

            if (!irq_size && irq_data == nullptr) {
                // we are done. return and do nothing
                return;
            }

            // check if we have more data to send
            if (!irq_size) {
                // we have nothing more to send. Send a empty report 
                // with no keypresses
                std::fill_n(report_data, sizeof(report_data), 0x00);

                // send the no key pressed to the host
                Usb::write(hid_callback<Usb>, used_endpoint, report_data, sizeof(report_data));

                // clear the data to notify we are done with the string
                irq_data = nullptr;

                // exit the callback
                return;
            }

            // check if the next character is the same as the last send character
            if (irq_size > 0 && irq_data[0] == irq_data[1] && repeated_key == false) {
                // we have detected a double character. Send a no keys pressed in between
                std::fill_n(report_data, sizeof(report_data), 0x00);

                // send the report to the host
                Usb::write(hid_callback<Usb>, used_endpoint, report_data, sizeof(report_data));

                // mark we have send a no keys between two repeated keys              
                repeated_key = true;

                // exit for now
                return;
            }

            // we still have data to send. Decrease the size and move the pointer
            irq_size--;
            irq_data++;

            // encode the next character
            encode_report(irq_data[0], report_data);

            // clear we have transmitted a no keys in between two repeated keys
            repeated_key = false;

            // send the report to the host
            Usb::write(hid_callback<Usb>, used_endpoint, report_data, sizeof(report_data));

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
        /**
         * @brief Init function. Called when the usb stack is initalized
         * 
         * @tparam Usb 
         */
        template <typename Usb>
        static void init() {
            // clear the used endpoint
            used_endpoint = 0;

            // init all the variables to default
            suspended = false;
            configured = false;
            remote_wakeup = false;

            // clear the strings we are sending
            irq_data = nullptr;
            irq_size = 0;
        }

        template <typename Usb>
        static bool write(const char *const data, const uint32_t size) {
            // check if we are configured
            if (!configuration_value) {
                return false;
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
            if (!Usb::write(hid_callback<Usb>, used_endpoint, report_data, sizeof(report_data))) {
                return false;
            }

            // wait until we are done transmitting all characters
            while (is_busy<Usb>()) {
                // do nothing
            }

            return true;
        }

        /**
         * @brief Write to the host. Buffer data is allocted in needs to stay valid 
         * until the hid keyboard is not busy anymore
         * 
         * @tparam Usb 
         * @param data 
         * @param size 
         * @return true 
         * @return false 
         */
        template <typename Usb>
        static bool write_async(const char *const data, const uint32_t size) {
            // check if we are configured
            if (!configuration_value) {
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
            return Usb::write(hid_callback<Usb>, used_endpoint, report_data, sizeof(report_data));
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
            used_endpoint = 0;
            configured = false;
        }

        /**
         * @brief Called when a bus reset has occured
         * 
         * @tparam Usb 
         */
        template <typename Usb>
        static void bus_reset() {
            // clear the used endpoint
            used_endpoint = 0;

            // clear all the variables to default
            configured = false;
            suspended = false;
        }

        /**
         * @brief Called when the usb tranceiver goes to sleep
         * 
         * @tparam Usb 
         */
        template <typename Usb>
        static void sleep() {
            suspended = true;
        }

        /**
         * @brief Called when the usb tranceiver wakes up
         * 
         * @tparam Usb 
         */
        template <typename Usb>
        static void wakeup() {
            suspended = false;
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
        static void clear_feature(const klib::usb::setup::feature feature, const klib::usb::setup_packet &packet) {
            // check if we support the feature
            if (feature != klib::usb::setup::feature::remote_wake) {
                // we only support remote wakeup
                Usb::stall(0);

                // do a early return
                return;
            }

            // disable remote wakeup
            remote_wakeup = false;

            // ack the packet
            Usb::ack(0);
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
        static void set_feature(const klib::usb::setup::feature feature, const klib::usb::setup_packet &packet) {
            // check if we support the feature
            if (feature != klib::usb::setup::feature::remote_wake) {
                // we only support remote wakeup
                Usb::stall(0);

                // do a early return
                return;
            }

            // enable remote wakeup
            remote_wakeup = true;

            // ack the packet
            Usb::ack(0);
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
                    return descriptor_to_desc(device, device.bLength);
                case descriptor::descriptor_type::configuration:
                    // return the whole configuration descriptor (total size is in 
                    // wTotalLength of the configuration)
                    return descriptor_to_desc(config, config.configuration.wTotalLength);
                case descriptor::descriptor_type::string:
                    // check what string descriptor to send
                    switch (static_cast<string_index>(index)) {
                        case string_index::language:
                            return descriptor_to_desc(language, language.bLength);
                        case string_index::manufacturer:
                            return descriptor_to_desc(manufacturer, manufacturer.bLength);
                        case string_index::product:
                            return descriptor_to_desc(product, product.bLength);
                        case string_index::serial:
                            return descriptor_to_desc(serial, serial.bLength);
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
                    return descriptor_to_desc(config.hid, config.hid.bLength);
                case class_type::report:
                    // return the raw report.
                    return descriptor_to_desc(report, sizeof(report));
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
        static void get_config(const klib::usb::setup_packet &packet) {
            // send the configuration back to the host
            const auto r = Usb::write(usb::status_callback<Usb>, 0, &configuration_value, sizeof(configuration_value));

            // check if something went wrong already
            if (!r) {
                // something went wrong stall
                Usb::stall(0);
            }

            // we do not ack here as the status callback 
            // will handle this for us
        }

        /**
         * @brief Set a configuration value
         * 
         * @tparam Usb 
         * @param packet 
         */
        template <typename Usb>
        static void set_config(const klib::usb::setup_packet &packet) {
            // check if the set is the same as the configuration we have stored
            if (packet.wValue == config.configuration.bConfigurationValue) {
                // get the endpoint we should configure
                used_endpoint = config.endpoint.bEndpointAddress & 0x0f;

                // configure the endpoint for our report data
                Usb::configure(used_endpoint, usb::endpoint_mode::in, sizeof(report_data));

                // store the configuration value
                configuration_value = packet.wValue;
                configured = true;

                // prepare a inital report with no keys pressed
                std::fill_n(report_data, sizeof(report_data), 0x00);

                // write the inital report
                if (Usb::write(hid_callback<Usb>, used_endpoint, report_data, sizeof(report_data))) {
                    // no issue for now ack
                    Usb::ack(0);
                }
                else {
                    // something went wrong stall for now
                    Usb::stall(0);
                }
            }
            else if (packet.wValue == 0) {
                // clear the configuration value
                configuration_value = 0x00;

                // reset the used endpoint if we have one
                if (used_endpoint != 0) {
                    // reset the endpoint
                    Usb::reset(used_endpoint);

                    // clear the used endpoint
                    used_endpoint = 0;
                } 

                // ack the packet
                Usb::ack(0);
            }
            else {
                // not sure what to do, stall
                Usb::stall(0);
            }
        }

        /**
         * @brief Get the interface object
         * 
         * @warning needs to stall when not supported
         * 
         * @tparam Usb 
         * @param packet 
         */
        template <typename Usb>
        static void get_interface(const klib::usb::setup_packet &packet) {
            // get interface is not supported. Stall
            Usb::stall(0);
        }

        /**
         * @brief Set the interface object
         * 
         * @warning needs to stall when not supported
         * 
         * @tparam Usb 
         * @param packet 
         */
        template <typename Usb>
        static void set_interface(const klib::usb::setup_packet &packet) {
            // set interface is not supported. Stall
            Usb::stall(0);
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
        static void handle_class_packet(const klib::usb::setup_packet &packet) {
            // check if we have a valid class packet
            if (usb::get_recipient(packet) != setup::recipient_code::interface || (packet.wIndex != 0)) {
                // packet is invalid. stall the packet
                Usb::stall(0);

                // do a early return
                return;
            }

            // convert the request to a class request
            const auto& request = static_cast<class_request>(packet.bRequest);

            // check what requets we got
            switch (request) {
                case class_request::get_report:
                    // the host should not use this as a substitute for the Interrupt EP
                    // we simply send a "no keys" report to the host
                    std::fill_n(report_data, sizeof(report_data), 0x00);

                    // write the data to the control endpoint
                    if (Usb::write(nullptr, 0, report_data, sizeof(report_data))) {
                        // no issue for now ack
                        Usb::ack(0);
                    }
                    else {
                        // something went wrong stall for now
                        Usb::stall(0);
                    }
                    break;
                case class_request::get_idle:
                    // TODO: add support for report id != 0
                    // for now we only support report id == 0
                    if ((packet.wValue & 0xff) != 0x00) {
                        // not supported for now
                        Usb::stall(0);
                    }
                    else {
                        // send 0 to the host
                        const uint8_t idle = 0;

                        // write the data to the control endpoint
                        if (Usb::write(nullptr, 0, report_data, sizeof(report_data))) {
                            // no issue for now ack
                            Usb::ack(0);
                        }
                        else {
                            // something went wrong stall for now
                            Usb::stall(0);
                        }
                    }   
                    break;
                case class_request::set_report:
                    // check if the packet length is not above the max endpoint size
                    if (packet.wLength > Usb::max_endpoint_size) {
                        // invalid length
                        Usb::stall(0);
                    }
                    else {
                        // accept and ignore the report
                        Usb::ack(0);
                    }
                    break;
                case class_request::set_idle:
                    // TODO: add support for report id != 0
                    // for now we only support report id == 0 and we do not 
                    // support changing the idle
                    if ((packet.wValue & 0xff) != 0 || (packet.wValue >> 8) != 0) {
                        Usb::stall(0);
                    }
                    else {
                        // ack and ignore
                        Usb::ack(0);
                    }
                    break;
                case class_request::get_protocol:
                case class_request::set_protocol:
                default:
                    // not supported. stall
                    Usb::stall(0);
                    break;
            }
        }

        /**
         * @brief Handle a vendor packet.
         * 
         * @warning needs to stall when not supported
         * 
         * @tparam Usb 
         * @param packet 
         */
        template <typename Usb>
        static void handle_vendor_packet(const klib::usb::setup_packet &packet) {
            // not supported
            Usb::stall(0);
        }
    };
}

#endif