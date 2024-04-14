#ifndef KLIB_ATSAM4S2B_USB_HPP
#define KLIB_ATSAM4S2B_USB_HPP

#include <cstdint>

#include <targets/core/atmel/atsam4s/usb.hpp>
#include "pins.hpp"

namespace klib::atsam4s2b::io::periph::detail::usb {
    enum class mode {
        dplus,
        dminus,
        vbus,
        connect,
        led,
    };

    template <typename Pin, mode Type>
    struct usb {
        // pin of the peripheral
        using pin = Pin;

        // type of the pin
        constexpr static mode type = Type;
    };
}

namespace klib::atsam4s2b::io::periph::wlcsp_64 {
    struct usb0 {
        // peripheral id (e.g usb0, usb1)
        constexpr static uint32_t id = 0;

        // peripheral clock bit position
        constexpr static uint32_t clock_id = 34;

        // interrupt id (including the arm vector table)
        constexpr static uint32_t interrupt_id = 50;

        // port to the usb hardware
        static inline UDP_Type *const port = UDP;

        // configuration of the pins (note: the usb pins are not switched
        // using the alternate functions)
        using dplus = detail::usb::usb<pins::package::wlcsp_64::pb4, detail::usb::mode::dplus>;
        using dminus = detail::usb::usb<pins::package::wlcsp_64::pb3, detail::usb::mode::dminus>;
    };
}

namespace klib::atsam4s2b::io {
    template <typename Usb, typename Device>
    using usb = core::atsam4s::io::usb<Usb, Device>;
}

#endif
