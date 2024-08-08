#ifndef KLIB_MB9BF566K_WATCHDOG_HPP
#define KLIB_MB9BF566K_WATCHDOG_HPP

#include <cstdint>

#include <targets/core/cypress/mb9bf560l/watchdog.hpp>

namespace klib::mb9bf566k::io::periph {
    struct wdt0 {
        // peripheral id (e.g wdt0, wdt1)
        constexpr static uint32_t id = 0;

        // peripheral interrupt position (uses the NMI interrupt for the watchdog)
        constexpr static uint32_t interrupt_id = 2;

        // port to the wdt hardware
        static inline HWWDT_Type *const port = HWWDT;
    };
}

namespace klib::mb9bf566k::io {
    template <typename Wdt>
    using watchdog = core::mb9bf560l::io::watchdog<Wdt>;
}

#endif