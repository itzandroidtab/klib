#ifndef KLIB_ATSAM4S2B_UART_HPP
#define KLIB_ATSAM4S2B_UART_HPP

#include <cstdint>

#include <atsam4s2b.hpp>

#include "pins.hpp"

namespace klib::atsam4s2b::io::periph::detail::uart {
    enum class mode {
        txd,
        rxd,
    };

    template <typename Pin, mode Type, typename Periph>
    struct uart {
        // pin of the peripheral
        using pin = Pin;

        // type of the pin
        constexpr static mode type = Type;

        // alternate function
        using periph = Periph;
    };
}

namespace klib::atsam4s2b::io::periph::wlcsp_64 {
    struct uart0 {
        // peripheral id (e.g uart0, uart1)
        constexpr static uint32_t id = 0;

        // peripheral clock bit position
        constexpr static uint32_t clock_id = 8;

        // interrupt id (including the arm vector table)
        constexpr static uint32_t interrupt_id = 24;

        // port to the UART hardware
        static inline UART0_Type *const port = UART0;

        // configuration of the pins
        using txd = detail::uart::uart<pins::package::wlcsp_64::pc5, detail::uart::mode::txd, core::atsam4s::io::detail::alternate::func_1>;
        using rxd = detail::uart::uart<pins::package::wlcsp_64::pg3, detail::uart::mode::rxd, core::atsam4s::io::detail::alternate::func_1>;
    };

    struct uart1 {
        // peripheral id (e.g uart0, uart1)
        constexpr static uint32_t id = 1;

        // peripheral clock bit position
        constexpr static uint32_t clock_id = 9;

        // interrupt id (including the arm vector table)
        constexpr static uint32_t interrupt_id = 25;

        // port to the UART hardware
        static inline UART1_Type *const port = UART1;

        // configuration of the pins
        using txd = detail::uart::uart<pins::package::wlcsp_64::pd8, detail::uart::mode::txd, core::atsam4s::io::detail::alternate::func_1>;
        using rxd = detail::uart::uart<pins::package::wlcsp_64::pc7, detail::uart::mode::rxd, core::atsam4s::io::detail::alternate::func_1>;
    };
}

#endif