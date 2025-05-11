#ifndef KLIB_USB_SERIAL_HPP
#define KLIB_USB_SERIAL_HPP

#include <algorithm>
#include <span>

#include <klib/ringbuffer.hpp>
#include <klib/string.hpp>
#include <klib/usb/usb/device.hpp>
#include <klib/usb/usb/descriptor.hpp>
#include <klib/usb/usb/cdc/descriptor.hpp>

namespace klib::usb::device {
    template <uint8_t CmdEndpoint = 1, uint8_t OutEndpoint = 2, uint8_t InEndpoint = 3, uint32_t RxSize = 128>
    class serial {
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
         * @brief The command we are receiving from the host
         *
         */
        struct command {
            // the requested opt-code
            uint8_t optcode;

            // the length of the command
            uint16_t length;
        };

        // TODO: change these sizes to the max enpoint size
        // rx buffer used for receiving
        static inline uint8_t rx_buffer[64] = {};

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
            descriptor::interface interface0;

            // cdc information
            cdc::header header;
            cdc::call_management call_management;
            cdc::abstract_control_management acm;
            cdc::union_cdc<1> union_cdc;

            // endpoint descriptor
            descriptor::endpoint endpoint0;

            // interface descriptor
            descriptor::interface interface1;

            // endpoint descriptor
            descriptor::endpoint endpoint1;

            // endpoint descriptor
            descriptor::endpoint endpoint2;
        };

        // release the old pack so the rest of the structs are not
        // affected by the pack(1)
        #pragma pack(pop)

        // device descriptor for the hid keyboard
        const __attribute__((aligned(4))) static inline descriptor::device device = {
            .bcdUSB = static_cast<uint16_t>(setup::usb_version::usb_v2_0),
            .bDeviceClass = descriptor::class_type::communication_and_cdc,
            .bDeviceSubClass = 0x02,
            .bDeviceProtocol = 0x00,
            .bMaxPacketSize = 0x40,
            .idVendor = 0x6666,
            .idProduct = 0xaaaa,
            .bcdDevice = static_cast<uint16_t>(setup::usb_version::usb_v2_0),
            .iManufacturer = static_cast<uint8_t>(string_index::manufacturer),
            .iProduct = static_cast<uint8_t>(string_index::product),
            .iSerialNumber = static_cast<uint8_t>(string_index::serial),
            .bNumConfigurations = 0x1
        };

        // configuration descriptor
        const __attribute__((aligned(4))) static inline config_descriptor config = {
            {
                .wTotalLength = sizeof(config_descriptor),
                .bNumInterfaces = 0x02,
                .bConfigurationValue = 0x01,
                .iConfiguration = 0x00,
                .bmAttributes = 0x80,
                .bMaxPower = 0x32,
            },
            {
                .bInterfaceNumber = 0x00,
                .bAlternateSetting = 0x00,
                .bNumEndpoints = 0x01,
                .bInterfaceClass = 0x02,
                .bInterfaceSubClass = 0x02,
                .bInterfaceProtocol = 0x01,
                .iInterface = 0x00
            },
            {
                .bcdCDC = static_cast<uint16_t>(setup::usb_version::usb_v1_1)
            },
            {
                .bmCapabilities = 0x00,
                .bDataInterface = 0x01,
            },
            {
                .bmCapabilities = 0x02,
            },
            {
                .bControlInterface = 0x00,
                .bSubordinateInterface = {
                    0x01
                }
            },
            {
                .bEndpointAddress = 0x80 | CmdEndpoint,
                .bmAttributes = static_cast<uint8_t>(klib::usb::descriptor::transfer_type::interrupt),
                .wMaxPacketSize = 0x0008,
                .bInterval = 0x0a
            },
            {
                .bInterfaceNumber = 0x01,
                .bAlternateSetting = 0x00,
                .bNumEndpoints = 0x02,
                .bInterfaceClass = 0x0a,
                .bInterfaceSubClass = 0x00,
                .bInterfaceProtocol = 0x00,
                .iInterface = 0x00
            },
            {
                .bEndpointAddress = OutEndpoint,
                .bmAttributes = static_cast<uint8_t>(klib::usb::descriptor::transfer_type::bulk),
                .wMaxPacketSize = sizeof(rx_buffer),
                .bInterval = 0x00
            },
            {
                .bEndpointAddress = 0x80 | InEndpoint,
                .bmAttributes = static_cast<uint8_t>(klib::usb::descriptor::transfer_type::bulk),
                .wMaxPacketSize = 0x40,
                .bInterval = 0x00
            }
        };

        // language descriptor for the keyboard
        const __attribute__((aligned(4))) static inline descriptor::string<1> language = {
            .bString = {0x0409}
        };

