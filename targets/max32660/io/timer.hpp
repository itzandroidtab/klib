#ifndef KLIB_MAX32660_TIMER_HPP
#define KLIB_MAX32660_TIMER_HPP

#include <klib/core_clock.hpp>
#include <klib/math.hpp>

#include <max32660.hpp>

#include "clocks.hpp"
#include "pins.hpp"

namespace klib::max32660::io::periph::detail::tc {
    enum class mode {
        tmr
    };

    template <typename Pin, mode Type, typename Periph>
    struct tc {
        // pin of the peripheral
        using pin = Pin;

        // type of the pin
        constexpr static mode type = Type;

        // alternate function
        using periph = Periph;
    };
}

// peripheral namespace for perihperals not affected by chip packages
namespace klib::max32660::io::periph {
    struct tc1 {
        // peripheral id (e.g tc0, tc1)
        constexpr static uint32_t id = 1;

        // peripheral clock bit position
        constexpr static uint32_t clock_id = 16;

        // peripheral interrupt position
        constexpr static uint32_t irq_id = 22;

        // port to the timer
        static inline TMR0_Type *const port = TMR1;
    };

    struct tc2 {
        // peripheral id (e.g tc0, tc1)
        constexpr static uint32_t id = 2;

        // peripheral clock bit position
        constexpr static uint32_t clock_id = 17;

        // peripheral interrupt position
        constexpr static uint32_t irq_id = 23;

        // port to the timer
        static inline TMR0_Type *const port = TMR2;
    };
}

// peripheral pin mapping to alternate functions.
namespace klib::max32660::io::periph::wlp {
    struct tc0 {
        // peripheral id (e.g tc0, tc1)
        constexpr static uint32_t id = 0;

        // peripheral clock bit position
        constexpr static uint32_t clock_id = 15;

        // peripheral interrupt position
        constexpr static uint32_t irq_id = 21;

        // port to the timer
        static inline TMR0_Type *const port = TMR0;

        using tmr = detail::tc::tc<pins::package::wlp::pd2, detail::tc::mode::tmr, io::detail::alternate::func_3>;
    };
}

namespace klib::max32660::io::periph::tqfn_24 {
    struct tc0 {
        // peripheral id (e.g tc0, tc1)
        constexpr static uint32_t id = 0;

        // peripheral clock bit position
        constexpr static uint32_t clock_id = 15;

        // peripheral interrupt position
        constexpr static uint32_t irq_id = 21;

        // port to the timer
        static inline TMR0_Type *const port = TMR0;

        using tmr = detail::tc::tc<pins::package::tqfn_24::p23, detail::tc::mode::tmr, io::detail::alternate::func_3>;
    };
}

namespace klib::max32660::io::detail::timer {
    /**
     * @brief Different timer modes
     * 
     */
    enum class mode {
        one_shot = 0x0,
        continuous = 0x1,
        counter = 0x2,
        pwm = 0x3,
        capture = 0x4,
        compare = 0x5,
        gated = 0x6,
        capture_compare = 0x7
    };

    /**
     * @brief Base timer for the max32660
     * 
     * @tparam Timer 
     * @tparam Mode 
     */
    template <typename Timer, mode Mode = mode::continuous>
    class base_timer {
    protected:
        // using for the array of callbacks
        using interrupt_callback = void (*)();

        // callback
        static inline interrupt_callback callback = nullptr;

        /**
         * @brief Interrupt handler
         * 
         */
        static void isr_handler() {
            // write 0 to the register to clear the value
            Timer::port->INTR = 0;

            // run the callback if provided
            if (callback) {
                callback();
            }
        }

    public:
        /**
         * @brief Init the provided timer
         * 
         * @param irq 
         * @param frequency 
         */
        static void init(const interrupt_callback& irq, const uint32_t frequency) {
            // enable the clock on the timer peripheral
            clocks::enable<Timer>();

            // disable the timer
            Timer::port->CN &= ~(0x1 << 7);

            // setup the timer (provided mode and prescaler of 1)
            Timer::port->CN = (static_cast<uint8_t>(Mode));

            // set the frequency of the timer
            set_frequency(frequency);

            // register the callback
            callback = irq;

            // make sure the irq is valid
            if (irq) {
                // register our handler
                max32660::irq::template register_irq<Timer::irq_id>(isr_handler);

                // enable the interrupt
                max32660::template enable_irq<Timer::irq_id>();
            }
            else {
                // disable the interrupt
                max32660::template disable_irq<Timer::irq_id>();
            }
        }

        /**
         * @brief Set the frequency of the timer
         * 
         * @param Frequency 
         */
        static void set_frequency(const uint32_t frequency) {
            // clear the prescaler
            Timer::port->CN &= ~((0x7 << 3) | (0x1 << 8));

            // set the new prescaler
            Timer::port->CN |= ((0x00 << 3) | (0x0 << 8));

            // get the peripheral clock
            const auto periph_clock = klib::clock::get() / 2;

            // calculate cmp value for the timer
            const uint32_t cmp = (periph_clock / frequency) + 1;

            // set the compare value
            Timer::port->CMP = cmp;
        }

