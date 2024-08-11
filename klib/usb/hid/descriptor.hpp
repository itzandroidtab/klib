#ifndef KLIB_USB_HID_DESCRIPTOR_HPP
#define KLIB_USB_HID_DESCRIPTOR_HPP

#include <cstdint>

// Push the current pack to the stack and set the pack to 1
// as all these structs have specific sizes
#pragma pack(push, 1)

namespace klib::usb::hid {
    /**
     * @brief Class specific descriptors
     *
     */
    enum class class_type: uint8_t {
        hid = 0x21,
        report = 0x22,
        pyhisical = 0x23
    };

    /**
     * @brief Hid device descriptor
     *
     */
    struct descriptor {
        // size of the descriptor
        const uint8_t bLength = sizeof(descriptor);

        // descriptor type
        class_type bDescriptorType;

        // Hid class specification number which device complies to
        uint16_t bcdHID;

        // hardware target country
        uint8_t bCountryCode;

        // number of hid class descriptors to follow
        uint8_t bNumDescriptors;

        // report descriptor type
        class_type bClassDescriptorType;

        // total length of the report descriptor
        uint16_t wDescriptorLength;
    };

    static_assert(sizeof(descriptor) == 0x09, "Hid descriptor size is wrong");
}

// release the old pack so the rest of the structs are not
// affected by the pack(1)
#pragma pack(pop)

#endif