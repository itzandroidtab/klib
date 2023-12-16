#ifndef KLIB_LPC1752_USB_HPP
#define KLIB_LPC1752_USB_HPP

#include <cstdint>

#include <targets/core/nxp/lpc175x/usb.hpp>
#include "pins.hpp"

namespace klib::lpc1752::io::periph::detail::usb {
    enum class mode {
        dplus,
        dminus,
        vbus,
        connect,
        led,
    };

    template <typename Pin, mode Type, typename Periph>
    struct usb {
        // pin of the peripheral
        using pin = Pin;

        // type of the pin
        constexpr static mode type = Type;

        // alternate function
        using periph = Periph;
    };
}

namespace klib::lpc1752::io::periph::lqfp_80 {
    struct usb0 {
        // peripheral id (e.g usb0, usb1)
        constexpr static uint32_t id = 0;

        // peripheral clock bit position
        constexpr static uint32_t clock_id = 31;

        // interrupt id (including the arm vector table)
        constexpr static uint32_t interrupt_id = 40;

        // port to the usb hardware
        static inline USB_Type *const port = USB;

        // configuration of the pins 
        using connect = detail::usb::usb<pins::package::lqfp_80::p49, detail::usb::mode::connect, core::lpc175x::io::detail::alternate::func_1>;
        using led = detail::usb::usb<pins::package::lqfp_80::p25, detail::usb::mode::led, core::lpc175x::io::detail::alternate::func_1>;
        using vbus = detail::usb::usb<pins::package::lqfp_80::p18, detail::usb::mode::vbus, core::lpc175x::io::detail::alternate::func_2>;
        using dplus = detail::usb::usb<pins::package::lqfp_80::p22, detail::usb::mode::dplus, core::lpc175x::io::detail::alternate::func_1>;
        using dminus = detail::usb::usb<pins::package::lqfp_80::p23, detail::usb::mode::dminus, core::lpc175x::io::detail::alternate::func_1>;
    };
}

namespace klib::lpc1752::io {
    template <typename Usb, typename Device>
    using usb = core::lpc175x::io::usb<Usb, Device>;
}

#endif
