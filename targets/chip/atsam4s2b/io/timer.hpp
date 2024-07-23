#ifndef KLIB_ATSAM4S2B_TIMER_HPP
#define KLIB_ATSAM4S2B_TIMER_HPP

#include <cstdint>

#include <targets/core/atmel/atsam4s/timer.hpp>

namespace klib::atsam4s2b::io::periph {
    struct tc0 {
        // peripheral id (e.g tc0, tc1)
        constexpr static uint32_t id = 0;

        // interrupt id (including the arm vector table)
        constexpr static uint32_t interrupt_id = (23 + 16);

        // peripheral clock bit position
        constexpr static uint32_t clock_id = 23;

        // port to the timer hardware
        static inline TC0_Type *const port = TC0;

        // available channels in the timer
        constexpr static uint32_t max_channels = 3;
    };

    struct tc1 {
        // peripheral id (e.g tc0, tc1)
        constexpr static uint32_t id = 1;

        // interrupt id (including the arm vector table)
        constexpr static uint32_t interrupt_id = (26 + 16);

        // power bit position
        constexpr static uint32_t clock_id = 26;

        // port to the timer hardware
        static inline TC0_Type *const port = TC1;

        // available channels in the timer
        constexpr static uint32_t max_channels = 3;
    };
}

namespace klib::atsam4s2b::io {
    /**
     * @brief Basic timer. Uses interrupts to call a callback.
     * 
     * @tparam Timer 
     */
    template <typename Timer, uint32_t Channel, uint32_t Div = 2>
    using timer = core::atsam4s::io::timer<Timer, Channel, 2, Div>;

    /**
     * @brief Oneshot timer. Uses interrupt to call a callback once.
     * 
     * @tparam Timer 
     */
    template <typename Timer, uint32_t Channel, uint32_t Div = 2>
    using oneshot_timer = core::atsam4s::io::oneshot_timer<Timer, Channel, 2, Div>;
}

#endif