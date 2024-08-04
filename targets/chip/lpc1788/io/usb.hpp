#ifndef KLIB_LPC1788_USB_HPP
#define KLIB_LPC1788_USB_HPP

#include <cstdint>

#include <targets/core/nxp/lpc17xx/usb.hpp>
#include "pins.hpp"

namespace klib::lpc1788::io::periph::detail::usb {
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

namespace klib::lpc1788::io::periph::lqfp_208 {
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
        using connect = detail::usb::usb<pins::package::lqfp_208::p132, detail::usb::mode::connect, core::lpc178x::io::detail::alternate::func_1>;
        using led = detail::usb::usb<pins::package::lqfp_208::p66, detail::usb::mode::led, core::lpc178x::io::detail::alternate::func_1>;
        using vbus = detail::usb::usb<pins::package::lqfp_208::p42, detail::usb::mode::vbus, core::lpc178x::io::detail::alternate::func_2>;
        using dplus = detail::usb::usb<pins::package::lqfp_208::p61, detail::usb::mode::dplus, core::lpc178x::io::detail::alternate::func_1>;
        using dminus = detail::usb::usb<pins::package::lqfp_208::p62, detail::usb::mode::dminus, core::lpc178x::io::detail::alternate::func_1>;
    };
}

namespace klib::lpc1788::io {
    template <typename Usb, typename Device>
    using usb = core::lpc17xx::io::usb<Usb, Device>;
}

#endif
