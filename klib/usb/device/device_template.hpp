#ifndef KLIB_USB_DEVICE_TEMPLATE_HPP
#define KLIB_USB_DEVICE_TEMPLATE_HPP

#include <algorithm>

#include <klib/usb/device.hpp>

namespace klib::usb::device {
    class device_template {
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
            return false;
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
        void wakeup() {
            // if not needed this function can be removed
        };

        /**
         * @brief Called when the usb tranceiver goes to sleep
         * 
         * @tparam Usb 
         */
        template <typename Usb>
        void sleep() {
            // if not needed this function can be removed
        };

        /**
         * @brief Called when the host is disconnected
         * 
         * @tparam Usb 
         */
        template <typename Usb>
        void disconnected() {
            // if not needed this function can be removed
        };

        /**
         * @brief Called when the usb has connected to a host
         * 
         * @tparam Usb 
         */
        template <typename Usb>
        void connected() {
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
        void activity() {
            // if not needed this function can be removed
        };

        /**
         * @brief Called when a bus reset has occured
         * 
         * @tparam Usb 
         */
        template <typename Usb>
        void bus_reset() {
            // if not needed this function can be removed
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
            // if not needed this function can be removed
            return usb::handshake::stall;
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
            // if not needed this function can be removed
            return usb::handshake::stall;
        }

        /**
         * @brief Called when something occured on a endpoint.
         * 
         * @tparam Usb 
         * @param endpoint 
         * @param mode 
         */
        template <typename Usb>
        void endpoint_callback(const uint8_t endpoint, const klib::usb::usb::endpoint_mode mode) {
            // if not needed this function can be removed
        };

        /**
         * @brief Called when a class specific packet is received
         * 
         * @tparam Usb 
         * @param packet 
         */
        template <typename Usb>
        static usb::handshake handle_class_packet(const klib::usb::setup_packet &packet) {
            // if not needed this function can be removed
            return usb::handshake::stall;
        }

        /**
         * @brief Called when a vendor specific packet is received
         * 
         * @tparam Usb 
         * @param packet 
         */
        template <typename Usb>
        static usb::handshake handle_vendor_packet(const klib::usb::setup_packet &packet) {
            // if not needed this function can be removed
            return usb::handshake::stall;
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
            return usb::handshake::stall;
        }

        /**
         * @brief Set a configuration value
         * 
         * @tparam Usb 
         * @param packet 
         */
        template <typename Usb>
        static usb::handshake set_config(const klib::usb::setup_packet &packet) {
            return usb::handshake::stall;
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