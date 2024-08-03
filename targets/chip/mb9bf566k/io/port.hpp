#ifndef KLIB_MB9BF566K_PORT_HPP
#define KLIB_MB9BF566K_PORT_HPP

#include <cstdint>
#include <cstddef>

#include <klib/klib.hpp>

// global peripherals, not affected by chip package
namespace klib::mb9bf566k::io::periph {
    struct gpio0 {
        // peripheral id (e.g gpio0, gpio1)
        constexpr static uint32_t id = 0;

        // interrupt id (including the arm vector table)
        constexpr static uint32_t interrupt_id = 37;

        // port to the gpio hardware
        static inline GPIO_Type *const port = GPIO;
    };

    struct gpio1 {
        // peripheral id (e.g gpio0, gpio1)
        constexpr static uint32_t id = 1;

        // interrupt id (including the arm vector table)
        constexpr static uint32_t interrupt_id = 37;

        // port to the gpio hardware
        static inline GPIO_Type *const port = GPIO;
    };

    struct gpio2 {
        // peripheral id (e.g gpio0, gpio1)
        constexpr static uint32_t id = 2;

        // interrupt id (including the arm vector table)
        constexpr static uint32_t interrupt_id = 37;

        // port to the gpio hardware
        static inline GPIO_Type *const port = GPIO;
    };

    struct gpio3 {
        // peripheral id (e.g gpio0, gpio1)
        constexpr static uint32_t id = 3;

        // interrupt id (including the arm vector table)
        constexpr static uint32_t interrupt_id = 37;

        // port to the gpio hardware
        static inline GPIO_Type *const port = GPIO;
    };

    struct gpio4 {
        // peripheral id (e.g gpio0, gpio1)
        constexpr static uint32_t id = 4;

        // interrupt id (including the arm vector table)
        constexpr static uint32_t interrupt_id = 37;

        // port to the gpio hardware
        static inline GPIO_Type *const port = GPIO;
    };

    struct gpio5 {
        // peripheral id (e.g gpio0, gpio1)
        constexpr static uint32_t id = 5;

        // interrupt id (including the arm vector table)
        constexpr static uint32_t interrupt_id = 37;

        // port to the gpio hardware
        static inline GPIO_Type *const port = GPIO;
    };

    struct gpio6 {
        // peripheral id (e.g gpio0, gpio1)
        constexpr static uint32_t id = 6;

        // interrupt id (including the arm vector table)
        constexpr static uint32_t interrupt_id = 37;

        // port to the gpio hardware
        static inline GPIO_Type *const port = GPIO;
    };

    struct gpio8 {
        // peripheral id (e.g gpio0, gpio1)
        constexpr static uint32_t id = 8;

        // interrupt id (including the arm vector table)
        constexpr static uint32_t interrupt_id = 37;

        // port to the gpio hardware
        static inline GPIO_Type *const port = GPIO;
    };

    struct gpio14 {
        // peripheral id (e.g gpio0, gpio1)
        constexpr static uint32_t id = 14;

        // interrupt id (including the arm vector table)
        constexpr static uint32_t interrupt_id = 37;

        // port to the gpio hardware
        static inline GPIO_Type *const port = GPIO;
    };
}

#endif