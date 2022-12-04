#ifndef KLIB_LPC1756_PORT_HPP
#define KLIB_LPC1756_PORT_HPP

#include <cstdint>

#include <lpc1756.hpp>

namespace klib::max32660::io::detail::alternate {
    // alternate functions for all the gpio
    // default function (view reference manual for 
    // default functions for every pin)
    struct none {};

    // alternate function 1
    struct func_1 {};

    // alternate function 2
    struct func_2 {};

    // alternate function 3
    struct func_3 {};
}

// global peripherals, not affected by chip package
namespace klib::lpc1756::io::periph {
    struct gpio0 {
        // peripheral id (e.g gpio0, gpio1)
        constexpr static uint32_t id = 0;

        // peripheral interrupt position
        // constexpr static uint32_t irq_id = 40;

        // port to the gpio hardware
        static inline GPIO0_Type *const port = GPIO0;
    };

    struct gpio1 {
        // peripheral id (e.g gpio0, gpio1)
        constexpr static uint32_t id = 1;

        // peripheral interrupt position
        // constexpr static uint32_t irq_id = 40;

        // port to the gpio hardware
        static inline GPIO0_Type *const port = GPIO1;
    };
    
    struct gpio2 {
        // peripheral id (e.g gpio0, gpio1)
        constexpr static uint32_t id = 2;

        // peripheral interrupt position
        // constexpr static uint32_t irq_id = 40;

        // port to the gpio hardware
        static inline GPIO0_Type *const port = GPIO2;
    };

    struct gpio3 {
        // peripheral id (e.g gpio0, gpio1)
        constexpr static uint32_t id = 3;

        // peripheral interrupt position
        // constexpr static uint32_t irq_id = 40;

        // port to the gpio hardware
        static inline GPIO0_Type *const port = GPIO3;
    };

    struct gpio4 {
        // peripheral id (e.g gpio0, gpio1)
        constexpr static uint32_t id = 4;

        // peripheral interrupt position
        // constexpr static uint32_t irq_id = 40;

        // port to the gpio hardware
        static inline GPIO0_Type *const port = GPIO4;
    };
}

namespace klib::lpc1756::io {

}

#endif