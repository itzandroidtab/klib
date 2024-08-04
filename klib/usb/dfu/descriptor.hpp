#ifndef KLIB_USB_DFU_DESCRIPTOR_HPP
#define KLIB_USB_DFU_DESCRIPTOR_HPP

#include <cstdint>

#include <klib/usb/descriptor.hpp>

// Push the current pack to the stack and set the pack to 1
// as all these structs have specific sizes
#pragma pack(push, 1)

namespace klib::usb::dfu {
    /**
     * @brief DFU functional descriptor
     *
     */
    struct functional {
        // size of the descriptor
        const uint8_t bLength = sizeof(functional);

        // descriptor type (DFU FUNCTIONAL)
        const uint8_t bDescriptorType = 0x21;

        // DFU attributes
        // b[0] = download capable
        // b[1] = upload capable
        // b[2] = manifestation tolerant
        // b[3] = will detach
        // b[4..7] = reserved, set to 0
        uint8_t bmAttributes;

        // detach timeout in milliseconds
        uint16_t wDetachTimeOut;

        // maximum number of bytes that the device can accept
        // per control-write transaction
        uint16_t wTransferSize;

        // numeric expression identifying the version of the
        // DFU specifiication release
        uint16_t bcdDFUVersion = 0x0101;
    };

    static_assert(sizeof(functional) == 0x09, "DFU descriptor size is wrong");
}

// release the old pack so the rest of the structs are not
// affected by the pack(1)
#pragma pack(pop)

#endif