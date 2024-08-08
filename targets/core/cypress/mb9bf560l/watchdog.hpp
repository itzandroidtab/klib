#ifndef KLIB_CYPRESS_MB9BF560L_WATCHDOG_HPP
#define KLIB_CYPRESS_MB9BF560L_WATCHDOG_HPP

#include <cstdint>

#include <klib/klib.hpp>

namespace klib::core::mb9bf560l::io {
    template <typename Wdt>
    class watchdog {
    protected:
        template <bool Control = false>
        static void unlock() {
            // unlock the watchdog registers
            Wdt::port->WDG_LCK = 0x1acce551;

            // if we need to enable the control
            // we need to do another write
            if constexpr (Control) {
                Wdt::port->WDG_LCK = 0xe5331aae;
            }
        }
    
    public:
        /**
         * @brief Disable the watchdog timer
         *
         */
        static void disable() {
            // unlock the registers + control register
            unlock<true>();

            // set the disable bit
            Wdt::port->WDG_CTL = 0x00;
        }

        /**
         * @brief Feed the watchdog timer to prevent the watchdog from triggering
         *
         */
        static void feed() {
            // write a arbitrary 8-bit value
            Wdt::port->WDG_ICL = 0xf0;

            // write the reversal value of the arbirary value
            Wdt::port->WDG_ICL = 0x0f;
        }

        /**
         * @brief Init the watchdog timer
         *
         * @tparam Irq enable/disable the watchdog timer interrupt
         * @tparam RstCPU enable/disable if a processor reset gets generated
         * @tparam Period reset period (refer to the datasheet for the timing)
         */
        template<bool Irq = true, bool RstCPU = false, uint32_t Period = 0xffff>
        static void init() {
            // make sure the input is valid
            static_assert(Period > 0, "Minimum period value is 1");
            static_assert((Irq && RstCPU) || (!RstCPU), "Reset does not work when Irq is not enabled");

            // unlock the registers
            unlock();

            // setup the parameters of the watchdog
            Wdt::port->WDG_LDR = Period;

            // unlock the control register
            unlock<true>();

            // enable or disable the interrupt bit
            Wdt::port->WDG_CTL = (RstCPU << 1) | Irq;

            // feed the watchdog
            feed();
        }
    };
}

#endif