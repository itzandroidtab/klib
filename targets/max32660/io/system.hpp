#ifndef KLIB_MAX32660_GCR_HPP
#define KLIB_MAX32660_GCR_HPP

#include <cstdint>

#include <klib/io/core_clock.hpp>

#include <max32660.hpp>
#include "pins.hpp"

namespace klib::max32660::io::periph::detail::swd {
    enum class mode {
        dio,
        clk
    };

    template <typename Pin, mode Type, typename Periph>
    struct swd {
        // pin of the peripheral
        using pin = Pin;

        // type of the pin
        constexpr static mode type = Type;

        // alternate function
        using periph = Periph;
    };
}

// peripheral pin mapping to alternate functions.
namespace klib::max32660::io::periph::wlp {
    struct swd0_0 {
        // peripheral id (e.g swd0, swd1)
        constexpr static uint32_t id = 0;

        // configuration of the pins
        using dio = detail::swd::swd<pins::package::wlp::pc1, detail::swd::mode::dio, io::detail::alternate::func_1>;
        using clk = detail::swd::swd<pins::package::wlp::pc2, detail::swd::mode::clk, io::detail::alternate::func_1>;
    };

    struct swd0_1 {
        // peripheral id (e.g swd0, swd1)
        constexpr static uint32_t id = 0;

        // configuration of the pins
        using dio = detail::swd::swd<pins::package::wlp::pb4, detail::swd::mode::dio, io::detail::alternate::func_2>;
        using clk = detail::swd::swd<pins::package::wlp::pb3, detail::swd::mode::clk, io::detail::alternate::func_2>;
    };
}

namespace klib::max32660::io::periph::tqfn_24 {
    struct swd0_0 {
        // peripheral id (e.g swd0, swd1)
        constexpr static uint32_t id = 0;

        // configuration of the pins
        using dio = detail::swd::swd<pins::package::tqfn_24::p2, detail::swd::mode::dio, io::detail::alternate::func_1>;
        using clk = detail::swd::swd<pins::package::tqfn_24::p1, detail::swd::mode::clk, io::detail::alternate::func_1>;
    };

    struct swd0_1 {
        // peripheral id (e.g swd0, swd1)
        constexpr static uint32_t id = 0;

        // configuration of the pins
        using dio = detail::swd::swd<pins::package::tqfn_24::p14, detail::swd::mode::dio, io::detail::alternate::func_2>;
        using clk = detail::swd::swd<pins::package::tqfn_24::p13, detail::swd::mode::clk, io::detail::alternate::func_2>;
    };
}

namespace klib::max32660::io::system {
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
                    klib::io::clock::set(96'000'000);

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
                    klib::io::clock::set(80'000);

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
                    klib::io::clock::set(32'768);
                    break;
            }
        }
    };
}

#endif