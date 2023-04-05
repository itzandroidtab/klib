#ifndef KLIB_WAIT_HPP
#define KLIB_WAIT_HPP

#include <klib/units.hpp>
#include <klib/core_clock.hpp>
#include <klib/io/systick.hpp>

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
    static void timer_delay_impl(const time::us time) {
        // do nothing when the value is 0
        if (time.value == 0) {
            // do a early return
            return;
        }

        // reset the flag we are waiting on
        done<Timer> = false;

        // init the timer
        Timer::init([]() {
            // set the flag we are done
            done<Timer> = true;
        }, 1'000'000 / time.value);

        // enable the timer and wait for it to finish
        Timer::enable();

        // loop until we are done
        while (!done<Timer>) {
            // if we dont have low power sleep enabled we will just busy loop
            if constexpr (TARGET_LOW_POWER_SLEEP == true) {
                // let the cpu sleep until we have a interrupt
                asm("WFE");
            }
        }

        // disable the timer afterwards
        Timer::disable();
    }

    /**
     * @brief Delay using the systick timer. Timing uses the 
     * remainder of the systick counter to increase accuracy.
     * 
     * @tparam Timer 
     * @tparam T 
     * @param time 
     */
    template <typename Timer>
    static void systick_delay_impl(const time::us time) {
        // get the counter value
        const uint32_t count = Timer::get_counter();

        // calculate the amount of time to wait
        const time::ms target = Timer::get_runtime() + static_cast<time::ms>(time);

        // wait until we have reached the target runtime
        while (Timer::get_runtime() != target) {
            // if we dont have low power sleep enabled we will just busy loop
            if constexpr (TARGET_LOW_POWER_SLEEP == true) {
                // let the cpu sleep until we have a interrupt
                asm("WFE");
            }
        }

        // wait until the counter goes over the count we 
        // had before 
        while (Timer::get_counter() < count) {
            // wait and do nothing. We cannot use the low power sleep here as 
            // no interrupt will trigger
        }
    }

    /**
     * @brief Do a busy wait for time amount of microseconds
     * 
     * @warning is a rough estimation based on the cpu clock and the 
     * amount of instructions used in a loop with optimization -Os
     * 
     * @param time 
     */
    static void __attribute__((__optimize__("-Os"))) busy_delay_impl(const time::us time) {
        // get the cpu speed
        const uint32_t interate_cycles = klib::clock::get() / 1'000;

        // calculate the amount of time to wait
        const time::ms msec = static_cast<time::ms>(time);

        // buys wait until the time has ran out
        for (uint32_t i = 0; i < msec.value; i++) {
            for (volatile uint32_t j = 0; j < interate_cycles; j += 36) {
                asm("NOP");
            }
        }
    }
}

namespace klib {
    /**
     * @brief Type that should be used for a loop that does 
     * a busy wait.
     * 
     * @warning this a aproximation. for accurate timing a 
     * timer should be used
     * 
     */
    struct busy_wait {};

    /**
     * @brief Delay using a hardware timer. When using the 
     * systick timer the accuracy drops to the millisecond 
     * level.
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
            if constexpr (std::is_same_v<Timer, klib::busy_wait>) {
                // busy wait just waits based on a aproximation
                detail::busy_delay_impl(time::s(1));
            }
            else if constexpr (std::is_same_v<Timer, klib::systick>) {
                // use the freerunning system timer
                detail::systick_delay_impl<Timer>(time::s(1));
            }
            else {
                // otherwise use the provided timer
                detail::timer_delay_impl<Timer>(time::s(1));
            }
        }

        // get the remainder of the time
        const auto usec = static_cast<time::us>(
            static_cast<time::us>(time) - static_cast<time::us>(sec)
        );

        // wait for the remainder
        if constexpr (std::is_same_v<Timer, klib::busy_wait>) {
            // busy wait just waits based on a aproximation
            detail::busy_delay_impl(usec);
        }
        else if constexpr (std::is_same_v<Timer, klib::systick>) {
            // use the freerunning system timer
            detail::systick_delay_impl<Timer>(usec);
        }
        else {
            // otherwise use the provided timer
            detail::timer_delay_impl<Timer>(usec);
        }
    }
}

#endif