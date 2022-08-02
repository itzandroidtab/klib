#ifndef KLIB_MAX32660_SPI_HPP
#define KLIB_MAX32660_SPI_HPP

#include <cstdint>

#include <klib/core_clock.hpp>
#include <klib/io/bus/spi.hpp>
#include <klib/math.hpp>
#include <max32660.h>

// disable the DMA define (TODO: switch over to a generate that does not generate defines)
#undef DMA

#include "clocks.hpp"
#include "pio.hpp"

namespace klib::max32660::io::detail::spi {
    // default type when using the port
    template<uint32_t Spi>
    SPI17Y_Type *const port = nullptr;

    // port when using the pio0
    template<>
    SPI17Y_Type *const port<0> = SPI17Y;
}

namespace klib::max32660::io::detail::spi_i2s {
    // default type when using the port
    template<uint32_t Spi>
    SPIMSS_Type *const port = nullptr;

    // port when using the pio0
    template<>
    SPIMSS_Type *const port<1> = SPIMSS;
}

namespace klib::max32660::io {
    template <typename Spi, class Enabled = void>
    class spi {};

    template <typename Spi>
    class spi<Spi, typename std::enable_if<Spi::id == 0>::type> {
    protected:
        // fifo size
        constexpr static uint8_t fifo_size = 32;

        // port to the spi peripheral
        static inline SPI17Y_Type *const port = io::detail::spi::port<Spi::id>;

        struct clock_info {
            // high and low clock periods
            uint8_t high_clk;
            uint8_t low_clk;

            // scale of the spi clk
            uint8_t scale;
        };

        template <klib::io::spi::bits Bits>
        constexpr static uint8_t convert_bits() {
            using bits = klib::io::spi::bits;

            // make sure we have an amount of bits we support
            static_assert(Bits != bits::bit_1 && Bits != bits::bit_9, "1-bit and 9-bit per character is not supported");
            static_assert(static_cast<uint8_t>(Bits) < 17, "Max of 16 bits per character is supported");

            // handle the special case
            switch (Bits) {
                case bits::bit_16:
                    return 0;
                default:
                    return static_cast<uint8_t>(Bits);
            }
        }

        static uint8_t convert_bits(const uint8_t bits) {
            // convert the value back to the amount of bits
            if (bits == 0) {
                return 16;
            }
            else {
                return bits;
            }
        }

        template <uint32_t Frequency>
        static clock_info calculate_clock() {
            // get the amount of ticks for the spi frequency
            uint32_t ticks = klib::max(
                (klib::clock::get() / 2) / static_cast<uint32_t>(Frequency), 
                static_cast<uint32_t>(1)
            );

            // calculate timing based on 50% dutycycle
            uint32_t high_clk = ticks / 2;
            uint8_t scale = 0;

            // calculate the scale
            while (high_clk >= 16 && scale < 8) {
                high_clk /= 2;
                scale++;
            }

            if (scale == 8) {
                return {0xf, 0xf, 8};
            }

            // return the timings
            return {
                static_cast<uint8_t>(high_clk), 
                static_cast<uint8_t>(high_clk), 
                scale
            };
        }

        template <bool TxEnabled, bool RxEnabled>
        static void setup_fifo() {
            if constexpr(TxEnabled) {
                port->DMA |= (0x1 << 6);
            }
            else {
                port->DMA &= ~(0x1 << 6);
            }

            if constexpr(RxEnabled) {
                port->DMA |= (0x1 << 22);
            }
            else {
                port->DMA &= ~(0x1 << 22);
            }
        }

        /**
         * @brief Write data to the fifo register. Maximum of fifo_size of bytes is allowed
         * 
         * @param data 
         * @param size 
         */
        static void write_fifo(const uint8_t *const data, const uint8_t size) {
            for (uint32_t i = 0; i < size; i++) {
                // write to the fifo
                port->DATA8[0] = data[i];
            }
        }

    public:
        template <
            klib::io::spi::mode Mode = klib::io::spi::mode::mode0,
            uint32_t Frequency = 1'000'000,
            klib::io::spi::bits Bits = klib::io::spi::bits::bit_8,
            bool ExternalCs = false
        >
        static void init() {
            // enable the clocks on the spi peripheral
            clocks::enable<Spi>();

            // configure the gpio pins
            io::detail::set_peripheral<typename Spi::sck::pin, typename Spi::sck::periph>();
            io::detail::set_peripheral<typename Spi::mosi::pin, typename Spi::mosi::periph>();
            io::detail::set_peripheral<typename Spi::miso::pin, typename Spi::miso::periph>();

            // only configure the chipselect when not using a different pin
            if constexpr (!ExternalCs) {
                io::detail::set_peripheral<typename Spi::cs0::pin, typename Spi::cs0::periph>();
            }

            // set if we are using the cs pin, enable that the slave select stays asserted at 
            // the end of a transmission, enable master mode and enable the peripheral
            port->CTRL0 = (
                ((!ExternalCs) << 15) | 
                (0x1 << 8) |
                (0x1 << 1) | 
                (0x1)
            );

            // set the amount of bits per character and set the spi mode
            port->CTRL2 = (
                (convert_bits<Bits>() << 8) |
                (!klib::io::spi::get_cpol<Mode>() << 1) |
                !klib::io::spi::get_cpha<Mode>()
            );

            // set the clock
            clock_info clk = calculate_clock<Frequency>();

            port->CLK_CFG = (
                (clk.scale << 16) |
                (clk.high_clk << 8) | 
                clk.low_clk
            );

            // setup the chip select wait times
            port->SS_TIME = (0x1 << 16) | (0x1);
        }

        static void write(const uint8_t *const data, const uint16_t size) {
            // clear the fifo buffers
            port->DMA |= (0x1 << 23) | (0x1 << 7);

            // clear the master data transmission done flag
            port->INT_FL = (0x1 << 11);

            // enable the transmit fifo port
            setup_fifo<true, false>();

            // set the amount of character we want to receive/write
            port->CTRL1 = size;

            uint32_t transmitted = 0;
            bool started = false;

            while ((size - transmitted) > 0) {
                // get the amount of space available in the fifo
                const uint8_t available_bytes = (fifo_size - (port->DMA >> 8) & 0x7f);

                // skip if we cannot write anything
                if (!available_bytes) {
                    continue;
                }

                // get the amount of data we can transfer at once
                const uint32_t t = klib::min(size - transmitted, static_cast<uint32_t>(available_bytes));

                // write the data to the fifo
                write_fifo(&data[transmitted], t);

                // check if the transmission is already started
                if (!started) {
                    // start the transmission
                    port->CTRL0 |= (0x1 << 5);

                    started = true;
                }

                // add the amount of data we have transmitted
                transmitted += t;
            }

            // wait until we are done
            while (!(port->INT_FL & (0x1 << 11))) {
                // do nothing
            }
        }

    };

    template <typename Spi>
    class spi<Spi, typename std::enable_if<Spi::id == 1>::type> {
    protected:
        // port to the spi peripheral
        static inline SPIMSS_Type *const port = io::detail::spi_i2s::port<Spi::id>;

    public:
        static void init() {
            
        }

    };
}

#endif