#ifndef KLIB_SYSTICK_HPP
#define KLIB_SYSTICK_HPP

#include <cstdint>

#include <klib/core_clock.hpp>
#include <klib/units.hpp>

namespace klib {
    class systick {
    protected:
        /**
         * @brief Struct to access the systick registers
         * 
         */
        struct systick_type {
            // control and status register
            volatile uint32_t ctrl;

            // reload value register
            volatile uint32_t load;

            // current value register
            volatile uint32_t value;

            // counter value for a 10ms interrupt
            volatile const uint32_t calibration;
        };

        // get a pointer to the systick registers
        static inline systick_type *const port = reinterpret_cast<systick_type*>(0xe000e010);

        // using for the array of callbacks
        using interrupt_callback = void (*)();

        // additional callback when the systick is triggered.
        static inline interrupt_callback callback = nullptr;

        // current runtime value in ms. Can store up to 49.71 
        // days of runtime in ms
        static volatile inline time::ms runtime = 0;

        /**
         * @brief Interrupt handler
         * 
         */
        static void isr_handler() {
            // clear the interrupt flag by reading 
            // the systick control register
            (void) port->ctrl;
            
            // increment the current runtime value
            runtime.value++;

            // run the callback if provided
            if (callback) {
                callback();
            }
        }

        template <typename Irq, bool ExternalClockSource = false>
        static void init_impl(const interrupt_callback& irq) {
            // clear the systick value
            port->value = 0;

            // register the callback
            callback = irq;

            // register our handler
            Irq::template register_irq<Irq::arm_vector::systick>(isr_handler);

            // setup the clock source and enable 
            // the systick interrupt
            port->ctrl = (ExternalClockSource << 2) | (0x1 << 1);
        }

    public:
        /**
         * @brief Init the systick timer to 1khz with a callback method using 
         * supplied clock value
         * 
         * @tparam Irq
         * @tparam ExternalClockSource
         * @param irq 
         * @param clock 
         */
        template <typename Irq, bool ExternalClockSource = false>
        static void init(const uint32_t clock, const interrupt_callback& irq = nullptr) {
            // calculate the interval for 1khz
            port->load = (clock / 1'000) - 1;

            // init the systick
            init_impl<Irq, ExternalClockSource>(irq);
        }

        /**
         * @brief Init the systick timer to 1khz with a callback method using 
         * the internal clock value
         * 
         * @tparam Irq
         * @tparam ExternalClockSource
         * @param irq 
         */
        template <typename Irq, bool ExternalClockSource = false>
        static void init(const interrupt_callback& irq = nullptr) {
            // init the systick
            init<Irq, ExternalClockSource>(klib::clock::get(), irq);
        }

        /**
         * @brief Init the systick using a calibration value. Klib expects 
         * a 1 khz output for the timing
         * 
         * @tparam Irq 
         * @tparam ExternalClockSource 
         * @param irq 
         */
        template <typename Irq, bool ExternalClockSource = false>
        static void init_with_calibration(const uint32_t calibration, const interrupt_callback& irq = nullptr) {
            // calculate the interval for 1khz
            port->load = calibration;

            // init the systick
            init_impl<Irq, ExternalClockSource>(irq);
        }

        /**
         * @brief Disable the timer
         * 
         */
        static void disable() {
            // disable the timer
            port->ctrl &= ~0x1;
        }

        /**
         * @brief Enable the timer
         * 
         */
        static void enable() {
            // enable the timer
            port->ctrl |= 0x1;
        }

        /**
         * @brief Returns the current value of a counter
         * 
         * @return uint32_t 
         */
        static uint32_t get_counter() {
            return port->value;
        }

        /**
         * @brief Clear the counter in the timer
         * 
         */
        static void clear_counter() {
            port->value = 0;
        }

        /**
         * @brief Get the runtime of the cpu in milliseconds
         * 
         * @return klib::time::ms 
         */
        static klib::time::ms get_runtime() {
            return {runtime.value};
        }
    };
}

#endif