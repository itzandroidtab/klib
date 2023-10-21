#ifndef KLIB_LPC55S66_PORT_HPP
#define KLIB_LPC55S66_PORT_HPP

#include <cstdint>
#include <cstddef>

#include <lpc55s66.hpp>

namespace klib::lpc55s66::io::detail::alternate {
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

    // alternate function 4
    struct func_4 {};

    // alternate function 5
    struct func_5 {};

    // alternate function 6
    struct func_6 {};

    // alternate function 7
    struct func_7 {};

    // alternate function 8
    struct func_8 {};

    // alternate function 9
    struct func_9 {};
}


// global peripherals, not affected by chip package
namespace klib::lpc55s66::io::periph {
    struct gpio0 {
        // peripheral id (e.g gpio0, gpio1)
        constexpr static uint32_t id = 0;

        // peripheral clock bit position
        constexpr static uint32_t clock_id = 14;

        // peripheral interrupt position
        // constexpr static uint32_t interrupt_id = 40;

        // port to the gpio hardware
        static inline GPIO_Type *const port = GPIO;
    };

    struct gpio1 {
        // peripheral id (e.g gpio0, gpio1)
        constexpr static uint32_t id = 1;

        // peripheral clock bit position
        constexpr static uint32_t clock_id = 15;

        // peripheral interrupt position
        // constexpr static uint32_t interrupt_id = 40;

        // port to the gpio hardware
        static inline GPIO_Type *const port = GPIO;
    };
    
    struct gpio2 {
        // peripheral id (e.g gpio0, gpio1)
        constexpr static uint32_t id = 2;

        // peripheral clock bit position
        constexpr static uint32_t clock_id = 16;

        // peripheral interrupt position
        // constexpr static uint32_t interrupt_id = 40;

        // port to the gpio hardware
        static inline GPIO_Type *const port = GPIO;
    };

    struct gpio3 {
        // peripheral id (e.g gpio0, gpio1)
        constexpr static uint32_t id = 3;

        // peripheral clock bit position
        constexpr static uint32_t clock_id = 17;

        // peripheral interrupt position
        // constexpr static uint32_t interrupt_id = 40;

        // port to the gpio hardware
        static inline GPIO_Type *const port = GPIO;
    };
}


#endif