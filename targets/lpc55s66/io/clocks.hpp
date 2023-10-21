#ifndef KLIB_LPC55S66_CLOCKS_HPP
#define KLIB_LPC55S66_CLOCKS_HPP

#include <lpc55s66.hpp>

namespace klib::lpc55s66 {
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
            if constexpr (P::clock_id < 32) {
                // set the bit in the first register
                SYSCON->AHBCLKCTRL0 |= (0x1 << P::clock_id);
            }
            else if constexpr (P::clock_id < 64) {
                // set the bit in the second register
                SYSCON->AHBCLKCTRL1 |= (0x1 << (P::clock_id - 32));
            }
            else {
                // set the bit in the third register
                SYSCON->AHBCLKCTRL2 |= (0x1 << (P::clock_id - 64));
            }
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
            if constexpr (P::clock_id < 32) {
                // clear the bit in the first register
                SYSCON->AHBCLKCTRL0 &= ~(0x1 << P::clock_id);
            }
            else if constexpr (P::clock_id < 64) {
                // clear the bit in the second register
                SYSCON->AHBCLKCTRL1 &= ~(0x1 << (P::clock_id - 32));
            }
            else {
                // clear the bit in the third register
                SYSCON->AHBCLKCTRL2 &= ~(0x1 << (P::clock_id - 64));
            }
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
            // return the value of the clock id in the register (1 = enabled, 0 = disabled)
            if constexpr (P::clock_id < 32) {
                // clear the bit in the first register
                return SYSCON->AHBCLKCTRL0 & (0x1 << P::clock_id);
            }
            else if constexpr (P::clock_id < 64) {
                // clear the bit in the second register
                return SYSCON->AHBCLKCTRL1 & (0x1 << (P::clock_id - 32));
            }
            else {
                // clear the bit in the third register
                return SYSCON->AHBCLKCTRL2 & (0x1 << (P::clock_id - 64));
            }
        }
    };
}

#endif
