#ifndef KLIB_NXP_LPC175X_CLOCKS_HPP
#define KLIB_NXP_LPC175X_CLOCKS_HPP

#include <klib/klib.hpp>

namespace klib::core::lpc175x::io {
    class clocks {
    public:
        /**
         * @brief Available clock dividers
         * 
         * 0b11 is a div by 8 for everyting except can1, can2 and 
         * can filtering. Then it is div by 6
         * 
         */
        enum class divider {
            div_4 = 0b00,
            div_1 = 0b01,
            div_2 = 0b10,
            div_8_or_6 = 0b11
        };

        /**
         * @brief Enable the clock on the peripheral
         * 
         * @tparam P 
         */
        template <typename P, divider Divider = divider::div_1> 
        static void set() {
            // set the bit to enable the clock on the peripheral
            if constexpr ((P::clock_id * 2) < 32) {
                // clear any previous bits set for the peripheral
                SYSCON->PCLKSEL0 &= ~(0b11 << (P::clock_id * 2));

                // set the main clock as the clock input
                SYSCON->PCLKSEL0 |= (static_cast<uint32_t>(Divider) << (P::clock_id * 2));
            }
            else {
                // clear any previous bits set for the peripheral
                SYSCON->PCLKSEL1 &= ~(0b11 << ((P::clock_id * 2) - 32));

                // set the main clock as the clock input
                SYSCON->PCLKSEL1 |= (static_cast<uint32_t>(Divider) << ((P::clock_id * 2) - 32));
            }
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
