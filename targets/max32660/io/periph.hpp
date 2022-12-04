#ifndef KLIB_MAX32660_PERIPH_HPP
#define KLIB_MAX32660_PERIPH_HPP

#include <cstdint>

#include "pins.hpp"

// different types of peripherals on the chip
namespace klib::max32660::io::periph::detail {
    namespace swd {
        enum class mode {
            dio,
            clk
        };

        template <typename Pin, mode Type, typename Periph>
        struct swd {
            // pin of the peripheral
            using pin = Pin;

            // type of the pin
            constexpr static mode type = Type;

            // alternate function
            using periph = Periph;
        };
    }

    namespace uart {
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
}

// global peripherals, not affected by chip package
namespace klib::max32660::io::periph {
    struct gpio0 {
        // peripheral id (e.g gpio0, gpio1)
        constexpr static uint32_t id = 0;

        // peripheral interrupt position
        constexpr static uint32_t irq_id = 40;
    };

    struct gpio0_wakeup {
        // peripheral id (e.g gpio0, gpio1)
        constexpr static uint32_t id = 0;

        // peripheral interrupt position
        constexpr static uint32_t irq_id = 70;
    };

    struct wdt0 {
        // peripheral id (e.g wdt0, wdt1)
        constexpr static uint32_t id = 0;

        // peripheral interrupt position
        constexpr static uint32_t irq_id = 17;
    };

    struct dma0 {
        // peripheral id (e.g dma0, dma1)
        constexpr static uint32_t id = 0;

        // peripheral clock bit position
        constexpr static uint32_t clock_id = 5;

        // peripheral interrupt position
        constexpr static uint32_t irq_id = 44;
    };
}

// peripheral pin mapping to alternate functions.
namespace klib::max32660::io::periph::wlp {
    struct swd0_0 {
        // peripheral id (e.g swd0, swd1)
        constexpr static uint32_t id = 0;

        // configuration of the pins
        using dio = detail::swd::swd<pins::package::wlp::pc1, detail::swd::mode::dio, io::detail::alternate::func_1>;
        using clk = detail::swd::swd<pins::package::wlp::pc2, detail::swd::mode::clk, io::detail::alternate::func_1>;
    };

    struct swd0_1 {
        // peripheral id (e.g swd0, swd1)
        constexpr static uint32_t id = 0;

        // configuration of the pins
        using dio = detail::swd::swd<pins::package::wlp::pb4, detail::swd::mode::dio, io::detail::alternate::func_2>;
        using clk = detail::swd::swd<pins::package::wlp::pb3, detail::swd::mode::clk, io::detail::alternate::func_2>;
    };
}

namespace klib::max32660::io::periph::tqfn_24 {
    struct swd0_0 {
        // peripheral id (e.g swd0, swd1)
        constexpr static uint32_t id = 0;

        // configuration of the pins
        using dio = detail::swd::swd<pins::package::tqfn_24::p2, detail::swd::mode::dio, io::detail::alternate::func_1>;
        using clk = detail::swd::swd<pins::package::tqfn_24::p1, detail::swd::mode::clk, io::detail::alternate::func_1>;
    };

    struct swd0_1 {
        // peripheral id (e.g swd0, swd1)
        constexpr static uint32_t id = 0;

        // configuration of the pins
        using dio = detail::swd::swd<pins::package::tqfn_24::p14, detail::swd::mode::dio, io::detail::alternate::func_2>;
        using clk = detail::swd::swd<pins::package::tqfn_24::p13, detail::swd::mode::clk, io::detail::alternate::func_2>;
    };
}

#endif