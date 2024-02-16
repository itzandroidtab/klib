#ifndef KLIB_TMPM373_PORT_HPP
#define KLIB_TMPM373_PORT_HPP

#include <cstdint>
#include <cstddef>

#include <targets/core/toshiba/tmpm37x/port.hpp>

// global peripherals, not affected by chip package
namespace klib::tmpm373::io::periph {
    struct gpio0 {
        // peripheral id (e.g gpio0, gpio1)
        constexpr static uint32_t id = 0;

        // port to the gpio hardware
        static inline PA_Type *const port = PA;
    };

    struct gpio1 {
        // peripheral id (e.g gpio0, gpio1)
        constexpr static uint32_t id = 1;

        // port to the gpio hardware
        static inline PB_Type *const port = PB;
    };

    struct gpio3 {
        // peripheral id (e.g gpio0, gpio1)
        constexpr static uint32_t id = 3;

        // port to the gpio hardware
        static inline PD_Type *const port = PD;
    };

    struct gpio4 {
        // peripheral id (e.g gpio0, gpio1)
        constexpr static uint32_t id = 4;

        // port to the gpio hardware
        static inline PE_Type *const port = PE;
    };

    struct gpio5 {
        // peripheral id (e.g gpio0, gpio1)
        constexpr static uint32_t id = 5;

        // port to the gpio hardware
        static inline PF_Type *const port = PF;
    };

    struct gpio6 {
        // peripheral id (e.g gpio0, gpio1)
        constexpr static uint32_t id = 6;

        // port to the gpio hardware
        static inline PG_Type *const port = PG;
    };

    struct gpio8 {
        // peripheral id (e.g gpio0, gpio1)
        constexpr static uint32_t id = 8;

        // port to the gpio hardware
        static inline PI_Type *const port = PI;
    };

    struct gpio9 {
        // peripheral id (e.g gpio0, gpio1)
        constexpr static uint32_t id = 9;

        // port to the gpio hardware
        static inline PJ_Type *const port = PJ;
    };

    struct gpio10 {
        // peripheral id (e.g gpio0, gpio1)
        constexpr static uint32_t id = 10;

        // port to the gpio hardware
        static inline PK_Type *const port = PK;
    };

    struct gpio12 {
        // peripheral id (e.g gpio0, gpio1)
        constexpr static uint32_t id = 12;

        // port to the gpio hardware
        static inline PM_Type *const port = PM;
    };
}

namespace klib::tmpm373::io::detail::pins {
    using namespace klib::core::tmpm37x::io::detail::pins;
}

namespace klib::tmpm373::io {
    template <typename Pin>
    using pin_in = klib::core::tmpm37x::io::pin_in<Pin>;

    template <typename Pin>
    using pin_out = klib::core::tmpm37x::io::pin_out<Pin>;
}

#endif