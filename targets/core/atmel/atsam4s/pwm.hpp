#ifndef KLIB_ATMEL_ATSAM4S_PWM_HPP
#define KLIB_ATMEL_ATSAM4S_PWM_HPP

#include <cstdint>

#include <klib/klib.hpp>
#include <klib/io/core_clock.hpp>
#include <klib/io/peripheral.hpp>
#include <klib/math.hpp>

#include <io/power.hpp>

#include "clocks.hpp"
#include "port.hpp"

namespace klib::core::atsam4s::io::detail::pwm {
    /**
     * @brief Available pwm clock sources
     * 
     */
    enum class pwm_clock {
        clka = 0,
        clkb = 1,
        mck,
    };

    /**
     * @brief Available prescalers for PWM
     * 
     */
    enum class pwm_prescaler {
        mck = 0,
        mck_div_2 = 1,
        mck_div_4 = 2,
        mck_div_8 = 3,
        mck_div_16 = 4,
        mck_div_32 = 5,
        mck_div_64 = 6,
        mck_div_128 = 7,
        mck_div_256 = 8,
        mck_div_512 = 9,
        mck_div_1024 = 10,
        clka = 11,
        clkb = 12,
    };

    /**
     * @brief Helper struct for calculating the pwm clock
     * 
     */
    struct pwm_clock_data {
        // the main prescaler
        pwm_prescaler prescaler;

        // additional divider
        uint8_t divider;
    };

    /**
     * @brief Check if the divider is a prescale divider
     * 
     * @param div 
     * @return true 
     * @return false 
     */
    constexpr static bool is_prescale_divider(uint32_t div) {
        // check if we have a divider that matches a prescaler. If 
        // we have a match we dont have to use a additional clock
        for (uint8_t i = 0; i < static_cast<uint32_t>(pwm_prescaler::clka); i++) {
            if (klib::exp2(i) == div) {
                return true;
            }
        }
        return false;
    }

    /**
     * @brief Calculate the best divider for the Frequency with the amount of bits
     * 
     * @tparam Frequency 
     * @tparam Bits 
     * @tparam MckOnly 
     * @return pwm_clock_data 
     */
    template <uint32_t Frequency, uint8_t Bits, bool MckOnly>
    static pwm_clock_data calculate_divider() {
        // get the current clock divided by the amount of bits we have
        const uint32_t clock = (klib::io::clock::get() / (klib::exp2(Bits) - 1));

        // check if the clock is too high
        if (Frequency >= clock) {
            // return we should use the mck
            return {pwm_prescaler::mck, 0};
        }

        // default values
        uint8_t divider = 1;
        pwm_prescaler prescaler = pwm_prescaler::mck_div_1024;

        // calculate the best clock divider
        for (uint8_t i = 0; i < static_cast<uint32_t>(pwm_prescaler::clka); i++) {
            // calculate the divider with the current prescale
            uint32_t div = ((clock / Frequency) / klib::exp2(i));

            // check if we only want to use the Mck
            if constexpr (MckOnly) {
                // we only want the Mck. Wait until we have 
                if (div > 1) {
                    continue;
                }
            }
            else {
                // check if we have a valid divider
                if (div > 0xff) {
                    // not a valid divider
                    continue;
                }
            }

            // we have a valid divider
            divider = div;
            prescaler = static_cast<pwm_prescaler>(i);

            // stop the loop
            break;
        }

        // check if the divider is a multiplier
        if (is_prescale_divider(divider)) {
            // return the divider as the prescale
            return {static_cast<pwm_prescaler>(klib::log2(divider)), 0x00};
        }

        // return the prescaler + divider
        return {prescaler, divider};
    }
}

namespace klib::core::atsam4s::io {
    /**
     * @brief Pin that uses a pwm to toggle the output.
     * 
     * @warning When disabling the pwm the output of the gpio is not changed.
     */
    template <
        typename Pin, typename Pwm, uint32_t Frequency, 
        uint8_t Bits = 12, detail::pwm::pwm_clock UseClock = detail::pwm::pwm_clock::mck,
        typename PPin = std::tuple_element<klib::io::peripheral::get_index<Pin, typename Pwm::pwm_pins>(), typename Pwm::pwm_pins>::type
    >
    class pwm {
    protected:
        // make sure we have a valid match register
        static_assert(static_cast<uint32_t>(PPin::channel) < Pwm::max_pwm_channels, "Pwm only has 4 channels");

        // make sure the frequency is valid
        static_assert(Frequency != 0, "Pwm frequency cannot be 0");

        // check if the amount of bits is valid
        static_assert(Bits >= 1 && Bits <= 16, "Amount of bits must be >= 1 && <= 16");

        // multiplier for the frequency
        constexpr static uint32_t multiplier = (klib::exp2(Bits) - 1);

