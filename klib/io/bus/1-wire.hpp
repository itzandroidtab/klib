#ifndef KLIB_1_WIRE_HPP
#define KLIB_1_WIRE_HPP

#include <cstdint>

#include <klib/klib.hpp>
#include <klib/multispan.hpp>
#include <klib/delay.hpp>

namespace klib::io {
    /**
     * @brief A basic 1 wire implementation
     * 
     * @tparam Pin 
     */
    template <typename Pin, typename OneShotTimer>
    class wire_1 {
    protected:
        /**
         * @brief Start a oneshot timer with a specific delay
         * 
         * @param microseconds 
         */
        static void delay_start(const uint32_t microseconds) {
            // start the oneshot timer
            OneShotTimer::init(nullptr, 1'000'000 / microseconds);
            OneShotTimer::enable();
        }

        /**
         * @brief Helper function to start a oneshot timer and
         * wait until it is done
         * 
         * @param microseconds 
         * @return true 
         * @return false 
         */
        static void delay(const uint32_t microseconds) {
            // start the oneshot timer
            delay_start(microseconds);

            // wait until the timer is done
            while (!OneShotTimer::done()) {
                // do nothing
            }
        }

        /**
         * @brief Read a single bit on the 1-wire bus
         * 
         * @return true 
         * @return false 
         */
        static bool read_bit() {
            // disable all interrupts
            target::disable_irq();

            // set the pin low
            Pin::template set<false>();

            // when writing we need to send a 1 - 15 microseconds low 
            // pulse on the bus. If the device wants to send a '1' it
            // does nothing. If the device wants to send a '0' the 
            // device pulls the data line to ground for 60 microseconds
            delay(3);

            // set the pin to high (in open drain mode it doesnt 
            // stops pulling the pin low)
            Pin::template set<true>();

            // start a one shot timer for 10 microseconds
            delay(10);

            // get the state
            const bool bit = Pin::get();

            // disable all interrupts
            target::enable_irq();

            // start a one shot timer for 53 microseconds
            delay(52);

            // return the bit we just read
            return bit;
        }

        /**
         * @brief Write a single bit on the 1-wire bus
         * 
         * @param bit 
         */
        static void write_bit(bool bit) {
            // disable all interrupts
            target::disable_irq();

            // set the pin low
            Pin::template set<false>();

            // start a oneshot timer. A '1' needs a 1 - 15 microseconds
            // low pulse. A '0' has needs a 60 microseconds low pulse.
            //
            // The device reads the the data line about 30 microseconds
            // after the falling edge. Due to inexpencive timers in the
            // device the timing can be off.
            delay(bit ? 10 : 60);

            // set the pin to high (in open drain mode it doesnt 
            // stops pulling the pin low)
            Pin::template set<true>();

            // disable all interrupts
            target::enable_irq();

            // start a one shot timer for 55 or 5 microseconds 
            // based on if the bit is set or not
            delay(bit ? 55 : 5);
        }

    public:
        /**
         * @brief Init the bus and count the amount of devices on the bus
         * 
         * @return uint32_t 
         */
        static uint32_t init() {
            // reset the bus.
            reset();

            // amount of devices we found
            uint32_t count = 0;

            // Wait for 
            for (uint32_t i = 0; i < 8; i++) {
                // start a timer to check if we have 
                delay(10);

                // check if the pin is low
                if (Pin::get()) {
                    continue;
                }

                // increment the amount of devices
                // we have found
                count++;

                // reset the index
                i = 0;

                // start a timer to wait for a maximum
                // of 60 microseconds
                delay_start(65);

                // wait until the pin is high again (or
                // until the timeout has reached)
                while (!Pin::get() && !OneShotTimer::done()) {
                    // wait and do nothing
                }
            }

            // return the count
            return count;
        }

        /**
         * @brief Reset the 1-wire bus
         * 
         */
        static void reset() {
            // disable all interrupts
            target::disable_irq();

            // set the pin low
            Pin::template set<false>();

            // start a one shot timer for at least to 480 
            // microseconds to reset all the devices on the
            // bus. Every device will pull the bus low for 
            // at least 60 microseconds
            delay(500);

            // set the pin to high (in open drain mode it doesnt 
            // stops pulling the pin low)
            Pin::template set<true>();

            // disable all interrupts
            target::enable_irq();
        }

        /**
         * @brief Read a array of bytes from the 1-wire bus
         * 
         * @param data 
         */
        static void read(std::span<uint8_t> data) {
            for (uint32_t i = 0; i < data.size(); i++) {
                uint8_t bits = 0;

                // read all the bits
                for (uint32_t j = 0; j < 8; j++) {
                    // read a bit
                    const bool b = read_bit();

                    // set the bits
                    bits <<= 1;
                    bits |= b;
                }

                data[i] = bits;
            }
        }

        /**
         * @brief Write a array of bytes from the 1-wire bus
         * 
         * @param data 
         */
        static void write(std::span<const uint8_t> data) {
            for (uint32_t i = 0; i < data.size(); i++) {
                const uint8_t bits = data[i];

                // write all the bits
                for (uint32_t j = 0; j < 8; j++) {
                    write_bit((bits >> j) & 0x1);
                }
            }
        }
    };
}

#endif