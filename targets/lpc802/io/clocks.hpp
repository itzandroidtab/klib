#ifndef KLIB_LPC802_CLOCKS_HPP
#define KLIB_LPC802_CLOCKS_HPP

#include <lpc802.hpp>

namespace klib::lpc802 {
    class clocks {
    public:
        /**
         * @brief Enable the clock on the peripheral
         * 
         * @tparam P 
         */
        template <typename P> 
        static void enable() {
            // set the bit to enable the clock on the peripheral
            SYSCON->SYSAHBCLKCTRL0 |= (1 << P::clock_id);
        }

        /**
         * Enable the clock on multiple peripherals.
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
         * @brief Disable the clock on the peripheral
         * 
         * @tparam P 
         */
        template <typename P> 
        static void disable() {
            // clear the bit to Disable the clock on the peripheral
            SYSCON->SYSAHBCLKCTRL0 &= ~(1 << P::clock_id);
        }

        /**
         * Disable the clock on multiple peripherals.
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
         * @brief Get the clock status of a peripheral 
         * 
         * @tparam P 
         * @return true 
         * @return false 
         */
        template <typename P> 
        static bool status() {
            // return the value of the clock id in the register (1 = enabled, 0 = disabled)
            return (SYSCON->SYSAHBCLKCTRL0 & (1 << P::clock_id));
        }
    };
}

#endif
