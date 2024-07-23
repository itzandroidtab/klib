#ifndef KLIB_ATMEL_ATSAM4S_TIMER_HPP
#define KLIB_ATMEL_ATSAM4S_TIMER_HPP

#include <cstdint>

#include <klib/klib.hpp>
#include <klib/io/core_clock.hpp>
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
     * @brief Supported edge selection
     * 
     */
    enum class edge {
        none = 0x00,
        falling = 0x01,
        rising = 0x02,
        dual_edge = 0x03
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
        static void init_impl(const interrupt_callback& irq, const uint32_t frequency, const edge tioa, const edge tiob) {
            // enable power to the timer peripheral
            target::io::power_control::enable<tc_clock<Timer::clock_id + Channel>>();

            // disable write protection
            Timer::port->WPMR = (0x54494D << 2);

            // disable the channel
            disable();

            // set the timer in waveform mode. Convert the divider 2 -> 0, 8 -> 1, 32 -> 2, 128 -> 3 and 
            // configure the other parameters based on if we are in one_shot mode
            const uint32_t value = (
                ((klib::log2(Div) / 2) & 0x7) | ((Mode == mode::one_shot) << 6) | ((Mode == mode::one_shot) << 7) | 
                (0b10 << 13) | (0x1 << 15) | (static_cast<uint32_t>(tioa) << 16) | (static_cast<uint32_t>(tiob) << 18)
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
                target::enable_irq<Timer::interrupt_id>();          

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
            >::init_impl(irq, frequency, detail::timer::edge::none, detail::timer::edge::none);
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
            >::init_impl(irq, frequency, detail::timer::edge::none, detail::timer::edge::none);
        }
    };
}

#endif