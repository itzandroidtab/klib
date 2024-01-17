#ifndef KLIB_TOSHIBA_TMPM37X_WATCHDOG_HPP
#define KLIB_TOSHIBA_TMPM37X_WATCHDOG_HPP

#include <cstdint>

#include <klib/klib.hpp>

namespace klib::core::tmpm37x::io {
    template <typename Wdt>
    class watchdog {
    public:
        /**
         * @brief Disable the watchdog
         * 
         */
        static void disable() {
            // disable the watchdog
            Wdt::port->MOD &= ~(0x1 << 7);

            // feed the watchdog with the disable byte
            Wdt::port->CR = 0xb1;
        }

        /**
         * @brief Feed the watchdog to prevent the cpu from resetting 
         * or triggering the watchdog interrupt.
         * 
         */
        static void feed() {
            // feed the watchdog
            Wdt::port->CR = 0x4e;
        }

        /**
         * @brief Init the watchdog timer.
         * 
         * @tparam Reset enable/disable if the cpu resets when the watchdog timer overflows. When
         * reset is disabled it will generate a interrupt
         * @tparam PauseInIdle for the watchdog
         * @tparam DetectionTime reset period (see datasheet page 512 for more info. The timing 
         * depends on the system frequency)
         */
        template <bool Reset = false, bool PauseInIdle = true, uint8_t DetectionTime = 0b101>
        static void init() {
            // make sure the detection time is valid
            static_assert(DetectionTime <= 0b101, "Invalid detection time");

            // enable the watchdog timer
            Wdt::port->MOD = (
                (0x1 << 7) | (PauseInIdle << 2) | 
                (Reset << 1) | (DetectionTime << 4)
            );

            // clear the watchdog just in case
            feed();
        }
    };
}

#endif