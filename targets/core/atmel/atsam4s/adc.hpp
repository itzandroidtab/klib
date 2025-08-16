#ifndef KLIB_ATMEL_ATSAM4S_ADC_HPP
#define KLIB_ATMEL_ATSAM4S_ADC_HPP

#include <io/power.hpp>

namespace klib::core::atsam4s::io {
    template <typename Adc>
    class adc {
    public:
        // amount of bits in the ADC
        constexpr static uint32_t bits = 12;

        /**
         * @brief Available hardware trigger modes
         * 
         */
        enum class trigger_mode {
            disabled,
            external = 0x0,
            timer_0 = 0x1,
            timer_1 = 0x2,
            timer_2 = 0x3,
            pwm_event_0 = 0x4,
            pwm_event_1 = 0x5,
        };

        /**
         * @brief Start to sample on all the enabled channels
         * 
         */
        static void sample() {
            // start the conversion for all the enabled channels
            Adc::port->CR = 0x1 << 1;
        }

        /**
         * @brief Initialize the ADC
         * 
         * @tparam FreeRun 
         * @tparam Mode 
         * @tparam Sleep 
         * @tparam FastWakeup 
         * @tparam startup 
         * @tparam prescale 
         */
        template <bool FreeRun = false, trigger_mode Mode = trigger_mode::disabled, bool Sleep = false, bool FastWakeup = true, uint8_t startup = 0, uint8_t prescale = 1> 
        static void init() {
            // enable power to the adc
            target::io::power_control::enable<Adc>();

            // flag if we have a hardware trigger
            const bool hardware_trigger = (Mode != trigger_mode::disabled);

            // disable the write protection
            Adc::port->WPMR = (0x414443 << 8);

            // setup the adc with the provided configuration
            Adc::port->MR = (
                hardware_trigger | (hardware_trigger ? (static_cast<uint32_t>(Mode) << 1) : 0x00) | 
                (Sleep << 5) | (FastWakeup << 6) | (FreeRun << 7) | (startup << 16) | 
                (prescale << 8) | (2 << 28)
            );
        }
    };

    template <typename Adc, typename Pin>
    class adc_channel {
    public:
        // amount of bits in the ADC
        constexpr static uint32_t bits = adc<Adc>::bits;

        /**
         * @brief Init the adc channel for a specific pin
         * 
         */
        static void init() {
            // init the channel
            Adc::port->CHER |= (0x1 << Pin::adc_number);
        }

        /**
         * @brief Do a sample request.
         * 
         */
        static void sample() {
            // Note: we cannot sample a single channel. Sample all the enabled 
            // channels
            adc<Adc>::sample();
        }

        /**
         * @brief Returns if the adc is busy sampling. Only valid after starting 
         * a sample request.
         * 
         * @warning Undefined value is returned if calling this function before
         * starting a measurement
         * 
         * @return status 
         */
        static bool is_busy() {
            return !(Adc::port->ISR & (0x1 << Pin::adc_number));
        }

        /**
         * @brief Read a sampled result. If the async flag is set to false this will 
         * sample and wait until the conversion is complete
         * 
         * @tparam Async 
         * @tparam Override
         * @return uint32_t 
         */
        template <bool Async = true>
        static uint32_t get() {
            // check if we need to sample the data or if we only 
            // need to read the result
            if constexpr (!Async) {
                // start a measurement
                sample();

                // wait until the current channel is done
                while (is_busy()) {
                    // do nothing
                }
            }

            // return the result
            return Adc::port->CDR[Pin::adc_number] & 0xffff;
        }
    };
}

#endif