#ifndef KLIB_USB_BULK_HPP
#define KLIB_USB_BULK_HPP

#include <algorithm>

#include <klib/string.hpp>
#include <klib/usb/device.hpp>
#include <klib/usb/descriptor.hpp>

#include <klib/stream.hpp>

namespace klib::usb::device {
    class bulk {
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

            // endpoint descriptor
            descriptor::endpoint endpoint0;

            // endpoint descriptor
            descriptor::endpoint endpoint1;
        };

        // release the old pack so the rest of the structs are not 
        // affected by the pack(1)
        #pragma pack(pop)

        // device descriptor for the hid keyboard
        const __attribute__((aligned(4))) static inline descriptor::device device = {
            .bcdUSB = 0x0110,
            .bDeviceClass = 0xff,
            .bDeviceSubClass = 0x00,
            .bDeviceProtocol = 0x00,
            .bMaxPacketSize = 0x40,
            .idVendor = 0x0B6A,
            .idProduct = 0x1717,
            .bcdDevice = 0x0200,
            .iManufacturer = 0x01,
            .iProduct = 0x02,
            .iSerialNumber = 0x03,
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
                .bEndpointAddress = 0x82,
                .bmAttributes = 0x02,
                .wMaxPacketSize = 0x0040,
                .bInterval = 0x01
            },
            {
                .bEndpointAddress = 0x05,
                .bmAttributes = 0x02,
                .wMaxPacketSize = 0x0040,
                .bInterval = 0x01
            }
        };

        // language descriptor for the keyboard
        const __attribute__((aligned(4))) static inline descriptor::string<1> language = {
            .bString = {0x0409}
        };

        // manufacturer string descriptor
        const __attribute__((alligned(4))) static inline auto manufacturer = string_descriptor("KLIB");

        // product string descriptor
        const __attribute__((aligned(4))) static inline auto product = string_descriptor("KLIB Bulk USB");

        // serial number string descriptor
        const __attribute__((aligned(4))) static inline auto serial = string_descriptor("00001337");

        // configuration value. Value is set in the set config function
        static inline uint8_t configuration = 0x00;

        // flag if remote wakeup is supported
        static inline bool remote_wakeup = false;

        struct _rx_data {
            // pointer where to store the data
            uint8_t *volatile data;

            // requested amount of data
            volatile uint32_t size;
        };

        struct _tx_data {
            // pointer where to store the data
            const uint8_t *volatile data;

            // requested amount of data
            volatile uint32_t size;
        };

        // static parameters with data to write to the host
        static inline _rx_data rx_data = {nullptr, 0};

        // flag to signal we are transmitting
        static inline volatile bool is_receiving = false;
        static inline volatile bool is_transmitting = false;

        template <typename Usb>
        static void callback(const uint8_t endpoint, const usb::endpoint_mode mode, const usb::error error_code) {
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
                // we just wrote data. Clear the tx data
                is_transmitting = false;
            }
            else {
                // clear the size to mark we have data
                rx_data.size = 0;

                // clear the flag we are receiving
                is_receiving = false;
            }
        }

    public:
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

            // clear all the pointers and flags
            rx_data = {nullptr, 0};

            is_receiving = false;
            is_transmitting = false;
        }

        template <typename Usb>
        static bool setup(uint8_t *const data, const uint32_t size) {
            if (rx_data.size || is_receiving) {
                // we are still transmitting
                return false;
            }

            // set the buffer so we can receive data
            rx_data = {data, size};

            // return success
            return true;
        }

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
            if (is_busy<Usb>()) {
                // a other request is already pending exit
                return false;
            }

            // set the pointer and the size
            is_transmitting = true;

            // start to the write to the endpoint
            if (!Usb::write(callback<Usb>, config.endpoint0.bEndpointAddress & 0xf, 
                usb::endpoint_mode::in, data, size))
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
         * @brief Returns if we have data available in the receive buffer
         * 
         * @tparam Usb 
         * @return true 
         * @return false 
         */
        template <typename Usb>
        static bool has_data() {
            // return if we have the size still set. If it is 
            // empty we have received the whole buffer and cannot
            // receive more
            return (!rx_data.size) && (rx_data.data != nullptr);
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
            // return if the data is set. If it is we are still transmitting
            return is_transmitting;
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
         * @brief Called when the usb tranceiver goes to sleep
         * 
         * @tparam Usb 
         */
        template <typename Usb>
        static void sleep() {
            // not implemented
        }

        /**
         * @brief Called when the usb tranceiver wakes up
         * 
         * @tparam Usb 
         */
        template <typename Usb>
        static void wakeup() {
            // not implemented
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
                usb::endpoint_mode::in, &configuration, 
                sizeof(configuration)
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
                    config.endpoint0.bEndpointAddress & 0xf, 
                    usb::get_endpoint_mode(config.endpoint0.bEndpointAddress), 
                    config.endpoint0.wMaxPacketSize
                );
                Usb::configure(
                    config.endpoint1.bEndpointAddress & 0xf, 
                    usb::get_endpoint_mode(config.endpoint1.bEndpointAddress), 
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
                        config.endpoint0.bEndpointAddress & 0xf, 
                        usb::get_endpoint_mode(config.endpoint0.bEndpointAddress)
                    );

                    // reset the endpoint
                    Usb::reset(
                        config.endpoint1.bEndpointAddress & 0xf, 
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

        template <typename Usb>
        static void endpoint_callback(const uint8_t endpoint, const usb::endpoint_mode mode) {
            // check if we we are receiving data
            if (endpoint == (config.endpoint1.bEndpointAddress & 0xf)) {
                // check if we are receiving already. Do not continue if that is the case
                if (is_receiving) {
                    // do a early return
                    return;
                }

                // check if we have a buffer to store the data into
                if (rx_data.data == nullptr || (!rx_data.size)) {
                    // we do not have a valid buffer or we have received a 
                    // message already. do a early return
                    return;
                }

                // mark we are receiving data
                is_receiving = true;

                // start a read on the endpoint
                Usb::read(callback<Usb>, config.endpoint1.bEndpointAddress & 0xf, 
                    usb::endpoint_mode::out, rx_data.data, rx_data.size
                );
            }
        }
    };
}

#endif