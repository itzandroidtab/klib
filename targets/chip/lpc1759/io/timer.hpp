#ifndef KLIB_LPC1759_TIMER_HPP
#define KLIB_LPC1759_TIMER_HPP

#include <cstdint>

#include <targets/core/nxp/lpc175x/timer.hpp>

namespace klib::lpc1759::io::periph {
    struct tc0 {
        // peripheral id (e.g tc0, tc1)
        constexpr static uint32_t id = 0;

        // interrupt id (including the arm vector table)
        constexpr static uint32_t interrupt_id = 17;

        // peripheral clock bit position
        constexpr static uint32_t clock_id = 1;

        // port to the timer hardware
        static inline TIMER0_Type *const port = TIMER0;

        // available channels in the timer
        constexpr static uint32_t max_channels = 4;
    };

    struct tc1 {
        // peripheral id (e.g tc0, tc1)
        constexpr static uint32_t id = 1;

        // interrupt id (including the arm vector table)
        constexpr static uint32_t interrupt_id = 18;

        // power bit position
        constexpr static uint32_t clock_id = 2;

        // port to the timer hardware
        static inline TIMER0_Type *const port = TIMER1;

        // available channels in the timer
        constexpr static uint32_t max_channels = 4;
    };

    struct tc2 {
        // peripheral id (e.g tc0, tc1)
        constexpr static uint32_t id = 2;

        // interrupt id (including the arm vector table)
        constexpr static uint32_t interrupt_id = 19;

        // power bit position
        constexpr static uint32_t clock_id = 22;

        // port to the timer hardware
        static inline TIMER0_Type *const port = TIMER2;

        // available channels in the timer
        constexpr static uint32_t max_channels = 4;
    };

    struct tc3 {
        // peripheral id (e.g tc0, tc1)
        constexpr static uint32_t id = 3;

        // interrupt id (including the arm vector table)
        constexpr static uint32_t interrupt_id = 20;

        // power bit position
        constexpr static uint32_t clock_id = 23;

        // port to the timer hardware
        static inline TIMER0_Type *const port = TIMER3;

        // available channels in the timer
        constexpr static uint32_t max_channels = 4;
    };

    struct pwm1 {
        // peripheral id (e.g pwm0, pwm1)
        constexpr static uint32_t id = 1;

        // interrupt id (including the arm vector table)
        constexpr static uint32_t interrupt_id = 35;

        // power bit position
        constexpr static uint32_t clock_id = 6;

        // port to the PWM hardware
        static inline PWM1_Type *const port = PWM1;

        // available channels in the pwm (Base and timer mode)
        constexpr static uint32_t max_channels = 4;

        // available channels in pwm mode
        constexpr static uint32_t max_pwm_channels = 7;
    };
}

namespace klib::lpc1759::io {
    /**
     * @brief Basic timer. Uses interrupts to call a callback.
     *
     * @tparam Timer
     */
    template <typename Timer, uint32_t Channel>
    using timer = core::lpc175x::io::timer<Timer, Channel>;

    /**
     * @brief Oneshot timer. Uses interrupt to call a callback once.
     *
     * @tparam Timer
     */
    template <typename Timer, uint32_t Channel>
    using oneshot_timer = core::lpc175x::io::oneshot_timer<Timer, Channel>;

    /**
     * @brief Pwm timer.
     *
     * @tparam Pwm
     * @tparam Channel
     */
    template <
        typename Pin, typename Pwm, uint8_t Channel,
        uint32_t Frequency, uint8_t Bits
    >
    using pin_timer = core::lpc175x::io::pin_timer<Pin, Pwm, Channel, Frequency, Bits>;
}

#endif