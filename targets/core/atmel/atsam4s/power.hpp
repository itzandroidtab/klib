#ifndef KLIB_ATMEL_ATSAM4S_POWER_CONTROL_HPP
#define KLIB_ATMEL_ATSAM4S_POWER_CONTROL_HPP

#include <klib/klib.hpp>

namespace klib::core::atsam4s::io {
    class power_control {
    public:
        /**
         * @brief Enable the clock on the peripheral
         *
         * @tparam P
         */
        template <typename P>
        static void enable() {
            // disable the write protect
            write_protect<false>();

            // set the bit to enable the clock on the peripheral
            if constexpr (P::clock_id < 32) {
                PMC->PMC_PCER0 = (0x1 << P::clock_id);
            }
            else {
                PMC->PMC_PCER1 = (0x1 << (P::clock_id - 32));
            }

            // enable the write protect again
            write_protect<true>();
        }

        /**
         * @brief Disable the clock on the peripheral
         *
         * @tparam P
         */
        template <typename P>
        static void disable() {
            // disable the write protect
            write_protect<false>();

            // set the bit to enable the clock on the peripheral
            if constexpr (P::clock_id < 32) {
                PMC->PMC_PCDR0 = (0x1 << P::clock_id);
            }
            else {
                PMC->PMC_PCDR1 = (0x1 << (P::clock_id - 32));
            }

            // enable the write protect again
            write_protect<true>();
        }

        /**
         * @brief Get the clock status of a peripheral
         *
         * @tparam P
         * @return true
         * @return false
         */
        template <typename P>
        static bool status() {
            // return the value of the clock id in the register (1 = enabled, 0 = disabled)
            if constexpr ((P::clock_id * 2) < 32) {
                return PMC->PMC_PCSR0 & (0x1 << P::clock_id);
            }
            else {
                return PMC->PMC_PCSR1 & (0x1 << (P::clock_id - 32));
            }
        }

        /**
         * @brief Disables/enables the write protect on the PMC
         *
         */
        template <bool Enable>
        static void write_protect() {
            // disable the PMC write protection
            PMC->PMC_WPMR = (0x504D43 << 8) | Enable;
        }
    };
}

#endif
