#ifndef KLIB_USB_HPP
#define KLIB_USB_HPP

#include "descriptor.hpp"
#include "setup.hpp"

namespace klib::usb {
    class usb {
    public:
        struct descriptor_desc {
            const void *desc;
            uint32_t size;
        };
    
    protected:
        /**
         * @brief Helper function to get the direction of a packet
         * 
         * @param packet 
         * @return setup::direction 
         */
        constexpr static setup::direction get_direction(const setup_packet &packet) {
            return static_cast<setup::direction>((packet.bmRequestType >> 7) & 0x1);
        }

        /**
         * @brief Helper function to get the recipient code from a packet
         * 
         * @param packet 
         * @return constexpr setup::recipient_code 
         */
        constexpr static setup::recipient_code get_recipient(const setup_packet &packet) {
            return static_cast<setup::recipient_code>(packet.bmRequestType & 0x1f);
        }

        /**
         * @brief Helper function to get the feature from a packet
         * 
         * @param packet 
         * @return constexpr setup::feature 
         */
        constexpr static setup::feature get_feature(const setup_packet &packet) {
            return static_cast<setup::feature>(packet.wValue);
        }

        /**
         * @brief Send the status of the usb device
         * 
         * @tparam Usb 
         * @param packet 
         */
        template <typename Usb>
        static void get_status(const setup_packet &packet) {
            // wValue = zero
            // wIndex = zero
            // wLength = 2
            // data = device status

            const auto recipient = get_recipient(packet);

            // check if any of the values are out of the usb 2.0 spec range
            if (packet.wValue != 0x0 || packet.wLength != 2 || 
                (packet.wIndex != 0 && recipient == setup::recipient_code::device)) 
            {
                // stall the usb
                Usb::stall(0);
                return;
            }

            // get status response (2 bytes). Data needs to be 4 
            // byte alligned on some devices. So always allign it.
            // Data needs to be static to make sure it is still
            // allocated when the dma is sending it.
            alignas(4) static uint8_t response[2];

            switch (recipient) {
                case setup::recipient_code::endpoint:
                    // check if the usb is stalled
                    response[0] = Usb::is_stalled(packet.wIndex & 0xf);
                    response[1] = 0x00;
                    break;
                
                case setup::recipient_code::interface:
                    response[0] = 0x00;
                    response[1] = 0x00;
                    break;
                
                case setup::recipient_code::device:
                    // response[0] = Usb::device::get_device_status();
                    response[0] = 0x00; // dont enable remote wake or self powered for now
                    response[1] = 0x00;
                
                default:
                    // this should not happen only with reserved. 
                    // send a stall when that happens
                    Usb::stall(0);
                    return;
            }

            // write and check if we got any errors straight away
            if (Usb::write_endpoint(0, response, sizeof(response))) {
                // no errors return
                return;
            }

            // we have a error stall
            Usb::stall(0);
        }

        /**
         * @brief Clear a feature in the usb device
         * 
         * @tparam Usb 
         * @param packet 
         */
        template <typename Usb>
        static void clear_feature(const setup_packet &packet) {
            // wValue = feature selector
            // wIndex = zero
            // wLength = zero
            // data = none

            const auto direction = get_direction(packet);

            // check if any values are out of the usb spec
            if ((direction == setup::direction::device_to_host) || 
                (packet.wLength != 0)) 
            {
                // stall the usb
                Usb::stall(0);
                return;
            }

            const auto feature = get_feature(packet);
            const auto recipient = get_recipient(packet);

            // check if the feature is called correctly
            if (((feature == setup::feature::endpoint_halt) && recipient != setup::recipient_code::endpoint) ||
                ((feature == setup::feature::remote_wake) && recipient != setup::recipient_code::device) ||
                ((feature == setup::feature::test_mode) && recipient != setup::recipient_code::device))
            {
                // stall the usb
                Usb::stall(0);
                return;
            }

            // call the device clear feature
            Usb::device::template clear_feature<Usb>(feature, packet);
        }

