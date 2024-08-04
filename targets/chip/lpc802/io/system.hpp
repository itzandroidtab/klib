#ifndef KLIB_LPC802_GCR_HPP
#define KLIB_LPC802_GCR_HPP

#include <cstdint>

#include <lpc802.hpp>

#include <klib/io/core_clock.hpp>

namespace klib::lpc802::io::system {
    /**
     * @brief System clock control register
     *
     */
    class clock {
    public:
        // available clock sources
        enum class clock_source {
            mhz_9 = 9'000'000,
            mhz_12 = 12'000'000,
            mhz_15 = 15'000'000,
        };

        // get a pointer to the clock trimming register. This is a
        // undocumented register that is used in the bootrom
        static volatile inline uint32_t *const trim = reinterpret_cast<volatile uint32_t*>(0x40048000 + 0x28);

        // get a pointer to the syscon
        static inline SYSCON_Type *const port = SYSCON;

    public:
        template <clock_source Clock>
        static void set() {
            // check if we have a valid clock
            static_assert(
                Clock == clock_source::mhz_9 ||
                Clock == clock_source::mhz_12 ||
                Clock == clock_source::mhz_15,
                "Invalid clock value. Use manual trimming instead"
            );

            // check what clock to setup
            switch (Clock) {
                case clock_source::mhz_9:
                    // read the calibrated value from the flash
                    (*trim) = ((volatile uint32_t*)(0x3fcc))[0];
                    break;
                case clock_source::mhz_15:
                    // read the calibrated value from the flash
                    (*trim) = ((volatile uint32_t*)(0x3fd4))[0];
                    break;
                case clock_source::mhz_12:
                    // read the calibrated value from the flash
                    (*trim) = ((volatile uint32_t*)(0x3fd0))[0];
                    break;
                default:
                    break;
            }

            // set the clock divider to 1
            port->SYSAHBCLKDIV = 0x1;

            // set the clock speed
            klib::io::clock::set(static_cast<uint32_t>(Clock));
        }

        template <uint32_t Trim, uint32_t ClockFreq>
        static void set() {
            // check if the trim value will lock up the cpu
            static_assert(
                Trim <= (klib::exp2(18) - 1),
                "Trim value sets bit that will lock up the cpu"
            );

            // set the trim value in the register
            (*trim) = Trim;

            // set the clock divider to 1
            port->SYSAHBCLKDIV = 0x1;

            // set the frequency the user provides
            klib::io::clock::set(ClockFreq);
        }
    };
}

#endif