    public:
        static void init() {
            // enable power to the pwm peripheral
            target::io::power_control::enable<Pwm>();

            // disable write protection for all groups of registers
            Pwm::port->WPCR = ((0x50574D << 8) | 0xfc);

            // calculate what divider we should use
            const auto clock = detail::pwm::calculate_divider<Frequency, Bits, UseClock == detail::pwm::pwm_clock::mck>();

            // check if we only want to set the mck
            if constexpr (UseClock == detail::pwm::pwm_clock::mck) {
                // set the prescale in the register
                Pwm::port->CH[PPin::channel].CMR = static_cast<uint32_t>(clock.prescaler);
            }
            else {
                // check if we realy need to use a special clock
                if (clock.divider <= 1) {
                    // no we dont. Use the prescale only
                    Pwm::port->CH[PPin::channel].CMR = static_cast<uint32_t>(clock.prescaler);
                }
                else {
                    // we need to use a additional clock. Configure the clock
                    // with the provided prescale and divider
                    const uint32_t clk = Pwm::port->CLK;

                    // get the index to the additional clock
                    const uint8_t index = static_cast<uint8_t>(UseClock);

                    // configure the clock with the provided value (clear previous value 
                    // and set the new value)
                    Pwm::port->CLK = (
                        (clk & (~(0xffff << (16 * index)))) | 
                        ((clock.divider << (16 * index)) | (static_cast<uint32_t>(clock.prescaler) << ((16 * index) + 8)))
                    );

                    // Change the prescaler to either clka or clkb
                    Pwm::port->CH[PPin::channel].CMR = (
                        static_cast<uint32_t>(detail::pwm::pwm_prescaler::clka) + 
                        static_cast<uint32_t>(UseClock)
                    );
                }                
            }

            // set the sync mode
            Pwm::port->SCM = (Pwm::port->SCM & (~(0x3 << 16))) | (0x1 << 16);

            // write to the cprd register when the channel is off
            Pwm::port->CH[PPin::channel].CPRD = multiplier;
        }

        /**
         * @brief Disable the pwm
         * 
         * @warning disables the whole channel
         */
        static void disable() {
            // disable the pwm
            Pwm::port->DIS = klib::exp2(PPin::channel);
        }

        /**
         * @brief Enable the pwm
         * 
         * @warning enables the whole channel
         */
        static void enable() {
            // enable the pwm
            Pwm::port->ENA = klib::exp2(PPin::channel);
        }

        /**
         * @brief Set the dutycycle of the Pwm pin
         * 
         * @tparam Dutycycle 
         */
        template <uint16_t Dutycycle>
        static void dutycycle() {
            // check if we need to disable the output or 
            // reenable it
            if constexpr (!Dutycycle) {
                // disable the output to get a low level
                set<false>();
            }
            else if (Pwm::port->CH[PPin::channel].CDTY == Pwm::port->CH[PPin::channel].CPRD) {
                // reenable the pin when the pin was disabled
                set<true>();
            }

            // set the new duty cycle in the shadow register
            // (duty cycle we write is inverted. Change it back)
            Pwm::port->CH[PPin::channel].CDTYUPD = (multiplier - (Dutycycle & multiplier));
        }

        /**
         * @brief Set the dutycycle of the pwm pin
         * 
         * @param dutycycle
         */
        static void dutycycle(uint16_t dutycycle) {
            // check if we need to disable the output or 
            // reenable it
            if (!dutycycle) {
                // disable the output to get a low level
                set<false>();
            }
            else if (Pwm::port->CH[PPin::channel].CDTY == Pwm::port->CH[PPin::channel].CPRD) {
                // reenable the pin when the pin was disabled
                set<true>();
            }

            // set the new duty cycle in the shadow register
            // (duty cycle we write is inverted. Change it back)
            Pwm::port->CH[PPin::channel].CDTYUPD = (multiplier - (dutycycle & multiplier));
        }

        /**
         * @brief Enable or disable output.
         * 
         * @tparam Value 
         */
        template <bool Value>
        static void set() {
            // clear or set the pin to the peripheral
            if constexpr (Value) {
                io::detail::pins::set_peripheral<Pin, typename PPin::periph>();
            }
            else {
                // change the mode to a output pin
                pin_out<Pin>::init();

                // set the pin to output a low
                pin_out<Pin>::template set<false>();
            }
        }

        /**
         * @brief Enable or disable output.
         * 
         * @param value 
         */
        static void set(bool value) {
            // clear or set the pin to the peripheral
            if (value) {
                io::detail::pins::set_peripheral<Pin, typename PPin::periph>();
            }
            else {
                // change the mode to a output pin
                pin_out<Pin>::init();

                // set the pin to output a low
                pin_out<Pin>::template set<false>();
            }
        }

    };
}

#endif