        /**
         * @brief Set a feature in the usb device
         * 
         * @tparam Usb 
         * @param packet 
         */
        template <typename Usb>
        static void set_feature(const setup_packet &packet) {
            // wValue = feature selector
            // wIndex = zero
            // wLength = zero
            // data = none

            const auto direction = get_direction(packet);

            // check if any values are out of the usb spec
            if ((direction == setup::direction::device_to_host) || 
                (packet.wLength != 0)) 
            {
                // stall the usb
                Usb::stall(0);
                return;
            }

            const auto feature = get_feature(packet);
            const auto recipient = get_recipient(packet);

            // check if the feature is called correctly
            if (((feature == setup::feature::endpoint_halt) && recipient != setup::recipient_code::endpoint) ||
                ((feature == setup::feature::remote_wake) && recipient != setup::recipient_code::device) ||
                ((feature == setup::feature::test_mode) && recipient != setup::recipient_code::device))
            {
                // stall the usb
                Usb::stall(0);
                return;
            }

            // call the device clear feature
            Usb::device::template set_feature<Usb>(feature, packet);         
        }

        /**
         * @brief Set the device address
         * 
         * @tparam Usb 
         * @param packet 
         */
        template <typename Usb>
        static void set_device_address(const setup_packet &packet) {
            // wValue = device address
            // wIndex = zero
            // wLength = zero
            // data = none

            // check if the values are out of spec
            if (packet.wIndex != 0 || packet.wLength != 0) {
                // stall the usb
                Usb::stall(0);

                return;
            }

            // set the device address in the usb hardware
            Usb::set_device_address(packet.wValue & 0x7f);
        }

        /**
         * @brief Send the usb descriptor
         * 
         * @tparam Usb 
         * @param packet 
         */
        template <typename Usb>
        static void get_descriptor(const setup_packet &packet) {
            // wValue = descriptor type & index
            // wIndex = zero or language id
            // wLength = descriptor length
            // data = descriptor

            // get the descriptor type from the high byte in wValue
            const auto type = static_cast<descriptor::descriptor_type>(packet.wValue >> 8);

            // get the index of the descriptor request
            const uint8_t index = packet.wValue & 0xff;

            // request the descriptor from the device code
            const descriptor_desc& descriptor = Usb::device::template get_descriptor<Usb>(packet, type, index);

            // check what parameters we should check
            switch (type) {
                case descriptor::descriptor_type::device:
                case descriptor::descriptor_type::configuration:
                case descriptor::descriptor_type::device_qualifier:
                case descriptor::descriptor_type::other_speed_config:
                    // check the direction and if the wIndex is not 0 (only string descriptor should have non zero)
                    if ((get_recipient(packet) != setup::recipient_code::device) || (packet.wIndex != 0)) {
                        // stall when something is wrong with the input
                        Usb::stall(0);

                        return;
                    }

                    // check if the descriptor is set
                    if (!descriptor.size) {
                        // stall when we dont have any descriptors
                        Usb::stall(0);

                        return;
                    }
                    break;

                case descriptor::descriptor_type::string:
                    // only check the direction of the packet
                    if ((get_recipient(packet) != setup::recipient_code::device)) {
                        // stall when something is wrong with the input
                        Usb::stall(0);

                        return;
                    }

                    if (!descriptor.size) {
                        // descriptor out of range. Stall
                        Usb::stall(0);

                        return;
                    }

                default:
                    // when another descriptor is requested only check if the descriptor size
                    // is not 0. otherwise stall
                    if (!descriptor.size) {
                        // descriptor out of range. Stall
                        Usb::stall(0);

                        return;
                    }

                    break;
            }

            // adjust the length to the requested length
            const uint32_t size = klib::min(descriptor.size, static_cast<uint32_t>(packet.wLength));

            // write the data to the endpoint buffer and check if we directly fail
            if (Usb::write_endpoint(0, reinterpret_cast<const uint8_t*>(descriptor.desc), descriptor.size)) {
                // no errors do a early return
                return;
            }

            // we failed so stall the endpoint
            Usb::stall(0);
        }

