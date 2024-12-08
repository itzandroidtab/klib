#ifndef KLIB_CYPRESS_MB9BF560L_SYSTEM_HPP
#define KLIB_CYPRESS_MB9BF560L_SYSTEM_HPP

#include <klib/klib.hpp>
#include <klib/math.hpp>
#include <klib/io/core_clock.hpp>

namespace klib::core::mb9bf560l::io::system {
    class flash {
    public:
        /**
         * @brief Available wait states
         * 
         * @details last entry is a safe setting for all conditions as
         * it is above the max cpu frequency
         * 
         */
        enum class waitstate {
            upto_72mhz = 0x00,
            upto_160mhz = 0x02,
            upto_200mhz = 0x03
        };

        /**
         * @brief Set the amount of waitstates used for accessing flash
         *
         * @details Additional wait states are only used when the flash 
         * accelerator is turned off.
         * 
         * @tparam WaitState
         * @tparam AdditionalWaitState
         */
        template <waitstate WaitState, uint8_t AdditionalWaitState = 0>
        static void setup() {
            // check the wait state from the user
            static_assert(
                AdditionalWaitState <= 7 && AdditionalWaitState != 2 && 
                AdditionalWaitState != 4 && AdditionalWaitState != 6, 
                "Not supported wait states"
            );

            // write the read wait cycle
            FLASH_IF->FRWTR = static_cast<uint8_t>(WaitState);

            // do a dummy read afterwards
            (void)FLASH_IF->FRWTR;

            // write the remainder to the flash sync down register
            FLASH_IF->FSYNDN = AdditionalWaitState;

            // do a dummy read afterwards
            (void)FLASH_IF->FSYNDN;
        }
    };

    class clock {
    protected:
        /**
         * @brief Calculate a divider for the given frequency 
         * to go below 80mhz
         * 
         * @tparam Freq 
         * @return constexpr uint8_t 
         */
        template <uint32_t Freq>
        consteval static uint8_t calculate_divider_80mhz() {
            // default value that should give a error
            uint32_t divider = 0x00;

            // get the divider needed to go below 80 mhz
            for (uint32_t i = 1; i < 4; i++) {
                if (Freq / klib::exp2(i) <= 80'000'000) {
                    divider = i;
                    break;
                }
            }

            return divider;
        }

    public:
        /**
         * @brief Available plls on the chip
         *
         */
        enum class pll {
            pll = 0,
            none,
        };

        /**
         * @brief Available clock sources
         *
         */
        enum class source {
            internal = 0,
            main = 1,
            internal_low_speed = 4,
            rtc = 5,
        };

