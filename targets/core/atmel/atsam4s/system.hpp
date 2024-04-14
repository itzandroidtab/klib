#ifndef KLIB_ATMEL_ATSAM4S_SYSTEM_HPP
#define KLIB_ATMEL_ATSAM4S_SYSTEM_HPP

#include <klib/klib.hpp>
#include <klib/io/core_clock.hpp>

namespace klib::core::atsam4s::io::system {
    class flash {
    public:
        /**
         * @brief Set the amount of waitstates used for accessing flash
         * 
         * @details
         * 0 = for up to 20Mhz
         * 1 = for up to 40Mhz
         * 2 = for up to 60Mhz
         * 3 = for up to 80Mhz
         * 4 = for up to 100Mhz
         * 5 = for up to 120Mhz (safe setting for all conditions as 
         * it is the max cpu frequency)
         * 
         * @tparam WaitState 
         */
        template <uint8_t WaitState, bool Access64BitMode = false>
        static void setup() {
            // check the wait state from the user
            static_assert(WaitState <= 15, "Max wait cycles is 15 (+ 1)");

            // set the other bits and write it back
            EFC0->FMR = (
                (WaitState << 8) | (Access64BitMode << 24) | 
                (0x1 << 26) | (0x1 << 16)
            );
        }
    };

    class crystal {
    public:
        static bool is_enabled() {
            return PMC->CKGR_MOR & (0x1 << 16);
        }

        template <bool Enable>
        static void enable() {
            if constexpr (Enable) {
                // enable the oscillator, write with the key
                // and a maximum startup time
                PMC->CKGR_MOR = (
                    (PMC->CKGR_MOR & ~(0x1 << 1)) | (0x37 << 16) |
                    0x1 | (0xff << 8)
                );
            }
            else {
                // clear the enable bit
                PMC->CKGR_MOR = (PMC->CKGR_MOR & ~(0x1)) | (0x37 << 16);
            }
        }
    };

    class internal_rc {
    public:
        /**
         * @brief All the frequencies for the internal oscillator
         * 
         */
        enum class frequency {
            mhz_4 = 0,
            mhz_8 = 1,
            mhz_12 = 2,
        };

        static bool is_enabled() {
            return PMC->PMC_SR & (0x1 << 17);
        }

        template <bool Enable>
        static void enable() {
            if constexpr (Enable) {
                // enable the oscillator, write with the key
                PMC->CKGR_MOR |= (0x1 << 3) | (0x37 << 16);
            }
            else {
                // clear the enable bit
                PMC->CKGR_MOR = (PMC->CKGR_MOR & ~(0x1 << 3)) | (0x37 << 16);
            }
        }

        template <frequency Frequency>
        static void change_frequency() {
            // make sure the frequency is valid
            static_assert(static_cast<uint8_t>(Frequency) <= 2, "Invalid frequency");

            // set the frequency in the register
            PMC->CKGR_MOR = (
                (PMC->CKGR_MOR & ~(0x7 << 4)) | (static_cast<uint8_t>(Frequency) << 4) | 
                (0x37 << 16)
            );
        }
    };

}

#endif