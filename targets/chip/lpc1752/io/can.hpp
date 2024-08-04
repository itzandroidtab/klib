#ifndef KLIB_LPC1752_CAN_HPP
#define KLIB_LPC1752_CAN_HPP

#include <cstdint>

#include <targets/core/nxp/lpc175x/can.hpp>
#include "pins.hpp"

namespace klib::lpc1752::io::periph::detail::can {
    enum class mode {
        rd,
        td,
    };

    template <typename Pin, mode Type, typename Periph>
    struct can {
        // pin of the peripheral
        using pin = Pin;

        // type of the pin
        constexpr static mode type = Type;

        // alternate function
        using periph = Periph;
    };
}

namespace klib::lpc1752::io::periph::lqfp_80 {
    template <
        typename Td = pins::package::lqfp_80::p38
    >
    struct can0 {
        // peripheral id (e.g can0, can1)
        constexpr static uint32_t id = 0;

        // peripheral clock bit position
        constexpr static uint32_t clock_id = 13;

        // interrupt id (including the arm vector table)
        constexpr static uint32_t interrupt_id = 41;

        // port to the CAN hardware
        static inline CAN1_Type *const port = CAN1;

        // amount of hardware buffers for transmitting data
        constexpr static uint32_t tx_buffers = 3;

        // pins allowed per output pin. Used for determining if a pin is valid on compile time
        using td_pins = std::tuple<
            detail::can::can<pins::package::lqfp_80::p38, detail::can::mode::td, core::lpc175x::io::detail::alternate::func_1>,
            detail::can::can<pins::package::lqfp_80::p44, detail::can::mode::td, core::lpc175x::io::detail::alternate::func_3>
        >;

        // pin configuration. Uses above mapping ()
        using rd = detail::can::can<pins::package::lqfp_80::p37, detail::can::mode::rd, core::lpc175x::io::detail::alternate::func_1>;
        using td = std::tuple_element<klib::io::peripheral::get_index<Td, td_pins>(), td_pins>::type;
    };
}

namespace klib::lpc1752::io {
    /**
     * @brief lpc1752 can driver
     *
     * @tparam Can
     * @tparam CanTxBuffers amount of tx hardware buffers used. Can help
     * when a protocol needs a specific order of messages
     */
    template <typename Can, uint32_t CanTxBuffers = Can::tx_buffers>
    using can = core::lpc175x::io::can<Can, CanTxBuffers>;
}

#endif