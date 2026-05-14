#ifndef KLIB_NXP_LPC175X_ADC_HPP
#define KLIB_NXP_LPC175X_ADC_HPP

#include <klib/klib.hpp>
#include <klib/io/peripheral.hpp>

#include <io/power.hpp>
#include <io/port.hpp>
#include <io/clocks.hpp>

namespace klib::core::lpc175x::io {
    template <typename Adc>
    class adc {
    public:
        // amount of bits in the ADC
        constexpr static uint32_t bits = 12;

        /**
         * @brief Start sampling the input pins
         * 
         * @note conversions require 65 clock 
         * cycles
         */
        static void sample() {
            // check if burst mode is enabled. If it is we
            // do nothing here
            if (Adc::port->CR & (0x1 << 16)) {
                // do nothing here
                return;
            }

            // start a conversion now
            Adc::port->CR = (Adc::port->CR & (~(0b111 << 24))) | (0b001 << 24);
        }

        /**
         * @brief Returns if the adc is busy sampling. Only valid after starting 
         * a sample request.
         * 
         * @warning Undefined value is returned if calling this function before
         * starting a measurement
         * 
         * @note This value is cleared after reading
         * 
         * @return status
         */
        static bool is_busy() {
            return !(Adc::port->GDR & (0x1 << 31));
        }

        /**
         * @brief Init the adc peripheral
         * 
         * @tparam FreeRun 
         * @tparam Divider 
         */
        template <bool FreeRun = false, uint8_t Divider = 0>
        static void init() {
            // enable power to the adc
            target::io::power_control::enable<Adc>();

            // make the adc operational by setting the pdn 
            // bit. FreeRun enables burst mode. That will
            // allow conversions up to 200 kHz
            Adc::port->CR = (
                (Divider << 8) | (0x1 << 21) | (FreeRun << 16)
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
            static_assert(Pin::analog_number < 8, "Invalid analog pin number");
            using pin = std::tuple_element<klib::io::peripheral::get_index<Pin, typename Adc::pins>(), typename Adc::pins>::type;

            // switch the gpio to adc mode
            target::io::detail::pins::set_peripheral<typename pin::pin, typename pin::periph>();

            // init the channel
            Adc::port->CR |= (0x1 << Pin::analog_number);
        }

        /**
         * @brief Do a sample request.
         * 
         */
        static void sample() {
            // Note: we sample all the enabled channels with this request
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
            return !(Adc::port->STAT & (0x1 << Pin::analog_number));
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
            return (Adc::port->DR[Pin::analog_number] >> 4) & 0xfff;
        }
    };
}

#endif