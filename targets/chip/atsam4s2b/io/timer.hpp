#ifndef KLIB_ATSAM4S2B_TIMER_HPP
#define KLIB_ATSAM4S2B_TIMER_HPP

#include <cstdint>

#include <targets/core/atmel/atsam4s/timer.hpp>
#include "pins.hpp"

namespace klib::atsam4s2b::io::periph::detail::timer {
    // get the first few modes from the driver and add the
    // chip select pins for the current mcu
    enum class mode {
        tioa = 0,
        tiob = 1
    };

    template <typename Pin, mode Type, typename Periph, uint32_t Channel>
    struct timer {
        // pin of the peripheral
        using pin = Pin;

        // type of the pin
        constexpr static mode type = Type;

        // alternate function
        using periph = Periph;

        // channel used for the timer
        constexpr static uint32_t channel = Channel;
    };
}

namespace klib::atsam4s2b::io::periph::wlcsp_64 {
    struct tc0 {
        // peripheral id (e.g tc0, tc1)
        constexpr static uint32_t id = 0;

        // interrupt id (including the arm vector table)
        constexpr static uint32_t interrupt_id = (23 + 16);

        // peripheral clock bit position
        constexpr static uint32_t clock_id = 23;

        // port to the timer hardware
        static inline TC0_Type *const port = TC0;

        // available channels in the timer
        constexpr static uint32_t max_channels = 3;

        // pins available in the timer
        using tc_pins = std::tuple<
            detail::timer::timer<pins::package::wlcsp_64::pc2, detail::timer::mode::tioa, core::atsam4s::io::detail::alternate::func_2, 0>,
            detail::timer::timer<pins::package::wlcsp_64::ph6, detail::timer::mode::tioa, core::atsam4s::io::detail::alternate::func_2, 1>,
            detail::timer::timer<pins::package::wlcsp_64::pd5, detail::timer::mode::tioa, core::atsam4s::io::detail::alternate::func_2, 2>,
            detail::timer::timer<pins::package::wlcsp_64::pb1, detail::timer::mode::tiob, core::atsam4s::io::detail::alternate::func_2, 0>,
            detail::timer::timer<pins::package::wlcsp_64::pd7, detail::timer::mode::tiob, core::atsam4s::io::detail::alternate::func_2, 1>,
            detail::timer::timer<pins::package::wlcsp_64::pe4, detail::timer::mode::tiob, core::atsam4s::io::detail::alternate::func_2, 2>
        >;
    };
}

namespace klib::atsam4s2b::io {
    /**
     * @brief Basic timer. Uses interrupts to call a callback.
     *
     * @tparam Timer
     */
    template <typename Timer, uint32_t Channel, uint32_t Div = 2>
    using timer = core::atsam4s::io::timer<Timer, Channel, 2, Div>;

    /**
     * @brief Oneshot timer. Uses interrupt to call a callback once.
     *
     * @tparam Timer
     */
    template <typename Timer, uint32_t Channel, uint32_t Div = 2>
    using oneshot_timer = core::atsam4s::io::oneshot_timer<Timer, Channel, 2, Div>;

    /**
     * @brief Pin that uses a timer to toggle the output.
     *
     * @warning When disabling the timer the output of the gpio is not changed.
     */
    template <
        typename Pin, typename Timer, uint32_t Frequency,
        uint8_t Bits, uint32_t Div = 2
    >
    using pin_timer = core::atsam4s::io::pin_timer<Pin, Timer, Frequency, Bits, Div>;
}

#endif