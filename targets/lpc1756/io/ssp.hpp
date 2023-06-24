#ifndef KLIB_LPC1756_SSP_HPP
#define KLIB_LPC1756_SSP_HPP

#include <cstdint>

#include <lpc1756.hpp>

#include <klib/io/core_clock.hpp>
#include <klib/io/bus/spi.hpp>
#include <klib/io/peripheral.hpp>

#include "pins.hpp"
#include "power.hpp"
#include "clocks.hpp"

namespace klib::lpc1756::io::periph::detail::ssp {
    enum class mode {
        miso,
        mosi,
        sck,
        cs0
    };

    template <typename Pin, mode Type, typename Periph>
    struct ssp {
        // pin of the peripheral
        using pin = Pin;

        // type of the pin
        constexpr static mode type = Type;

        // alternate function
        using periph = Periph;
    };
}

namespace klib::lpc1756::io::periph::lqfp_80 {
    template <
        typename Mosi = pins::package::lqfp_80::p45, 
        typename Miso = pins::package::lqfp_80::p46,
        typename Sck = pins::package::lqfp_80::p47
    >
    struct ssp0 {
        // peripheral id (e.g ssp0, ssp1)
        constexpr static uint32_t id = 0;

        // interrupt id (including the arm vector table)
        constexpr static uint32_t interrupt_id = 30;

        // peripheral clock bit position
        constexpr static uint32_t clock_id = 21;

        // port to the SSP hardware
        static inline SSP0_Type *const port = SSP0;

        // pins allowed per output pin. Used for determining if a pin is valid on compile time
        using mosi_pins = std::tuple<
            detail::ssp::ssp<pins::package::lqfp_80::p45, detail::ssp::mode::mosi, io::detail::alternate::func_2>,
            detail::ssp::ssp<pins::package::lqfp_80::p30, detail::ssp::mode::mosi, io::detail::alternate::func_3>
        >;

        using miso_pins = std::tuple<
            detail::ssp::ssp<pins::package::lqfp_80::p46, detail::ssp::mode::miso, io::detail::alternate::func_2>,
            detail::ssp::ssp<pins::package::lqfp_80::p29, detail::ssp::mode::miso, io::detail::alternate::func_3>
        >;

        using sck_pins = std::tuple<
            detail::ssp::ssp<pins::package::lqfp_80::p47, detail::ssp::mode::sck, io::detail::alternate::func_2>,
            detail::ssp::ssp<pins::package::lqfp_80::p27, detail::ssp::mode::sck, io::detail::alternate::func_3>
        >;

        // pin configuration for the ssp. Uses above mapping
        using mosi = std::tuple_element<klib::io::peripheral::get_index<Mosi, mosi_pins>(), mosi_pins>::type;
        using miso = std::tuple_element<klib::io::peripheral::get_index<Miso, miso_pins>(), miso_pins>::type;
        using sck = std::tuple_element<klib::io::peripheral::get_index<Sck, sck_pins>(), sck_pins>::type;
        using cs0 = detail::ssp::ssp<pins::package::lqfp_80::p48, detail::ssp::mode::cs0, io::detail::alternate::func_2>;
    };

    template <
        typename Sck = pins::package::lqfp_80::p63
    >
    struct ssp1 {
        // peripheral id (e.g ssp0, ssp1)
        constexpr static uint32_t id = 1;

        // interrupt id (including the arm vector table)
        constexpr static uint32_t interrupt_id = 31;

        // peripheral clock bit position
        constexpr static uint32_t clock_id = 10;

        // port to the SSP hardware
        static inline SSP0_Type *const port = SSP1;

        // pins allowed per output pin. Used for determining if a pin is valid on compile time
        using sck_pins = std::tuple<
            detail::ssp::ssp<pins::package::lqfp_80::p63, detail::ssp::mode::sck, io::detail::alternate::func_2>,
            detail::ssp::ssp<pins::package::lqfp_80::p17, detail::ssp::mode::sck, io::detail::alternate::func_2>
        >;

        // pin configuration for the ssp. Uses above mapping
        using mosi = detail::ssp::ssp<pins::package::lqfp_80::p61, detail::ssp::mode::mosi, io::detail::alternate::func_2>;
        using miso = detail::ssp::ssp<pins::package::lqfp_80::p62, detail::ssp::mode::miso, io::detail::alternate::func_2>;
        using sck = std::tuple_element<klib::io::peripheral::get_index<Sck, sck_pins>(), sck_pins>::type;
        using cs0 = detail::ssp::ssp<pins::package::lqfp_80::p64, detail::ssp::mode::cs0, io::detail::alternate::func_2>;
    };
}

