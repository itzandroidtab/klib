#ifndef KLIB_MAX32660_FLASH_HPP
#define KLIB_MAX32660_FLASH_HPP

#include <cstdint>
#include <klib/io/core_clock.hpp>

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
        static void init_clock() {
            // get the core clock
            const auto clock = klib::io::clock::get();

            // change the clock divider to have a 1Mhz FLC clock
            Flc::port->FLSH_CLKDIV = clock / 1'000'000;
        }

        static void unlock_flash() {
            // unlock the flash by setting the unlock value to 0x2
            Flc::port->FLSH_CN &= ~(0x7 << 28);
            Flc::port->FLSH_CN |= (0x2 << 28);
        }

        static void lock_flash() {
            // lock the flash again by clearing the unlock value and the erase code
            Flc::port->FLSH_CN &= ~((0x7 << 28) & (0xff << 8));
        }

    public:
    	static bool erase_page(const uint32_t address) {
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

        static bool write(const uint32_t address, const uint32_t value) {
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

    };
}

#endif
