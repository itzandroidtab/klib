#ifndef KLIB_MAX32660_WATCHDOG_HPP
#define KLIB_MAX32660_WATCHDOG_HPP

#include <cstdint>

#include <max32660.hpp>

// peripheral namespace for perihperals not affected by chip packages
namespace klib::max32660::io::periph {
    struct wdt0 {
        // peripheral id (e.g wdt0, wdt1)
        constexpr static uint32_t id = 0;

        // peripheral interrupt position
        constexpr static uint32_t irq_id = 17;

        // port to the watchdog hardware
        static inline WDT0_Type *const port = WDT0;
    };
}

namespace klib::max32660::io {
    template <typename Wdt>
    class watchdog {
    public:
        /**
         * @brief Disable the watchdog timer
         * 
         */
        static void disable() {
            // clear the enable bit
            Wdt::port->CTRL = 0;
        }

        /**
         * @brief Feed the watchdog timer to prevent the watchdog from triggering
         * 
         */
        static void feed() {
            // reset the counter by writing the reset sequence twice
            Wdt::port->RST = 0xa5;
            Wdt::port->RST = 0x5a;
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
        template <bool Irq = true, uint8_t IrqPeriod = 0, bool Rst = false, uint8_t RstPeriod = 0>
        static void init() {
            // setup the parameters of the watchdog
            Wdt::port->CTRL = (Irq << 10) | (IrqPeriod & 0xF) | (Rst << 11) | ((RstPeriod & 0xF) << 4);

            // feed the watchdog before enabling the watchdog timer
            feed();

            // enable the watchdog
            Wdt::port->CTRL |= (1 << 8);
        }
    };
}

#endif