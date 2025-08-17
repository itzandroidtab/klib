#ifndef KLIB_ATSAM3X8E_WATCHDOG_HPP
#define KLIB_ATSAM3X8E_WATCHDOG_HPP

#include <cstdint>

#include <targets/core/atmel/atsamxx/watchdog.hpp>

namespace klib::atsam3x8e::io::periph {
    struct wdt0 {
        // peripheral id (e.g wdt0, wdt1)
        constexpr static uint32_t id = 0;

        // peripheral interrupt position
        constexpr static uint32_t interrupt_id = 4;

        // port to the wdt hardware
        static inline WDT_Type *const port = WDT;
    };
}

namespace klib::atsam3x8e::io {
    template <typename Wdt>
    using watchdog = core::atsamxx::io::watchdog<Wdt>;
}

#endif