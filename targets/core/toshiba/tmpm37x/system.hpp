#ifndef KLIB_TOSHIBA_TMPM37X_SYSTEM_HPP
#define KLIB_TOSHIBA_TMPM37X_SYSTEM_HPP

#include <klib/klib.hpp>
#include <klib/io/core_clock.hpp>

namespace klib::core::tmpm37x::io::system {
    class clock {
    public:
        /**
         * @brief Available clock sources
         *
         */
        enum class source {
            internal = 0,
            external = 1,
        };

        /**
         * @brief Setup the pll to 8x the source frequency
         *
         * @tparam Frequency
         * @tparam Source
         */
        template <uint32_t Frequency, source Source>
        static void set_main() {
            // enable changing the oscillation frequency detector (ofd)
            OFD->CR1 = 0xf9;

            // disable the ofd while we change the PLL settings (dont
            // enable it again. No reason to reset when we have a out
            // of frequency oscillator)
            OFD->CR2 = 0x00;

            // clear the syscr
            CG->SYSCR = 0b01 << 16;

            // set the warmup time to 1ms. The PLL needs
            // 200 us before it is stabilized
            CG->OSCCR = (
                (CG->OSCCR & (klib::exp2(20) - 1)) |
                ((((1 * Frequency) / 1000) >> 4) << 20)
            );

            // check if we need to enable the external high speed
            // oscillator
            if constexpr (Source == source::external) {
                // enable the external high speed oscillator
                // as the source
                CG->OSCCR |= (0x1 << 8) | (0x1 << 19) | (0x1 << 18);
            }
            else {
                // enable the internal osscilator
                CG->OSCCR |= (0x1 << 16);

                // clear the bits for the external oscillator
                CG->OSCCR &= ~((0x1 << 8) | (0x1 << 19) | (0x1 << 18));
            }

            // start the warmup
            CG->OSCCR |= 0x1;

            // configure the PLL
            CG->PLLSEL |= (0b1010 << 12) | (0b0010011111 << 1);

            // wait until the warm up is completed
            while (CG->OSCCR & (0x1 << 1)) {
                // wait until warmup is complete
            }

            // check if we need to switch to the external oscillator
            if constexpr (Source == source::external) {
                // if we are switching to the external oscillator
                // disable the internal one
                CG->OSCCR |= (0x1 << 17);

                while (!(CG->OSCCR & (0x1 << 17))) {
                    // wait until the oscillator is switched
                }

                // turn off the internal oscillator
                CG->OSCCR &= ~(0x1 << 16);
            }

            // enable the PLL and start the warmup again
            CG->OSCCR |= (0x1 << 2) | 0x1;

            // wait until the warm up is completed
            while (CG->OSCCR & (0x1 << 1)) {
                // wait until warmup is complete
            }

            // switch to the pll
            CG->PLLSEL |= (0b1010 << 12) | (0b0010011111 << 1) | 0x1;

            // setup the peripheral clock with the FC and 0 prescale
            CG->SYSCR = (0b01 << 16) | (0x1 << 12);

            // set the correct frequency
            klib::io::clock::set(Frequency * 8);
        }
    };
}

#endif