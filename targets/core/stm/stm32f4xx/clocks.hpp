#ifndef KLIB_STM_STM32F4XX_CLOCKS_HPP
#define KLIB_STM_STM32F4XX_CLOCKS_HPP

#include <klib/klib.hpp>

namespace klib::core::stm32f4xx::io {
    class clocks {
    public:
        /**
         * @brief Set the global peripheral clock selection
         *
         */
        template <typename P>
        static void set() {
            // get the clock id and make sure it is valid
            constexpr static uint32_t id = P::clock_id;
            static_assert(id < (5 * 32), "Invalid clock id");

            // check what register to set
            if constexpr (id < (1 * 32)) {
                RCC->AHB1ENR |= (0x1 << P::clock_id);
            } 
            else if constexpr (id < (2 * 32)) {
                RCC->AHB2ENR |= (0x1 << (P::clock_id - (1 * 32)));
            }
            else if constexpr (id < (3 * 32)) {
                RCC->AHB3ENR |= (0x1 << (P::clock_id - (2 * 32)));
            }
            else if constexpr (id < (4 * 32)) {
                RCC->APB1ENR |= (0x1 << (P::clock_id - (3 * 32)));
            }
            else if constexpr (id < (5 * 32)) {
                RCC->APB2ENR |= (0x1 << (P::clock_id - (4 * 32)));
            }
        }
    };
}

#endif
