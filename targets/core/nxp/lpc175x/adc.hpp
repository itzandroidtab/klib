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

        // using for the array of callbacks
        using interrupt_callback = void (*)();

    protected:
        // callback when a adc interrupt
        static inline interrupt_callback adc_callback = nullptr;

    public:
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
         * @brief Returns if the adc has data to read.
         * 
         * @note This value is cleared after reading
         * 
         * @return status
         */
        static bool has_data() {
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
            // bit. 
            Adc::port->CR = (0x1 << 21);

            // disable the interrupts the adc might trigger
            Adc::port->INTEN = 0x00;

            // Configure the ADC FreeRun enables burst mode. 
            // That will allow conversions up to 200 kHz
            Adc::port->CR = (
                (Divider << 8) | (0x1 << 21) | (FreeRun << 16)
            );
        }

        /**
         * @brief Init the adc peripheral with a interrupt callback
         * 
         * @tparam FreeRun 
         * @tparam Divider 
         * @param callback 
         */
        template <bool FreeRun = false, uint8_t Divider = 0>
        static void init(const interrupt_callback& callback) {
            init<FreeRun, Divider>();

            if constexpr (!FreeRun) {
                // register the global done interrupt
                Adc::port->INTEN = 0x1 << 8;
            }
            else {
                // register all the individual channels
                Adc::port->INTEN = 0xff;
            }

            // set the callback
            adc_callback = callback;

            // register the interrupt
            target::irq::register_irq<Adc::interrupt_id>(interrupt_handler);

            // enable the interrupt
            target::enable_irq<Adc::interrupt_id>();
        }

    public:
        /**
         * @brief Interrupt handler for the ADC
         * 
         */
        static void interrupt_handler() {
            if (adc_callback) {
                adc_callback();
            }

            // clear interrupt by reading the data register(s)
            if (Adc::port->CR & (0x1 << 16)) {
                // burst/freerun: read each enabled channel DR to clear DONE flags
                const uint32_t channels = Adc::port->CR & 0xff;
                for (uint32_t i = 0; i < 8; i++) {
                    if (channels & (0x1 << i)) {
                        (void)Adc::port->DR[i];
                    }
                }
            }
            else {
                // single conversion: read GDR to clear global DONE interrupt
                (void)Adc::port->GDR;
            }
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

            // check if we are in burst/freerun mode. If we are we need 
            // to register our channel during init
            if (Adc::port->CR & (0x1 << 16)) {
                // init the channel
                Adc::port->CR |= (0x1 << Pin::analog_number);
            }
        }

        /**
         * @brief Do a sample request.
         * 
         */
        static void sample() {
            // check if we are in burst/freerun mode. If we are we
            // are not we need to clear any other channel before
            // starting a sample
            if (!(Adc::port->CR & (0x1 << 16))) {
                // init the channel and clear all the other channels
                Adc::port->CR = (Adc::port->CR & (~0xff)) | (0x1 << Pin::analog_number);
            }

            // Note: we sample all the enabled channels with this request
            adc<Adc>::sample();
        }

        /**
         * @brief Returns if the adc channel has data to read.
         * 
         * @note This value is cleared after reading
         * 
         * @return status
         */
        static bool has_data() {
            return (Adc::port->STAT & (0x1 << Pin::analog_number));
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
                while (!has_data()) {
                    // do nothing
                }
            }

            // return the result
            return (Adc::port->DR[Pin::analog_number] >> 4) & 0xfff;
        }
    };
}

#endif