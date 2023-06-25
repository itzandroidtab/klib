#ifndef KLIB_LPC1756_TIMER_HPP
#define KLIB_LPC1756_TIMER_HPP

#include <cstdint>
#include <klib/io/core_clock.hpp>

#include <lpc1756.hpp>

#include "clocks.hpp"
#include "power.hpp"

namespace klib::lpc1756::io::periph {
    struct tc0 {
        // peripheral id (e.g tc0, tc1)
        constexpr static uint32_t id = 0;

        // peripheral interrupt position
        constexpr static uint32_t irq_id = 17;

        // peripheral clock bit position
        constexpr static uint32_t clock_id = 1;

        // port to the timer hardware
        static inline TIMER0_Type *const port = TIMER0;

        // available channels in the timer
        constexpr static uint32_t max_channels = 4;
    };

    struct tc1 {
        // peripheral id (e.g tc0, tc1)
        constexpr static uint32_t id = 1;

        // peripheral interrupt position
        constexpr static uint32_t irq_id = 18;

        // power bit position
        constexpr static uint32_t clock_id = 2;

        // port to the timer hardware
        static inline TIMER0_Type *const port = TIMER1;

        // available channels in the timer
        constexpr static uint32_t max_channels = 4;
    };

    struct tc2 {
        // peripheral id (e.g tc0, tc1)
        constexpr static uint32_t id = 2;

        // peripheral interrupt position
        constexpr static uint32_t irq_id = 19;

        // power bit position
        constexpr static uint32_t clock_id = 22;

        // port to the timer hardware
        static inline TIMER0_Type *const port = TIMER2;

        // available channels in the timer
        constexpr static uint32_t max_channels = 4;
    };

    struct tc3 {
        // peripheral id (e.g tc0, tc1)
        constexpr static uint32_t id = 3;

        // peripheral interrupt position
        constexpr static uint32_t irq_id = 20;

        // power bit position
        constexpr static uint32_t clock_id = 23;

        // port to the timer hardware
        static inline TIMER0_Type *const port = TIMER3;

        // available channels in the timer
        constexpr static uint32_t max_channels = 4;
    };
}

namespace klib::lpc1756::io {
    /**
     * @brief Base timer for the lpc1756
     * 
     * @tparam Timer 
     * @tparam Mode 
     */
    template <typename Timer, uint8_t Channel>
    class timer {
    public:
        // using for the array of callbacks
        using interrupt_callback = void (*)();

    protected:
        // callback
        static inline interrupt_callback callback = nullptr;

        // make sure the channel is valid
        static_assert(Channel < Timer::max_channels, "Timer only supports 4 channels");

        /**
         * @brief Interrupt handler
         * 
         */
        static void isr_handler() {
            // read the register
            const uint32_t status = Timer::port->IR;

            // clear the whole interrupt register
            Timer::port->IR = status & 0b11111;

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
            // enable power to the timer peripheral
            power_control::enable<Timer>();

            // enable the clock on the timer peripheral
            clocks::set<Timer>();

            // disable the timer
            disable();

            // set the frequency of the timer
            set_frequency(frequency);

            // set the timer into timer count mode
            Timer::port->CTCR = 0x00;

            // clear the counter when it is stuck on 0
            if (!get_counter()) {
                // clear the counter
                clear_counter();
            }

            // setup to trigger a interrupt when matching the TC value
            Timer::port->MCR = (Timer::port->MCR & ~(0b111 << Channel)) | (0b011 << Channel);

            // set the callback
            callback = irq;

            if (irq) {
                // register our handler
                lpc1756::irq::template register_irq<Timer::irq_id>(isr_handler);

                // enable the interrupt
                lpc1756::template enable_irq<Timer::irq_id>();
            }
            else {
                // disable the interrupt
                lpc1756::template disable_irq<Timer::irq_id>();
            }
        }

        /**
         * @brief Set the frequency of the timer
         * 
         * @param Frequency 
         */
        static void set_frequency(const uint32_t frequency) {
            // set the match register for the desired frequency
            Timer::port->MR[Channel] = (klib::io::clock::get() / frequency) + 1;
        }

        /**
         * @brief Disable the timer
         * 
         */
        static void disable() {
            // disable the timer
            Timer::port->TCR &= ~(0x1);
        }

        /**
         * @brief Enable the timer
         * 
         */
        static void enable() {
            // enable the timer
            Timer::port->TCR |= 0x1;
        }

        /**
         * @brief Returns the current value of a counter
         * 
         * @return uint32_t 
         */
        static uint32_t get_counter() {
            return Timer::port->TC;
        }

        /**
         * @brief Clear the counter in the timer
         * 
         */
        static void clear_counter() {
            Timer::port->TC = 0x1;
        }
    };
}

#endif