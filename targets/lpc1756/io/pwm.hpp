#ifndef KLIB_LPC1756_PWM_HPP
#define KLIB_LPC1756_PWM_HPP

#include <cstdint>

#include <lpc1756.hpp>

namespace klib::lpc1756::io::periph {
    struct pwm0 {
        // peripheral id (e.g pwm0, pwm1)
        constexpr static uint32_t id = 0;

        // interrupt id (including the arm vector table)
        constexpr static uint32_t interrupt_id = 25;

        // port to the PWM hardware
        static inline PWM_Type *const port = PWM;
    };
}

#endif