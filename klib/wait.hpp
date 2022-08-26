#ifndef KLIB_WAIT_HPP
#define KLIB_WAIT_HPP

#include <klib/units.hpp>
#include <klib/core_clock.hpp>

namespace klib::detail {
    /**
     * @brief Flag for waiting until a timer is triggered
     * 
     * @tparam Timer 
     */
    template <typename Timer>
    static volatile bool done = false;

    /**
     * @brief Delay using a hardware timer. Max wait 
     * time is 1 second. 
     * 
     * @warning Input is not checked.
     * 
     * @tparam Timer
     * @param time 
     */
    template <typename Timer>
    static void delay_impl(const time::us time) {
        // reset the flag we are waiting on
        done<Timer> = false;

        // init the timer
        Timer::init([]() {
            done<Timer> = true;
        }, 1'000'000 / time.value);

        // enable the timer and wait for it to finish
        Timer::enable();

        // loop until we are done
        while (!done<Timer>) {
            // if we dont have low power sleep enabled we will just busy loop
            if constexpr (TARGET_LOW_POWER_SLEEP == true) {
                // let the cpu sleep until we have a interrupt
                asm("WFI");
            }
        }

        // disable the timer afterwards
        Timer::disable();
    }
}

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

    /**
     * @brief Delay using a hardware timer
     * 
     * @tparam Timer 
     * @tparam T 
     * @param time 
     */
    template <typename Timer, typename T>
    static void delay(const T time) {
        // get the amount of seconds in the time
        const auto sec = static_cast<time::s>(time);

        // delay in 1 second intervals as some timers 
        // do not support frequencies below 1hz
        for (uint32_t i = 0; i < sec.value; i++) {
            detail::delay_impl<Timer>(time::s(1));
        }

        // get the remainder of the time
        const auto usec = static_cast<time::us>(
            static_cast<time::us>(time) - static_cast<time::us>(sec)
        );

        // wait for the remainder
        detail::delay_impl<Timer>(usec);
    }
}

#endif