        // manufacturer string descriptor
        const __attribute__((aligned(4))) static inline auto manufacturer = string_descriptor("KLIB");

        // product string descriptor
        const __attribute__((aligned(4))) static inline auto product = string_descriptor("KLIB Serial port");

        // serial number string descriptor
        const __attribute__((aligned(4))) static inline auto serial_nr = string_descriptor("00001337");

        // configuration value. Value is set in the set config function
        static inline uint8_t configuration = 0x00;

        // temporary storage
        static inline klib::ringbuffer<uint8_t, RxSize> receive = {};

        // buffer to receive commands
        static inline uint8_t command_buffer[64] = {};

        // flag if we need to send a zlp after the data
        static inline volatile bool send_zlp = false;

        // flags if we are currently transmitting and or have received data
        static inline volatile bool is_transmitting = false;
        static inline volatile bool has_received_data = false;

        // the current command we are processing
        static inline command opt_code = {0xff, 0x00};

        /**
         * @brief Callback that can send more keypresses or will send no keys
         *
         * @tparam Usb
         * @param data
         */
        template <typename Usb>
        static void command_callback_handler(const uint8_t endpoint, const usb::endpoint_mode mode, const usb::error error_code, const uint32_t transferred) {
            // only continue if we do not have any errors
            if (error_code != usb::error::no_error) {
                // we have a error
                return;
            }

            // check if we are configured
            if (!configuration) {
                return;
            }

            // TODO: handle commands here

            // no error send a ack
            Usb::ack(endpoint, mode);
        }

        /**
         * @brief Callback that can send more keypresses or will send no keys
         *
         * @tparam Usb
         * @param data
         */
        template <typename Usb>
        static void transmit_callback_handler(const uint8_t endpoint, const usb::endpoint_mode mode, const usb::error error_code, const uint32_t transferred) {
            // only continue if we do not have any errors
            if (error_code != usb::error::no_error) {
                // we have a error
                return;
            }

            // check if we are configured
            if (!configuration) {
                return;
            }

            // check if we need to send a zero length packet before we are
            // done sending the current request
            if (send_zlp) {
                // clear the flag we need to send a zero length packet
                send_zlp = false;

                // send a zero length packet
                Usb::write(transmit_callback_handler<Usb>,
                    usb::get_endpoint(config.endpoint2.bEndpointAddress),
                    usb::get_endpoint_mode(config.endpoint2.bEndpointAddress),
                    {}
                );

                // wait until we have called this callback again before
                // we clear the is_transmitting flag
                return;
            }

            // mark we are not transmitting anymore
            is_transmitting = false;
        }

        /**
         * @brief Callback that can send more keypresses or will send no keys
         *
         * @tparam Usb
         * @param data
         */
        template <typename Usb>
        static void receive_callback_handler(const uint8_t endpoint, const usb::endpoint_mode mode, const usb::error error_code, const uint32_t transferred) {
            // only continue if we do not have any errors
            if (error_code != usb::error::no_error) {
                // we have a error
                return;
            }

            // check if we are configured
            if (!configuration) {
                return;
            }

            // add the received bytes to the ringbuffer
            for (uint32_t i = 0; i < transferred; i++) {
                receive.push(rx_buffer[i]);
            }

            // mark we have received data
            has_received_data = true;

            // start receiving a new packet
            Usb::read(receive_callback_handler<Usb>,
                usb::get_endpoint(config.endpoint1.bEndpointAddress),
                usb::get_endpoint_mode(config.endpoint1.bEndpointAddress),
                rx_buffer, 1, sizeof(rx_buffer)
            );
        }

    public:
        /**
         * @brief Returns if we have data available.
         *
         * @return true
         * @return false
         */
        static bool has_data() {
            // return if we have data in the buffer
            return has_received_data;
        }

        /**
         * @brief Returns data read into the receive buffer
         *
         * @warning the USB interrupt should be suppressed while
         * this function called. Undefined behaviour when no data
         * in ringbuffer and this function is called
         *
         * @return uint8_t
         */
        static uint8_t read() {
            // get the first item in the buffer
            const auto r = receive.copy_and_pop();

            // check if the buffer is empty and update the
            // flag if it is
            if (receive.empty()) {
                has_received_data = false;
            }

            // return the data in the receive buffer
            return r;
        }

        /**
         * @brief Function that returns if the transmitting side is
         * busy.
         *
         * @return true
         * @return false
         */
        static bool is_busy() {
            return is_transmitting;
        }

