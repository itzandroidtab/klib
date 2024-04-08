#ifndef KLIB_ATSAM4S2B_WATCHDOG_HPP
#define KLIB_ATSAM4S2B_WATCHDOG_HPP

#include <cstdint>

#include <atsam4s2b.hpp>

#include <targets/core/atmel/atsam4s/watchdog.hpp>

namespace klib::atsam4s2b::io::periph {
    struct wdt0 {
        // peripheral id (e.g wdt0, wdt1)
        constexpr static uint32_t id = 0;

        // peripheral interrupt position
        constexpr static uint32_t interrupt_id = 4;

        // port to the wdt hardware
        static inline WDT_Type *const port = WDT;
    };
}

namespace klib::atsam4s2b::io {
    template <typename Wdt>
    using watchdog = core::atsam4s::io::watchdog<Wdt>;
}

#endif