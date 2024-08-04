#ifndef KLIB_ATSAMXX_WATCHDOG_HPP
#define KLIB_ATSAMXX_WATCHDOG_HPP

#include <cstdint>

#include <klib/klib.hpp>

namespace klib::core::atsamxx::io {
    template <typename Wdt>
    class watchdog {
    public:
        /**
         * @brief Disable the watchdog timer
         *
         */
        static void disable() {
            // set the disable bit
            Wdt::port->MR = (0x1 << 15);
        }

        /**
         * @brief Feed the watchdog timer to prevent the watchdog from triggering
         *
         */
        static void feed() {
            // reset the counter by writing the reset sequence twice
            Wdt::port->CR = (0xA5 << 24) | 0x1;
        }

        /**
         * @brief Init the watchdog timer
         *
         * @warning this clears the watchdog reset flag and watchdog interrupt flag
         *
         * @tparam Irq enable/disable the watchdog timer interrupt
         * @tparam WdtRst enable/disable if a watchdog fault triggers a watchdog reset
         * @tparam RstCPU enable/disable if a processor reset gets generated
         * @tparam IdleHalt pauses the watchdog when the system is in idle mode
         * @tparam Period reset period (refer to the datasheet for the timing)
         */
        template<bool Irq = true, bool WdtRst = false, bool RstCPU = false, bool IdleHalt = false, uint16_t Period = 0>
        static void init() {
            // setup the parameters of the watchdog
            Wdt::port->MR = (
                (Period & 0xfff) | (Irq << 12) | (WdtRst << 13) | (RstCPU << 14) |
                ((Period & 0xfff) << 16) | (0x1 << 28) | (IdleHalt << 29)
            );
        }
    };
}

#endif