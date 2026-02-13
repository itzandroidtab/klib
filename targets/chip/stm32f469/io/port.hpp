#ifndef KLIB_STM32F469_PORT_HPP
#define KLIB_STM32F469_PORT_HPP

#include <cstdint>
#include <cstddef>

#include <targets/core/stm/stm32f4xx/port.hpp>

namespace klib::stm32f469::io::detail::pins {
    // get the pin mask of a pin number
    template <typename Pin>
    constexpr uint32_t mask = 1U << Pin::number;
}

// global peripherals, not affected by chip package
namespace klib::stm32f469::io::periph {
    struct gpio0 {
        // peripheral id (e.g gpio0, gpio1)
        constexpr static uint32_t id = 0;

        // port to the gpio hardware
        static inline GPIOA_Type *const port = GPIOA;
    };

    struct gpio1 {
        // peripheral id (e.g gpio0, gpio1)
        constexpr static uint32_t id = 1;

        // port to the gpio hardware
        static inline GPIOB_Type *const port = GPIOB;
    };

    struct gpio2 {
        // peripheral id (e.g gpio0, gpio1)
        constexpr static uint32_t id = 2;

        // port to the gpio hardware
        static inline GPIOK_Type *const port = GPIOC;
    };

    struct gpio3 {
        // peripheral id (e.g gpio0, gpio1)
        constexpr static uint32_t id = 3;

        // port to the gpio hardware
        static inline GPIOK_Type *const port = GPIOD;
    };

    struct gpio4 {
        // peripheral id (e.g gpio0, gpio1)
        constexpr static uint32_t id = 4;

        // port to the gpio hardware
        static inline GPIOK_Type *const port = GPIOE;
    };

    struct gpio5 {
        // peripheral id (e.g gpio0, gpio1)
        constexpr static uint32_t id = 5;

        // port to the gpio hardware
        static inline GPIOK_Type *const port = GPIOF;
    };

    struct gpio6 {
        // peripheral id (e.g gpio0, gpio1)
        constexpr static uint32_t id = 6;

        // port to the gpio hardware
        static inline GPIOK_Type *const port = GPIOG;
    };

    struct gpio7 {
        // peripheral id (e.g gpio0, gpio1)
        constexpr static uint32_t id = 7;

        // port to the gpio hardware
        static inline GPIOK_Type *const port = GPIOH;
    };

    struct gpio8 {
        // peripheral id (e.g gpio0, gpio1)
        constexpr static uint32_t id = 8;

        // port to the gpio hardware
        static inline GPIOK_Type *const port = GPIOI;
    };

    struct gpio9 {
        // peripheral id (e.g gpio0, gpio1)
        constexpr static uint32_t id = 9;

        // port to the gpio hardware
        static inline GPIOK_Type *const port = GPIOJ;
    };

    struct gpio10 {
        // peripheral id (e.g gpio0, gpio1)
        constexpr static uint32_t id = 10;

        // port to the gpio hardware
        static inline GPIOK_Type *const port = GPIOK;
    };
}

namespace klib::stm32f469::io::detail::pins {
    using namespace klib::core::stm32f4xx::io::detail::pins;
}

namespace klib::stm32f469::io::detail::alternate {
    using namespace klib::core::stm32f4xx::io::detail::alternate;
}

namespace klib::stm32f469::io {
    template <typename Pin>
    using pin_in = klib::core::stm32f4xx::io::pin_in<Pin>;

    template <typename Pin>
    using pin_out = klib::core::stm32f4xx::io::pin_out<Pin>;
}

#endif
