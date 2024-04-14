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
            return PMC->CKGR_MOR & 0x1;
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
            return PMC->CKGR_MOR & (0x1 << 3);
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

    class clock {
    public:
        /**
         * @brief Available plls on the chip
         * 
         */
        enum class pll {
            plla = 0,
            pllb = 1,
            none,
        };

        /**
         * @brief Available clock sources
         * 
         */
        enum class source {
            internal = 0,
            main = 1,
            rtc = 2,
        };

    protected:
        /**
         * @brief Helper function to check if a source is enabled
         * 
         * @tparam Source 
         * @return true 
         * @return false 
         */
        template <source Source>
        static bool is_enabled() {
            if constexpr (Source == source::main) {
                return crystal::is_enabled();
            }
            else if constexpr (Source == source::rtc) {
                // TODO: implement rtc crystal check
            }
            else if constexpr (Source == source::internal) {
                return internal_rc::is_enabled();
            }

            return false;
        }

        /**
         * @brief Enable or disable the specified pll
         * 
         * @tparam Pll 
         * @tparam Enable 
         * @return true 
         * @return false 
         */
        template <pll Pll, bool Enable = false, uint32_t Value = 0x00>
        static void setup_impl() {
            if constexpr (Pll == pll::plla) {
                if constexpr (Enable) {
                    PMC->CKGR_PLLAR = (0x1 << 29) | (Value & 0x07ff3fff);
                }
                else {
                    PMC->CKGR_PLLAR = (0x1 << 29);
                }
            }
            else {
                if constexpr (Enable) {
                    PMC->CKGR_PLLBR = (Value & 0x07ff3fff);
                }
                else {
                    PMC->CKGR_PLLBR = 0x00;
                }
            }
        }

        enum class master_clock_source {
            slow = 0x00,
            main = 0x01,
            plla = 0x02,
            pllb = 0x03
        };

        /**
         * @brief Switch to a specific master clock source
         * 
         * @tparam source 
         */
        template <master_clock_source Source>
        static void switch_master_clock() {
            // clear the processor clock prescaler
            PMC->PMC_MCKR = (PMC->PMC_MCKR & ~(0x7 << 4));

            // wait until the master clock output is stable
            while (!(PMC->PMC_SR & (0x1 << 3))) {
                // do nothing
            }

            // switch to the provided source
            PMC->PMC_MCKR = (
                (PMC->PMC_MCKR & ~((0x3) | (0x3 << 12))) | static_cast<uint32_t>(Source)
            );

            // wait until the master clock output is stable
            while (!(PMC->PMC_SR & (0x1 << 3))) {
                // do nothing
            }
        }

        template <source Source>
        static void enable() {
            // check what source we need to enable
            if constexpr (Source == source::main) {
                // check if we need to enable the source
                if (!is_enabled<Source>()) {
                    // enable the crystal
                    crystal::enable<true>();

                    // wait until stabilized
                    while (!(PMC->PMC_SR & 0x1)) {
                        // do nothing
                    }
                }

                // switch to the external crystal for
                // the main output
                PMC->CKGR_MOR |= (0x1 << 24) | (0x37 << 16);

                // wait until we have switched
                while (!(PMC->PMC_SR & (0x1 << 16))) {
                    // do nothing
                }

                // disable the internal rc
                internal_rc::enable<false>();
            }
            else if constexpr (Source == source::rtc) {
                // TODO: rtc crystal setup
                // SUPC->SUPC_CR = SUPC_CR_KEY_PASSWD | SUPC_CR_XTALSEL;
            }
            else if constexpr (Source == source::internal) {
                // check if we need to enable the source
                if (!is_enabled<Source>()) {
                    // enable the internal rc
                    internal_rc::enable<true>();

                    // wait until it has started
                    while (!(PMC->PMC_SR & (0x1 << 17))) {
                        // do nothing
                    }
                }

                // switch to the internal rc oscillator
                PMC->CKGR_MOR = (PMC->CKGR_MOR & ~(0x1 << 24)) | (0x37 << 16);

                // wait until we have switched
                while (!(PMC->PMC_SR & (0x1 << 16))) {
                    // do nothing
                }

                // disable the crystal
                crystal::enable<false>();
            }
        }

    public:
        /**
         * @brief Set the internal rc frequency
         * 
         * @tparam Freq 
         */
        template <uint32_t Freq>
        static void set_internal_rc() {
            // make sure we have a valid frequency
            static_assert(
                (Freq == 4'000'000) || (Freq == 8'000'000) || (Freq == 12'000'000),
                "Invalid internal rc frequency, supported are 4/8/12 mhz"
            );

            // check if the internal_rc is enabled
            if (!internal_rc::is_enabled()) {
                // enable the internal rc
                internal_rc::enable<true>();

                // wait until it has started
                while (!(PMC->PMC_SR & (0x1 << 17))) {
                    // do nothing
                }
            }

            // change to the requested frequency
            internal_rc::change_frequency<
                static_cast<internal_rc::frequency>((Freq / 4'000'000) - 1)
            >();

            // wait until the internal rc has stabalized
            while (!(PMC->PMC_SR & (0x1 << 17))) {
                // do nothing
            }
        }

        /**
         * @brief Setup a pll
         * 
         * @details PLL calculation
         * PLL frequency calculation:
         *     FCCO = ((Crystal * Multiplier) / Div)
         * 
         * @tparam Pll 
         * @param multiplier PLL multiplier.
         * @param divider
         */
        template <pll Pll, uint16_t Multiplier, const uint8_t Div>
        static void setup() {
            static_assert(Pll != pll::none, "Unsupported pll selected");

            // stop the pll. We always need to stop it first
            setup_impl<Pll, false>();

            // enable the pll
            setup_impl<Pll, true, (Div | (0x3f << 8) | (((Multiplier - 1) & 0x7ff) << 16))>();

            // wait until we have a lock
            while (!(PMC->PMC_SR & (0x1 << (1 + static_cast<uint8_t>(Pll))))) {
                // do nothing
            }
        }

        template <
            source Source, pll Pll, uint32_t Freq, 
            uint16_t Multiplier = 0, uint32_t Div = 0
        >
        static void set_main() {
            // disable the write protection
            PMC->PMC_WPMR = (0x504d43 << 8);   

            // notify klib what freqency we are running
            klib::io::clock::set(Freq);

            // enable the source we want to use
            enable<Source>();

            // switch to the slow or main clock
            switch_master_clock<
                (Source == source::rtc) ? 
                master_clock_source::slow : 
                master_clock_source::main
            >();

            // check if we need to configure a pll
            if constexpr (Pll != pll::none) {
                // make sure the user did not enable a pll. slow clock is not 
                // supported as a pll input
                static_assert(
                    Source != source::rtc, "slow clock (rtc) is not supported as a pll source"
                );

                // setup the pll
                setup<Pll, Multiplier, Div>();

                // switch to the pll
                switch_master_clock<
                    (Pll == pll::plla) ? 
                    master_clock_source::plla : 
                    master_clock_source::pllb
                >();
            }
        }

        template <uint32_t ExtCrystalFreq, uint8_t PreDivider = 0x1>
        static void set_usb() {
            // TODO: usb setup
        }
    };
}

#endif