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

namespace klib::lpc802::io {
    template <typename Pin, typename Timer, typename BaseChannel, typename Channel, uint32_t Frequency = 50'000, uint8_t Bits = 8>
    class pin_timer {
    protected:
        // check if the amount of bits is valid
        static_assert(Bits >= 1 && Bits <= 16, "Amount of bits must be >= 1 && <= 16");

        // make sure the frequency is valid
        static_assert(Frequency != 0, "Timer frequency cannot be 0");

        // check the channel id of both channels
        static_assert(BaseChannel::id < 4 && Channel::id < 4, "Invalid channel id");

        // multiplier for the frequency
        constexpr static uint32_t multiplier = (klib::exp2(Bits) - 1);

        /**
         * @brief Calculate the stepsize used in the set functions
         * 
         * @return uint32_t 
         */
        static uint32_t calculate_stepsize() {
            // calculate the maximum compare value
            const auto cmp = (klib::clock::get() / Frequency) + 1;

            // calculate the step size
            return klib::max(cmp / multiplier, 1);
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
            // using for the pin matrix
            using matrix = matrix<periph::matrix0>;

            // clear the pin from other functions
            matrix::clear<Pin>();

            // set the pin for the correct timer
            matrix::setup<Pin, static_cast<matrix::flex_matrix>(
                static_cast<uint32_t>(matrix::flex_matrix::timer0_mat0) + Channel::id)
            >();

            // enable the clock on the timer peripheral
            clocks::enable<Timer>();

            // disable the timer
            disable();

            // clear the prescale register to have the counter increment 
            // every clock cycle
            Timer::port->PR = 0;

            // set the frequency we want to target into the base channel
            timer<Timer, BaseChannel>::set_frequency(Frequency);

            // clear the counter when it is stuck on 0
            if (!get_counter()) {
                // clear the counter
                clear_counter();
            }

            // setup the base channel to reset automaticly and clear all the bits for the channel
            Timer::port->MCR = (Timer::port->MCR & ~(0b111 << Channel::id) & ~(0b111 << BaseChannel::id)) | (0b010 << BaseChannel::id);

            // set the pwm control register based on the channel
            Timer::port->PWMC = (Timer::port->PWMC & ~(0b1 << Channel::id)) | (0x1 << Channel::id);
        }

        /**
         * @brief Disable the timer pin
         * 
         */
        static void disable() {
            timer<Timer, Channel>::disable();
        }

        /**
         * @brief Enable the timer pin
         * 
         */
        static void enable() {
            timer<Timer, Channel>::enable();
        }

        /**
         * @brief Set the dutycycle of the timer pin
         * 
         * @tparam Dutycycle 
         */
        template <uint16_t Dutycycle>
        static void dutycycle() {
            // set a dutycycle (duty cycle we write is inverted. Change it back)
            Timer::port->MR[Channel::id] = calculate_stepsize() * (multiplier - (Dutycycle & multiplier));
        }

        /**
         * @brief Set the dutycycle of the timer pin
         * 
         * @param dutycycle
         */
        static void dutycycle(uint16_t dutycycle) {
            // set a dutycycle (duty cycle we write is inverted. Change it back)
            Timer::port->MR[Channel::id] = calculate_stepsize() * (multiplier - (dutycycle & multiplier));
        }

        /**
         * @brief Enable or disable output. When setting to false (0) it disables 
         * the output and makes the output low
         * 
         * @tparam Value 
         */
        template <bool Value>
        static void set() {
            // TODO: implement set
        }

        /**
         * @brief Enable or disable output. When setting to false (0) it disables 
         * the output and makes the output low
         * 
         * @param value 
         */
        static void set(bool value) {
            // TODO: implement set
        }

        /**
         * @brief Returns the current value of a counter
         * 
         * @return uint32_t 
         */
        static uint32_t get_counter() {
            return timer<Timer, Channel>::get_counter();
        }

        /**
         * @brief Clear the counter in the timer
         * 
         */
        static void clear_counter() {
            timer<Timer, Channel>::clear_counter();
        }
    };
}

#endif