#ifndef KLIB_ATMEL_ATSAM4S_TIMER_HPP
#define KLIB_ATMEL_ATSAM4S_TIMER_HPP

#include <cstdint>

#include <klib/klib.hpp>
#include <klib/io/core_clock.hpp>
#include <klib/io/peripheral.hpp>
#include <klib/math.hpp>

#include <io/power.hpp>

#include "clocks.hpp"
#include "port.hpp"

namespace klib::core::atsam4s::io::detail::timer {
    /**
     * @brief Different timer modes
     *
     */
    enum class mode {
        one_shot,
        continuous
    };

    /**
     * @brief Different compare effects
     *
     */
    enum class compare_effect {
        none = 0x00,
        set = 0x01,
        clear = 0x02,
        toggle = 0x03,
    };

    /**
     * @brief Base timer for the lpc1756
     *
     * @tparam Timer
     * @tparam Mode
     */
    template <typename Timer, uint8_t Channel, uint8_t Match, mode Mode = mode::continuous, uint32_t Div = 2>
    class base_timer {
    public:
        // using for the array of callbacks
        using interrupt_callback = void (*)();

    protected:
        // callback
        static inline interrupt_callback callback = nullptr;

        // make sure the channel is valid
        static_assert(Channel < Timer::max_channels, "Timer only supports 3 channels");
        static_assert(Match < 3, "Timer only has 3 match registers");
        static_assert((Div == 2) || (Div == 8) || (Div == 32)  || (Div == 128), "Invalid clock divider");

        /**
         * @brief Helper class to get the correct clock id for the channels
         *
         * @tparam Id
         */
        template <uint32_t Id>
        struct tc_clock {
            constexpr static uint32_t clock_id = Id;
        };

        /**
         * @brief Interrupt handler
         *
         */
        static void isr_handler() {
            // read the status to clear the fields
            const uint32_t status = Timer::port->CH[Channel].SR;

            // run the callback if provided
            if ((status & (0b111 << 2)) && callback) {
                callback();
            }
        }

        /**
         * @brief Set the A/B/C register based on the channel
         * 
         */
        static uint32_t get_register() {
            // get a pointer to the first register
            volatile uint32_t* reg = &(Timer::port->CH[Channel].RA);

            // use the channel as a offset to the correct channel register
            return reg[Match];
        }

        /**
         * @brief Set the A/B/C register based on the channel
         *
         * @note for some mcu's this is a 16 bit register. Check the manual 
         * for the MCU you are using
         * 
         * @param value
         */
        static void set_register(const uint32_t value) {
            // get a pointer to the first register
            volatile uint32_t* reg = &(Timer::port->CH[Channel].RA);

            // use the channel as a offset to the correct channel register
            reg[Match] = value;
        }

        /**
         * @brief Internal init implementation to prevent duplicate code
         *
         * @param irq
         * @param frequency
         */
        static void init_impl(const interrupt_callback& irq, const uint32_t frequency) {
            // enable power to the timer peripheral
            target::io::power_control::enable<tc_clock<Timer::clock_id + Channel>>();

            // disable write protection
            Timer::port->WPMR = (0x54494D << 8);

            // disable the channel
            disable();

            // set the timer in waveform mode. Convert the divider 2 -> 0, 8 -> 1, 32 -> 2, 128 -> 3 and
            // configure the other parameters based on if we are in one_shot mode
            const uint32_t value = (
                ((klib::log2(Div) / 2) & 0x7) | ((Mode == mode::one_shot) << 6) | ((Mode == mode::one_shot) << 7) |
                (0b10 << 13) | (0x1 << 15)
            );

            Timer::port->CH[Channel].CMR = value;

            // set the frequency of the timer
            set_frequency(frequency);

            // set the callback
            callback = irq;

            if (irq) {
                // register the interrupt handler
                target::irq::register_irq<Timer::interrupt_id + Channel>(isr_handler);

                // enable the interrupt
                target::enable_irq<Timer::interrupt_id + Channel>();

                // enable the correct compare status based on the
                // match register used
                Timer::port->CH[Channel].IER = (0x1 << (Match + 2));
            }
            else {
                // disable the current match register interrupt
                Timer::port->CH[Channel].IDR = (0x1 << (Match + 2));

                // check if we have a interrupt on another match register
                // if we have we dont disable the whole channel
                if (!Timer::port->CH[Channel].IMR) {
                    // disable the interrupt
                    target::disable_irq<Timer::interrupt_id + Channel>();
                }
            }
        }

    public:
        /**
         * @brief Set the frequency of the timer
         *
         * @param Frequency
         */
        static void set_frequency(const uint32_t frequency) {
            // set the match register for the desired frequency
            set_register(((klib::io::clock::get() / Div) / frequency) + 1);
        }

        /**
         * @brief Disable the timer
         *
         */
        static void disable() {
            // disable the timer
            Timer::port->CH[Channel].CCR = (0x1 << 1);
        }

        /**
         * @brief Enable the timer
         *
         */
        static void enable() {
            // enable the timer
            Timer::port->CH[Channel].CCR = 0x1 | (0x1 << 2);
        }

        /**
         * @brief Returns the current value of a counter
         *
         * @return uint32_t
         */
        static uint32_t get_counter() {
            return Timer::port->CH[Channel].CV;
        }

        /**
         * @brief Clear the counter in the timer
         *
         */
        static void clear_counter() {
            Timer::port->CH[Channel].CCR = (0x1 << 2);
        }
    };
}