        template <typename Usb>
        static void get_config(const setup_packet &packet) {
            // wValue = zero
            // wIndex = zero
            // wLength = 1
            // data = configuration value

            // check if the packet is follign the usb spec
            if (packet.wValue != 0 || packet.wIndex != 0 || packet.wLength != 1) {
                // packet is not correct
                Usb::stall(0);

                return;
            }

            // call the device function
            Usb::device::template get_config<Usb>(packet);
        }

        template <typename Usb>
        static void set_config(const setup_packet &packet) {
            // wValue = configuration value
            // wIndex = zero
            // wLength = zero
            // data = none

            // check if the packet is correct
            if (packet.wIndex != 0 || packet.wLength != 0) {
                // packet not following the usb spec
                Usb::stall(0);

                return;
            }

            // send the packet to the device
            Usb::device::template set_config<Usb>(packet);
        }

        template <typename Usb>
        static void get_interface(const setup_packet &packet) {
            // wValue = zero
            // wIndex = interface
            // wlength = one
            // data = alternate interface

            // check if the request is following the usb spec
            if (packet.wValue != 0 || packet.wLength != 1) {
                // stall we have a invalid packet
                Usb::stall(0);

                return;
            }

            // call the device get interface functionality
            Usb::device::template get_interface<Usb>(packet);
        }

        template <typename Usb>
        static void set_interface(const setup_packet &packet) {
            // wValue = alternate setting
            // wIndex = interface
            // wlength = zero
            // data = none

            // check if the packet is following the spec
            if (packet.wLength != 0) {
                // error wit hthe packet stall
                Usb::stall(0);

                return;
            }

            // send the packet to the device
            Usb::device::template set_interface<Usb>(packet);
        }

    public:
        template <typename Usb>
        static void handle_standard_packet(const setup_packet &packet) {
            // get the current request
            const auto& request = static_cast<setup::device_request>(packet.bRequest);

            // check what kind of request we have
            switch (request) {
                case setup::device_request::get_status:
                    return get_status<Usb>(packet);
                case setup::device_request::clear_feature:
                    return clear_feature<Usb>(packet);
                case setup::device_request::set_feature:
                    return set_feature<Usb>(packet);
                case setup::device_request::set_address:
                    return set_device_address<Usb>(packet);
                case setup::device_request::get_descriptor:
                    return get_descriptor<Usb>(packet);
                case setup::device_request::get_configuration:
                    return get_config<Usb>(packet);
                case setup::device_request::set_configuration:
                    return set_config<Usb>(packet);
                case setup::device_request::get_interface:
                    return get_interface<Usb>(packet);
                case setup::device_request::set_interface:
                    return set_interface<Usb>(packet);
                case setup::device_request::set_descriptor:
                    // we cannot handle this as the descriptors
                    // should be located in flash.
                default:
                    // other requests are not implemented. Send a 
                    // stall.
                    Usb::stall(0);
                    break;
            }
        }

        template <typename Usb>
        static void handle_setup_packet(const setup_packet &packet) {
            // ge the request type from the packet
            const auto type = static_cast<setup::request_type>((packet.bmRequestType >> 5) & 0x3);

            // check what to do with our current packet
            switch (type) {
                case setup::request_type::standard:
                    // the usb stack will stall/ack the request
                    // when required
                    return handle_standard_packet<Usb>(packet);
                case setup::request_type::uclass:
                    // the device needs to stall this request 
                    // if not supported using the Usb::stall()
                    // function
                    return Usb::device::template handle_class_packet<Usb>(packet);
                case setup::request_type::vendor:
                    // the device needs to stall this request 
                    // if not supported using the Usb::stall()
                    // function
                    return Usb::device::template handle_vendor_packet<Usb>(packet);
                case setup::request_type::reserved:
                default:
                    // send a stall as we have no clue what to do
                    // with our current packet
                    Usb::stall(0);
                    break;
            }

            // the request should be stalled/acked before we enter.
            // only exception is when there is a data stage in the
            // usb transfer. then it will be acked/stalled later in
            // the interrupt
        }

        template <typename Usb>
        static void status_callback(const uint8_t error_code) {
            // check if we have a error
            if (!error_code) {
                // no error send a ack
                Usb::ack_endpoint(0);
            }
            else {
                // we have a error send a stall
                Usb::stall(0);
            }
        }
    };
}

#endif