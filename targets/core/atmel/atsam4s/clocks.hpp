#ifndef KLIB_ATMEL_ATSAM4S_CLOCK_HPP
#define KLIB_ATMEL_ATSAM4S_CLOCK_HPP

#include <klib/klib.hpp>

#include <io/power.hpp>

namespace klib::core::atsam4s::io {
    class clocks {
    public:
        /**
         * @brief Available clock ids
         *
         */
        enum class id {
            udp = 7,
            pck0 = 8,
            pck1 = 9,
            pck2 = 10
        };

        /**
         * @brief Enable the clock on the peripheral
         *
         * @tparam Ids
         */
        template <id Ids>
        static void enable() {
            // disable the write protect
            power_control::write_protect<false>();

            // set the bit to enable the clock on the peripheral
            PMC->PMC_SCER = (0x1 << static_cast<uint8_t>(Ids));

            // enable the write protect again
            power_control::write_protect<true>();
        }

        /**
         * @brief Disable the clock on the peripheral
         *
         * @tparam Ids
         */
        template <id Ids>
        static void disable() {
            // disable the write protect
            power_control::write_protect<false>();

            // set the bit to enable the clock on the peripheral
            PMC->PMC_SCDR = (0x1 << static_cast<uint8_t>(Ids));

            // enable the write protect again
            power_control::write_protect<true>();
        }

        /**
         * @brief Get the clock status of a peripheral
         *
         * @tparam Ids
         * @return true
         * @return false
         */
        template <id Ids>
        static bool status() {
            // return the value of the clock id in the register (1 = enabled, 0 = disabled)
            return PMC->PMC_SCSR & (0x1 << static_cast<uint8_t>(Ids));
        }
    };
}

#endif
