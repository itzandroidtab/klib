#ifndef KLIB_MAX32660_SPI_HPP
#define KLIB_MAX32660_SPI_HPP

#include <cstdint>

#include <klib/core_clock.hpp>
#include <klib/io/bus/spi.hpp>
#include <klib/math.hpp>
#include <max32660.hpp>

// disable the DMA define (TODO: switch over to a generator that does not generate defines)
#undef DMA

#include "clocks.hpp"
#include "port.hpp"
#include "pins.hpp"

namespace klib::max32660::io::periph::detail::spi {
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

namespace klib::max32660::io::periph::wlp {
    struct spi0 {
        // peripheral id (e.g spi0, spi1)
        constexpr static uint32_t id = 0;

        // peripheral clock bit position
        constexpr static uint32_t clock_id = 6;

        // peripheral interrupt position
        constexpr static uint32_t irq_id = 32;

        // port to the spi peripheral
        static inline SPI17Y_Type *const port = SPI17Y;

        // configuration of the pins 
        using mosi = detail::spi::spi<pins::package::wlp::pd4, detail::spi::mode::mosi, io::detail::alternate::func_1>;
        using miso = detail::spi::spi<pins::package::wlp::pd3, detail::spi::mode::miso, io::detail::alternate::func_1>;
        using sck = detail::spi::spi<pins::package::wlp::pc3, detail::spi::mode::sck, io::detail::alternate::func_1>;
        using cs0 = detail::spi::spi<pins::package::wlp::pc4, detail::spi::mode::cs0, io::detail::alternate::func_1>;
    };

    struct spi1 {
        // peripheral id (e.g spi0, spi1)
        constexpr static uint32_t id = 1;

        // peripheral clock bit position
        constexpr static uint32_t clock_id = 7;

        // peripheral interrupt position
        constexpr static uint32_t irq_id = 33;

        // port to the spi peripheral
        static inline SPIMSS_Type *const port = SPIMSS;

        // configuration of the pins 
        using mosi = detail::spi::spi<pins::package::wlp::pc2, detail::spi::mode::mosi, io::detail::alternate::func_2>;
        using miso = detail::spi::spi<pins::package::wlp::pc1, detail::spi::mode::miso, io::detail::alternate::func_2>;
        using sck = detail::spi::spi<pins::package::wlp::pd1, detail::spi::mode::sck, io::detail::alternate::func_2>;
        using cs0 = detail::spi::spi<pins::package::wlp::pd2, detail::spi::mode::cs0, io::detail::alternate::func_2>;
    };
}

namespace klib::max32660::io::periph::tqfn_24 {
    struct spi0 {
        // peripheral id (e.g spi0, spi1)
        constexpr static uint32_t id = 0;

        // peripheral clock bit position
        constexpr static uint32_t clock_id = 6;

        // peripheral interrupt position
        constexpr static uint32_t irq_id = 32;

        // port to the spi peripheral
        static inline SPI17Y_Type *const port = SPI17Y;

        // configuration of the pins 
        using mosi = detail::spi::spi<pins::package::tqfn_24::p20, detail::spi::mode::mosi, io::detail::alternate::func_1>;
        using miso = detail::spi::spi<pins::package::tqfn_24::p21, detail::spi::mode::miso, io::detail::alternate::func_1>;
        using sck = detail::spi::spi<pins::package::tqfn_24::p19, detail::spi::mode::sck, io::detail::alternate::func_1>;
        using cs0 = detail::spi::spi<pins::package::tqfn_24::p18, detail::spi::mode::cs0, io::detail::alternate::func_1>;
    };

    struct spi1_0 {
        // peripheral id (e.g spi0, spi1)
        constexpr static uint32_t id = 1;

        // peripheral clock bit position
        constexpr static uint32_t clock_id = 7;

        // peripheral interrupt position
        constexpr static uint32_t irq_id = 33;

        // port to the spi peripheral
        static inline SPIMSS_Type *const port = SPIMSS;

