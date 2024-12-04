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
         * @brief Helper that writes data to the spi port
         *
         * @tparam T
         * @param data
         */
        template <typename T>
        static void write_helper(const T& data) {
            for (uint32_t i = 0; i < data.size(); i++) {
                // write the data
                Spi::port->DR = data[i];

                // wait until the write is done
                while (!is_done()) {
                    // do nothing
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

    /**
     * @brief Basic spi slave implementation
     * 
     * @tparam Spi 
     */
    template <typename Spi>
    class spi_slave: private spi<Spi> {
    public:
        // using for the array of callbacks
        using interrupt_callback = void(*)();

    protected:
        // callbacks
        static inline interrupt_callback transmit_callback = nullptr;
        static inline interrupt_callback receive_callback = nullptr;

        /**
         * @brief Irq handler that will call the provided callbacks
         * 
         */
        static void irq_handler() {
            // read the interrupt status register
            const uint32_t interrupt_status = Spi::port->INT;

            // get the spi status
            const uint32_t status = Spi::port->SR;

            // clear the interrupt bits
            Spi::port->INT = interrupt_status;

            // check if we have the spi interrupt flag set
            if (!(interrupt_status & 0x1)) {
                // no flag set, exit
                return;
            }

            // check if we have a transfer complete interrupt
            if (!(status & (0x1 << 7))) {
                // transfer is not done. Do not continue
                return;
            }

            // check if we have a receive callback
            if (receive_callback != nullptr) {
                // call the callback
                receive_callback();
            }

            // check if we have a transmit callback
            if (transmit_callback != nullptr) {
                // call the callback
                transmit_callback();
            }
        }

    public:
        template <
            klib::io::spi::mode Mode = klib::io::spi::mode::mode0,
            klib::io::spi::bits Bits = klib::io::spi::bits::bit_8
        >
        static void init(const interrupt_callback& transmit = nullptr, const interrupt_callback& receive = nullptr) {
            // enable power to the spi peripheral
            target::io::power_control::enable<Spi>();

            // enable the clocks on the spi peripheral
            clocks::set<Spi>();

            // configure the gpio pins
            io::detail::pins::set_peripheral<typename Spi::sck::pin, typename Spi::sck::periph>();
            io::detail::pins::set_peripheral<typename Spi::mosi::pin, typename Spi::mosi::periph>();
            io::detail::pins::set_peripheral<typename Spi::miso::pin, typename Spi::miso::periph>();
            io::detail::pins::set_peripheral<typename Spi::cs0::pin, typename Spi::cs0::periph>();

            // set the callbacks
            transmit_callback = transmit;
            receive_callback = receive;

            // check if we need to use interrupts
            const bool needs_irq = ((transmit != nullptr) || (receive != nullptr));

            // set the control register. Set the amount of bits per transfer, set
            // the cpha and cpol and enable master mode
            Spi::port->CR = (
                (0x1 << 1) | (spi<Spi>::template convert_bits<Bits>() << 8) |
                (klib::io::spi::get_cpha<Mode>() << 3) |
                (klib::io::spi::get_cpol<Mode>() << 4) |
                (needs_irq << 7)
            );

            // clear any errors we might have in the status register by reading it
            (void) Spi::port->SR;

            // check if we need to enable the interrupts
            if (needs_irq) {
                // register our interrupt if we are using a callback
                target::irq::register_irq<Spi::interrupt_id>(irq_handler);

                // enable the interrupts for the spi peripheral
                target::enable_irq<Spi::interrupt_id>();
            }
            else {
                // disable the interrupts for the spi peripheral
                target::disable_irq<Spi::interrupt_id>();
            }
        }

        /**
         * @brief Write data to the spi bus
         * 
         * @warning. Can only be called if the buffer is empty
         *
         * @param data
         */
        static void write(const uint8_t data) {
            // write the data 
            Spi::port->DR = data;
        }

        /**
         * @brief Read data to the spi bus
         *
         * @warning Can only be called when data has been received
         * 
         * @return uint16_t 
         */
        static uint16_t read() {
            // read the data from the register
            return Spi::port->DR;
        }
    };
}

#endif