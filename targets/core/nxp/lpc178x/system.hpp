#ifndef KLIB_NXP_LPC175X_SYSTEM_HPP
#define KLIB_NXP_LPC175X_SYSTEM_HPP

#include <klib/klib.hpp>
#include <io/port.hpp>

namespace klib::core::lpc178x::io::system {
    class crystal {
    public:
        static bool status() {
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
            alternate = 1
        };

        /**
         * @brief Available clock sources
         * 
         */
        enum class source {
            internal = 0,
            main = 1,
        };

        /**
         * @brief Available pre dividers
         * 
         */
        enum class pre_divider {
            div_1 = 0x00,
            div_2 = 0x01,
            div_4 = 0x02,
            div_8 = 0x03,
        };

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
         * @details PLL0 calculation:
         *     FCCO = (2 * M * FIN) / N
         * 
         *     M = (FCCO * N) / (2 * FIN)
         *     N = (2 * M * FIN) / FCCO
         *     FIN = (FCCO * N) / (2 * M)
         * 
         * @tparam Pll 
         * @param multiplier PLL multiplier.
         * @param pre_divider pre divider value.
         */
        template <pll Pll>
        static void setup(const uint16_t multiplier, const pre_divider divider) {
            // check what pll to configure
            if constexpr (Pll == pll::main) {
                SYSCON->PLL0CFG = (multiplier | (static_cast<uint32_t>(divider) << 5));
            }
            else {
                SYSCON->PLL1CFG = (multiplier | (static_cast<uint32_t>(divider) << 5));
            }
        }

        /**
         * @brief Enable/Disable a pll
         * 
         * @tparam Pll 
         */
        template <pll Pll, bool Enable>
        static void enable() {
            if constexpr (Pll == pll::main) {
                SYSCON->PLL0CON = Enable;
            }
            else {
                SYSCON->PLL1CON = Enable;
            }

            feed<Pll>();
        }

        /**
         * @brief Returns if the pll is locked
         * 
         * @tparam Pll 
         * @return true 
         * @return false 
         */
        template <pll Pll>
        static bool is_locked() {
            if constexpr (Pll == pll::main) {
                return SYSCON->PLL0STAT & (0x1 << 10);
            }
            else {
                return SYSCON->PLL1STAT & (0x1 << 10);
            }
        }

        template <
            source Source, uint32_t Freq, 
            uint16_t Multiplier, pre_divider PreDivider, 
            uint8_t Div
        >
        static void set_main() {
            // enable the crystal if we are changing to that
            if constexpr (Source == source::main) {
                crystal::enable<true>();

                // wait until the crystal is stable
                while (!crystal::status()) {
                    // do nothing
                }
            }

            // enable/disable the boost based on the frequency 
            // (> 100 Mhz = on)
            SYSCON->PBOOST = (Freq > 100'000'000) ? 0b11 : 0b00;

            // check if we need to enable the pll
            if constexpr (Multiplier) {
                // disconnect the main pll if it is connected.
                if (SYSCON->CCLKSEL & (0x1 << 8)) {
                    SYSCON->CCLKSEL &= ~(0x1 << 8);
                }

                // change usb to sysclk if it is connected to the pll
                if (((SYSCON->USBCLKSEL >> 8) & 0b11) == 0x01) {
                    // change to sysclk
                    SYSCON->USBCLKSEL &= ~(0b11 << 8);
                }

                // change spifi to sysclk if it is connected to the pll
                if (((SYSCON->SPIFICLKSEL >> 8) & 0b11) == 0x01) {
                    // change to sysclk
                    SYSCON->SPIFICLKSEL &= ~(0b11 << 8);
                }

                // Change to the requested source
                SYSCON->CLKSRCSEL = static_cast<uint32_t>(Source);

                // setup the main pll 
                setup<pll::main>(Multiplier, PreDivider);

                // enable the pll
                enable<pll::main, true>();

                // wait until the pll is locked
                while (!is_locked<pll::main>()) {
                    // do nothing
                }
            }
            else {
                // Change to the requested source
                SYSCON->CLKSRCSEL = static_cast<uint32_t>(Source);
            }

            // setup the cpu clock divider
            SYSCON->CCLKSEL = Div & 0x1f | ((Multiplier > 0) << 8);

            // notify klib what freqency we are running
            klib::io::clock::set(Freq);
        }

