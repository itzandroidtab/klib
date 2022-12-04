#ifndef KLIB_MAX32660_UART_HPP
#define KLIB_MAX32660_UART_HPP

#include "pins.hpp"

namespace klib::max32660::io::periph::detail::uart {
    enum class mode {
        txd,
        rxd,
        cts,
        rts
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

namespace klib::max32660::io::periph::wlp {
    struct uart0 {
        // peripheral id (e.g uart0, uart1)
        constexpr static uint32_t id = 0;

        // peripheral clock bit position
        constexpr static uint32_t clock_id = 9;

        // configuration of the pins
        using txd = detail::uart::uart<pins::package::wlp::pd3, detail::uart::mode::txd, io::detail::alternate::func_2>;
        using rxd = detail::uart::uart<pins::package::wlp::pd4, detail::uart::mode::rxd, io::detail::alternate::func_2>;
        using rts = detail::uart::uart<pins::package::wlp::pc4, detail::uart::mode::rts, io::detail::alternate::func_2>;
        using cts = detail::uart::uart<pins::package::wlp::pc3, detail::uart::mode::cts, io::detail::alternate::func_2>;
    };

    struct uart1_0 {
        // peripheral id (e.g uart0, uart1)
        constexpr static uint32_t id = 1;

        // peripheral clock bit position
        constexpr static uint32_t clock_id = 10;

        // configuration of the pins
        using txd = detail::uart::uart<pins::package::wlp::pc1, detail::uart::mode::txd, io::detail::alternate::func_3>;
        using rxd = detail::uart::uart<pins::package::wlp::pc2, detail::uart::mode::rxd, io::detail::alternate::func_3>;
    };

    struct uart1_1 {
        // peripheral id (e.g uart0, uart1)
        constexpr static uint32_t id = 1;

        // peripheral clock bit position
        constexpr static uint32_t clock_id = 10;

        // configuration of the pins
        using txd = detail::uart::uart<pins::package::wlp::pc3, detail::uart::mode::txd, io::detail::alternate::func_3>;
        using rxd = detail::uart::uart<pins::package::wlp::pc4, detail::uart::mode::rxd, io::detail::alternate::func_3>;
    };
}

namespace klib::max32660::io::periph::tqfn_24 {
    struct uart0 {
        // peripheral id (e.g uart0, uart1)
        constexpr static uint32_t id = 0;

        // peripheral clock bit position
        constexpr static uint32_t clock_id = 9;

        // peripheral interrupt position
        constexpr static uint32_t irq_id = 30;

        // configuration of the pins
        using txd = detail::uart::uart<pins::package::tqfn_24::p21, detail::uart::mode::txd, io::detail::alternate::func_2>;
        using rxd = detail::uart::uart<pins::package::tqfn_24::p20, detail::uart::mode::rxd, io::detail::alternate::func_2>;
        using rts = detail::uart::uart<pins::package::tqfn_24::p18, detail::uart::mode::rts, io::detail::alternate::func_2>;
        using cts = detail::uart::uart<pins::package::tqfn_24::p19, detail::uart::mode::cts, io::detail::alternate::func_2>;
    };

    struct uart1_0 {
        // peripheral id (e.g uart0, uart1)
        constexpr static uint32_t id = 1;

        // peripheral clock bit position
        constexpr static uint32_t clock_id = 10;

        // peripheral interrupt position
        constexpr static uint32_t irq_id = 31;

        // configuration of the pins
        using txd = detail::uart::uart<pins::package::tqfn_24::p14, detail::uart::mode::txd, io::detail::alternate::func_2>;
        using rxd = detail::uart::uart<pins::package::tqfn_24::p13, detail::uart::mode::rxd, io::detail::alternate::func_2>;
        using rts = detail::uart::uart<pins::package::tqfn_24::p3, detail::uart::mode::rts, io::detail::alternate::func_2>;
        using cts = detail::uart::uart<pins::package::tqfn_24::p4, detail::uart::mode::cts, io::detail::alternate::func_2>;
    };

    struct uart1_1 {
        // peripheral id (e.g uart0, uart1)
        constexpr static uint32_t id = 1;

        // peripheral clock bit position
        constexpr static uint32_t clock_id = 10;

        // peripheral interrupt position
        constexpr static uint32_t irq_id = 31;

        // configuration of the pins
        using txd = detail::uart::uart<pins::package::tqfn_24::p2, detail::uart::mode::txd, io::detail::alternate::func_3>;
        using rxd = detail::uart::uart<pins::package::tqfn_24::p1, detail::uart::mode::rxd, io::detail::alternate::func_3>;
        using rts = detail::uart::uart<pins::package::tqfn_24::p3, detail::uart::mode::rts, io::detail::alternate::func_2>;
        using cts = detail::uart::uart<pins::package::tqfn_24::p4, detail::uart::mode::cts, io::detail::alternate::func_2>;
    };

    struct uart1_2 {
        // peripheral id (e.g uart0, uart1)
        constexpr static uint32_t id = 1;

        // peripheral clock bit position
        constexpr static uint32_t clock_id = 10;

        // peripheral interrupt position
        constexpr static uint32_t irq_id = 31;

        // configuration of the pins
        using txd = detail::uart::uart<pins::package::tqfn_24::p19, detail::uart::mode::txd, io::detail::alternate::func_3>;
        using rxd = detail::uart::uart<pins::package::tqfn_24::p18, detail::uart::mode::rxd, io::detail::alternate::func_3>;
        using rts = detail::uart::uart<pins::package::tqfn_24::p3, detail::uart::mode::rts, io::detail::alternate::func_2>;
        using cts = detail::uart::uart<pins::package::tqfn_24::p4, detail::uart::mode::cts, io::detail::alternate::func_2>;
    };
}

#endif