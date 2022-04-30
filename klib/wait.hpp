#ifndef KLIB_WAIT_HPP
#define KLIB_WAIT_HPP

#include <klib/units.hpp>
#include <klib/core_clock.hpp>

namespace klib {
    /**
     * @brief Do a busy wait for msec amount of milliseconds
     * 
     * @warning is a rough estimation based on the cpu clock and the 
     * amount of instructions used in a loop with optimization -Os
     * 
     * @param msec 
     */
    static void __attribute__((__optimize__("-Os"))) delay_busy(const time::ms msec) {
        // get the cpu speed
        const uint32_t interate_cycles = klib::clock::get() / 1'000;

        // buys wait until the time has ran out
        for (uint32_t i = 0; i < msec.value; i++) {
            for (volatile int j = 0; j < interate_cycles; j += 36) {
                asm("NOP");
            }
        }
    }
    
    /**
     * @brief Do a busy wait for sec amount of seconds
     * 
     * @param sec 
     */
    static void delay_busy(const time::s sec) {
        // buys wait until the time has ran out
        for (uint32_t i = 0; i < sec.value; i++) {
            delay_busy(time::ms(1000));
        }
    }
}

#endif