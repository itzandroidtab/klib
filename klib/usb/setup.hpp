#ifndef KLIB_USB_SETUP_HPP
#define KLIB_USB_SETUP_HPP

#include <cstdint>

// Push the current pack to the stack and set the pack to 1
// as all these structs have specific sizes
#pragma pack(push, 1)

/**
 * Definitions are taken from:
 * https://www.beyondlogic.org/usbnutshell/
 * 
 */
namespace klib::usb {
    /**
     * More information can be found here:
     * https://www.beyondlogic.org/usbnutshell/usb6.shtml
     * 
     */

    /**
     * 8 byte long USB setup packet
     * 
     */
    struct setup_packet {
        // b[0..4] = Recipient
        // 0x0 = device
        // 0x1 = interface
        // 0x2 = endpoint
        // 0x3 = Other
        // b[5..6] = Type
        // 0x0 = standard (standard)
        // 0x1 = interface (uclass)
        // 0x2 = vendor (vendor)
        // 0x3 = reserved (reserved)
        // b[7] = data phase transfer
        // 0x0 = host to device
        // 0x1 = device to host
        uint8_t bmRequestType;

        // b[0..7] request
        uint8_t bRequest;

        // b[0..16] value
        uint16_t wValue;

        // b[0..16] index or offset
        uint16_t wIndex;

        // b[0..16] Count
        uint16_t wLength;
    };

    static_assert(sizeof(setup_packet) == 8, "Setup packet is not 8 bytes in length");
}

namespace klib::usb::setup {
    /**
     * Speed mode of a USB device
     * 
     */
    enum class usb_speed {
        low_speed,
        full_speed,
        high_speed
    };

    /**
     * Version of the USB device
     * 
     */
    enum class usb_version {
        usb_v1_0 = 0x0100,
        usb_v1_1 = 0x0110,
        usb_v2_0 = 0x0200,
        usb_v2_1 = 0x0201
    };

    /**
     * USB request data transfer direction
     * (bmRequestType)
     * 
     */
    enum class direction {
        host_to_device = 0,
        device_to_host = 1
    };

    /**
     * Usb request types
     * (bmRequestType)
     * 
     */
    enum class request_type {
        standard = 0,
        uclass = 1,
        vendor = 2,
        reserved = 3
    };

    /**
     * USB recipient codes
     * (bmRequestType)
     * 
     */
    enum class recipient_code {
        device = 0,
        interface = 1,
        endpoint = 2,
        other = 3,
        reserved
    };

    /**
     * Standard USB requests
     * (bRequest)
     * 
     */
    enum class device_request {
        // wValue = zero
        // wIndex = zero
        // wLength = 2
        // data = device status
        get_status = 0x00,

        // wValue = feature selector
        // wIndex = zero
        // wLength = zero
        // data = none
        clear_feature = 0x01,

        // wValue = feature selector
        // wIndex = zero
        // wLength = zero
        // data = none
        set_feature = 0x03,

        // wValue = device address
        // wIndex = zero
        // wLength = zero
        // data = none
        set_address = 0x05,

        // wValue = descriptor type & index
        // wIndex = zero or language id
        // wLength = descriptor length
        // data = descriptor
        get_descriptor = 0x06,

        // wValue = descriptor type & index
        // wIndex = zero or language id
        // wLength = descriptor length
        // data = descriptor
        set_descriptor = 0x07,

        // wValue = zero
        // wIndex = zero
        // wLength = 1
        // data = configuration value
        get_configuration = 0x08,

        // wValue = configuration value
        // wIndex = zero
        // wLength = zero
        // data = none
        set_configuration = 0x09,

        // wValue = zero
        // wIndex = interface
        // wLength = one
        // data = alternate interface
        get_interface = 0x0a,

        // wValue = alternate setting
        // wIndex = interface
        // wLength = zero
        // data = none
        set_interface = 0x0b,

        // wValue = zero
        // wIndex = endpoint
        // wLength = 2
        // data = frame number
        synch_frame = 0x0c,
    };

    /**
     * interface USB requests
     * (bRequest)
     * 
     * wIndex is normally used to specify the referring interface for 
     * requests directed at the interface. Its format is shown below.
     * 
     * wIndex:
     * b[0..7] = interface number
     * b[8..15] = reserved
     * 
     */
    enum class interface_request {
        // wValue = zero
        // wIndex = interface
        // wLength = 2
        // data = interface status
        get_status = 0x00,

        // wValue = feature selector
        // wIndex = interface
        // wLength = zero
        // data = none
        clear_feature = 0x01,

        // wValue = feature selector
        // wIndex = interface
        // wLength = zero
        // data = none
        set_feature = 0x03,

        // wValue = zero
        // wIndex = interface
        // wLength = 1
        // data = alternate interface
        get_interface = 0x0A,

        // wValue = alternative setting
        // wIndex = interface
        // wLength = zero
        // data = none
        set_interface = 0x11
    };

    /**
     * endpoint USB requests
     * (bRequest)
     * 
     * The wIndex field is normally used to specify the referring endpoint 
     * and direction for requests directed to an endpoint. Its format is 
     * shown below.
     * 
     * windex:
     * b[0..3] = endpoint number
     * b[4..6] = reserved
     * b[7] = dir
     * b[8..15] = reserved
     * 
     */
    enum class endpoint_request {
        // wValue = zero
        // wIndex = endpoint
        // wLength = 2
        // data = endpoint status
        get_status = 0x00,

        // wValue = feature selector
        // wIndex = endpoint
        // wLength = zero
        // data = none
        clear_feature = 0x01,

        // wValue = feature selector
        // wIndex = endpoint
        // wLength = zero
        // data = none
        set_feature = 0x03,

        // wValue = zero
        // wIndex = endpoint
        // wLength = 2
        // data = framenumber
        synch_frame = 0x12
    };

    /**
     * USB features
     * (wValue)
     * 
     */
    enum class feature {
        endpoint_halt = 0,
        remote_wake = 1,
        test_mode = 2,
    };
}

// release the old pack so the rest of the structs are not 
// affected by the pack(1)
#pragma pack(pop)

#endif 