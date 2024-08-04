#ifndef KLIB_NXP_LPC17XX_POWER_CONTROL_HPP
#define KLIB_NXP_LPC17XX_POWER_CONTROL_HPP

#include <klib/klib.hpp>

namespace klib::core::lpc17xx::io {
    class power_control {
    public:
        /**
         * @brief Enable power to the peripheral
         *
         * @tparam P
         */
        template <typename P>
        static void enable() {
            // set the bit to enable the power to the peripheral
            SYSCON->PCONP |= (1 << P::clock_id);
        }

        /**
         * Enable power to multiple peripherals.
         *
         * @tparam P
         * @tparam Args
         */
        template <typename P, typename P2, typename ...Args>
        static void enable() {
            enable<P>();
            enable<P2, Args...>();
        }

        /**
         * @brief Disable the power to a peripheral
         *
         * @tparam P
         */
        template <typename P>
        static void disable() {
            // clear the bit to Disable the power to the peripheral
            SYSCON->PCONP &= ~(1 << P::clock_id);
        }

        /**
         * Disable power to multiple peripherals.
         *
         * @tparam P
         * @tparam Args
         */
        template <typename P, typename P2, typename ...Args>
        static void disable() {
            disable<P>();
            disable<P2, Args...>();
        }

        /**
         * @brief Get the power status of a peripheral
         *
         * @tparam P
         * @return true
         * @return false
         */
        template <typename P>
        static bool status() {
            // return the value of the clock id in the register (1 = enabled, 0 = disabled)
            return (SYSCON->PCONP & (1 << P::clock_id));
        }
    };
}

#endif
