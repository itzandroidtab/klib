#ifndef KLIB_USB_DEVICE_HPP
#define KLIB_USB_DEVICE_HPP

namespace klib::usb {
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
}

#endif