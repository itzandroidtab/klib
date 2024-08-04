#ifndef KLIB_MAX32625_POWER_HPP
#define KLIB_MAX32625_POWER_HPP

#include <max32625.hpp>

namespace klib::max32625 {
    class power {
    public:
        /**
         * @brief Enable the clock on the peripheral
         *
         * @tparam P
         */
        template <typename P>
        static void enable() {
            // clear the bit to enable the clock on the peripheral
            PWRMAN->PWR_RST_CTRL &= ~(1 << P::clock_id);
        }

        /**
         * Enable the clock on multiple peripherals.
         *
         * @tparam P
         * @tparam Args
         */
        template<typename P, typename P2, typename ...Args>
        static void enable() {
            enable<P>();
            enable<P2, Args...>();
        }

        /**
         * @brief Disable the clock on the peripheral
         *
         * @tparam P
         */
        template <typename P>
        static void disable() {
            // clear the bit to Disable the clock on the peripheral
            PWRMAN->PWR_RST_CTRL |= (1 << P::clock_id);
        }

        /**
         * Disable the clock on multiple peripherals.
         *
         * @tparam P
         * @tparam Args
         */
        template<typename P, typename P2, typename ...Args>
        static void disable() {
            disable<P>();
            disable<P2, Args...>();
        }

        /**
         * @brief Get the clock status of a peripheral
         *
         * @tparam P
         * @return true
         * @return false
         */
        template <typename P>
        static bool status() {
            // return the inverse of the register (0 = enabled, 1 = disabled)
            return !(PWRMAN->PWR_RST_CTRL & (1 << P::clock_id));
        }
    };
}

#endif
