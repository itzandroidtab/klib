#ifndef KLIB_LPC1756_TIMER_HPP
#define KLIB_LPC1756_TIMER_HPP

#include <cstdint>

#include <lpc1756.hpp>

namespace klib::lpc1756::io::periph {
    struct tc0 {
        // peripheral id (e.g tc0, tc1)
        constexpr static uint32_t id = 0;

        // peripheral interrupt position
        constexpr static uint32_t irq_id = 19;

        // port to the timer hardware
        static inline TIMER0_Type *const port = TIMER0;
    };

    struct tc1 {
        // peripheral id (e.g tc0, tc1)
        constexpr static uint32_t id = 1;

        // peripheral interrupt position
        constexpr static uint32_t irq_id = 20;

        // port to the timer hardware
        static inline TIMER0_Type *const port = TIMER1;
    };

    struct tc2 {
        // peripheral id (e.g tc0, tc1)
        constexpr static uint32_t id = 2;

        // peripheral interrupt position
        constexpr static uint32_t irq_id = 21;

        // port to the timer hardware
        static inline TIMER0_Type *const port = TIMER2;
    };

    struct tc3 {
        // peripheral id (e.g tc0, tc1)
        constexpr static uint32_t id = 3;

        // peripheral interrupt position
        constexpr static uint32_t irq_id = 22;

        // port to the timer hardware
        static inline TIMER0_Type *const port = TIMER3;
    };
}



#endif