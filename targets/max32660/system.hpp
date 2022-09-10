#ifndef KLIB_MAX32660_GCR_HPP
#define KLIB_MAX32660_GCR_HPP

#include <cstdint>

#include <max32660.hpp>
#include <klib/core_clock.hpp>

namespace klib::max32660::system {
    /**
     * @brief System control register
     * 
     */
    class control {
    public:
        /**
         * @brief Enable or disable the floating point unit (FPU)
         * 
         * @tparam Enabled 
         */
        template <bool Enabled>
        static void enable_fpu() {
            // enable or disable the fpu
            if constexpr (Enabled) {
                // clear the bit to enable the fpu
                GCR->SCON &= ~(1 << 5);
            }
            else {
                // set the bit to disable the fpu
                GCR->SCON |= (1 << 5);
            }
        } 

        /**
         * @brief Flush the cache and wait for the flush to finish
         * 
         */
        static void flush_cache() {
            // trigger the cache flush
            GCR->SCON |= (1 << 6);

            // wait until the cache flush is finished
            while (GCR->SCON & (1 << 6)) {}
        }

        /**
         * @brief Enable or disable the serial wire debug (SWD). Only 
         * writable if the fmv lock word is not programmed.
         * 
         * @tparam Enabled 
         */
        template <bool Enabled>
        static void enable_swd() {
            // enable or disable the swd interface
            if constexpr (Enabled) {
                // clear the bit to enable the swd interface
                GCR->SCON &= ~(1 << 14);
            }
            else {
                // set the bit to disable the swd interface
                GCR->SCON |= (1 << 14);
            }
        }
    };

    /**
     * @brief System clock control register
     * 
     */
    class clock {
    public:
        // available clock sources
        enum class clock_source {
            // high frequency internal oscillator
            hfio = 0,
            // 80khz nano-ring oscillator
            nano = 3,
            // external 32khz crystal or external clock
            x32k = 6
        };

    protected:
        template <clock_source Clock>
        static void set_source() {
            // set the correct oscillator selection
            GCR->CLKCN = (GCR->CLKCN & ~(0x7 << 9)) | (static_cast<uint8_t>(Clock) << 9);
        }

    public:
        template <clock_source Clock>
        static void set() {
            // check what clock to setup
            switch (Clock) {
                case clock_source::hfio:
                    // check if the high frequency clock is already enabled
                    if (GCR->CLKCN & (0x1 << 18)) {
                        // enable the clock
                        GCR->CLKCN |= (0x1 << 18);
                    }

                    // set the oscillator source
                    set_source<Clock>();

                    // wait until the flag is ready
                    while (!(GCR->CLKCN & (0x1 << 26))) {
                        // do nothing
                    }

                    // change the core clock to the correct frequency
                    klib::clock::set(96'000'000);

                    break;
                case clock_source::nano:
                    // // check if the nano frequency clock is already enabled
                    // if (GCR->CLKCN & (0x1 << 18)) {
                    //     // enable the clock
                    //     GCR->CLKCN |= (0x1 << 18);
                    // }

                    // set the oscillator source
                    set_source<Clock>();

                    // wait until the flag is ready
                    while (!(GCR->CLKCN & (0x1 << 29))) {
                        // do nothing
                    }

                    // change the core clock to the correct frequency
                    klib::clock::set(80'000);

                    break;
                case clock_source::x32k:
                    // check if the external clock is already enabled
                    if (GCR->CLKCN & (0x1 << 17)) {
                        // enable the clock
                        GCR->CLKCN |= (0x1 << 17);
                    }

                    // set the oscillator source
                    set_source<Clock>();

                    // wait until the flag is ready
                    while (!(GCR->CLKCN & (0x1 << 25))) {
                        // do nothing
                    }

                    // change the core clock to the correct frequency
                    klib::clock::set(32'768);
                    break;
            }
        }
    };
}

#endif