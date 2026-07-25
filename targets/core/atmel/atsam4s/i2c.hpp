#ifndef KLIB_ATMEL_ATSAM4S_I2C_HPP
#define KLIB_ATMEL_ATSAM4S_I2C_HPP

#include <klib/klib.hpp>
#include <klib/math.hpp>
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
         * @tparam std::span<const uint8_t> 
         * @param address 
         * @param data 
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
         * @brief Helper function to read data and set the stop bit
         * at the last byte. Returns false if an error occured
         *
         * @param data
         * @param stop
         * @return status
         */
        template <typename T>
        constexpr static bool read_helper_impl(const T& data, const bool stop) {
            // read all the data
            for (uint32_t i = 0; i < data.size(); i++) {
                // check if this is the last byte
                if ((i + 1) >= data.size() && stop) {
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

            return true;
        }

        /**
         * @brief Internal read implementation
         *
         * @note software doesnt check if stop is correctly send
         *
         * @tparam SendStop
         * @tparam RepeatedStart
         * @tparam std::span<const uint8_t>
         * @param address
         * @param data
         * @return true
         * @return false
         */
        template <bool SendStop = true, bool WaitForStatus = true, bool RepeatedStart = false, typename T = std::span<uint8_t>>
        constexpr static bool read_impl(const uint8_t address, const T& data, const uint32_t device_address = 0, const uint8_t device_address_size = 0) {
            // hardware does not writes with less than 1 byte
            if (!data.size()) {
                // return we could not transmit the data
                return false;
            }

            // check if we have a repeated start condition
            if constexpr (RepeatedStart) {
                // make sure the SendStop flag is true
                static_assert(SendStop, "Repeated starts require the SendStop flag to be enabled");

                // set the address we want to read from
                read_write_set_address<true>(address, device_address_size);

                // set the data to transmit
                I2c::port->IADR = device_address;
            }
            else {
                // set the address we want to read from
                read_write_set_address<true>(address);
            }

            // start the transaction
            I2c::port->CR = 0x1;

            // read all the data (sets the stop bit before the last byte)
            if (!read_helper_impl(data, SendStop)) {
                return false;
            }

            // check if we need to wait for the status
            if constexpr (!WaitForStatus) {
                return true;
            }

            // mask to check for
            constexpr uint32_t mask = (0x1 << 8) | 0x1;

            // wait until we are done with the transaction
            return (wait_for_status(mask) & mask) == 0x1;
        }

        template <typename T = std::span<const uint8_t>, typename G = std::span<uint8_t>>
        constexpr static bool write_read_repeated_impl(const uint8_t address, const T& tx, const G& rx) {
            // check if data is out of range
            if (tx.size() > 3) {
                return false;
            }

            // get the device address
            uint32_t addr = 0;
            for (uint8_t i = 0; i < tx.size(); i++) {
                addr = (addr << 8) | tx[i];
            }

            // read the data
            return read_impl<true, true, true>(address, rx, addr, tx.size());
        }

        template <bool RepeatedStart = true, typename T = std::span<const uint8_t>, typename G = std::span<uint8_t>>
        constexpr static bool write_read_impl(const uint8_t address, const T& tx, const G& rx) {
            if constexpr (RepeatedStart) {
                return write_read_repeated_impl(address, tx, rx);
            }
            else {
                // write with a stop, then read in a separate transaction
                if (!write_impl<true>(address, tx)) {
                    return false;
                }

                // read the data (this sends the stop)
                return read_impl<true, true, false>(address, rx);
            }
        }

        /**
         * @brief Internal length prefixed read implementation
         *
         * @details the length is not known until the prefix is read, so the
         * prefix and the payload are read in a single transaction
         *
         * @tparam PrefixBytes
         * @param address
         * @param buffer
         * @return status
         */
        template <uint8_t PrefixBytes = 1>
        constexpr static bool read_length_prefixed_impl(const uint8_t address, const std::span<uint8_t>& buffer) {
            static_assert(PrefixBytes <= 4, "Invalid prefix byte lenght. Up to 4 bytes is supported");

            // make sure the size fits inside the buffer
            if (buffer.size() < PrefixBytes) {
                return false;
            }


            // start the read into our internal buffer
            if (!read_impl<false, false>(address, {buffer.data(), PrefixBytes})) {
                return false;
            }
            
            // get the length
            uint32_t length = 0;
            
            // copy the data into our length variable
            for (uint32_t i = 0; i < PrefixBytes; i++) {
                length <<= 8;
                length |= buffer[i];
            }

            // clamp the amount to read to the buffer size
            const uint32_t want = klib::min<uint32_t>(length, static_cast<uint32_t>(buffer.size() - PrefixBytes));

            // check if we have something to read
            if (want) {
                if (!read_helper_impl(std::span<uint8_t>(buffer.data() + PrefixBytes, want), true)) {
                    return false;
                }
            }
            else {
                uint8_t dummy[1];

                // the device has no bytes. This should never
                // happen. In the state machine we need to set
                // a flag at the end to prevent it from getting
                // stuck. Do a 1 byte read and set the stop bit.
                // ignore the result
                read_helper_impl(std::span<uint8_t>(dummy, sizeof(dummy)), true);
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
         * @param address
         * @param data
         * @return state if read was successfull
         */
        constexpr static bool read(const uint8_t address, const std::span<uint8_t>& data) {
            return read_impl(address, data);
        }

        /**
         * @brief Read from a device on the i2c bus
         *
         * @param address
         * @param data
         * @return state if read was successfull
         */
        constexpr static bool read(const uint8_t address, const multispan<uint8_t>& data) {
            return read_impl(address, data);
        }

        /**
         * @brief Write to a device on the i2c bus
         *
         * @param address
         * @param data
         * @return state if write was successfull
         */
        constexpr static bool write(const uint8_t address, const std::span<const uint8_t>& data) {
            return write_impl(address, data);
        }

        /**
         * @brief Write to a device on the i2c bus
         *
         * @param address
         * @param data
         * @return state if write was successfull
         */
        constexpr static bool write(const uint8_t address, const multispan<const uint8_t>& data) {
            return write_impl(address, data);
        }

        /**
         * @brief Write up to 3 bytes and read data. By default this
         * uses the repeated start in the i2c protocol. Setting
         * RepeatedStart to false will do a write read cycle
         * 
         * @param address 
         * @param tx 
         * @param rx 
         * @return status
         */
        template <bool RepeatedStart = true>
        constexpr static bool write_read(const uint8_t address, const std::span<const uint8_t>& tx, const std::span<uint8_t>& rx) {
            return write_read_impl<RepeatedStart>(address, tx, rx);
        }

        /**
         * @brief Write up to 3 bytes and read data. By default this
         * uses the repeated start in the i2c protocol. Setting
         * RepeatedStart to false will do a write read cycle
         * 
         * @param address 
         * @param tx 
         * @param rx 
         * @return status
         */
        template <bool RepeatedStart = true>
        constexpr static bool write_read(const uint8_t address, const std::span<const uint8_t>& tx, const klib::multispan<uint8_t>& rx) {
            return write_read_impl<RepeatedStart>(address, tx, rx);
        }

        /**
         * @brief Read a variable amount of bytes based on the data
         * in the PrefixBytes first read. Length is clamped on the 
         * buffer size. Buffer stores the length and then full payload
         * 
         * @tparam PrefixBytes 
         * @param address 
         * @param buffer 
         * @return status
         */
        template <uint8_t PrefixBytes = 1>
        constexpr static bool read_length_prefixed(const uint8_t address, const std::span<uint8_t>& buffer) {
            return read_length_prefixed_impl<PrefixBytes>(address, buffer);
        }
    };
}

#endif