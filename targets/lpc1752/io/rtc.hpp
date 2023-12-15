#ifndef KLIB_LPC1752_RTC_HPP
#define KLIB_LPC1752_RTC_HPP

#include <cstdint>

#include <targets/core/nxp/lpc175x/rtc.hpp>

namespace klib::lpc1752::io::periph {
    struct rtc0 {
        // peripheral id (e.g rtc0, rtc1)
        constexpr static uint32_t id = 0;

        // peripheral clock bit position
        constexpr static uint32_t clock_id = 9;

        // interrupt id (including the arm vector table)
        constexpr static uint32_t interrupt_id = 33;

        // port to the RTC hardware
        static inline RTC_Type *const port = RTC;
    };
}

namespace klib::lpc1752::io {
    template <typename Rtc>
    using rtc = core::lpc175x::io::rtc<Rtc>;
}

#endif