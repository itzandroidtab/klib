#ifndef KLIB_USB_MASS_STORAGE_DEVICE_HPP
#define KLIB_USB_MASS_STORAGE_DEVICE_HPP

#include <cstdint>

// Push the current pack to the stack and set the pack to 1
// as all these structs have specific sizes
#pragma pack(push, 1)

namespace klib::usb::msc {
    /**
     * @brief Mass storage class requests
     *
     */
    enum class requests: uint8_t {
        get_status = 0x00,
        get_requests = 0xfc,
        put_requests = 0xfd,
        get_max_lun = 0xfe,
        bulk_only_reset = 0xff,
    };
}

// release the old pack so the rest of the structs are not
// affected by the pack(1)
#pragma pack(pop)

#endif