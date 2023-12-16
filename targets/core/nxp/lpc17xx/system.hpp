#ifndef KLIB_NXP_LPC17XX_SYSTEM_HPP
#define KLIB_NXP_LPC17XX_SYSTEM_HPP

#include <klib/klib.hpp>
#include <klib/io/core_clock.hpp>

namespace klib::core::lpc17xx::io::system {
    class flash {
    public:
        /**
         * @brief Set the amount of waitstates used for accessing flash
         * 
         * @details
         * 0 = for up to 20Mhz
         * 1 = for up to 40Mhz
         * 2 = for up to 60Mhz
         * 3 = for up to 80Mhz
         * 4 = for up to 100Mhz
         * 5 = safe setting for all conditions
         * 
         * @tparam WaitState 
         */
        template <uint8_t WaitState>
        static void setup() {
            // check the wait state from the user
            static_assert(WaitState <= 5, "Max wait cycles is 5 (+ 1)");

            // get the register
            const uint32_t tmp = SYSCON->FLASHCFG & 0xFFF;

            // set the other bits and write it back
            SYSCON->FLASHCFG = tmp | (WaitState << 12);
        }
    };
}

#endif