#ifndef KLIB_ATSAM4S2B_PORT_HPP
#define KLIB_ATSAM4S2B_PORT_HPP

#include <cstdint>
#include <cstddef>

#include <targets/core/atmel/atsam4s/port.hpp>
#include <targets/core/atmel/atsam4s/port_interrupt.hpp>

// global peripherals, not affected by chip package
namespace klib::atsam4s2b::io::periph {
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
}

namespace klib::atsam4s2b::io::detail::pins {
    using namespace klib::core::atsam4s::io::detail::pins;
}

namespace klib::atsam4s2b::io {
    template <typename Pin>
    using pin_in = klib::core::atsam4s::io::pin_in<Pin>;

    template <typename Pin>
    using pin_out = klib::core::atsam4s::io::pin_out<Pin>;

    template <typename Pin>
    using pin_in_out = klib::core::atsam4s::io::pin_in_out<Pin>;

    template <typename Pin>
    using pin_od = klib::core::atsam4s::io::pin_od<Pin>;

    template <typename Pin>
    using pin_in_out_od = klib::core::atsam4s::io::pin_in_out_od<Pin>;

    template <typename Pin>
    using pin_interrupt = klib::core::atsam4s::io::pin_interrupt<Pin>;
}

#endif