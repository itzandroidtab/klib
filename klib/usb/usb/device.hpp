#ifndef KLIB_USB_DEVICE_HPP
#define KLIB_USB_DEVICE_HPP

#include "usb.hpp"
#include "descriptor.hpp"

namespace klib::usb::device {
    /**
     * @brief Helper function to convert a descriptor struct to a descriptor description
     *
     * @tparam T
     * @param desc
     * @param size
     * @return constexpr description
     */
    template <typename T>
    constexpr usb::description to_description(const T& desc, const uint32_t size) {
        return {reinterpret_cast<const uint8_t *const>(&desc), size};
    }

    /**
     * @brief Helper function to convert a raw descriptor array to a descriptor description
     *
     * @param desc
     * @param size
     * @return constexpr usb::description
     */
    constexpr usb::description to_description(const uint8_t *const desc, const uint32_t size) {
        return {desc, size};
    }

    /**
     * @brief Helper function to create a string descriptor from a string (removes the null terminator)
     *
     * @param str
     * @return consteval
     */
    template<uint32_t Size>
    consteval descriptor::string<Size - 1> string_descriptor(const char (&str)[Size]) {
        static_assert(Size > 0, "String descriptor does not support string lengths of 0");
        descriptor::string<Size - 1> ret;

        // copy the string into the string descriptor
        for (uint32_t i = 0; i < Size - 1; i++) {
            ret.bString[i] = str[i];
        }

        return ret;
    }
}

#endif