#ifndef KLIB_MAX32660_FLASH_HPP
#define KLIB_MAX32660_FLASH_HPP

#include <cstdint>

#include <klib/io/core_clock.hpp>
#include <klib/multispan.hpp>

#include <max32660.hpp>

// peripheral namespace for perihperals not affected by chip packages
namespace klib::max32660::io::periph {
    struct flc0 {
        // peripheral id (e.g flc0, flc1)
        constexpr static uint32_t id = 0;

        // port to the flash controller
        static inline FLC_Type *const port = FLC;
    };
}

namespace klib::max32660::io {
    template <typename Flc>
    class flash {
    protected:
        /**
         * @brief Init the FLC clock to 1 mhz
         *
         */
        static void init_clock() {
            // get the core clock
            const auto clock = klib::io::clock::get();

            // change the clock divider to have a 1Mhz FLC clock
            Flc::port->FLSH_CLKDIV = clock / 1'000'000;
        }

        /**
         * @brief Unlock the flash
         *
         */
        static void unlock_flash() {
            // unlock the flash by setting the unlock value to 0x2
            Flc::port->FLSH_CN &= ~(0x7 << 28);
            Flc::port->FLSH_CN |= (0x2 << 28);
        }

        /**
         * @brief Lock the flash
         *
         */
        static void lock_flash() {
            // lock the flash again by clearing the unlock value and the erase code
            Flc::port->FLSH_CN &= ~((0x7 << 28) & (0xff << 8));
        }

        /**
         * @brief Writes 4 bytes of data to address
         *
         * @param address
         * @param value
         * @return true
         * @return false
         */
        static bool write_impl(const uint32_t address, const uint32_t value) {
            // wait until the flash is ready
            while (Flc::port->FLSH_CN & (0x1 << 24)) {
                // do nothing
            }

            // clear all interrupt flags
            Flc::port->FLSH_INTR = 0;

            // init the flc clock
            init_clock();

            // set the address
            Flc::port->FLSH_ADDR = address;

            // set the width of the write to 4 bytes
            Flc::port->FLSH_CN |= (0x1 << 4);

            // set the data in the data register
            Flc::port->FLSH_DATA[0] = value;

            // unlock the flash
            unlock_flash();

            // wait until the flash is ready
            while (Flc::port->FLSH_CN & (0x1 << 24)) {
                // do nothing
            }

            // start a write
            Flc::port->FLSH_CN |= 0x1;

            // wait until the write is done
            while (Flc::port->FLSH_CN & 0x1) {
                // do nothing
            }

            // lock the flash again
            lock_flash();

            // get the status
            const uint32_t status = Flc::port->FLSH_INTR;

            // return if the operation has completed
            return (status & 0x1) && !(status & (0x1 << 1));
        }

        /**
         * @brief Write helper that writes data in 4 byte chunks
         *
         * @tparam T
         * @param address
         * @param data
         * @return true
         * @return false
         */
        template <typename T>
        static bool write_helper(const uint32_t address, const T& data) {
            // write all the data we have
            for (uint32_t i = 0; i < data.size_bytes(); i += 4) {
                // write in 4 byte chunks
                const auto x = write_impl(address + i,
                    (data[i] << 24) | (data[i + 1] << 16) |
                    (data[i + 2] << 8) | data[i + 1]
                );

                // check the result
                if (!x) {
                    return false;
                }
            }

            return true;
        }

    public:
        /**
         * @brief Available erase modes
         *
         * sector size = 8192 bytes
         */
        enum class erase_mode {
            sector,
        };

        /**
         * @brief Erase using the mode
         *
         * @param mode
         * @param address
         * @return true
         * @return false
         */
    	static bool erase(const erase_mode mode, const uint32_t address) {
            // wait until the flash is ready
            while (Flc::port->FLSH_CN & (0x1 << 24)) {
                // do nothing
            }

            // clear all interrupt flags
            Flc::port->FLSH_INTR = 0;

            // init the flc clock
            init_clock();

            // set the address
            Flc::port->FLSH_ADDR = address;

            // set the page erase code
            Flc::port->FLSH_CN &= ~(0xff << 8);
            Flc::port->FLSH_CN |= (0x55 << 8);

            // unlock the flash
            unlock_flash();

            // wait until the flash is ready
            while (Flc::port->FLSH_CN & (0x1 << 24)) {
                // do nothing
            }

            // start a page erase
            Flc::port->FLSH_CN |= 0x1 << 2;

            // wait until the page erase is done
            while (Flc::port->FLSH_CN & (0x1 << 2)) {
                // do nothing
            }

            // lock the flash again
            lock_flash();

            // get the status
            const uint32_t status = Flc::port->FLSH_INTR;

            // return if the operation has completed
            return (status & 0x1) && !(status & (0x1 << 1));
        }

        /**
         * @brief Write to the address with data
         *
         * @warning can only write multiplies of 4 bytes at a time
         *
         * @tparam T
         * @param address
         * @param data
         * @return success
         */
        static bool write(const uint32_t address, const std::span<const uint8_t>& data) {
            return write_helper(address, data);
        }

        /**
         * @brief Write to the address with data
         *
         * @tparam T
         * @tparam G
         * @param address
         * @param data
         * @return true
         * @return false
         */
        static bool write(const uint32_t address, const klib::multispan<const uint8_t>& data) {
            return write_helper(address, data);
        }
    };
}

#endif
