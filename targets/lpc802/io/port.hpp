#ifndef KLIB_LPC802_PORT_HPP
#define KLIB_LPC802_PORT_HPP

#include <cstdint>
#include <cstddef>

#include <lpc802.hpp>
#include "clocks.hpp"
#include "matrix.hpp"

namespace klib::lpc802::io::detail::pins {
    // get the pin mask of a pin number
    template <typename Pin>
    constexpr uint32_t mask = 1U << Pin::number;
}

// global peripherals, not affected by chip package
namespace klib::lpc802::io::periph {
    struct gpio0 {
        // peripheral id (e.g gpio0, gpio1)
        constexpr static uint32_t id = 0;

        // clock id of the peripheral
        constexpr static uint32_t clock_id = 6;

        // port to the gpio hardware
        static inline GPIO_Type *const port = GPIO;
    };
}

namespace klib::lpc802::io {
    template <typename Pin>
    class pin_in {
    public:
        constexpr static void init() {
            // enable the gpio clock
            clocks::enable<typename Pin::port>();

            // clear all the alternate functions
            matrix<periph::matrix0>::clear<Pin>();

            // set the pin as a input (clear the pin int the direction mask)
            Pin::port::port->DIRCLR0 |= detail::pins::mask<Pin>;
        }

        constexpr static bool get() {
            // get the status of the pin
            return Pin::port::port->B[Pin::number];
        }
    };
 
    template <typename Pin>
    class pin_out {
    public:
        constexpr static void init() {
            // enable the gpio clock
            clocks::enable<typename Pin::port>();

            // clear all the alternate functions
            matrix<periph::matrix0>::clear<Pin>();

            // enable the gpio output
            Pin::port::port->DIRSET0 |= detail::pins::mask<Pin>;
        }

        template <bool Val>
        constexpr static void set() {
            // set the value into the register
            Pin::port::port->B[Pin::number] = Val;
        }

        constexpr static void set(const bool val) {
            // set the value into the register
            Pin::port::port->W[Pin::number] = val;
        }
    };
}

#endif