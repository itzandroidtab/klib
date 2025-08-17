#ifndef KLIB_ATSAM3X8E_PORT_HPP
#define KLIB_ATSAM3X8E_PORT_HPP

#include <cstdint>
#include <cstddef>

#include <targets/core/atmel/atsam3x/port.hpp>

// global peripherals, not affected by chip package
namespace klib::atsam3x8e::io::periph {
    struct gpio0 {
        // peripheral id (e.g gpio0, gpio1)
        constexpr static uint32_t id = 0;

        // peripheral clock bit position
        constexpr static uint32_t clock_id = 11;

        // interrupt id (including the arm vector table)
        constexpr static uint32_t interrupt_id = 27;

        // port to the gpio hardware
        static inline PIOA_Type *const port = PIOA;
    };

    struct gpio1 {
        // peripheral id (e.g gpio0, gpio1)
        constexpr static uint32_t id = 1;

        // peripheral clock bit position
        constexpr static uint32_t clock_id = 12;

        // interrupt id (including the arm vector table)
        constexpr static uint32_t interrupt_id = 28;

        // port to the gpio hardware
        static inline PIOB_Type *const port = PIOB;
    };

    struct gpio2 {
        // peripheral id (e.g gpio0, gpio1)
        constexpr static uint32_t id = 1;

        // peripheral clock bit position
        constexpr static uint32_t clock_id = 12;

        // interrupt id (including the arm vector table)
        constexpr static uint32_t interrupt_id = 28;

        // port to the gpio hardware
        static inline PIOC_Type *const port = PIOC;
    };    

    struct gpio3 {
        // peripheral id (e.g gpio0, gpio1)
        constexpr static uint32_t id = 1;

        // peripheral clock bit position
        constexpr static uint32_t clock_id = 12;

        // interrupt id (including the arm vector table)
        constexpr static uint32_t interrupt_id = 28;

        // port to the gpio hardware
        static inline PIOD_Type *const port = PIOD;
    };

}

namespace klib::atsam3x8e::io::detail::pins {
    using namespace klib::core::atsam3x8e::io::detail::pins;
}

namespace klib::atsam3x8e::io {
    template <typename Pin>
    using pin_in = klib::core::atsam3x8e::io::pin_in<Pin>;

    template <typename Pin>
    using pin_out = klib::core::atsam3x8e::io::pin_out<Pin>;

    template <typename Pin>
    using pin_in_out = klib::core::atsam3x8e::io::pin_in_out<Pin>;

    template <typename Pin>
    using pin_od = klib::core::atsam3x8e::io::pin_od<Pin>;

    template <typename Pin>
    using pin_in_out_od = klib::core::atsam3x8e::io::pin_in_out_od<Pin>;
}

#endif