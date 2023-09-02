#ifndef KLIB_LPC1756_SYSTEM_HPP
#define KLIB_LPC1756_SYSTEM_HPP

#include <lpc1756.hpp>

#include <klib/io/core_clock.hpp>

namespace klib::lpc1756::io::system {
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
         * 5 = safe setting for all conditions
         * 
         * @tparam WaitState 
         */
        template <uint8_t WaitState>
        static void setup() {
            // check the wait state from the user
            static_assert(WaitState <= 5, "Max wait cycles is 5 (+ 1)");

            // get the register
            const uint32_t tmp = SYSCON->FLASHCFG & 0xFFF;

            // set the other bits and write it back
            SYSCON->FLASHCFG = tmp | (WaitState << 12);
        }
    };

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

            // enable the crystal if it is not enabled
            if (!crystal::is_enabled()) {
                // enable the crystal
                crystal::enable<true>();
            }

            // set the clock divider to 0
            SYSCON->CCLKCFG = 0;

            // set the clock source for PLL0 to the oscillator
            SYSCON->CLKSRCSEL = static_cast<uint32_t>(Source);

            // setup the pll
            setup<pll::main>(Multiplier, PreDivider);

            // enable the pll after configuring it
            enable<pll::main, true>();

            // setup the divider to get the correct cpu frequency
            SYSCON->CCLKCFG = Div;

            // wait until the pll is locked
            while (!is_locked<pll::main>()) {
                // do nothing
            }

            // connect the pll
            connect<pll::main, true>();
            
            // notify klib what freqency we are running
            klib::io::clock::set(Freq);
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
    };

    class trace {
    public:
        template <bool Enable>
        static void enable() {
            if constexpr (Enable) {
                PINCONNECT->PINSEL10 = 0x1 << 3;
            }
            else {
                PINCONNECT->PINSEL10 = 0;
            }
        }

    };
}

#endif