        // configuration of the pins 
        using mosi = detail::spi::spi<pins::package::tqfn_24::p16, detail::spi::mode::mosi, io::detail::alternate::func_1>;
        using miso = detail::spi::spi<pins::package::tqfn_24::p17, detail::spi::mode::miso, io::detail::alternate::func_1>;
        using sck = detail::spi::spi<pins::package::tqfn_24::p5, detail::spi::mode::sck, io::detail::alternate::func_1>;
        using cs0 = detail::spi::spi<pins::package::tqfn_24::p4, detail::spi::mode::cs0, io::detail::alternate::func_1>;
    };

    struct spi1_1 {
        // peripheral id (e.g spi0, spi1)
        constexpr static uint32_t id = 1;

        // peripheral clock bit position
        constexpr static uint32_t clock_id = 7;

        // peripheral interrupt position
        constexpr static uint32_t irq_id = 33;

        // port to the spi peripheral
        static inline SPIMSS_Type *const port = SPIMSS;

        // configuration of the pins 
        using mosi = detail::spi::spi<pins::package::tqfn_24::p1, detail::spi::mode::mosi, io::detail::alternate::func_2>;
        using miso = detail::spi::spi<pins::package::tqfn_24::p2, detail::spi::mode::miso, io::detail::alternate::func_2>;
        using sck = detail::spi::spi<pins::package::tqfn_24::p24, detail::spi::mode::sck, io::detail::alternate::func_2>;
        using cs0 = detail::spi::spi<pins::package::tqfn_24::p23, detail::spi::mode::cs0, io::detail::alternate::func_2>;
    };
}

namespace klib::max32660::io {
    template <typename Spi, class Enabled = void>
    class spi {};

    template <typename Spi>
    class spi<Spi, typename std::enable_if<Spi::id == 0>::type> {
    protected:
        // fifo size
        constexpr static uint8_t fifo_size = 32;

        /**
         * @brief Struct used for passing clock calculations
         * 
         */
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
                Spi::port->DMA |= (0x1 << 6);
            }
            else {
                Spi::port->DMA &= ~(0x1 << 6);
            }

            if constexpr(RxEnabled) {
                Spi::port->DMA |= (0x1 << 22);
            }
            else {
                Spi::port->DMA &= ~(0x1 << 22);
            }
        }

        /**
         * @brief Write size amount of data to the fifo buffer
         * 
         * @param data 
         * @param size 
         * @param transmitted 
         * @return uint16_t 
         */
        static uint16_t write_fifo(const uint8_t *const data, const uint16_t size, const uint16_t transmitted) {
            // get the amount of space available in the fifo
            const uint8_t available_bytes = (fifo_size - (Spi::port->DMA >> 8) & 0x7f);

            // skip if we cannot write anything
            if (!available_bytes) {
                return 0;
            }

            // get the amount of data we can transfer at once
            const uint32_t t = klib::min(size - transmitted, static_cast<uint32_t>(available_bytes));

            // check if we need to write something
            if (!t) {
                return 0;
            }

            // check if we have a valid pointer to write
            if (data == nullptr) {
                // write all the data to the fifo
                for (uint32_t i = 0; i < t; i++) {
                    // write 1 byte at the time
                    Spi::port->DATA8[0] = 0x00;
                }
            }
            else {
                // write all the data to the fifo
                for (uint32_t i = 0; i < t; i++) {
                    // write 1 byte at the time
                    Spi::port->DATA8[0] = data[transmitted + i];
                }
            }

            return t;
        }

        /**
         * @brief Read size amount of data from the fifo buffer
         * 
         * @param data 
         * @param size 
         * @param received 
         * @return uint16_t 
         */
        static uint16_t read_fifo(uint8_t *const data, const uint16_t size, const uint16_t received) {
            // get the amount of space available in the fifo
            const uint8_t available_bytes = ((Spi::port->DMA >> 24) & 0x7f);

            // skip if we cannot write anything
            if (!available_bytes) {
                return 0;
            }

            // get the amount of data we can read at once
            const uint32_t t = klib::min(size - received, static_cast<uint32_t>(available_bytes));

            // check if we need to read something
            if (!t) {
                return 0;
            }

            // read from the fifo buffer
            for (uint32_t i = 0; i < t; i++) {
                // read 1 byte at the time
                data[received + i] = Spi::port->DATA8[0];
            }

            return t;
        }

