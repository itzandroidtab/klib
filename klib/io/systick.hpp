#ifndef KLIB_SYSTICK_HPP
#define KLIB_SYSTICK_HPP

#include <cstdint>

#include <klib/io/core_clock.hpp>
#include <klib/units.hpp>

// set a default if the define is not set
#ifndef SYSTICK_CALLBACK_ENABLED
    #define SYSTICK_CALLBACK_ENABLED false
#endif

namespace klib::io {
    /**
     * @brief Systick class uses cpu id 0 by default
     * 
     * @tparam CpuId 
     */
    template <uint32_t CpuId = 0, bool Callback = SYSTICK_CALLBACK_ENABLED>
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

        template <typename Irq, bool ExternalClockSource = false>
        static void init_impl(const interrupt_callback& irq) {
            // clear the systick value
            port->value = 0;

            // only register the callback when enabled
            if constexpr (Callback) {
                // register the callback
                callback = irq;
            }

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
            init<Irq, ExternalClockSource>(klib::io::clock::get(), irq);
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
         * @brief Init the systick using the default 100hz calibration value 
         * in the systick register. Should only be used when calibration value
         * is valid for the current clock frequency
         * 
         * @tparam Irq 
         * @tparam ExternalClockSource 
         */
        template <typename Irq, bool ExternalClockSource = false>
        static void init_with_default_calibration(const interrupt_callback& irq = nullptr) {
            // divide the 100hz by 10 to get the interval for 1khz
            port->load = (port->calibration / 10);

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
         * @note when using ns we uint64_t for the conversions 
         * this might increase code size on certain micro's.
         * Due to the internal usage of uint32_t the ns counter
         * overflows every 4.29 seconds. 
         * 
         * To get the best accuracy the conversion should be done
         * after the fact. This function does the conversion 
         * every time it is called. This can cause overhead. Use
         * get_counter for the raw value that the systick 
         * provides (uses around 75 clock cycles for this call on
         * a cortex-m4 with time units ns)
         * 
         * @tparam T 
         * @return T 
         */
        template <typename T = time::ms>
        static T get_runtime() requires time::is_time_unit<T> {
            // check what time unit the callee wants
            if constexpr (std::is_same_v<T, time::us>) {
                // get the current counter
                const uint32_t value = get_counter();

                // return the higher precision conversion (as the counter
                // counts down we have to invert the result of the micro
                // second calculation)
                return (
                    static_cast<time::us>(time::ms{runtime.value}) + 
                    (time::us{999} - time::us{(value * 1'000) / port->load})
                );
            }
            else if constexpr (std::is_same_v<T, time::ns>) {
                // get the current counter
                const uint32_t value = get_counter();

                // return the even higher precision conversion. (as the 
                // counter counts down we have to invert the result of 
                // the nano second calculation). This uses uint64_t and
                // might be slow on some hardware
                return (
                    static_cast<time::ns>(time::ms{runtime.value}) + 
                    time::ns{999'999} - time::ns{
                        static_cast<uint32_t>(
                            (static_cast<uint64_t>(value) * 1'000'000) / port->load
                        )
                    }
                );
            }
            else {
                // use the ms runtime value if not us or ns
                return {runtime.value};
            }
        }

    public:
        /**
         * @brief Interrupt handler. Should not be called by the user
         * 
         */
        static void isr_handler() {
            // clear the interrupt flag by reading 
            // the systick control register
            (void) port->ctrl;
            
            // increment the current runtime value
            runtime.value++;

            // only run the callback when enabled
            if constexpr (Callback) {
                // run the callback if provided
                if (callback) {
                    callback();
                }
            }
        }
    };
}

#endif