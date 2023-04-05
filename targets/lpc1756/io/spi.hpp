#ifndef KLIB_LPC1756_SPI_HPP
#define KLIB_LPC1756_SPI_HPP

#include <cstdint>

#include <lpc1756.hpp>

#include <klib/io/core_clock.hpp>
#include <klib/io/bus/spi.hpp>

#include "clocks.hpp"
#include "power.hpp"
#include "pins.hpp"

namespace klib::lpc1756::io::periph::detail::spi {
    enum class mode {
        miso,
        mosi,
        sck,
        cs0
    };

    template <typename Pin, mode Type, typename Periph>
    struct spi {
        // pin of the peripheral
        using pin = Pin;

        // type of the pin
        constexpr static mode type = Type;

        // alternate function
        using periph = Periph;
    };
}

namespace klib::lpc1756::io::periph::lqfp_80 {
    struct spi0 {
        // peripheral id (e.g spi0, spi1)
        constexpr static uint32_t id = 0;

        // interrupt id (including the arm vector table)
        constexpr static uint32_t interrupt_id = 29;

        // peripheral clock bit position
        constexpr static uint32_t clock_id = 8;

        // port to the SPI hardware
        static inline SPI_Type *const port = SPI;

        // configuration of the pins 
        using mosi = detail::spi::spi<pins::package::lqfp_80::p45, detail::spi::mode::mosi, io::detail::alternate::func_3>;
        using miso = detail::spi::spi<pins::package::lqfp_80::p46, detail::spi::mode::miso, io::detail::alternate::func_3>;
        using sck = detail::spi::spi<pins::package::lqfp_80::p47, detail::spi::mode::sck, io::detail::alternate::func_3>;
        using cs0 = detail::spi::spi<pins::package::lqfp_80::p48, detail::spi::mode::cs0, io::detail::alternate::func_3>;
    };
}


namespace klib::lpc1756::io {
    template <typename Spi>
    class spi {
    protected:
        template <klib::io::spi::bits Bits>
        constexpr static uint8_t convert_bits() {
            using bits = klib::io::spi::bits;

            // make sure we have an amount of bits we support
            static_assert(static_cast<uint32_t>(Bits) >= 8, "below 8 bits per transfer is not supported");
            static_assert(static_cast<uint32_t>(Bits) <= 16, "above 16 bits per transfer is not supported");

            // handle the special case
            switch (Bits) {
                case bits::bit_16:
                    return 0x00;
                default:
                    // use the default 8 bit mode
                    return static_cast<uint8_t>(Bits);
            }
        }

        /**
         * @brief Returns if the transaction is done
         * 
         * @return true 
         * @return false 
         */
        static bool is_done() {
            return (Spi::port->SR & (0x1 << 7));
        }

    public:
        template <
            klib::io::spi::mode Mode = klib::io::spi::mode::mode0,
            uint32_t Frequency = 1'000'000,
            klib::io::spi::bits Bits = klib::io::spi::bits::bit_8,
            bool ExternalCs = false
        >
        static void init() {
            // enable power to the spi peripheral
            power_control::enable<Spi>();

            // enable the clocks on the spi peripheral
            clocks::enable<Spi>();

            // configure the gpio pins
            io::detail::pins::set_peripheral<typename Spi::sck::pin, typename Spi::sck::periph>();
            io::detail::pins::set_peripheral<typename Spi::mosi::pin, typename Spi::mosi::periph>();
            io::detail::pins::set_peripheral<typename Spi::miso::pin, typename Spi::miso::periph>();

            // only configure the chipselect when not using a different pin
            if constexpr (!ExternalCs) {
                io::detail::pins::set_peripheral<typename Spi::cs0::pin, typename Spi::cs0::periph>();
            }

            // set the frequency (minimum should be 8. After testing it 
            // looks like 2 is also working. Value below 8 might not
            // work as the datasheet implies). The last bit does not stick.
            // (e.g 5 -> 4)
            Spi::port->CCR = static_cast<uint8_t>(
                klib::clock::get() / static_cast<uint32_t>(Frequency)
            );

            // set the control register. Set the amount of bits per transfer, set 
            // the cpha and cpol and enable master mode
            Spi::port->CR = (
                (0x1 << 1) | (convert_bits<Bits>() << 8) |
                (klib::io::spi::get_cpha<Mode>() << 3) |
                (klib::io::spi::get_cpol<Mode>() << 4) | 
                (0x1 << 5)
            );

            // clear any errors we might have in the status register by reading it
            (void) Spi::port->SR;
        }

        /**
         * @brief Write and read from the spi bus
         * 
         * @param tx 
         * @param rx 
         * @param size 
         */
        static void write_read(const uint8_t *const tx, uint8_t *const rx, const uint16_t size) {
            // check if we have a valid pointer to write
            if (tx == nullptr) {
                // write all the data to the fifo
                for (uint32_t i = 0; i < size; i++) {
                    // write dummy data
                    Spi::port->DR = 0x00;

                    // wait until the write is done
                    while (!is_done()) {
                        // do nothing
                    }

                    rx[i] = Spi::port->DR;
                }
            }
            else {
                for (uint32_t i = 0; i < size; i++) {
                    // write the data
                    Spi::port->DR = tx[i];

                    // wait until the write is done
                    while (!is_done()) {
                        // do nothing
                    }

                    rx[i] = Spi::port->DR;
                }
            }
        }

        /**
         * @brief Write to the spi bus
         * 
         * @param data 
         * @param size 
         */
        static void write(const uint8_t *const data, const uint16_t size) {
            for (uint32_t i = 0; i < size; i++) {
                // write the data
                Spi::port->DR = data[i];

                // wait until the write is done
                while (!is_done()) {
                    // do nothing
                }
            }
        }
    };
}

#endif