#ifndef KLIB_NXP_LPC175X_SYSTEM_HPP
#define KLIB_NXP_LPC175X_SYSTEM_HPP

#include <klib/klib.hpp>
#include <klib/io/core_clock.hpp>

namespace klib::core::lpc175x::io::system {
    class crystal {
    public:
        static bool is_enabled() {
            return SYSCON->SCS & (0x1 << 6);
        }

        template <bool Enable>
        static void enable() {
            if constexpr (Enable) {
                SYSCON->SCS |= (0x1 << 5);
            }
            else {
                SYSCON->SCS &= ~(0x1 << 5);
            }
        }
    };

    class clock {
    public:
        /**
         * @brief Available plls on the chip
         *
         */
        enum class pll {
            main = 0,
            usb = 1
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

        /**
         * @brief USB PLL clock divider if the USB pll is not used for USB
         *
         */
        enum class usb_pll_divider {
            mhz_288 = 5,
            mhz_384 = 7,
            mhz_480 = 9,
        };

        /**
         * @brief Check if a pll is connected
         *
         * @tparam Pll
         * @return bool
         */
        template <pll Pll>
        static bool is_connected() {
            if constexpr (Pll == pll::main) {
                return SYSCON->PLL0STAT & (0x1 << 25);
            }
            else {
                return SYSCON->PLL1STAT & (0x1 << 9);
            }
        }

        /**
         * @brief
         *
         * @tparam Pll
         * @tparam Connect
         */
        template <pll Pll, bool Connect>
        static void connect() {
            if constexpr (Pll == pll::main) {
                if constexpr (Connect) {
                    SYSCON->PLL0CON |= (0x1 << 1);
                }
                else {
                    SYSCON->PLL0CON &= ~(0x1 << 1);
                }
            }
            else {
                if constexpr (Connect) {
                    SYSCON->PLL1CON |= (0x1 << 1);
                }
                else {
                    SYSCON->PLL1CON &= ~(0x1 << 1);
                }
            }

            // write the settings using the feed
            feed<Pll>();
        }

        /**
         * @brief Returns if a pll is enabled
         *
         * @tparam Pll
         * @return true
         * @return false
         */
        template <pll Pll>
        static bool is_enabled() {
            if constexpr (Pll == pll::main) {
                return SYSCON->PLL0STAT & (0x1 << 24);
            }
            else {
                return SYSCON->PLL1STAT & (0x1 << 8);
            }
        }

        /**
         * @brief Enable or disable the specified pll
         *
         * @tparam Pll
         * @tparam Enable
         * @return true
         * @return false
         */
        template <pll Pll, bool Enable>
        static void enable() {
            if constexpr (Pll == pll::main) {
                if constexpr (Enable) {
                    SYSCON->PLL0CON |= 0x1;
                }
                else {
                    SYSCON->PLL0CON &= ~(0x1);
                }
            }
            else {
                if constexpr (Enable) {
                    SYSCON->PLL1CON |= 0x1;
                }
                else {
                    SYSCON->PLL1CON &= ~(0x1);
                }
            }

            // write the settings using the feed
            feed<Pll>();
        }

        /**
         * @brief Feed the pll
         *
         * @tparam Pll
         */
        template <pll Pll>
        static void feed() {
            // check what pll to feed with the feed sequence
            if constexpr (Pll == pll::main) {
                SYSCON->PLL0FEED = 0xaa;
                SYSCON->PLL0FEED = 0x55;
            }
            else {
                SYSCON->PLL1FEED = 0xaa;
                SYSCON->PLL1FEED = 0x55;
            }
        }

        /**
         * @brief Setup a pll
         *
         * @details PLL0 calculation
         * PLL0 frequency:
         *     FCCO = (2 * M * FIN) / N
         *
         *     M = (FCCO * N) / (2 * FIN)
         *     N = (2 * M * FIN) / FCCO
         *     FIN = (FCCO * N) / (2 * M)
         *
         * PLL1 frequency:
         *     USBCLK (48Mhz) = M * FOSC or
         *     USBCLK (48Mhz) = FCCO / (2 * P)
         *
         *     The CCO frequency can be computed as:
         *     FCCO = USBCLK (48Mhz) * 2 * P or
         *     FCCO = FOSC ´ M * 2 * P
         *
         * @tparam Pll
         * @param multiplier PLL multiplier.
         * @param pre_divider pre divider value.
         */
        template <pll Pll>
        static void setup(const uint16_t multiplier, const uint8_t pre_divider) {
            if constexpr (Pll == pll::main) {
                SYSCON->PLL0CFG = (multiplier | (static_cast<uint32_t>(pre_divider) << 16));
            }
            else {
                SYSCON->PLL1CFG = (multiplier | (static_cast<uint32_t>(pre_divider) << 5));
            }

            // write the settings using the feed
            feed<Pll>();
        }

