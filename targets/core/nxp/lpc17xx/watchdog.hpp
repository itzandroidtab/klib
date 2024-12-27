#ifndef KLIB_NXP_LPC175X_WATCHDOG_HPP
#define KLIB_NXP_LPC175X_WATCHDOG_HPP

#include <cstdint>

#include <klib/klib.hpp>

namespace klib::core::lpc175x::io {
    template <typename Wdt>
    class watchdog {
    public:
        /**
         * @brief Disable the watchdog
         *
         * @warning Does nothing for the lpc1756
         *
         */
        static void disable() {
            // The watchdog timer can only be turned off
            // by a hardware reset. So do nothing here.
        }

        /**
         * @brief Feed the watchdog to prevent the cpu from resetting
         * or triggering the watchdog interrupt.
         *
         */
        static void feed() {
            // Restart the watchdog timer
            Wdt::port->FEED = 0xaa;
            Wdt::port->FEED = 0x55;
        }

        /**
         * @brief Init the watchdog timer.
         *
         * @details If the reset is disabled the watchdog will
         * call the interrupt if enabled in the NVIC
         *
         * @warning After triggering the IRQ the interrupt should
         * be disabled otherwise the interrupt request will be
         * generated indefinitely.
         *
         * @tparam Reset enable/disable if the cpu resets when the watchdog timer overflows.
         * @tparam ResetPeriod reset period
         *
         * @tparam ClkSource for the watchdog
         */
        template <bool Reset = false, uint32_t ResetPeriod = 0, uint8_t ClkSource>
        static void init() {
            // Set the period
            Wdt::port->TC = ResetPeriod;

            // check if we have a clock selection (lpc1788 does not have this)
            constexpr bool has_clocksel = requires() {
                (void)Wdt::port->CLKSEL;
            };

            if constexpr (has_clocksel) {
                // set the clocksource
                Wdt::port->CLKSEL = ClkSource & 0x3;
            }

            // enable the watchdog timer
            Wdt::port->MOD = (Reset << 1) | 0x1;

            // feed the watchdog to start it
            feed();
        }
    };
}

#endif