#ifndef KLIB_LPC1752_WATCHDOG_HPP
#define KLIB_LPC1752_WATCHDOG_HPP

#include <cstdint>

#include <targets/core/nxp/lpc175x/watchdog.hpp>

namespace klib::lpc1752::io::periph {
    struct wdt0 {
        // peripheral id (e.g wdt0, wdt1)
        constexpr static uint32_t id = 0;

        // peripheral interrupt position
        constexpr static uint32_t interrupt_id = 16;

        // port to the wdt hardware
        static inline WDT_Type *const port = WDT;
    };
}

namespace klib::lpc1752::io {
    template <typename Wdt>
    using watchdog = core::lpc175x::io::watchdog<Wdt>;
}

#endif