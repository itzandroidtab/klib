#ifndef KLIB_LPC1788_PORT_HPP
#define KLIB_LPC1788_PORT_HPP

#include <cstdint>
#include <cstddef>

#include <lpc1788.hpp>

namespace klib::lpc1788::io::detail::pins {
    // get the pin mask of a pin number
    template <typename Pin>
    constexpr uint32_t mask = 1U << Pin::number;
}

// global peripherals, not affected by chip package
namespace klib::lpc1788::io::periph {
    struct gpio0 {
        // peripheral id (e.g gpio0, gpio1)
        constexpr static uint32_t id = 0;

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

    struct gpio5 {
        // peripheral id (e.g gpio0, gpio1)
        constexpr static uint32_t id = 5;

        // port to the gpio hardware
        static inline GPIO0_Type *const port = GPIO5;
    };
}

#endif