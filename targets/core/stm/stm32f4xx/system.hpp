#ifndef KLIB_STM_STM32F4XX_SYSTEM_HPP
#define KLIB_STM_STM32F4XX_SYSTEM_HPP

#include <klib/klib.hpp>
#include <klib/io/core_clock.hpp>

namespace klib::core::stm32f4xx::io::system {
    class flash {
    public:
        /**
         * @brief Set the amount of waitstates used for accessing flash
         *
         * @details 
         * 0 = for 0 to 30Mhz
         * 1 = for 30 to 60Mhz
         * 2 = for 60 to 90Mhz
         * 3 = for 90 to 120Mhz
         * 4 = for 120 to 150Mhz
         * 5 = for 150 to 180Mhz
         *
         * Note: for voltage ranges of 2.7V - 3.6V
         * 
         * More info can be found in the datasheet at page 81
         * 
         * @tparam WaitState 
         * @tparam EnablePrefetch 
         * @tparam EnableInstructionCache 
         * @tparam EnableDataCache 
         */
        template <uint8_t WaitState, bool EnablePrefetch, bool EnableInstructionCache, bool EnableDataCache>
        static void setup() {
            // check the wait state from the user
            static_assert(WaitState <= 15, "Max wait cycles is 15");

            // clear the wait state bits and set the new wait state
            FLASH->ACR = (
                (FLASH->ACR & (~0x70F)) | WaitState | (EnablePrefetch << 8) | 
                (EnableInstructionCache << 9) | (EnableDataCache << 10)
            );
        }
    };

    class crystal {
    public:
        /**
         * @brief Available clock sources for the system clock
         * 
         */
        enum class source {
            internal = 0,
            external = 1,
        };

        /**
         * @brief Returns if the requested crystal is enabled
         * 
         * @tparam Source 
         * @return true 
         * @return false 
         */
        template <source Source>
        static bool is_enabled() {
            // calculate the bit to check
            constexpr static uint32_t bit = (Source == source::internal) ? 0 : 16;

            // return the status of the bit
            return RCC->CR & (0x1 << bit);
        }

        /**
         * @brief Returns if the requested crystal is stable
         * 
         * @tparam Source 
         * @return true 
         * @return false 
         */
        template <source Source>
        static bool is_stable() {
            // calculate the bit to check
            constexpr static uint32_t bit = (Source == source::internal) ? 1 : 17;

            // return the status of the bit
            return RCC->CR & (0x1 << bit);
        }

