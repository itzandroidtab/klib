#ifndef KLIB_NXP_ATSAM4S_SPI_HPP
#define KLIB_NXP_ATSAM4S_SPI_HPP

#include <klib/klib.hpp>
#include <klib/multispan.hpp>
#include <klib/math.hpp>

#include <klib/io/core_clock.hpp>
#include <klib/io/bus/spi.hpp>

#include <io/port.hpp>
#include <io/power.hpp>

namespace klib::core::atsam4s::io::detail::spi {
    // we define the modes here. This is so we can
    // use the chip select start as a index 
    enum class mode {
        miso = 0,
        mosi = 1,
        sck = 2,
        cs_start = 3,
    };
}

namespace klib::core::atsam4s::io {
    template <typename Spi>
    class spi {
    protected:
        /**
         * @brief Convert the bits per transmission to the format 
         * the hardware expects. This changes 8 -> 0 and 9 -> 1 
         * etc
         * 
         * @tparam Bits 
         * @return constexpr uint8_t 
         */
        template <klib::io::spi::bits Bits>
        constexpr static uint8_t convert_bits() {
            using bits = klib::io::spi::bits;

            // make sure we have an amount of bits we support
            static_assert(static_cast<uint32_t>(Bits) >= 8, "below 8 bits per transfer is not supported");
            static_assert(static_cast<uint32_t>(Bits) <= 16, "above 16 bits per transfer is not supported");

            // return the correct value for the register
            return static_cast<uint8_t>(Bits) - 8;
        }

        /**
         * @brief Get the chipselect for the current chip select
         * 
         * @return constexpr uint32_t 
         */
        constexpr static uint32_t get_chipselect_id() {   
            // make sure we have a valid configuration for the chip select
            static_assert(
                (static_cast<uint8_t>(Spi::cs0::type) >= static_cast<uint8_t>(detail::spi::mode::cs_start)), 
                "invalid chip select configuration"
            );

            // get the offset to the start of the chip select
            return static_cast<uint8_t>(Spi::cs0::type) - static_cast<uint8_t>(detail::spi::mode::cs_start);
        }

        /**
         * @brief Helper function that writes and reads from the spi bus
         * 
         * @tparam T 
         * @tparam G 
         * @param tx 
         * @param rx 
         */
        template <typename T, typename G>
        static void write_read_helper(const T& tx, const G& rx) {
            // get the amount of data to receive and transmit. To read more data
            // we still need to write. Otherwise we will stall and wait endlessly
            // on more data
            const uint32_t size = klib::max(tx.size(), rx.size());

            // write all the data to the fifo
            for (uint32_t i = 0; i < size; i++) {
                // write real or dummy data, set the chip select
                // and the flag if it is the last transfer
                const uint32_t value = (
                    (((!tx.empty()) || (i < tx.size())) ? tx[i] : 0x00) | 
                    (((i + 1) == size) << 24) | ((~(1 << get_chipselect_id()) & 0xf) << 16)
                );

                // wait until we can write 
                while (!(Spi::port->SR & (0x1 << 9))) {
                    // do nothing
                }

                // write the data
                Spi::port->TDR = value;

                // wait until the write and read is done
                while ((Spi::port->SR & 0x3) != 0x3) {
                    // do nothing
                }

                // read the data from the bus
                const uint32_t data = Spi::port->RDR;

                // store the data if we can
                if ((!rx.empty()) && (i < rx.size())) {
                    rx[i] = data;
                }
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
            // enable power to the spi peripheral
            target::io::power_control::enable<Spi>();

            // disable the write protection for the spi
            Spi::port->WPMR = (0x535049 << 8);

            // disable the spi and reset the spi
            Spi::port->CR = (0x1 << 1) | (0x1 << 7);

            // configure the gpio pins
            target::io::detail::pins::set_peripheral<typename Spi::sck::pin, typename Spi::sck::periph>();
            target::io::detail::pins::set_peripheral<typename Spi::mosi::pin, typename Spi::mosi::periph>();
            target::io::detail::pins::set_peripheral<typename Spi::miso::pin, typename Spi::miso::periph>();

            // only configure the chipselect when not using a external pin
            if constexpr (!ExternalCs) {
                target::io::detail::pins::set_peripheral<typename Spi::cs0::pin, typename Spi::cs0::periph>();
            }

            // set the spi to master mode, enable variable
            // chip select and disable mode fault detection
            Spi::port->MR = (
                0x1 | (0x1 << 1) | (0x1 << 4)
            );

            // get the divider for the current frequency. Minimum is 1 and max is 0xff
            const auto bit_rate = klib::max(
                1, klib::min(0xff, 
                    static_cast<uint32_t>(
                        klib::io::clock::get() / static_cast<uint32_t>(Frequency)
                    )
                )
            );

            // set the configuration for the correct chip select
            Spi::port->CSR[get_chipselect_id()] = (
                (klib::io::spi::get_cpol<Mode>()) | 
                ((!klib::io::spi::get_cpha<Mode>()) << 1) |
                (0x1 << 3) | (convert_bits<Bits>() << 4) |
                (bit_rate << 8)
            );

            // enable the spi peripheral
            Spi::port->CR = 0x1;
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
            return write_read_helper(data, std::span<uint8_t>{});
        }

        /**
         * @brief Write data to the spi bus
         * 
         * @param data 
         */
        static void write(const multispan<const uint8_t>& data) {
            return write_read_helper(data, std::span<uint8_t>{});
        }
    };
}

#endif