namespace klib::lpc1756::io {
    template <typename Ssp>
    class ssp {
    protected:
        template <klib::io::spi::bits Bits>
        constexpr static uint8_t convert_bits() {
            using bits = klib::io::spi::bits;

            // make sure we have an amount of bits we support
            static_assert(static_cast<uint32_t>(Bits) >= 8, "below 8 bits per transfer is not supported");
            static_assert(static_cast<uint32_t>(Bits) <= 16, "above 16 bits per transfer is not supported");

            // return the raw value
            return static_cast<uint8_t>(Bits) - 1;
        }

    public:
        template <
            klib::io::spi::mode Mode = klib::io::spi::mode::mode0,
            uint32_t Frequency = 1'000'000,
            klib::io::spi::bits Bits = klib::io::spi::bits::bit_8,
            bool ExternalCs = false
        >
        static void init() {
            // enable the power for the ssp
            power_control::enable<Ssp>();

            // setup the clock
            clocks::set<Ssp>();

            // configure the gpio pins
            io::detail::pins::set_peripheral<typename Ssp::sck::pin, typename Ssp::sck::periph>();
            io::detail::pins::set_peripheral<typename Ssp::mosi::pin, typename Ssp::mosi::periph>();
            io::detail::pins::set_peripheral<typename Ssp::miso::pin, typename Ssp::miso::periph>();

            // only configure the chipselect when not using a different pin
            if constexpr (!ExternalCs) {
                io::detail::pins::set_peripheral<typename Ssp::cs0::pin, typename Ssp::cs0::periph>();
            }
            
            // setup the clock for the ssp. Value should be between 2 - 254. 
            // Note: to get lower frequencies. the SCR in CR0 can be used to
            // decrease the clock even further. (PCLK / (CPSDVSR * [SCR + 1]))
            // As most of the time higher speeds is better, SCR is not used
            // at the moment
            Ssp::port->CPSR = static_cast<uint8_t>(
                klib::io::clock::get() / static_cast<uint32_t>(Frequency)
            );
            
            // set all the settings in the CR0 register
            Ssp::port->CR0 = (
                convert_bits<Bits>() | 
                (klib::io::spi::get_cpha<Mode>() << 6) |
                (klib::io::spi::get_cpol<Mode>() << 7)
            );

            // enable the ssp
            Ssp::port->CR1 = 0x1 << 1;
        }

        /**
         * @brief Write and read from the ssp bus
         * 
         * @param tx 
         * @param rx 
         * @param size 
         */
        static void write_read(const uint8_t *const tx, uint8_t *const rx, const uint16_t size) {
            // discard anything that is left in the fifo
            while (Ssp::port->SR & (0x1 << 2)) {
                // discard the old data and make place for 
                // the new data
                (void)Ssp::port->DR;
            }

            // check if we have a valid pointer to write
            if (tx == nullptr) {
                // write all the data to the fifo
                for (uint32_t i = 0; i < size; i++) {
                    // wait until we can write to the fifo
                    while (!Ssp::port->SR & (0x1 << 1)) {
                        // do nothing
                    }

                    // write dummy data
                    Ssp::port->DR = 0x00;

                    // wait until there is data in the receive fifo
                    while (!Ssp::port->SR & (0x1 << 2)) {
                        // do nothing
                    }

                    rx[i] = Ssp::port->DR;
                }
            }
            else {
                for (uint32_t i = 0; i < size; i++) {
                    // wait until we can write to the fifo
                    while (!Ssp::port->SR & (0x1 << 1)) {
                        // do nothing
                    }

                    // write the data
                    Ssp::port->DR = tx[i];

                    // wait until there is data in the receive fifo
                    while (!Ssp::port->SR & (0x1 << 2)) {
                        // do nothing
                    }

                    rx[i] = Ssp::port->DR;
                }
            }
        }

        /**
         * @brief Write to the ssp bus
         * 
         * @param data 
         * @param size 
         */
        template <bool Async>
        static void write(const uint8_t *const data, const uint16_t size) {
            for (uint32_t i = 0; i < size; i++) {
                // wait until we can write to the fifo
                while (!Ssp::port->SR & (0x1 << 1)) {
                    // do nothing
                }

                // write the data
                Ssp::port->DR = data[i];
            }

            // check if we should wait until completion
            if constexpr (!Async) {
                while (is_busy()) {
                    // wait until we are done
                }
            }
        }

        /**
         * @brief Returns if the current ssp port is busy. (transmitting/receiving data 
         * or TX fifo is not empty)
         * 
         * @return true 
         * @return false 
         */
        static bool is_busy() {
            return Ssp::port->SR & 0x1 << 4;
        }
    };
}

#endif