        /**
         * @brief Returns if the transaction is done
         * 
         * @return true 
         * @return false 
         */
        static bool is_done() {
            return (Spi::port->INT_FL & (0x1 << 11));
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
            io::detail::pins::set_peripheral<typename Spi::sck::pin, typename Spi::sck::periph>();
            io::detail::pins::set_peripheral<typename Spi::mosi::pin, typename Spi::mosi::periph>();
            io::detail::pins::set_peripheral<typename Spi::miso::pin, typename Spi::miso::periph>();

            // only configure the chipselect when not using a different pin
            if constexpr (!ExternalCs) {
                io::detail::pins::set_peripheral<typename Spi::cs0::pin, typename Spi::cs0::periph>();
            }

            // set if we are using the cs pin, enable that the slave select stays asserted at 
            // the end of a transmission, enable master mode and enable the peripheral
            Spi::port->CTRL0 = (
                ((!ExternalCs) << 15) | 
                (0x1 << 8) |
                (0x1 << 1) | 
                (0x1)
            );

            // set the amount of bits per character and set the spi mode
            Spi::port->CTRL2 = (
                (convert_bits<Bits>() << 8) |
                (klib::io::spi::get_cpol<Mode>() << 1) |
                klib::io::spi::get_cpha<Mode>()
            );

            // set the clock
            clock_info clk = calculate_clock<Frequency>();

            Spi::port->CLK_CFG = (
                (clk.scale << 16) |
                (clk.high_clk << 8) | 
                clk.low_clk
            );

            // setup the chip select wait times
            Spi::port->SS_TIME = (0x1 << 16) | (0x1);
        }

        /**
         * @brief Write and read from the spi bus
         * 
         * @param tx 
         * @param rx 
         * @param size 
         */
        static void write_read(const uint8_t *const tx, uint8_t *const rx, const uint16_t size) {
            // clear the fifo buffers
            Spi::port->DMA |= (0x1 << 23) | (0x1 << 7);

            // clear the master data transmission done flag
            Spi::port->INT_FL = (0x1 << 11);

            // enable the transmit fifo port
            setup_fifo<true, true>();

            // set the amount of character we want to receive/write
            Spi::port->CTRL1 = size;

            // amount of data received/transmitted
            uint32_t transmitted = 0;
            uint32_t received = 0;

            bool started = false;

            while ((size - transmitted) > 0 || (size - received) > 0) {
                // write the tx data
                const uint16_t t = write_fifo(tx, size, transmitted);

                // read the rx data
                const uint16_t r = read_fifo(rx, size, received);

                // check if we did something
                if (!t && !r) {
                    continue;
                }

                // check if the transmission is already started
                if (!started) {
                    // start the transmission
                    Spi::port->CTRL0 |= (0x1 << 5);

                    started = true;
                }

                // add the amount of data we have transmitted
                transmitted += t;
                received += r;
            }

            // wait until we are done
            while (!is_done()) {
                // do nothing
            }
        }

        /**
         * @brief Write to the spi bus
         * 
         * @param data 
         * @param size 
         */
        static void write(const uint8_t *const data, const uint16_t size) {
            // clear the fifo buffers
            Spi::port->DMA |= (0x1 << 23) | (0x1 << 7);

            // clear the master data transmission done flag
            Spi::port->INT_FL = (0x1 << 11);

            // enable the transmit fifo port
            setup_fifo<true, false>();

            // set the amount of character we want to receive/write
            Spi::port->CTRL1 = size;

            uint32_t transmitted = 0;
            bool started = false;

            while ((size - transmitted) > 0) {
                // write the tx data
                const uint16_t t = write_fifo(data, size, transmitted);

                // check if we did something
                if (!t) {
                    continue;
                }

                // check if the transmission is already started
                if (!started) {
                    // start the transmission
                    Spi::port->CTRL0 |= (0x1 << 5);

                    started = true;
                }

                // add the amount of data we have transmitted
                transmitted += t;
            }

            // wait until we are done
            while (!is_done()) {
                // do nothing
            }
        }
    };

    template <typename Spi>
    class spi<Spi, typename std::enable_if<Spi::id == 1>::type> {
    protected:
        // port to the spi peripheral
        // static inline SPIMSS_Type *const port = io::periph::detail::_i2s::port<Spi::id>;

    public:
        static void init() {
            
        }

    };
}

#endif