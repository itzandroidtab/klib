#ifndef KLIB_MAX32660_WATCHDOG_HPP
#define KLIB_MAX32660_WATCHDOG_HPP

#include <cstdint>

#include "max32660.h"

namespace klib::max32660 {
    class watchdog {
    public:
        // interrupt number
        constexpr static uint32_t irq_id = 17;

        /**
         * @brief Disable the watchdog timer
         * 
         */
        static void disable() {
            // clear the enable bit
            WDT0->CTRL = 0;
        }

        /**
         * @brief Feed the watchdog timer to prevent the watchdog from triggering
         * 
         */
        static void feed() {
            // reset the counter by writing the reset sequence twice
            WDT0->RST = 0xa5;
            WDT0->RST = 0xa5;
        }

        /**
         * @brief Init the watchdog timer
         * 
         * @warning this clears the watchdog reset flag and watchdog interrupt flag
         * 
         * @tparam Irq enable/disable the watchdog timer interrupt
         * @tparam IrqPeriod interrupt period (refer to the datasheet for the timing)
         * @tparam Rst enable/disable if the cpu resets when the watchdog timer overflows 
         * @tparam RstPeriod reset period (refer to the datasheet for the timing)
         */
        template<bool Irq = true, uint8_t IrqPeriod = 0, bool Rst = false, uint8_t RstPeriod = 0>
        static void init() {
            // setup the parameters of the watchdog
            WDT0->CTRL = (Irq << 10) | (IrqPeriod & 0xF) | (Rst << 11) | ((RstPeriod & 0xF) << 4);

            // feed the watchdog before enabling the watchdog timer
            feed();

            // enable the watchdog
            WDT0->CTRL |= (1 << 8);
        }
    };
}

#endif