        /**
         * @brief Enable or disable the requested crystal
         * 
         * @tparam Source 
         * @tparam Enable 
         */
        template <source Source, bool Enable>
        static void enable() {
            // calculate the bit to check
            constexpr static uint32_t bit = (Source == source::internal) ? 0 : 16;

            // enable the crystal by setting the requested source bit
            if constexpr (Enable) {
                RCC->CR |= (0x1 << bit);
            }
            else {
                RCC->CR &= ~(0x1 << bit);
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
            plli2s = 1,
            pllsai = 2
        };

        /**
         * @brief Available clock sources for the system clock
         * 
         */
        enum class sysclock_source {
            internal = 0,
            external = 1,
            pll = 2,
        };

        /**
         * @brief Get the current sysclock source
         * 
         * @return sysclock_source 
         */
        static sysclock_source get_sysclock_source() {
            return static_cast<sysclock_source>((RCC->CFGR >> 2) & 0x3);
        }

        /**
         * @brief Set the sysclock source
         * 
         * @note the clock needs to be stable before switching to it
         * 
         * @param source 
         */
        template <sysclock_source Source>
        static void set_sysclock_source() {
            // set the new source
            RCC->CFGR = (RCC->CFGR & ~(0x3)) | static_cast<uint32_t>(Source);
        }

        template <pll Pll, bool Enable>
        static void enable() {
            // calculate the bit to set or clear
            constexpr static uint32_t bit = 24 + (static_cast<uint32_t>(Pll) * 2);

            if constexpr (Enable) {
                RCC->CR |= (0x1 << bit);
            }
            else {
                RCC->CR &= ~(0x1 << bit);
            }
        }

        template <pll Pll>
        static bool is_locked() {
            // calculate the bit to check
            constexpr static uint32_t bit = 25 + (static_cast<uint32_t>(Pll) * 2);

            // return if the pll is locked by checking the ready flag
            return RCC->CR & (0x1 << bit);
        }

        /**
         * @brief Set the main pll to the specified frequency and source
         * 
         * @tparam Source 
         * @tparam Freq 
         * @tparam Multiplier 
         * @tparam PreDivider 
         * @tparam Div 
         * @tparam UsbSdRngDiv 
         * @tparam DsiDiv 
         */
        template <crystal::source Source, uint32_t Freq, uint16_t Multiplier, uint8_t PreDivider, uint8_t Div, uint8_t UsbSdRngDiv = 2, uint8_t DsiDiv = 2>
        static void set_main() {
            // check if the requested source is stable and enabled
            if (!crystal::is_enabled<Source>()) {
                crystal::enable<Source, true>();

                // wait until the crystal is stable
                while (!crystal::is_stable<Source>()) {
                    // do nothing
                }
            }

            // check if we need to use the pll or not
            if constexpr (Multiplier > 1) {
                // we need to enable the pll. Check everything is valid before enabling it
                static_assert(Multiplier >= 50 && Multiplier <= 432, "Invalid multiplier");
                static_assert(PreDivider >= 2 && PreDivider <= 63, "Invalid pre-divider");
                static_assert(Div == 2 || Div == 4 || Div == 6 || Div == 8, "Invalid divider");
                static_assert(UsbSdRngDiv >= 2 && UsbSdRngDiv <= 7, "Invalid USB/SD/RNG divider");
                static_assert(DsiDiv >= 2 && DsiDiv <= 15, "Invalid DSI divider");
    
                // calculate the vco input frequency
                constexpr static uint32_t vco_input_freq = Freq / PreDivider;
    
                static_assert(
                    vco_input_freq >= 1'000'000 && vco_input_freq <= 2'000'000,
                    "Invalid vco input frequency. Freq / PreDivider must be between 1 and 2mHz to limit pll jitter"
                );
    
                // calculate the vco frequency using the oscilator and multiplier
                //
                // Note: we use the formula (Freq * Multiplier) / PreDivider 
                // instead of (Freq / PreDivider) * Multiplier to avoid overflow 
                // when calculating the vco frequency
                constexpr static uint32_t partial_freq = (Freq * Multiplier) / PreDivider;
    
                static_assert(
                    (Multiplier == 1) || (partial_freq >= 100'000'000 && partial_freq <= 432'000'000), 
                    "Invalid vco output. PLL frequency needs to be between 100 and 432mhz"
                );
    
                // calculate the final frequency
                constexpr static uint32_t freq = (partial_freq / Div);
    
                // disconnect the main pll if it is connected.
                if (get_sysclock_source() == sysclock_source::pll) {
                    // get the current source of the pll
                    const auto source = static_cast<crystal::source>((RCC->PLLCFGR >> 22) & 0x1);
    
                    // check where we need to switch back to before 
                    // reconfiguring the main PLL
                    if (source == crystal::source::external) {
                        set_sysclock_source<sysclock_source::external>();
                    }
                    else {
                        set_sysclock_source<sysclock_source::internal>();
                    }
                }
    
                // disable the pll
                enable<pll::main, false>();

                // wait until the pll is disabled
                while (is_locked<pll::main>()) {
                    // do nothing
                }

                // set the new configuration for the pll
                RCC->PLLCFGR = (
                    (DsiDiv << 28) | (UsbSdRngDiv << 24) | (static_cast<uint32_t>(Source) << 22) | 
                    ((((Div / 2) - 1) << 16) | (Multiplier << 6) | PreDivider)
                );

                // enable the pll
                enable<pll::main, true>();

                // wait until the pll is locked
                while (!is_locked<pll::main>()) {
                    // do nothing
                }

                // switch to the pll
                set_sysclock_source<sysclock_source::pll>();
    
                // notify klib what freqency we are running
                klib::io::clock::set(freq);
            }
            else {
                // we don't need to enable the pll, just switch to the new source
                if constexpr (Source == crystal::source::external) {
                    set_sysclock_source<sysclock_source::external>();
                }
                else {
                    static_assert(
                        Freq == 16'000'000, "Internal oscillator frequency is fixed at 16Mhz"
                    );

                    set_sysclock_source<sysclock_source::internal>();
                }

                klib::io::clock::set(Freq);
            }

            // clear APB1 and APB2 dividers to 0 so they are not divided
            RCC->CFGR &= ~((0x7 << 10) | (0x7 << 13));
        }
    };
}

#endif