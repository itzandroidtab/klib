#ifndef KLIB_MAX32660_WATCHDOG_HPP
#define KLIB_MAX32660_WATCHDOG_HPP

#include <cstdint>

#include <max32660.hpp>

namespace klib::max32660::io::detail::watchdog {
    // default type when using the port
    template <uint32_t Wdt>
    WDT0_Type *const port = nullptr;

    // port when using the Wdt0
    template <>
    WDT0_Type *const port<0> = WDT0;
}

namespace klib::max32660::io {
    template <typename Wdt>
    class watchdog {
    protected:
        // port to the Flc peripheral
        static inline WDT0_Type *const port = detail::watchdog::port<Wdt::id>;

    public:
        /**
         * @brief Disable the watchdog timer
         * 
         */
        static void disable() {
            // clear the enable bit
            port->CTRL = 0;
        }

        /**
         * @brief Feed the watchdog timer to prevent the watchdog from triggering
         * 
         */
        static void feed() {
            // reset the counter by writing the reset sequence twice
            port->RST = 0xa5;
            port->RST = 0x5a;
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
            port->CTRL = (Irq << 10) | (IrqPeriod & 0xF) | (Rst << 11) | ((RstPeriod & 0xF) << 4);

            // feed the watchdog before enabling the watchdog timer
            feed();

            // enable the watchdog
            port->CTRL |= (1 << 8);
        }
    };
}

#endif