        template <
            uint32_t Freq, uint16_t Multiplier, 
            pre_divider PreDivider
        >
        static void set_usb() {
            static_assert((Freq % 48'000'000) == 0, "Pll frequency needs to be 48, 96 or 144Mhz");
            static_assert(Freq <= 144'000'000, "Maximum USB pll frequency supported is 144Mhz");
            static_assert(Freq >= 48'000'000, "Minumim USB pll frequency supported is 48Mhz");

            // check if the crystal is stable and enabled
            if (!crystal::status()) {
                crystal::enable<true>();

                // wait until the crystal is stable
                while (!crystal::status()) {
                    // do nothing
                }
            }

            // change usb to sysclk if it is connected to the pll
            if (((SYSCON->USBCLKSEL >> 8) & 0b11) == 0x02) {
                // change to sysclk
                SYSCON->USBCLKSEL &= ~(0b11 << 8);
            }

            // setup the second pll 
            setup<pll::alternate>(Multiplier, PreDivider);

            // enable the pll
            enable<pll::alternate, true>();

            // wait until the pll is locked
            while (!is_locked<pll::alternate>()) {
                // do nothing
            }

            // setup the correct divider for the usb and 
            // change to the alternate pll
            SYSCON->USBCLKSEL = (Freq / 48'000'000) | (0x2 << 8);
        }
    };


    class trace {
    private:
        struct clk {
            // P2.06
            using port = lpc1788::io::periph::gpio2;

            // bit number in port
            constexpr static uint32_t number = 6;
        };

        struct d0 {
            // P2.05
            using port = lpc1788::io::periph::gpio2;

            // bit number in port
            constexpr static uint32_t number = 5;
        };

        struct d1 {
            // P2.04
            using port = lpc1788::io::periph::gpio2;

            // bit number in port
            constexpr static uint32_t number = 4;
        };

        struct d2 {
            // P2.03
            using port = lpc1788::io::periph::gpio2;

            // bit number in port
            constexpr static uint32_t number = 3;
        };

        struct d3 {
            // P2.02
            using port = lpc1788::io::periph::gpio2;

            // bit number in port
            constexpr static uint32_t number = 2;
        };

    public:
        template <bool Enable, uint8_t PortWidth = 4>
        static void enable() {
            if constexpr (Enable) {
                // validate the port width
                static_assert(PortWidth > 0 && PortWidth <= 4, "Invalid port width");

                // always set the clock
                io::detail::pins::set_peripheral<clk, io::detail::alternate::func_5>();

                if constexpr (PortWidth >= 1) {
                    io::detail::pins::set_peripheral<d0, io::detail::alternate::func_5>();
                }
                if constexpr (PortWidth >= 2) {
                    io::detail::pins::set_peripheral<d1, io::detail::alternate::func_5>();
                }
                if constexpr (PortWidth >= 4) {
                    io::detail::pins::set_peripheral<d2, io::detail::alternate::func_5>();
                    io::detail::pins::set_peripheral<d3, io::detail::alternate::func_5>();
                }
            }
            else {
                // clear all the pins from the trace peripheral
                io::detail::pins::set_peripheral<clk, io::detail::alternate::none>();
                io::detail::pins::set_peripheral<d0, io::detail::alternate::none>();
                io::detail::pins::set_peripheral<d1, io::detail::alternate::none>();
                io::detail::pins::set_peripheral<d2, io::detail::alternate::none>();
                io::detail::pins::set_peripheral<d3, io::detail::alternate::none>();
            }
        }

    };
}

#endif