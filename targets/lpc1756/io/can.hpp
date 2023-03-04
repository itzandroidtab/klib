#ifndef KLIB_LPC1756_CAN_HPP
#define KLIB_LPC1756_CAN_HPP

#include <cstdint>

#include <lpc1756.hpp>

#include "pins.hpp"
#include "port.hpp"

namespace klib::lpc1756::io::periph::detail::can {
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

namespace klib::lpc1756::io::periph::lqfp_80 {
    struct can0_0 {
        // peripheral id (e.g can0, can1)
        constexpr static uint32_t id = 0;

        // peripheral clock bit position
        constexpr static uint32_t clock_id = 13;

        // port to the CAN hardware
        static inline CAN1_Type *const port = CAN1;

        // P0.0 and P0.1
        using rd = detail::can::can<pins::package::lqfp_80::p37, detail::can::mode::rd, io::detail::alternate::func_1>;
        using td = detail::can::can<pins::package::lqfp_80::p38, detail::can::mode::td, io::detail::alternate::func_1>;
    };

    struct can0_1 {
        // peripheral id (e.g can0, can1)
        constexpr static uint32_t id = 0;

        // peripheral clock bit position
        constexpr static uint32_t clock_id = 13;

        // port to the CAN hardware
        static inline CAN1_Type *const port = CAN1;

        // P0.21 (not available on 80 pin package) and P0.22
        // TODO: decide/check if rd of the other configuration can be used
        using td = detail::can::can<pins::package::lqfp_80::p44, detail::can::mode::td, io::detail::alternate::func_3>;
    };

    struct can1 {
        // peripheral id (e.g can0, can1)
        constexpr static uint32_t id = 1;

        // peripheral clock bit position
        constexpr static uint32_t clock_id = 14;
        
        // port to the CAN hardware
        static inline CAN1_Type *const port = CAN2;

        // P2.7 and P2.8
        using rd = detail::can::can<pins::package::lqfp_80::p51, detail::can::mode::rd, io::detail::alternate::func_1>;
        using td = detail::can::can<pins::package::lqfp_80::p50, detail::can::mode::td, io::detail::alternate::func_1>;

        // 80 pin package does not have the following so only 1 configuration for can1
        // P0.4 and P0.5
    };
}

#endif