#ifndef KLIB_NXP_LPC175X_SPI_HPP
#define KLIB_NXP_LPC175X_SPI_HPP

#include <cstdint>

#include <klib/klib.hpp>
#include <klib/multispan.hpp>
#include <klib/io/core_clock.hpp>
#include <klib/io/bus/spi.hpp>
#include <klib/math.hpp>

#include <io/power.hpp>

#include "clocks.hpp"
#include "port.hpp"

namespace klib::core::lpc175x::io {
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
            target::io::power_control::enable<Spi>();

            // enable the clocks on the spi peripheral
            clocks::set<Spi>();

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
                klib::io::clock::get() / static_cast<uint32_t>(Frequency)
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
         */
        template <
            bool Async = false,
            typename T = std::span<const uint8_t>,
            typename G = std::span<uint8_t>
        > 
        static void write_read(const T& tx, const G& rx) requires is_span_type_c<uint8_t, T> && is_span_type<uint8_t, G> {
            // get the amount of data to receive and transmit. To read more data
            // we still need to write. Otherwise we will stall and wait endlessly
            // on more data
            const uint32_t size = klib::max(tx.size(), rx.size());

            // write all the data to the fifo
            for (uint32_t i = 0; i < size; i++) {
                // write real or dummy data
                if ((!tx.empty()) || (i < tx.size())) {
                    Spi::port->DR = tx[i];
                }
                else {
                    Spi::port->DR = 0x00;
                }

                // wait until the write is done
                while (!is_done()) {
                    // do nothing
                }

                // read the data from the bus
                const uint32_t data = Spi::port->DR;

                // store the data if we can
                if ((!rx.empty()) && (i < rx.size())) {
                    rx[i] = data;
                }
            }
        }

        /**
         * @brief Write to the spi bus
         * 
         * @param data 
         */
        template <typename T = std::span<const uint8_t>>
        static void write(const T& data) requires is_span_type_c<uint8_t, T> {
            for (uint32_t i = 0; i < data.size(); i++) {
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