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
    public:
        /**
         * @brief All the individual keys we support
         * 
         */
        enum class key_t: uint8_t {
            key_none = 0x00,
            key_a = 0x04,
            key_b = 0x05,
            key_c = 0x06,
            key_d = 0x07,
            key_e = 0x08,
            key_f = 0x09,
            key_g = 0x0a,
            key_h = 0x0b,
            key_i = 0x0c,
            key_j = 0x0d,
            key_k = 0x0e,
            key_l = 0x0f,
            key_m = 0x10,
            key_n = 0x11,
            key_o = 0x12,
            key_p = 0x13,
            key_q = 0x14,
            key_r = 0x15,
            key_s = 0x16,
            key_t = 0x17,
            key_u = 0x18,
            key_v = 0x19,
            key_w = 0x1a,
            key_x = 0x1b,
            key_y = 0x1c,
            key_z = 0x1d,
            key_1 = 0x1e,
            key_2 = 0x1f,
            key_3 = 0x20,
            key_4 = 0x21,
            key_5 = 0x22,
            key_6 = 0x23,
            key_7 = 0x24,
            key_8 = 0x25,
            key_9 = 0x26,
            key_0 = 0x27,
            key_enter = 0x28,
            key_escape = 0x29,
            key_backspace = 0x2a,
            key_tab = 0x2b,
            key_space = 0x2c,
            key_minus = 0x2d,
            key_equal = 0x2e,
            key_bracket_left = 0x2f,
            key_bracket_right = 0x30,
            key_backslash = 0x31,
            key_europe_1 = 0x32,
            key_semicolon = 0x33,
            key_apostrophe = 0x34,
            key_grave = 0x35,
            key_comma = 0x36,
            key_period = 0x37,
            key_slash = 0x38,
            key_caps_lock = 0x39,
            key_f1 = 0x3a,
            key_f2 = 0x3b,
            key_f3 = 0x3c,
            key_f4 = 0x3d,
            key_f5 = 0x3e,
            key_f6 = 0x3f,
            key_f7 = 0x40,
            key_f8 = 0x41,
            key_f9 = 0x42,
            key_f10 = 0x43,
            key_f11 = 0x44,
            key_f12 = 0x45,
            key_print_screen = 0x46,
            key_scroll_lock = 0x47,
            key_pause = 0x48,
            key_insert = 0x49,
            key_home = 0x4a,
            key_page_up = 0x4b,
            key_delete = 0x4c,
            key_end = 0x4d,
            key_page_down = 0x4e,
            key_right = 0x4f,
            key_left = 0x50,
            key_down = 0x51,
            key_up = 0x52,
            key_num_lock = 0x53,
            key_keypad_divide = 0x54,
            key_keypad_multiply = 0x55,
            key_keypad_subtract = 0x56,
            key_keypad_add = 0x57,
            key_keypad_enter = 0x58,
            key_keypad_1 = 0x59,
            key_keypad_2 = 0x5a,
            key_keypad_3 = 0x5b,
            key_keypad_4 = 0x5c,
            key_keypad_5 = 0x5d,
            key_keypad_6 = 0x5e,
            key_keypad_7 = 0x5f,
            key_keypad_8 = 0x60,
            key_keypad_9 = 0x61,
            key_keypad_0 = 0x62,
            key_keypad_decimal = 0x63,
            key_europe_2 = 0x64,
            key_application = 0x65,
            key_power = 0x66,
            key_keypad_equal = 0x67,
            key_f13 = 0x68,
            key_f14 = 0x69,
            key_f15 = 0x6a,
            key_f16 = 0x6b,
            key_f17 = 0x6c,
            key_f18 = 0x6d,
            key_f19 = 0x6e,
            key_f20 = 0x6f,
            key_f21 = 0x70,
            key_f22 = 0x71,
            key_f23 = 0x72,
            key_f24 = 0x73,
            key_execute = 0x74,
            key_help = 0x75,
            key_menu = 0x76,
            key_select = 0x77,
            key_stop = 0x78,
            key_again = 0x79,
            key_undo = 0x7a,
            key_cut = 0x7b,
            key_copy = 0x7c,
            key_paste = 0x7d,
            key_find = 0x7e,
            key_mute = 0x7f,
            key_volume_up = 0x80,
            key_volume_down = 0x81,
            key_locking_caps_lock = 0x82,
            key_locking_num_lock = 0x83,
            key_locking_scroll_lock = 0x84,
            key_keypad_comma = 0x85,
            key_keypad_equal_sign = 0x86,
            key_kanji1 = 0x87,
            key_kanji2 = 0x88,
            key_kanji3 = 0x89,
            key_kanji4 = 0x8a,
            key_kanji5 = 0x8b,
            key_kanji6 = 0x8c,
            key_kanji7 = 0x8d,
            key_kanji8 = 0x8e,
            key_kanji9 = 0x8f,
            key_lang1 = 0x90,
            key_lang2 = 0x91,
            key_lang3 = 0x92,
            key_lang4 = 0x93,
            key_lang5 = 0x94,
            key_lang6 = 0x95,
            key_lang7 = 0x96,
            key_lang8 = 0x97,
            key_lang9 = 0x98,
            key_alternate_erase = 0x99,
            key_sysreq_attention = 0x9a,
            key_cancel = 0x9b,
            key_clear = 0x9c,
            key_prior = 0x9d,
            key_return = 0x9e,
            key_separator = 0x9f,
            key_out = 0xa0,
            key_oper = 0xa1,
            key_clear_again = 0xa2,
            key_crsel_props = 0xa3,
            key_exsel = 0xa4,
            key_control_left = 0xe0,
            key_shift_left = 0xe1,
            key_alt_left = 0xe2,
            key_gui_left = 0xe3,
            key_control_right = 0xe4,
            key_shift_right = 0xe5,
            key_alt_right = 0xe6,
            key_gui_right = 0xe7,
        };

        static_assert(sizeof(key_t) == sizeof(uint8_t), "invalid keys size");

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
         * @brief All the report ids we have in our descriptor
         * 
         */
        enum class report_id: uint8_t {
            keyboard = 1,
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
        const __attribute__((aligned(4))) static inline uint8_t report_desc[] = {
            // Keyboard report (Report ID 1)
            0x05, 0x01,        // Usage Page (Generic Desktop)
            0x09, 0x06,        // Usage (Keyboard)
            0xa1, 0x01,        // Collection (Application)
                0x85, 0x01,        // Report ID (1)
                0x05, 0x07,        // Usage Page (Keyboard/Keypad)
                0x19, 0xe0,        // Usage Minimum (224)
                0x29, 0xe7,        // Usage Maximum (231)
                0x15, 0x00,        // Logical Minimum (0)
                0x25, 0x01,        // Logical Maximum (1)
                0x75, 0x01,        // Report Size (1)
                0x95, 0x08,        // Report Count (8)
                0x81, 0x02,        // Input (Data, Variable, Absolute) ; Modifier byte
                0x19, 0x00,        // Usage Minimum (0)
                0x29, 0x65,        // Usage Maximum (101)
                0x15, 0x00,        // Logical Minimum (0)
                0x25, 0x65,        // Logical Maximum (101)
                0x75, 0x08,        // Report Size (8)
                0x95, 0x01,        // Report Count (1)
                0x81, 0x00,        // Input (Data, Array) ; Keycode byte
            0xc0,              // End Collection
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
                .wDescriptorLength = sizeof(report_desc)
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
        static inline volatile uint8_t configuration = 0x00;

        // flag if remote wakeup is supported
        static inline bool remote_wakeup = false;

        // Push the current pack to the stack and set the pack to 1
        // as the following structs have specific sizes
        #pragma pack(push, 1)

        struct keycode_report_t {
            // fixed report id for the keyboard
            const report_id id = report_id::keyboard;

            // key data
            uint8_t modifier;
            key_t key;
        };

        static_assert(sizeof(keycode_report_t) == 3, "invalid keyboard report size");

        // release the old pack so the rest of the structs are not
        // affected by the pack(1)
        #pragma pack(pop)

        // storage for the keyboard hid messages
        static inline keycode_report_t keycode_report = {};

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
                keycode_report.modifier = 0x00;
                keycode_report.key = key_t::key_none;

                // send the no key pressed to the host
                Usb::write(
                    hid_callback<Usb>, usb::get_endpoint(config.endpoint.bEndpointAddress),
                    mode, {reinterpret_cast<const uint8_t*>(&keycode_report), sizeof(keycode_report)}
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
                keycode_report.modifier = 0x00;
                keycode_report.key = key_t::key_none;

                // send the report to the host
                Usb::write(
                    hid_callback<Usb>, usb::get_endpoint(config.endpoint.bEndpointAddress),
                    mode, {reinterpret_cast<const uint8_t*>(&keycode_report), sizeof(keycode_report)}
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
            encode_report(irq_data[0], keycode_report);

            // clear we have transmitted a no keys in between two repeated keys
            repeated_key = false;

            // send the report to the host
            Usb::write(
                hid_callback<Usb>, usb::get_endpoint(config.endpoint.bEndpointAddress),
                mode, {reinterpret_cast<const uint8_t*>(&keycode_report), sizeof(keycode_report)}
            );

            // return we are done
            return;
        }

        static void encode_report(const char ch, keycode_report_t& report) {
            // check what data we have
            if (klib::string::is_character(ch)) {
                // offset between the character (in the ascii table) and in the
                // scan codes
                const uint8_t offset = ('A' - static_cast<uint8_t>(key_t::key_a));

                // we have a character. re-align to hid codes
                if (klib::string::is_upper(ch)) {
                    report.modifier = 0x02;
                    report.key = static_cast<key_t>(ch - offset);
                }
                else {
                    report.modifier = 0x00;
                    report.key = static_cast<key_t>(klib::string::to_upper(ch) - offset);
                }

                return;
            }

            if (klib::string::is_digit(ch)) {
                // set the first two bytes of the report
                report.modifier = 0x00;

                // we have a number
                if (ch == '0') {
                    report.key = key_t::key_0;
                }
                else {
                    report.key = static_cast<key_t>(ch - (static_cast<uint8_t>(key_t::key_1) - '1'));
                }

                return;
            }

            // temporary variables to store the code and the shift
            key_t code;
            uint8_t shift = 0x00;

            // handle some of the other characters
            switch (ch) {
                case '\r':
                case '\n':
                    code = key_t::key_enter;
                    break;
                case '\b':
                    code = key_t::key_backspace;
                    break;
                case '\e':
                    code = key_t::key_escape;
                    break;
                case '\t':
                    code = key_t::key_tab;
                    break;
                case ' ':
                    code = key_t::key_space;
                    break;
                case ',':
                    code = key_t::key_comma;
                    break;
                case '.':
                    code = key_t::key_period;
                case '!':
                    shift = 0x02;

                    // shift and key_1 == !, use a static cast
                    // to signal this is not key_1
                    code = static_cast<key_t>(0x1e);
                default:
                    // all others are mapped to a question mark
                    shift = 0x02;

                    // shift and key_slash == ?, use a static cast
                    // to signal this is not key_slash
                    code = static_cast<key_t>(0x38);
                    break;
            }

            // set the code and the shift
            report.modifier = shift;
            report.key = code;
        }

        template <typename Usb>
        static bool is_invalid_or_busy() {
            // check if we are configured
            if (!configuration) {
                return true;
            }

            // check if a request is already pending
            if (is_busy<Usb>()) {
                // a other request is already pending exit
                return true;
            }

            // return we are not busy and we do not have a 
            // invalid configuration
            return false;
        }

        template <typename Usb, bool Async = false, typename T = keycode_report_t>
        static bool write_impl(const T& r) {
            // write the first report to the endpoint
            if (!Usb::write(hid_callback<Usb>, usb::get_endpoint(config.endpoint.bEndpointAddress),
                usb::get_endpoint_mode(config.endpoint.bEndpointAddress), {reinterpret_cast<const uint8_t*>(&r), sizeof(r)}))
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

    public:
        template <typename Usb, bool Async = false>
        static bool write(const char *const data, const uint32_t size) {
            // check if we have a valid size
            if (size == 0) {
                // return we are done. Nothing to do here
                return true;
            }

            // check if we are busy or not
            if (is_invalid_or_busy<Usb>()) {
                return false;
            }

            // encode the first report
            encode_report(data[0], keycode_report);

            // set the data in the interrupt
            irq_size = size - 1;
            irq_data = data;

            // write the data
            return write_impl<Usb, Async, true>(keycode_report);
        }

        template <typename Usb, bool Async = false>
        static bool write(const key_t key) {
            // static dummy data for the interrupt handler
            // to signal if we are still busy
            static char dummy;

            // check if we are busy or not
            if (is_invalid_or_busy<Usb>()) {
                return false;
            }

            // set the report
            keycode_report.modifier = 0x00;
            keycode_report.key = key;

            // set the data in the interrupt
            irq_size = 0;
            irq_data = &dummy;

            // write the data
            return write_impl<Usb, Async>(keycode_report);
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
            return irq_data != nullptr || Usb::is_pending(
                usb::get_endpoint(config.endpoint.bEndpointAddress), 
                usb::get_endpoint_mode(config.endpoint.bEndpointAddress)
            );
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
            static_assert(Usb::template is_valid_endpoint<
                Endpoint,
                klib::usb::descriptor::transfer_type::interrupt
            >(), "invalid interrupt endpoint selected");
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
                    return to_description(report_desc, sizeof(report_desc));
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
                {const_cast<uint8_t*>(&configuration), sizeof(configuration)}
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
                    usb::get_endpoint_mode(config.endpoint.bEndpointAddress),
                    usb::get_transfer_type(config.endpoint.bmAttributes),
                    sizeof(keycode_report)
                );

                // store the configuration value
                configuration = packet.wValue;

                // notify the usb driver we are configured
                Usb::configured(true);

                // prepare a inital report with no keys pressed
                keycode_report.modifier = 0x00;
                keycode_report.key = key_t::key_none;

                // write the inital report
                if (Usb::write(
                    hid_callback<Usb>, usb::get_endpoint(config.endpoint.bEndpointAddress),
                    usb::get_endpoint_mode(config.endpoint.bEndpointAddress), 
                    {reinterpret_cast<const uint8_t*>(&keycode_report), sizeof(keycode_report)}))
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
                        usb::get_endpoint_mode(config.endpoint.bEndpointAddress)
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
                    keycode_report.modifier = 0x00;
                    keycode_report.key = key_t::key_none;

                    // write the data to the control endpoint
                    if (Usb::write(nullptr, usb::control_endpoint,
                        usb::endpoint_mode::in, {reinterpret_cast<const uint8_t*>(&keycode_report), sizeof(keycode_report)}))
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
                    switch (static_cast<report_id>(packet.wValue & 0xff)) {
                        case report_id::keyboard:
                            // write the data to the control endpoint
                            if (Usb::write(nullptr, usb::control_endpoint, usb::endpoint_mode::in,
                                {reinterpret_cast<const uint8_t*>(&keycode_report), sizeof(keycode_report)}))
                            {
                                // no issue for now ack
                                return usb::handshake::ack;
                            }
                            else {
                                // something went wrong stall for now
                                return usb::handshake::stall;
                            }
                            break;
                        default:
                            // not supported for now
                            return usb::handshake::stall;
                    }
                    break;
                case hid::class_request::set_report:
                    // check if the packet length is not above the max endpoint size
                    if (packet.wLength > Usb::max_endpoint_size.size(usb::control_endpoint, klib::usb::descriptor::transfer_type::control)) {
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