        template <pll Pll>
        static bool is_locked() {
            if constexpr (Pll == pll::main) {
                return SYSCON->PLL0STAT & (0x1 << 26);
            }
            else {
                return SYSCON->PLL1STAT & (0x1 << 10);
            }
        }

        template <source Source, uint32_t Freq, uint16_t Multiplier, uint8_t PreDivider, uint32_t Div>
        static void set_main() {
            // make sure the input is valid
            static_assert(Freq > 0, "Invalid frequency");
            static_assert(Multiplier > 0, "Invalid multiplier");
            static_assert(Div > 0 && Div <= 256, "Invalid divider");
            static_assert(PreDivider > 0, "Invalid pre-divider");

            // calculate the fcco frequency using the oscilator and multiplier
            constexpr static uint32_t partial_freq = (Freq * 2 * Multiplier) / klib::exp2(static_cast<uint32_t>(PreDivider));

            static_assert(
                (Multiplier == 1) || (partial_freq >= 156'000'000 && partial_freq <= 320'000'000), 
                "Invalid Fcco output. PLL frequency needs to be between 156 and 320mhz"
            );

            // calculate the final frequency
            constexpr static uint32_t freq = (partial_freq / Div);

            // disconnect the main pll if it is connected.
            if (is_connected<pll::main>()) {
                // disconnect the pll
                connect<pll::main, false>();
            }

            // check if the pll is enabled
            if (is_enabled<pll::main>()) {
                // disable the pll
                enable<pll::main, false>();
            }

            // check if we need the crystal
            if constexpr (Source == source::main) {
                // enable the crystal if it is not enabled
                if (!crystal::is_enabled()) {
                    // enable the crystal
                    crystal::enable<true>();
                }
            }

            // set the clock divider to 0
            SYSCON->CCLKCFG = 0;

            // set the clock source for the system clock
            SYSCON->CLKSRCSEL = static_cast<uint32_t>(Source);

            // check if we need to setup the pll
            if constexpr (Multiplier > 1) {
                // setup the pll
                setup<pll::main>((Multiplier - 1), (PreDivider - 1));

                // enable the pll after configuring it
                enable<pll::main, true>();

                // wait until the pll is locked
                while (!is_locked<pll::main>()) {
                    // do nothing
                }
            }

            // setup the divider to get the correct cpu frequency
            SYSCON->CCLKCFG = (Div - 1);

            // check if we need to connect the pll
            if constexpr (Multiplier > 1) {
                // connect the pll
                connect<pll::main, true>();
            }

            // notify klib what freqency we are running
            klib::io::clock::set(freq);
        }

        template <uint32_t ExtCrystalFreq, uint8_t PreDivider = 0x1>
        static void set_usb() {
            static_assert(
                (48'000'000 % ExtCrystalFreq) == 0,
                "Invalid external crystal frequency. Official supported frequencies are 12Mhz, 16Mhz and 24Mhz"
            );

            // setup the clock to 48 mhz using the oscillator
            clock::setup<clock::pll::usb>((48'000'000 / ExtCrystalFreq) - 1, PreDivider);

            // enable the pll after configuring it
            clock::enable<clock::pll::usb, true>();

            // wait until the pll is locked
            while (!clock::is_locked<clock::pll::usb>()) {
                // do nothing
            }

            // connect the usb pll
            clock::connect<clock::pll::usb, true>();
        }

        /**
         * @brief Set the USB clock input to the main pll with
         * a divider
         *
         * @tparam Freq
         */
        template <usb_pll_divider Freq>
        static void set_usb() {
            // disable the USB pll to switch to main pll
            // with a divider
            clock::enable<clock::pll::usb, false>();

            // set the clock divider to the one provided
            SYSCON->USBCLKCFG = static_cast<uint32_t>(Freq);
        }
    };

    class trace {
    public:
        template <bool Enable>
        static void enable() {
            if constexpr (Enable) {
                PINCONNECT->PINSEL[10] = 0x1 << 3;
            }
            else {
                PINCONNECT->PINSEL[10] = 0;
            }
        }

    };
}

#endif