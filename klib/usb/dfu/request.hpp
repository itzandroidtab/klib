#ifndef KLIB_USB_DFU_REQUEST_HPP
#define KLIB_USB_DFU_REQUEST_HPP

#include <cstdint>

#include <klib/usb/descriptor.hpp>

// Push the current pack to the stack and set the pack to 1
// as all these structs have specific sizes
#pragma pack(push, 1)

namespace klib::usb::dfu {
    /**
     * DFU USB requests
     * 
     */
    enum class dfu_request {
        // wValue = wTimeout
        // wIndex = interface
        // wLength = zero
        // data = none
        detach = 0x00,

        // wValue = wBlockNum
        // wIndex = interface
        // wLength = length
        // data = firmware
        download = 0x01,

        // wValue = zero
        // wIndex = interface
        // wLength = length
        // data = firmware
        upload = 0x02,

        // wValue = zero
        // wIndex = interface
        // wLength = 6
        // data = status
        get_status = 0x03,

        // wValue = zero
        // wIndex = interface
        // wLength = zero
        // data = none
        clear_status = 0x04,

        // wValue = zero
        // wIndex = interface
        // wLength = 1
        // data = state
        get_state = 0x05,

        // wValue = zero
        // wIndex = interface
        // wLength = zero
        // data = none
        abort = 0x06,
    };

    /**
     * @brief Device status
     * 
     */
    enum class device_status: uint8_t {
        ok = 0x00,
        target_error = 0x01,
        file_error = 0x02,
        write_error = 0x03,
        erase_error = 0x04,
        erase_check_error = 0x05,
        program_error = 0x06,
        verify_error = 0x07,
        address_error = 0x08,
        not_done_error = 0x09,
        firmware_error = 0x0a,
        vendor_error = 0x0b,
        usb_reset_error = 0x0c,
        por_error = 0x0d,
        unknown_error = 0x0e,
        stall_error = 0x0f
    };

    /**
     * @brief Device state
     * 
     */
    enum class device_state: uint8_t {
        app_idle = 0x00,
        app_detach = 0x01,
        dfu_idle = 0x02,
        download_sync = 0x03,
        download_busy = 0x04,
        download_idle = 0x05,
        manifest_sync = 0x06,
        manifset = 0x07,
        manifest_wait_reset = 0x08,
        upload = 0x09,
        error = 0x0a,
    };

    /**
     * @brief Status struct. Response the device should send when a 
     * getstatus request is received
     * 
     */
    struct status {
        // an indication of the status resulting from the
        // execution of the most recent request
        uint8_t bStatus;

        // minimum time, in milliseconds, that the host
        // should wait before sending a subsequent
        // DFU_GETSTATUS request
        uint32_t bwPollTimout: 24;

        // an indication of the state that the device is going to
        // enter immediately following transmission of this
        // response. (By the time the host receives this
        // information, this is the current state of the device.) 
        uint8_t bState;

        // index of status description in string table
        uint8_t iString;
    };

    static_assert(sizeof(status) == 0x06, "DFU status size is wrong");
}

// release the old pack so the rest of the structs are not 
// affected by the pack(1)
#pragma pack(pop)

#endif