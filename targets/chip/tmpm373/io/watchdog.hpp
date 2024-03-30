#ifndef KLIB_TMPM37X_WATCHDOG_HPP
#define KLIB_TMPM37X_WATCHDOG_HPP

#include <cstdint>

#include <targets/core/toshiba/tmpm37x/watchdog.hpp>

namespace klib::tmpm373::io::periph {
    struct wdt0 {
        // peripheral id (e.g wdt0, wdt1)
        constexpr static uint32_t id = 0;

        // peripheral interrupt position
        constexpr static uint32_t interrupt_id = 1;

        // port to the wdt hardware
        static inline WD_Type *const port = WD;
    };
}

namespace klib::tmpm373::io {
    template <typename Wdt>
    using watchdog = core::tmpm37x::io::watchdog<Wdt>;
}

#endif