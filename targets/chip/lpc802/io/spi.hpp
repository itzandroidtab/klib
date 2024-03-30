#ifndef KLIB_LPC802_SPI_HPP
#define KLIB_LPC802_SPI_HPP

#include <klib/io/bus/spi.hpp>
#include <klib/multispan.hpp>

#include "pins.hpp"
#include "matrix.hpp"

namespace klib::lpc802::io::periph::detail::spi {
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

namespace klib::lpc802::io::periph {
    /**
     * @brief Struct stores information about the spi0 
     * hardware.
     * 
     */
    struct spi0 {
        // peripheral id (e.g spi0, spi1)
        constexpr static uint32_t id = 0;

        // peripheral clock bit position
        constexpr static uint32_t clock_id = 11;

        // interrupt id for the spi peripheral
        constexpr static uint32_t interrupt_id = 0;

        // port to the spi peripheral.
        static inline SPI0_Type *const port = SPI0;
    };
}

namespace klib::lpc802::io {
    template <typename Spi, klib::io::spi::bits Bits = klib::io::spi::bits::bit_8, bool ExternalCs = false>
    class spi {
    protected:
        // check if we are in 8 bit mode
        static_assert(Bits == klib::io::spi::bits::bit_8, "Only 8 bit mode is currently supported");

        /**
         * @brief Helper to write and read from the spi bus
         * 
         * @tparam T 
         * @tparam G 
         * @param tx 
         * @param rx 
         */
        template <typename T, typename G> 
        static void write_read_helper(const T& tx, const G& rx) {
            // get the size we should read/write
            const auto size = klib::min(tx.size(), rx.size());

            // check if we need to write anything
            if (!size) {
                // exit if we do not need to write anything
                return;
            }

            // write and set the control parameters (ignore the received data and set the length)
            Spi::port->TXDATCTL = (
                tx[0] | ((!ExternalCs) << 16) | 
                ((static_cast<uint32_t>(Bits) - 1) << 24)
            );

            // wait until we can receive
            while (!(Spi::port->STAT & 0x1)) {
                // wait until we can read the data
            }

            // read the data from the hardware
            rx[0] = Spi::port->RXDAT & 0xff;

            // write and read the data from the hardware
            for (uint32_t i = 1; i < size; i++) {
                // wait until we can transmit
                while (!(Spi::port->STAT & (0x1 << 1))) {
                    // wait until we are done writing the previous data
                }

                // check if we need to clear the chip select pin at the end of the transfer
                if constexpr (ExternalCs) {
                    // write and keep the previous control parameters
                    Spi::port->TXDAT = tx[i];
                }
                else {
                    // check if we are the last write in the data
                    if (i + 1 >= size) {
                        // set the end of transfer bit
                        Spi::port->TXDATCTL = tx[i] | (0x1 << 20);
                    }
                    else {
                        // write and keep the previous control parameters
                        Spi::port->TXDAT = tx[i];
                    }
                }

                // wait until we can receive
                while (!(Spi::port->STAT & 0x1)) {
                    // wait until we can read the data
                }

                // read the data from the hardware
                rx[i] = Spi::port->RXDAT & 0xff;
            }
        }

        /**
         * @brief Helper to write to the spi bus
         * 
         * @tparam T 
         * @param data 
         */
        template <typename T>
        static void write_helper(const T& data) {
            // check if we need to write anything
            if (!data.size()) {
                // exit if we do not need to write anything
                return;
            }

            // write and set the control parameters (ignore the received data and set the length)
            Spi::port->TXDATCTL = (
                data[0] | ((!ExternalCs) << 16) | 
                (0x1 << 22) | ((static_cast<uint32_t>(Bits) - 1) << 24)
            );

            // check if any rx data was left in the register
            if (Spi::port->STAT & 0x1) {
                // clear the RX data if we have any
                (void) Spi::port->RXDAT;
            }

            // write all the data to the hardware
            for (uint32_t i = 1; i < data.size(); i++) {
                // wait until we can transmit
                while (!(Spi::port->STAT & (0x1 << 1))) {
                    // wait until we are done writing the previous data
                }

                // check if we need to clear the chip select pin at the end of the transfer
                if constexpr (ExternalCs) {
                    // write and keep the previous control parameters
                    Spi::port->TXDAT = data[i];
                }
                else {
                    // check if we are the last write in the data
                    if (i + 1 >= data.size()) {
                        // set the end of transfer bit
                        Spi::port->TXDATCTL = data[i] | (0x1 << 20);
                    }
                    else {
                        // write and keep the previous control parameters
                        Spi::port->TXDAT = data[i];
                    }
                }
            }
        }

    public:
        template <
            typename Sck, typename Mosi, 
            typename Miso, typename Cs,
            klib::io::spi::mode Mode = klib::io::spi::mode::mode0,
            uint32_t Frequency = 1'000'000
        >
        constexpr static void init() {
            // enable the spi perihperal clock
            clocks::enable<Spi>();

            // configure the pins using the switch matrix
            using matrix = matrix<periph::matrix0>;

            // setup the matrix with the supplied pins
            matrix::setup<Sck, matrix::flex_matrix::spi0_sck>();
            matrix::setup<Mosi, matrix::flex_matrix::spi0_mosi>();
            matrix::setup<Miso, matrix::flex_matrix::spi0_miso>();

            // check if we should setup the cs
            if constexpr (!ExternalCs) {
                matrix::setup<Cs, matrix::flex_matrix::spi0_ssel0>();
            }

            // setup the spi clock
            // TODO: make the clock selectable (currently set to Main clock)
            SYSCON->SPI0CLKSEL = 1;

            // set the spi configuration (master mode and spi mode)
            Spi::port->CFG = (
                (0x1 << 2) | 
                (klib::io::spi::get_cpol<Mode>() << 5) |
                (klib::io::spi::get_cpha<Mode>() << 4)
            );

            // enable the spi
            Spi::port->CFG |= 0x1;
        }

        /**
         * @brief Write and read from the spi bus
         * 
         * @param tx 
         * @param rx 
         */
        static void write_read(const std::span<const uint8_t>& tx, const std::span<uint8_t>& rx) {
            return write_read_helper(tx, rx);
        }

        /**
         * @brief Write and read from the spi bus
         * 
         * @param tx 
         * @param rx 
         */
        static void write_read(const std::span<const uint8_t>& tx, const multispan<uint8_t>& rx) {
            return write_read_helper(tx, rx);
        }

        /**
         * @brief Write and read from the spi bus
         * 
         * @param tx 
         * @param rx 
         */
        static void write_read(const multispan<const uint8_t>& tx, const std::span<uint8_t>& rx) {
            return write_read_helper(tx, rx);
        }

        /**
         * @brief Write and read from the spi bus
         * 
         * @param tx 
         * @param rx 
         */
        static void write_read(const multispan<const uint8_t>& tx, const multispan<uint8_t>& rx) {
            return write_read_helper(tx, rx);
        }

        /**
         * @brief Write to the spi bus
         * 
         * @param data 
         */
        static void write(const std::span<const uint8_t>& data) {
            return write_helper(data);
        }

        /**
         * @brief Write data to the spi bus
         * 
         * @param data 
         */
        static void write(const multispan<const uint8_t>& data) {
            return write_helper(data);
        }
    };
}

#endif