        /**
         * @brief Set the main clock source.
         * 
         * @tparam Source 
         * @tparam Pll 
         * @tparam Freq 
         * @tparam Multiplier 
         * @tparam Div 
         */
        template <source Source, pll Pll, uint32_t Freq, uint16_t Multiplier = 1, uint32_t Div = 1>
        static void set_main() {
            // make sure the input is valid
            static_assert(Freq > 0, "Invalid frequency");

            // setup the wait time for the main and sub clock
            CRG->CSW_TMR = (0b101 << 4) | (0b1100);

            // check if we should enable the main oscillator
            if constexpr (Source == source::main) {
                // enable the main oscillator
                CRG->SCM_CTL |= (0x1 << 1);

                // wait until the main oscillator is locked
                while (!(CRG->SCM_STR & (0x1 << 1))) {
                    // do nothing
                }
            }
            else if constexpr (Source == source::rtc) {
                // enable the rtc oscillator
                CRG->SCM_CTL |= (0x1 << 3);

                // wait until the rtc oscillator is locked
                while (!(CRG->SCM_STR & (0x1 << 3))) {
                    // do nothing
                }
            }

            // check if we should enable the pll
            if constexpr (Pll == pll::pll) {
                static_assert(Multiplier > 0 && Multiplier < (0x3f + 1), "Invalid multiplier");
                static_assert(Div > 0 && Div <= (0xf + 1), "Invalid divider");
                static_assert(Source == source::main, "PLL is only supported for the main oscillator");

                // calculate the frequency we are gonna run at
                constexpr static uint32_t freq = (Freq * Multiplier) / Div;

                // configure the PLL
                CRG->PSW_TMR = 0x00;

                // set PPLK and clear PLLM
                CRG->PLL_CTL1 = ((Div - 1) << 4);

                // set plln
                CRG->PLL_CTL2 = (Multiplier - 1);

                // enable the pll
                CRG->SCM_CTL |= (0x1 << 4);

                // wait until the PLL is locked
                while (!(CRG->SCM_STR & (0x1 << 4))) {
                    // do nothing
                }

                // make sure the internal clocks are not to high. Add 
                // a prescaler if they are above 80 mhz (needed for 
                // apbc 0/2 and wdt)
                if constexpr (freq >= 80'000'000) {
                    // calculate the divider required to go below 80mhz
                    constexpr uint8_t divider = calculate_divider_80mhz<freq>();

                    // make sure we have a valid divider. We expect something other than 0
                    static_assert(divider > 0, "Invalid divider for internal clocks");

                    // set apb0 presacaler
                    CRG->APBC0_PSR = divider;

                    // set apb2 presacaler (+ enable it)
                    CRG->APBC2_PSR = (0x01 << 7) | divider;

                    // set sw watchdog presacaler
                    CRG->SWC_PSR = divider;
                }

                // set the clock speed
                klib::io::clock::set(freq);

                // switch to the main clock with pll
                CRG->SCM_CTL = (CRG->SCM_CTL & ~(0b111 << 5)) | (0x2 << 5);
            }
            else {
                // set the clock speed
                klib::io::clock::set(Freq);

                // switch to the provided source
                CRG->SCM_CTL |= (
                    (CRG->SCM_CTL & ~(0b111 << 5)) | (static_cast<uint8_t>(Source) << 5)
                );
            }

            // wait until the master clock has been switched
            while ((CRG->SCM_STR & (0b111 << 5)) != (CRG->SCM_CTL & (0b111 << 5))) {
                // do nothing
            }
        }

        /**
         * @brief Configure the USB clock
         *
         * @tparam Pll
         * @tparam OscillatorFreq
         */
        template <uint32_t OscillatorFreq, uint16_t Multiplier = 1, uint8_t PreDivider = 1, uint8_t Div = 1>
        static void set_usb() {
            // disable the usb clock outputs
            USBCLK->UCCR = 0x00;

            // wait until the clock outputs are disabled
            while (USBCLK->UCCR) {
                // do nothing
            }

            // check if we can use the oscillator straight away. If we 
            // have a 48mhz oscillator we can use it straight away
            if constexpr (OscillatorFreq != 48'000'000) {
                // make sure the input is valid
                static_assert(Multiplier >= 14 && Multiplier <= 100, "Invalid multiplier");
                static_assert(Div > 0 && Div <= (0xf + 1), "Invalid divider");
                static_assert(PreDivider > 0 && PreDivider <= 32, "Invalid pre-divider");

                // make sure we have a valid 48mhz output from the pll
                static_assert(
                    (48'000'000 == ((((OscillatorFreq / PreDivider) * Multiplier) / Div))),
                    "Invalid USB frequency calculation"
                );

                // switch to the pll output
                USBCLK->UCCR |= (0x1 << 1);

                // disable the pll and switch the input to the clkmo
                USBCLK->UPCR1 = 0x00;

                // disable the usb pll interrupt
                USBCLK->UPINT_ENR = 0x00;

                // setup the wait time for the pll to stabalize (to 1.02 ms)
                USBCLK->UPCR2 = 0x2;

                // setup the pll
                USBCLK->UPCR3 = (PreDivider - 1);
                USBCLK->UPCR4 = (Multiplier - 1);
                USBCLK->UPCR5 = (Div - 1);

                // enable the pll
                USBCLK->UPCR1 |= 0x1;

                // wait until the pll is stable
                while (!(USBCLK->UP_STR & 0x1)) {
                    // do nothing
                }
            }

            // enable the clock output
            USBCLK->UCCR |= (0x1 << 3) | 0x1;
        }
    };
}

#endif

