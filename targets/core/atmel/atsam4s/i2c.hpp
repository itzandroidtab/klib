#ifndef KLIB_ATMEL_ATSAM4S_I2C_HPP
#define KLIB_ATMEL_ATSAM4S_I2C_HPP

#include <klib/klib.hpp>
#include <klib/multispan.hpp>
#include <klib/io/bus/i2c.hpp>
#include <klib/io/core_clock.hpp>

#include <io/power.hpp>
#include <io/port.hpp>

namespace klib::core::atsam4s::io {
    template <typename I2c>
    class i2c {
    protected:
        /**
         * @brief Set the address with read/write mode
         *
         * @tparam Read
         * @param address
         * @param device_address_size
         */
        template <bool Read>
        constexpr static void read_write_set_address(const uint8_t address, const uint8_t device_address_size = 0) {
            // set the address we want to write to
            I2c::port->MMR = ((address & 0x7f) << 16) | (Read << 12) | ((device_address_size & 0x3) << 8);
        }

        /**
         * @brief Wait until some bits are set in the status register
         *
         * @param bits
         * @return constexpr uint32_t
         */
        constexpr static uint32_t wait_for_status(const uint32_t bits) {
            // check for valid bits
            if (!bits) {
                return 0x00;
            }

            // loop until we have a match
            while (true) {
                // read the status (this can clear some bits)
                const uint32_t status = I2c::port->SR;

                // check if we have one of the bits set
                if (status & bits) {
                    return status;
                }
            }
        }

        /**
         * @brief Internal write implementation
         * 
         * @note software doesnt check if stop is correctly send
         *
         * @tparam SendStop
         * @param address
         * @param data
         * @param size
         * @return true
         * @return false
         */
        template <bool SendStop = true, typename T = std::span<const uint8_t>>
        constexpr static bool write_impl(const uint8_t address, const T& data) {
            // hardware does not support writes with less than 1 byte
            if (!data.size()) {
                // return we could not transmit the data
                return false;
            }

            // set the address we want to write to
            read_write_set_address<false>(address);

            // wait until we can write into the transmit holding register
            const uint32_t s = wait_for_status((0x1 << 8) | (0x1 << 2));

            // check if we have a nack
            if (s & (0x1 << 8)) {
                return false;
            }

            // transfer all the data
            for (uint32_t i = 0; i < data.size(); i++) {
                // write the data into the transfer hold register
                I2c::port->THR = data[i];

                // wait on either a empty transmit hold register or a nack
                const uint32_t status = wait_for_status((0x1 << 8) | (0x1 << 2));

                // check if we are done transmitting or
                // we have a nack
                if (status & (0x1 << 8)) {
                    // return a error
                    return false;
                }
            }

            // do not send the stop again if less or equal to 1 byte
            if (data.size() > 1 && SendStop) {
                // mark we are done transmitting (by setting the stop bit)
                I2c::port->CR = (0x1 << 1);
            }

            // mask to check for
            constexpr uint32_t mask = (0x1 << 8) | 0x1;

            // wait until we are done with the transaction
            return (wait_for_status(mask) & mask) == 0x1;
        }

        /**
         * @brief Internal read implementation
         * 
         * @note software doesnt check if stop is correctly send
         * 
         * @tparam SendStop 
         * @tparam std::span<const uint8_t> 
         * @param address 
         * @param data 
         * @return true 
         * @return false 
         */
        template <bool SendStop = true, typename T = std::span<const uint8_t>>
        constexpr static bool read_impl(const uint8_t address, const T& data) {
            // hardware does not writes with less than 1 byte
            if (!data.size()) {
                // return we could not transmit the data
                return false;
            }

            // set the address we want to read from
            read_write_set_address<true>(address);

            // start the transaction
            I2c::port->CR = 0x1;

            // read all the data
            for (uint32_t i = 0; i < data.size(); i++) {
                // check if this is the last byte
                if ((i + 1) >= data.size() && SendStop) {
                    // mark the next byte is the last
                    I2c::port->CR |= (0x1 << 1);
                }

                // wait until a nack or we have received data
                const uint32_t status = wait_for_status((0x1 << 8) | (0x1 << 1));

                // check if we have a nack or data
                if (status & (0x1 << 8)) {
                    // return a error
                    return false;
                }

                // read the data into the array
                data[i] = I2c::port->RHR & 0xff;
            }

            // mask to check for
            constexpr uint32_t mask = (0x1 << 8) | 0x1;

            // wait until we are done with the transaction
            return (wait_for_status(mask) & mask) == 0x1;
        }

    public:
        /**
         * @brief Init the i2c bus
         * 
         * @tparam Speed 
         */
        template <klib::io::i2c::speed Speed = klib::io::i2c::speed::fast>
        constexpr static void init() {
            // enable the clocks on the i2c peripheral
            target::io::power_control::enable<I2c>();

            // calculate the amount of ticks a clock pulse takes
            uint32_t ticks = klib::io::clock::get() / static_cast<uint32_t>(Speed);

            // setup the clock waveform
            I2c::port->CWGR = ((ticks / 2) << 8) | (ticks / 2);

            // enable master mode
            I2c::port->CR = (0x1 << 2) | (0x1 << 5);

            // configure the gpio pins
            target::io::detail::pins::set_peripheral<typename I2c::sda::pin, typename I2c::sda::periph>();
            target::io::detail::pins::set_peripheral<typename I2c::scl::pin, typename I2c::scl::periph>();
        }

        /**
         * @brief Read from a device on the i2c bus
         *
         * @tparam SendStop
         * @param address
         * @param data
         * @param size
         * @return state if read was successfull
         */
        template <bool SendStop = true>
        constexpr static bool read(const uint8_t address, const std::span<uint8_t>& data) {
            return read_impl<SendStop>(address, data);
        }

        /**
         * @brief Read from a device on the i2c bus
         *
         * @tparam SendStop
         * @param address
         * @param data
         * @param size
         * @return state if read was successfull
         */
        template <bool SendStop = true>
        constexpr static bool read(const uint8_t address, const multispan<uint8_t>& data) {
            return read_impl<SendStop>(address, data);
        }

        /**
         * @brief Write to a device on the i2c bus
         *
         * @tparam SendStop
         * @param address
         * @param data
         * @param size
         * @return state if read was successfull
         */
        template <bool SendStop = true>
        constexpr static bool write(const uint8_t address, const std::span<const uint8_t>& data) {
            return write_impl<SendStop>(address, data);
        }

        /**
         * @brief Write to a device on the i2c bus
         *
         * @tparam SendStop
         * @param address
         * @param data
         * @param size
         * @return state if read was successfull
         */
        template <bool SendStop = true>
        constexpr static bool write(const uint8_t address, const multispan<const uint8_t>& data) {
            return write_impl<SendStop>(address, data);
        }
    };
}

#endif