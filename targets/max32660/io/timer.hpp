#ifndef KLIB_MAX32660_TIMER_HPP
#define KLIB_MAX32660_TIMER_HPP

#include <klib/core_clock.hpp>

#include "max32660.h"
#include <clocks.hpp>

namespace klib::max32660::io::detail::timer {
    // default type when using the port
    template<uint32_t Timer>
    TMR0_Type *const port = nullptr;

    // port when using the pio0
    template<>
    TMR0_Type *const port<0> = TMR0;

    template<>
    TMR0_Type *const port<1> = TMR1;

    template<>
    TMR0_Type *const port<2> = TMR2; 

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
}

namespace klib::max32660::io {
    template <typename Timer>
    class timer {
    protected:
        // port to the timer peripheral
        static inline TMR0_Type *const port = io::detail::timer::port<Timer::id>;

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
            port->INTR = 0;

            // run the callback if provided
            if (callback) {
                callback();
            }
        }

    public:
        /**
         * @brief Init the provided timer
         * 
         * @tparam Irq 
         * @tparam Frequency 
         * @param callback 
         */
        template <typename Irq, uint32_t Frequency = 100'000>
        static void init(const interrupt_callback& irq) {
            // enable the clock on the timer peripheral
            clocks::enable<Timer>();

            // disable the timer
            port->CN &= ~(0x1 << 7);

            // setup the timer (continuous mode and prescaler of 1)
            port->CN = (static_cast<uint8_t>(detail::timer::mode::continuous));

            // set the frequency of the timer
            set_frequency<Frequency>();

            // register the callback
            callback = irq;

            // register our handler
            Irq::template register_irq<Timer::irq_id>(isr_handler);

            // enable the interrupt
            Irq::template enable_irq<Timer::irq_id>();
        }

        template <uint32_t Frequency>
        static void set_frequency() {
            // make sure the frequency is valid
            static_assert(Frequency != 0, "Timer frequency cannot be 0");

            // clear the prescaler
            port->CN &= ~((0x7 << 3) | (0x1 << 8));

            // set the new prescaler
            port->CN |= ((0x00 << 3) | (0x0 << 8));

            // get the peripheral clock
            const auto periph_clock = klib::clock::get() / 2;

            // calculate cmp value for the timer
            const uint32_t cmp = (periph_clock / Frequency) + 1;

            // set the compare value
            port->CMP = cmp;
        }

        /**
         * @brief Disable the timer
         * 
         */
        static void disable() {
            // disable the timer
            port->CN &= ~(0x1 << 7);
        }

        /**
         * @brief Enable the timer
         * 
         */
        static void enable() {
            // enable the timer
            port->CN |= (0x1 << 7);
        }

        /**
         * @brief Returns the current value of a counter
         * 
         * @return uint32_t 
         */
        static uint32_t get_counter() {
            return port->CNT;
        }

        /**
         * @brief Clear the counter in the timer
         * 
         */
        static void clear_counter() {
            port->CNT = 1;
        }
    };
}

#endif