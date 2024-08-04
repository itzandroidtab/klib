#ifndef KLIB_USB_DEVICE_CAPABILITY_HPP
#define KLIB_USB_DEVICE_CAPABILITY_HPP

#include <cstdint>

#include "descriptor.hpp"

// Push the current pack to the stack and set the pack to 1
// as all these structs have specific sizes
#pragma pack(push, 1)

namespace klib::usb::capability {
    /**
     * @brief Device capability type codes
     *
     */
    enum class capabilty_type {
        wireless_usb = 0x01,
        usb_2_0_extension = 0x02,
        superspeed_usb = 0x03,
        container_id = 0x04,
    };

    /**
     * @brief USB 2.0 extension
     *
     */
    struct usb_2_0_extension {
        // bitmap encoding of supported device level features.
        // b[0] = reserved
        // b[1] = lpm
        // b[2..31] = reserved
        uint32_t bmAttributes;
    };
}

// release the old pack so the rest of the structs are not
// affected by the pack(1)
#pragma pack(pop)

#endif