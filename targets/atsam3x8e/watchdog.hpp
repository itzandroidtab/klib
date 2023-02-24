#ifndef KLIB_ATSAM3X8E_WATCHDOG_HPP
#define KLIB_ATSAM3X8E_WATCHDOG_HPP

#include "ATSAM3X8E.h"

namespace klib::atsam3x8e {
    class watchdog {
    public:
        constexpr static uint32_t instance_id = 4;
        constexpr static uint32_t irqn = static_cast<uint32_t>(WDT_IRQn);

        static void disable() {
            // Disable the watchdog timer
            WDT->WDT_MR = WDT_MR_WDDIS_Msk;
        }

        static void feed() {
            // Restart the watchdog timer
            WDT->WDT_CR = WDT_CR_WDRSTT_Msk | WDT_CR_KEY_PASSWD;
        }

        template <unsigned int CounterStart = 0xFFF, unsigned int Delta = 0xFFF, bool Reset = true>
        static void init() {
            WDT->WDT_MR = WDT_MR_WDIDLEHLT_Msk // Stop watchdog if cpu is in idle mode
                        | WDT_MR_WDDBGHLT_Msk // Stop watchdog if cpu is in debug state
                        | WDT_MR_WDFIEN_Msk // Enable watchdog interrupts
                        | (WDT_MR_WDV_Msk & (CounterStart << WDT_MR_WDV_Pos)) // Set counter value (default takes 16 seconds to trigger Reset)
                        | (WDT_MR_WDD_Msk & (Delta << WDT_MR_WDD_Pos)) // Set the range the watchdog is allowed to restart
                                            // If restart is before this value it will throw an error
                        | static_cast<uint32_t>(Reset << 13); // If true this will Reset the cpu if underflow or an error occurred
        }

        static bool watchdog_error() {
            // Returns if an error occurred since last read
            return static_cast<bool>(WDT->WDT_SR & WDT_SR_WDERR_Msk);
        }

        static bool watchdog_underflow() {
            // Returns if an underflow occurred since last read
            return static_cast<bool>(WDT->WDT_SR & WDT_SR_WDUNF_Msk);
        }

    };
}

#endif