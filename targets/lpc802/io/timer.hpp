#ifndef KLIB_LPC802_TIMER_HPP
#define KLIB_LPC802_TIMER_HPP

#include <klib/core_clock.hpp>
#include <klib/irq_helper.hpp>

#include <lpc802.hpp>

#include "clocks.hpp"
#include "pins.hpp"

// peripheral namespace for perihperals not affected by chip packages
namespace klib::lpc802::io::periph {
    struct tc0 {
        // peripheral id (e.g tc0, tc1)
        constexpr static uint32_t id = 0;

        // peripheral clock bit position
        constexpr static uint32_t clock_id = 25;

        // peripheral interrupt position
        constexpr static uint32_t irq_id = 39;

        // port to the timer
        static inline CTIMER0_Type *const port = CTIMER0;

        // available channels in the timer
        struct channel_0 {
            constexpr static uint32_t id = 0;
        };

        struct channel_1 {
            constexpr static uint32_t id = 1;
        };

        struct channel_2 {
            constexpr static uint32_t id = 2;
        };
        
        struct channel_3 {
            constexpr static uint32_t id = 3;
        };
    };
}

namespace klib::lpc802::io {
    template <typename Timer, typename Channel>
    class timer {
    public:
        // using for the array of callbacks
        using interrupt_callback = void (*)();

    protected:
        // callback to call when the interrupt is triggered
        static inline interrupt_callback callback = nullptr;

        /**
         * @brief Interrupt handler
         * 
         */
        static void isr_handler() {
            // read the register
            const uint32_t status = Timer::port->IR;

            // clear the whole interrupt register
            Timer::port->IR = status;

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
            disable();

            // clear the prescale register to have the counter increment 
            // every clock cycle
            Timer::port->PR = 0;

            // set the frequency of the timer
            set_frequency(frequency);

            // clear the counter when it is stuck on 0
            if (!get_counter()) {
                // clear the counter
                clear_counter();
            }

            // setup to trigger a interrupt when matching the TC value
            Timer::port->MCR = (Timer::port->MCR & ~(0b111 << Channel::id)) | (0b011 << Channel::id);

            // register our handler
            lpc802::irq::template register_irq<Timer::irq_id>(isr_handler);

            // enable the interrupt
            lpc802::template enable_irq<Timer::irq_id>();
        }

        static void set_frequency(const uint32_t frequency) {
            // set the match register for the desired frequency
            Timer::port->MR[Channel::id] = (klib::clock::get() / frequency) + 1;
        }

        /**
         * @brief Disable the timer
         * 
         */
        static void disable() {
            // disable the timer
            Timer::port->TCR &= ~0x1;
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
            Timer::port->TC = 1;
        }
    };
}

#endif