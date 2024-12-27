#ifndef KLIB_LPC1788_WATCHDOG_HPP
#define KLIB_LPC1788_WATCHDOG_HPP

#include <cstdint>

#include <targets/core/nxp/lpc17xx/watchdog.hpp>

namespace klib::lpc1788::io::periph {
    struct wdt0 {
        // peripheral id (e.g wdt0, wdt1)
        constexpr static uint32_t id = 0;

        // peripheral interrupt position
        constexpr static uint32_t interrupt_id = 16;

        // port to the wdt hardware
        static inline WWDT_Type *const port = WWDT;
    };
}

namespace klib::lpc1788::io {
    template <typename Wdt>
    using watchdog = core::lpc175x::io::watchdog<Wdt>;
}

#endif