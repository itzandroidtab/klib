#ifndef KLIB_LPC1756_PORT_HPP
#define KLIB_LPC1756_PORT_HPP

#include <cstdint>
#include <cstddef>

#include <targets/core/nxp/lpc175x/port.hpp>
#include <targets/core/nxp/lpc175x/port_interrupt.hpp>

// global peripherals, not affected by chip package
namespace klib::lpc1756::io::periph {
    struct gpio0 {
        // peripheral id (e.g gpio0, gpio1)
        constexpr static uint32_t id = 0;

        // interrupt id (including the arm vector table)
        constexpr static uint32_t interrupt_id = 37;

        // port to the gpio hardware
        static inline GPIO0_Type *const port = GPIO0;
    };

    struct gpio1 {
        // peripheral id (e.g gpio0, gpio1)
        constexpr static uint32_t id = 1;

        // port to the gpio hardware
        static inline GPIO0_Type *const port = GPIO1;
    };
    
    struct gpio2 {
        // peripheral id (e.g gpio0, gpio1)
        constexpr static uint32_t id = 2;

        // interrupt id (including the arm vector table)
        constexpr static uint32_t interrupt_id = 37;

        // port to the gpio hardware
        static inline GPIO0_Type *const port = GPIO2;
    };

    struct gpio3 {
        // peripheral id (e.g gpio0, gpio1)
        constexpr static uint32_t id = 3;

        // port to the gpio hardware
        static inline GPIO0_Type *const port = GPIO3;
    };

    struct gpio4 {
        // peripheral id (e.g gpio0, gpio1)
        constexpr static uint32_t id = 4;

        // port to the gpio hardware
        static inline GPIO0_Type *const port = GPIO4;
    };
}

namespace klib::lpc1756::io {
    template <typename Pin>
    using pin_in = klib::core::lpc175x::io::pin_in<Pin>;

    template <typename Pin>
    using pin_out = klib::core::lpc175x::io::pin_out<Pin>;

    template <typename Pin>
    using pin_interrupt = klib::core::lpc175x::io::pin_interrupt<Pin>;
}

#endif