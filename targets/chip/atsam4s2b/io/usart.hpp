#ifndef KLIB_ATSAM4S2B_USART_HPP
#define KLIB_ATSAM4S2B_USART_HPP

#include <cstdint>

#include <atsam4s2b.hpp>

namespace klib::atsam4s2b::io::periph::detail::usart {
    enum class mode {
        txd,
        rxd,
        sck,
        cts,
        rts,
        dtr,
        dsr,
        dcd,
        ri
    };

    template <typename Pin, mode Type, typename Periph>
    struct usart {
        // pin of the peripheral
        using pin = Pin;

        // type of the pin
        constexpr static mode type = Type;

        // alternate function
        using periph = Periph;
    };
}


namespace klib::atsam4s2b::io::periph::wlcsp_64 {
    struct usart0 {
        // peripheral id (e.g usart0, usart1)
        constexpr static uint32_t id = 0;

        // peripheral clock bit position
        constexpr static uint32_t clock_id = 14;

        // interrupt id (including the arm vector table)
        constexpr static uint32_t interrupt_id = 24;

        // port to the USART hardware
        static inline USART0_Type *const port = USART0;

        // configuration of the pins
        using txd = detail::usart::usart<pins::package::wlcsp_64::pf4, detail::usart::mode::txd, core::atsam4s::io::detail::alternate::func_1>;
        using rxd = detail::usart::usart<pins::package::wlcsp_64::pf3, detail::usart::mode::rxd, core::atsam4s::io::detail::alternate::func_1>;
        using sck = detail::usart::usart<pins::package::wlcsp_64::pd2, detail::usart::mode::sck, core::atsam4s::io::detail::alternate::func_2>;
        using cts = detail::usart::usart<pins::package::wlcsp_64::ph3, detail::usart::mode::cts, core::atsam4s::io::detail::alternate::func_1>;
        using rts = detail::usart::usart<pins::package::wlcsp_64::ph2, detail::usart::mode::rts, core::atsam4s::io::detail::alternate::func_1>;
    };

    struct usart1 {
        // peripheral id (e.g usart0, usart1)
        constexpr static uint32_t id = 1;

        // peripheral clock bit position
        constexpr static uint32_t clock_id = 15;

        // interrupt id (including the arm vector table)
        constexpr static uint32_t interrupt_id = 25;

        // port to the USART hardware
        static inline USART1_Type *const port = USART1;

        // configuration of the pins
        using txd = detail::usart::usart<pins::package::wlcsp_64::pf6, detail::usart::mode::txd, core::atsam4s::io::detail::alternate::func_1>;
        using rxd = detail::usart::usart<pins::package::wlcsp_64::pf7, detail::usart::mode::rxd, core::atsam4s::io::detail::alternate::func_1>;
        using sck = detail::usart::usart<pins::package::wlcsp_64::pd6, detail::usart::mode::sck, core::atsam4s::io::detail::alternate::func_1>;
        using cts = detail::usart::usart<pins::package::wlcsp_64::pg5, detail::usart::mode::cts, core::atsam4s::io::detail::alternate::func_1>;
        using rts = detail::usart::usart<pins::package::wlcsp_64::pe5, detail::usart::mode::rts, core::atsam4s::io::detail::alternate::func_1>;
        using dtr = detail::usart::usart<pins::package::wlcsp_64::pe4, detail::usart::mode::dtr, core::atsam4s::io::detail::alternate::func_1>;
        using dsr = detail::usart::usart<pins::package::wlcsp_64::pd3, detail::usart::mode::dsr, core::atsam4s::io::detail::alternate::func_1>;
        using dcd = detail::usart::usart<pins::package::wlcsp_64::pd5, detail::usart::mode::dcd, core::atsam4s::io::detail::alternate::func_1>;
        using ri = detail::usart::usart<pins::package::wlcsp_64::pe3, detail::usart::mode::ri, core::atsam4s::io::detail::alternate::func_1>;
    };
}

#endif