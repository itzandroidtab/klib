#ifndef KLIB_MB9BF566K_USB_HPP
#define KLIB_MB9BF566K_USB_HPP

#include <cstdint>

#include <targets/core/cypress/mb9bf560l/usb.hpp>
#include "pins.hpp"

namespace klib::mb9bf566k::io::periph::detail::usb {
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

namespace klib::mb9bf566k::io::periph::lqfp_48 {
    struct usb0 {
        // peripheral id (e.g usb0, usb1)
        constexpr static uint32_t id = 0;

        // peripheral clock bit position
        constexpr static uint32_t clock_id = 31;

        // interrupt ids (including the arm vector table)
        // we have 2 interrupts for usb. One interrupt that
        // handles the states + ep0 and another that handles
        // all the other interrupts
        constexpr static uint32_t interrupt_id = 16 + 78;

        constexpr static uint32_t interrupt_ep0_id = 16 + 79;

        // port to the usb hardware
        static inline USB0_Type *const port = USB0;

        // configuration of the pins
        using connect = detail::usb::usb<pins::package::lqfp_48::p43, detail::usb::mode::connect, core::mb9bf560l::io::detail::alternate::system_function::usbp0e<true>>;

        // these two pins are switched using the special port settings register
        using dplus = detail::usb::usb<pins::package::lqfp_48::p47, detail::usb::mode::dplus, core::mb9bf560l::io::detail::alternate::special::usb0c<true>>;
        using dminus = detail::usb::usb<pins::package::lqfp_48::p46, detail::usb::mode::dminus, core::mb9bf560l::io::detail::alternate::special::usb0c<true>>;
    };
}

namespace klib::mb9bf566k::io {
    template <typename Usb, typename Device>
    using usb = core::mb9bf560l::io::usb<Usb, Device>;
}

#endif