        /**
         * @brief Disable the timer
         * 
         */
        static void disable() {
            // disable the timer
            Timer::port->CN &= ~(0x1 << 7);
        }

        /**
         * @brief Enable the timer
         * 
         */
        static void enable() {
            // enable the timer
            Timer::port->CN |= (0x1 << 7);
        }

        /**
         * @brief Returns the current value of a counter
         * 
         * @return uint32_t 
         */
        static uint32_t get_counter() {
            return Timer::port->CNT;
        }

        /**
         * @brief Clear the counter in the timer
         * 
         */
        static void clear_counter() {
            Timer::port->CNT = 1;
        }
    };
}

namespace klib::max32660::io {
    /**
     * @brief Basic timer. Uses interrupts to call a callback.
     * 
     * @tparam Timer 
     */
    template <typename Timer>
    using timer = detail::timer::base_timer<Timer, detail::timer::mode::continuous>;

    /**
     * @brief Oneshot timer. Uses interrupt to call a callback once.
     * 
     * @tparam Timer 
     */
    template <typename Timer>
    using oneshot_timer = detail::timer::base_timer<Timer, detail::timer::mode::continuous>;

    /**
     * @brief Pin that uses a timer to toggle the output.
     * 
     * @warning When disabling the timer the output of the gpio is not changed.
     * 
     * @tparam Timer 
     * @tparam Frequency 
     * @tparam Bits 
     */
    template <typename Timer, uint32_t Frequency = 50'000, uint8_t Bits = 8>
    class pin_timer {
    protected:
        // check if the amount of bits is valid
        static_assert(Bits >= 1 && Bits <= 16, "Amount of bits must be >= 1 && <= 16");

        // make sure the frequency is valid
        static_assert(Frequency != 0, "Timer frequency cannot be 0");

        // multiplier for the frequency
        constexpr static uint32_t multiplier = (klib::exp2(Bits) - 1);

        /**
         * @brief Calculate the stepsize used in the set functions
         * 
         * @return float 
         */
        static float calculate_stepsize() {
            // get the peripheral clock
            const uint32_t periph_clock = klib::clock::get() / 2;

            // calculate the maximum compare value
            const uint32_t cmp = (periph_clock / Frequency) + 1;

            // calculate the step size
            return klib::max(static_cast<float>(cmp) / multiplier, 1.f);
        }

    public:
        // provide the amount of bits used and the frequency for if
        // someone wants to set the dutycycle
        [[maybe_unused]]
        constexpr static uint32_t bits = Bits;

        [[maybe_unused]]
        constexpr static uint32_t frequency = Frequency;

        /**
         * @brief Init the timer pin.
         * 
         */
        static void init() {
            // enable the clock on the timer peripheral
            clocks::enable<Timer>();

            // disable the timer
            Timer::port->CN &= ~(0x1 << 7);

            // setup the timer (pwm mode and prescaler of 1)
            Timer::port->CN = (static_cast<uint8_t>(detail::timer::mode::pwm));

            // init the gpio pin as a output from the timer
            detail::pins::set_peripheral<typename Timer::tmr::pin, typename Timer::tmr::periph>();

            // clear the prescaler
            Timer::port->CN &= ~((0x7 << 3) | (0x1 << 8));

            // set the new prescaler
            Timer::port->CN |= ((0x00 << 3) | (0x0 << 8));

            // get the peripheral clock
            const uint32_t periph_clock = klib::clock::get() / 2;

            // calculate the maximum compare value (minimum of 1)
            Timer::port->CMP = (periph_clock / Frequency) + 1;
        }

        /**
         * @brief Disable the timer pin
         * 
         */
        static void disable() {
            // disable the timer
            timer<Timer>::disable();
        }

        /**
         * @brief Enable the timer pin
         * 
         */
        static void enable() {
            // enable the timer
            timer<Timer>::enable();
        }

        /**
         * @brief Set the dutycycle of the timer pin
         * 
         * @tparam Dutycycle 
         */
        template <uint16_t Dutycycle>
        static void set() {
            // set the dutycycle
            Timer::port->PWM = calculate_stepsize() * (Dutycycle & multiplier);
        }

        /**
         * @brief Set the dutycycle of the timer pin
         * 
         * @param dutycycle
         */
        static void set(uint16_t dutycycle) {
            // set the dutycycle
            Timer::port->PWM = calculate_stepsize() * (dutycycle & multiplier);
        }

        /**
         * @brief Returns the current value of a counter
         * 
         * @return uint32_t 
         */
        static uint32_t get_counter() {
            return timer<Timer>::get_counter();
        }

        /**
         * @brief Clear the counter in the timer
         * 
         */
        static void clear_counter() {
            timer<Timer>::clear_counter();
        }
    };
}

#endif