namespace klib::core::atsam4s::io {
    /**
     * @brief Basic timer. Uses interrupts to call a callback.
     *
     * @warning Timer counter only resets to 0 when the counter matches
     * match register 2. The other match registers can still trigger
     * interrupts (if below match register 2). All match registers share
     * registers and thus share all the configuration.
     *
     * @tparam Timer
     */
    template <typename Timer, uint32_t Channel, uint32_t Match = 2, uint32_t Div = 2>
    class timer: public detail::timer::base_timer<Timer, Channel, Match, detail::timer::mode::continuous, Div> {
    public:
        // using for the array of callbacks
        using interrupt_callback = void (*)();

        /**
         * @brief Init the timer
         *
         * @param irq
         * @param frequency
         */
        static void init(const interrupt_callback& irq, const uint32_t frequency) {
            // init the base timer
            detail::timer::base_timer<
                Timer, Channel, Match, detail::timer::mode::continuous, Div
            >::init_impl(irq, frequency);
        }
    };

    /**
     * @brief Oneshot timer. Uses interrupt to call a callback once.
     *
     * @tparam Timer
     */
    template <typename Timer, uint32_t Channel, uint32_t Match = 2, uint32_t Div = 2>
    class oneshot_timer: public detail::timer::base_timer<Timer, Channel, Match, detail::timer::mode::one_shot, Div> {
    protected:
        // make sure the correct match register is used. For one shot we need to
        // turn off the channel. This can only be done when match register 2 is
        // used
        static_assert(Match == 2, "Timer only supports Match register 2 for oneshot mode");

    public:
        // using for the array of callbacks
        using interrupt_callback = void (*)();

        /**
         * @brief Init the timer
         *
         * @param irq
         * @param frequency
         */
        static void init(const interrupt_callback& irq, const uint32_t frequency) {
            // init the base timer
            detail::timer::base_timer<
                Timer, Channel, Match, detail::timer::mode::one_shot, Div
            >::init_impl(irq, frequency);
        }

        /**
         * @brief Flag if the oneshot timer is done
         * 
         * @return true 
         * @return false 
         */
        static bool done() {
            // get the current counter
            const uint32_t c = detail::timer::base_timer<
                Timer, Channel, Match, detail::timer::mode::one_shot, Div
            >::get_counter();

            // get the maximum value of the counter (this is calculated in
            // init/set_frequency)
            const uint32_t m = detail::timer::base_timer<
                Timer, Channel, Match, detail::timer::mode::one_shot, Div
            >::get_register();

            // return if the register matches the RC register (match register 2)
            return c >= m;
        }
    };

    /**
     * @brief Pin that uses a timer to toggle the output.
     *
     * @warning When disabling the timer the output of the gpio is not changed.
     */
    template <
        typename Pin, typename Timer, uint32_t Frequency,
        uint8_t Bits, uint32_t Div = 2,
        typename PPin = std::tuple_element<klib::io::peripheral::get_index<Pin, typename Timer::tc_pins>(), typename Timer::tc_pins>::type
    >
    class pin_timer: public detail::timer::base_timer<Timer, PPin::channel, 2, detail::timer::mode::continuous, Div> {
    public:
        // export the frequency
        constexpr static uint32_t frequency = Frequency;

    protected:
        // make sure we have a valid match register
        static_assert(static_cast<uint32_t>(PPin::type) < 2, "Timer only has 2 match registers available for PWM mode");

        // check if the amount of bits is valid
        static_assert(Bits >= 1 && Bits <= 16, "Amount of bits must be >= 1 && <= 16");

        // make sure the frequency is valid
        static_assert(Frequency != 0, "Timer frequency cannot be 0");

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
            const auto cmp = ((klib::io::clock::get() / Div) / Frequency) + 1;

            if constexpr (FloatingPoint) {
                // calculate the step size
                return klib::max(static_cast<float>(cmp) / multiplier, 1.f);
            }
            else {
                // calculate the step size
                return klib::max(cmp / multiplier, 1);
            }
        }

        static auto dutycycle_impl(const uint32_t value) {
            // get a pointer to the first register
            volatile uint32_t* reg = &(Timer::port->CH[PPin::channel].RA);

            // check if we need to disable the output or
            // reenable it
            if (!value) {
                // disable the output to get a low level
                set<false>();
            }
            else if (!reg[static_cast<uint32_t>(PPin::type)]) {
                // reenable the pin when the pin was disabled
                set<true>();
            }

            // set the correct match register
            reg[static_cast<uint32_t>(PPin::type)] = value;
        }

    public:
        static void init() {
            // enable the power for the pio
            target::io::power_control::enable<typename Pin::port>();

            // init the base timer
            detail::timer::base_timer<
                Timer, PPin::channel, 2, detail::timer::mode::continuous, Div
            >::init_impl(nullptr, Frequency);

            // modify the channel CMR register to match the current pin
            Timer::port->CH[PPin::channel].CMR |= (
                // change to XC0 to allow TIOB as output. Note we do not have the
                // external event enabled.
                (0b01 << 10) |
                // TIOA
                (static_cast<uint32_t>(detail::timer::compare_effect::clear) << 16) |
                (static_cast<uint32_t>(detail::timer::compare_effect::set) << 18) |
                // TIOB
                (static_cast<uint32_t>(detail::timer::compare_effect::clear) << 24) |
                (static_cast<uint32_t>(detail::timer::compare_effect::set) << 26)
            );
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

            // set the dutycycle
            dutycycle_impl(value);
        }

        /**
         * @brief Set the dutycycle of the timer pin
         *
         * @param dutycycle
         */
        static void dutycycle(uint16_t dutycycle) {
            // calculate the step size
            const uint32_t value = calculate_stepsize() * (dutycycle & multiplier);

            // set the dutycycle
            dutycycle_impl(value);
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
                io::detail::pins::set_peripheral<Pin, typename PPin::periph>();
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
                io::detail::pins::set_peripheral<Pin, typename PPin::periph>();
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