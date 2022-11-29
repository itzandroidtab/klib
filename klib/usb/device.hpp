#ifndef KLIB_USB_DEVICE_HPP
#define KLIB_USB_DEVICE_HPP

#include <klib/usb/usb.hpp>

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
    constexpr usb::description descriptor_to_desc(const T& desc, const uint32_t size) {
        return {reinterpret_cast<const uint8_t *const>(&desc), size};
    }

    /**
     * @brief Helper function to convert a raw descriptor array to a descriptor description
     * 
     * @param desc 
     * @param size 
     * @return constexpr usb::description 
     */
    constexpr usb::description descriptor_to_desc(const uint8_t *const desc, const uint32_t size) {
        return {desc, size};
    }
}

#endif