        /**
         * @brief Write data to the serial buffer. Waits until data
         * can be written using the USB driver. Buffer needs to be
         * available until transfer is done (can be checked using
         * 'is_busy()')
         *
         * @tparam Async
         * @param data
         */
        template <typename Usb, bool Async = false>
        static void write(const std::span<const uint8_t> data) {
            // the current index in the data
            uint32_t index = 0;

            // wait until we are not busy anymore
            while (is_busy()) {
                // do nothing
            }

            // set the flag we are transmitting
            is_transmitting = true;

            // check if we need to send a zlp
            if ((data.size() % 0x40) == 0) {
                // mark we need to send a zlp
                send_zlp = true;
            }

            // write the data
            Usb::write(transmit_callback_handler<Usb>,
                usb::get_endpoint(config.endpoint2.bEndpointAddress),
                usb::get_endpoint_mode(config.endpoint2.bEndpointAddress),
                data
            );

            // check if we should wait until the full transfer is done
            if constexpr (Async) {
                // wait until we are done writing
                while (is_busy()) {
                    // do nothing
                }
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
            // make sure the endpoints support the transfer types
            static_assert(Usb::template is_valid_endpoint<
                CmdEndpoint,
                klib::usb::descriptor::transfer_type::interrupt
            >(), "invalid interrupt endpoint selected");

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
            // clear all the variables to default
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
            // we do not support any features
            return usb::handshake::stall;
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
            // we do not support any features
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
                            return to_description(serial_nr, serial_nr.bLength);
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
                // configure the endpoint for our report data
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

                Usb::configure(
                    usb::get_endpoint(config.endpoint2.bEndpointAddress),
                    usb::get_endpoint_mode(config.endpoint2.bEndpointAddress),
                    usb::get_transfer_type(config.endpoint2.bmAttributes),
                    config.endpoint2.wMaxPacketSize
                );

                // start receiving on the out endpoint
                Usb::read(
                    receive_callback_handler<Usb>,
                    usb::get_endpoint(config.endpoint1.bEndpointAddress),
                    usb::get_endpoint_mode(config.endpoint1.bEndpointAddress),
                    rx_buffer, 1, sizeof(rx_buffer)
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

                // reset the used endpoint if we have one
                if (configuration) {
                    // reset the endpoints
                    Usb::reset(
                        usb::get_endpoint(config.endpoint0.bEndpointAddress),
                        usb::get_endpoint_mode(config.endpoint0.bEndpointAddress)
                    );

                    Usb::reset(
                        usb::get_endpoint(config.endpoint1.bEndpointAddress),
                        usb::get_endpoint_mode(config.endpoint1.bEndpointAddress)
                    );

                    Usb::reset(
                        usb::get_endpoint(config.endpoint2.bEndpointAddress),
                        usb::get_endpoint_mode(config.endpoint2.bEndpointAddress)
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
            // get the direction of the packet
            const auto direction = usb::get_direction(packet);

            // check if we have a packet with additional data
            if (packet.wLength) {
                // do something based on the direction
                if (direction == klib::usb::setup::direction::device_to_host) {
                    // send the interface back to the host
                    const auto result = Usb::write(
                        usb::status_callback<Usb>, usb::control_endpoint,
                        usb::endpoint_mode::in,
                        {command_buffer, packet.wLength}
                    );

                    // check if something went wrong already
                    if (!result) {
                        // something went wrong stall
                        return usb::handshake::stall;
                    }
                }
                else {
                    // get the op-code and the length
                    opt_code.optcode = packet.bRequest;
                    opt_code.length = packet.wLength;

                    // start a read with the provided length
                    const auto result = Usb::read(
                        command_callback_handler<Usb>, usb::control_endpoint,
                        usb::endpoint_mode::in,
                        {command_buffer, opt_code.length}
                    );

                    // check if something went wrong already
                    if (!result) {
                        // something went wrong stall
                        return usb::handshake::stall;
                    }
                }
            }
            else {
                // we have a packet without extra data
            }

            // always ack this message
            return usb::handshake::ack;
        }

        /**
         * @brief Called when get interface is received
         *
         * @tparam Usb
         * @param packet
         * @return usb::handshake
         */
        template <typename Usb>
        static usb::handshake get_interface(const klib::usb::setup_packet &packet) {
            // always send back 0x00 as the interface
            constexpr static uint8_t alternate = 0x00;

            // send the interface back to the host
            const auto result = Usb::write(
                usb::status_callback<Usb>, usb::control_endpoint,
                usb::endpoint_mode::in,
                {&alternate, sizeof(alternate)}
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
         * @brief Called when set interface is received
         *
         * @tparam Usb
         * @param packet
         * @return usb::handshake
         */
        template <typename Usb>
        static usb::handshake set_interface(const klib::usb::setup_packet &packet) {
            // always ack this packet. We do not use it
            return usb::handshake::ack;
        }
    };
}


#endif