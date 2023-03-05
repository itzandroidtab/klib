#ifndef KLIB_LPC1756_CLOCKS_HPP
#define KLIB_LPC1756_CLOCKS_HPP

#include <lpc1756.hpp>

namespace klib::lpc1756 {
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
            if constexpr ((P::clock_id * 2) < 32) {
                // clear any previous bits set for the peripheral
                SYSCON->PCLKSEL0 &= ~(0b11 << (P::clock_id * 2));

                // set the main clock as the clock input
                SYSCON->PCLKSEL0 |= (0b01 << (P::clock_id * 2));
            }
            else {
                // clear any previous bits set for the peripheral
                SYSCON->PCLKSEL1 &= ~(0b11 << ((P::clock_id * 2) - 32));

                // set the main clock as the clock input
                SYSCON->PCLKSEL1 |= (0b01 << ((P::clock_id * 2) - 32));
            }
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
            if constexpr ((P::clock_id * 2) < 32) {
                SYSCON->PCLKSEL0 &= ~(0b11 << (P::clock_id * 2));
            }
            else {
                SYSCON->PCLKSEL1 &= ~(0b11 << ((P::clock_id * 2) - 32));
            }
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
            if constexpr ((P::clock_id * 2) < 32) {
                return SYSCON->PCLKSEL0 & (0b11 << (P::clock_id * 2));
            }
            else {
                return SYSCON->PCLKSEL1 & (0b11 << ((P::clock_id * 2) - 32));
            }
        }
    };
}

#endif
