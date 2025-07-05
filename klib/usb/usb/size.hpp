#ifndef KLIB_USB_SIZE_HPP
#define KLIB_USB_SIZE_HPP

#include <array>

#include "descriptor.hpp"

namespace klib::usb {
    /**
     * @brief Helper class to get the size of a specific endpoint by transfer type
     * 
     * @note Gives the same interface as the other classes in this file
     * 
     * @tparam Control 
     * @tparam Isochronous 
     * @tparam BulkSize 
     * @tparam Interrupt 
     */
    template <
        uint16_t Control,
        uint16_t Isochronous,
        uint16_t BulkSize,
        uint16_t Interrupt
    >
    class endpoint_size_type {
    protected:
        // storage for all the sizes of every endpoint type
        const std::array<uint16_t, 4> s = {
            Control, Isochronous, BulkSize, Interrupt
        };

    public:
        /**
         * @brief Get the size of a endpoint
         * 
         * @param endpoint 
         * @param type 
         * @return constexpr auto 
         */
        constexpr auto size(const uint8_t endpoint, const descriptor::transfer_type& type) const {
            return s[static_cast<uint8_t>(type)];
        }
    };

    /**
     * @brief Helper class to get the size of a specific endpoint by the endpoint number
     * 
     * @note Gives the same interface as the other classes in this file
     * 
     * @tparam EndpointCount 
     * @tparam Values 
     */
    template <
        uint8_t EndpointCount, 
        uint16_t... Values
    >
    class endpoint_size_endpoint {
    protected:
        static_assert(sizeof...(Values) == EndpointCount, "Invalid endpoint sizes received");

        // storage for all the endpoint sizes
        const std::array<uint16_t, EndpointCount> s = {
            Values...
        };

    public:
        /**
         * @brief Get the size of a endpoint
         * 
         * @param endpoint 
         * @param type 
         * @return constexpr auto 
         */
        constexpr auto size(const uint8_t endpoint, const descriptor::transfer_type& type) const {
            return s[endpoint];
        }
    };
}

#endif

