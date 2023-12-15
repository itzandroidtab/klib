#ifndef KLIB_NXP_LPC175X_TIMER_HPP
#define KLIB_NXP_LPC175X_TIMER_HPP

#include <cstdint>

#include <klib/klib.hpp>
#include <klib/io/core_clock.hpp>

#include "clocks.hpp"
#include "power.hpp"
#include "port.hpp"

namespace klib::core::lpc175x::io::detail::timer {
    /**
     * @brief Different timer modes
     * 
     */
    enum class mode {
        one_shot,
        continuous,
        counter
    };

    /**
     * @brief Base timer for the lpc1756
     * 
     * @tparam Timer 
     * @tparam Mode 
     */
    template <typename Timer, uint8_t Channel, mode Mode = mode::continuous>
    class base_timer {
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

            // clear the counter
            clear_counter();

            // channel configuration value
            const uint8_t value = (
                (irq != nullptr) | ((Mode == mode::one_shot) << 2) |
                ((Mode != mode::one_shot && Mode != mode::counter) << 1)
            );

            // setup to trigger a interrupt when matching the TC value
            Timer::port->MCR = (Timer::port->MCR & ~(0b111 << Channel)) | (value << Channel);

            // set the callback
            callback = irq;

            if (irq) {
                // register our handler
                target::irq::register_irq<Timer::interrupt_id>(isr_handler);

                // enable the interrupt
                target::enable_irq<Timer::interrupt_id>();
            }
            else {
                // disable the interrupt
                target::disable_irq<Timer::interrupt_id>();
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

namespace klib::core::lpc175x::io {
    /**
     * @brief Basic timer. Uses interrupts to call a callback.
     * 
     * @tparam Timer 
     */
    template <typename Timer, uint32_t Channel>
    using timer = detail::timer::base_timer<Timer, Channel>;

    /**
     * @brief Oneshot timer. Uses interrupt to call a callback once.
     * 
     * @tparam Timer 
     */
    template <typename Timer, uint32_t Channel>
    using oneshot_timer = detail::timer::base_timer<Timer, Channel, detail::timer::mode::one_shot>;

    /**
     * @brief Pwm timer.
     * 
     * @tparam Pwm 
     * @tparam Channel 
     */
    template <
        typename Pin, typename Pwm, uint8_t Channel, 
        uint32_t Frequency, uint8_t Bits
    >
    class pwm_timer: public detail::timer::base_timer<Pwm, 0, detail::timer::mode::continuous> {
    protected:
        // make sure the channel is valid
        static_assert(Channel < Pwm::max_pwm_channels, "Pwm only supports 7 channels");
        static_assert(Channel != 0, "Pwm does not support channel 0 (is used as the base channel)");

        // check if the amount of bits is valid
        static_assert(Bits >= 1 && Bits <= 16, "Amount of bits must be >= 1 && <= 16");

        // make sure the frequency is valid
        static_assert(Frequency != 0, "Pwm frequency cannot be 0");

        // multiplier for the frequency
        constexpr static uint32_t multiplier = (klib::exp2(Bits) - 1);

        /**
         * @brief Calculate the stepsize used in the set functions
         * 
         * @return uint32_t 
         */
        template <bool FloatingPoint = true>
        static auto calculate_stepsize() {
            // calculate the maximum compare value
            const auto cmp = (klib::io::clock::get() / Frequency) + 1;

            if constexpr (FloatingPoint) {
                // calculate the step size
                return klib::max(static_cast<float>(cmp) / multiplier, 1.f);
            }
            else {
                // calculate the step size
                return klib::max(cmp / multiplier, 1);
            }
        }

    public:
        static void init() {
            // init the base timer on the BaseChannel
            detail::timer::base_timer<
                Pwm, 0, detail::timer::mode::continuous
            >::init(nullptr, Frequency);

            // disable the interrupts
            Pwm::port->IR = 0;

            // enable the pwm
            Pwm::port->TCR |= (0x1 << 3);

            // TODO: add support for this
            io::detail::pins::set_peripheral<Pin, io::detail::alternate::func_1>();

            // enable the channel as pwm
            Pwm::port->PCR |= (0x1 << (Channel + 8));
        }

        /**
         * @brief Set the dutycycle of the Pwm pin
         * 
         * @tparam Dutycycle 
         */
        template <uint16_t Dutycycle>
        static void dutycycle() {
            // calculate the step size
            const uint32_t value = calculate_stepsize() * (Dutycycle & multiplier);

            // set the dutycycle (if we are above channel 4 we switch 
            // from array to direct naming)
            if constexpr (Channel < 4) {
                Pwm::port->MR[Channel] = value;
            }
            else if constexpr (Channel == 4) {
                Pwm::port->MR4 = value;
            }
            else if constexpr (Channel == 5) {
                Pwm::port->MR5 = value;
            }
            else if constexpr (Channel == 6) {
                Pwm::port->MR6 = value;
            }

            // update the pwm
            Pwm::port->LER |= (0x1 << Channel);
        }

        /**
         * @brief Set the dutycycle of the timer pin
         * 
         * @param dutycycle
         */
        static void dutycycle(uint16_t dutycycle) {
            // calculate the step size
            const uint32_t value = calculate_stepsize() * (dutycycle & multiplier);

            // set the dutycycle (if we are above channel 4 we switch 
            // from array to direct naming)
            if constexpr (Channel < 4) {
                Pwm::port->MR[Channel] = value;
            }
            else if constexpr (Channel == 4) {
                Pwm::port->MR4 = value;
            }
            else if constexpr (Channel == 5) {
                Pwm::port->MR5 = value;
            }
            else if constexpr (Channel == 6) {
                Pwm::port->MR6 = value;
            }

            // update the pwm
            Pwm::port->LER |= (0x1 << Channel);
        }

        /**
         * @brief Enable or disable output.
         * 
         * @tparam Value 
         */
        template <bool Value>
        static void set() {
            // clear or set the pin to the peripheral
            if constexpr (Value) {
                io::detail::pins::set_peripheral<Pin, io::detail::alternate::func_1>();
            }
            else {
                // change the mode to a output pin
                pin_out<Pin>::init();

                // set the pin to output a low
                pin_out<Pin>::template set<false>();
            }
        }

        /**
         * @brief Enable or disable output.
         * 
         * @param value 
         */
        static void set(bool value) {
            // clear or set the pin to the peripheral
            if (value) {
                io::detail::pins::set_peripheral<Pin, io::detail::alternate::func_1>();
            }
            else {
                // change the mode to a output pin
                pin_out<Pin>::init();

                // set the pin to output a low
                pin_out<Pin>::template set<false>();
            }
        }

    };
}

#endif