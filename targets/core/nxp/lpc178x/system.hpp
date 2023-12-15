#ifndef KLIB_NXP_LPC175X_SYSTEM_HPP
#define KLIB_NXP_LPC175X_SYSTEM_HPP

#include <klib/klib.hpp>
#include <io/port.hpp>

namespace klib::core::lpc178x::io::system {
    class trace {
    private:
        struct clk {
            // P2.06
            using port = lpc1788::io::periph::gpio2;

            // bit number in port
            constexpr static uint32_t number = 6;
        };

        struct d0 {
            // P2.05
            using port = lpc1788::io::periph::gpio2;

            // bit number in port
            constexpr static uint32_t number = 5;
        };

        struct d1 {
            // P2.04
            using port = lpc1788::io::periph::gpio2;

            // bit number in port
            constexpr static uint32_t number = 4;
        };

        struct d2 {
            // P2.03
            using port = lpc1788::io::periph::gpio2;

            // bit number in port
            constexpr static uint32_t number = 3;
        };

        struct d3 {
            // P2.02
            using port = lpc1788::io::periph::gpio2;

            // bit number in port
            constexpr static uint32_t number = 2;
        };

    public:
        template <bool Enable, uint8_t PortWidth = 4>
        static void enable() {
            if constexpr (Enable) {
                // validate the port width
                static_assert(PortWidth > 0 && PortWidth <= 4, "Invalid port width");

                // always set the clock
                io::detail::pins::set_peripheral<clk, io::detail::alternate::func_5>();

                if constexpr (PortWidth >= 1) {
                    io::detail::pins::set_peripheral<d0, io::detail::alternate::func_5>();
                }
                if constexpr (PortWidth >= 2) {
                    io::detail::pins::set_peripheral<d1, io::detail::alternate::func_5>();
                }
                if constexpr (PortWidth >= 4) {
                    io::detail::pins::set_peripheral<d2, io::detail::alternate::func_5>();
                    io::detail::pins::set_peripheral<d3, io::detail::alternate::func_5>();
                }
            }
            else {
                // clear all the pins from the trace peripheral
                io::detail::pins::set_peripheral<clk, io::detail::alternate::none>();
                io::detail::pins::set_peripheral<d0, io::detail::alternate::none>();
                io::detail::pins::set_peripheral<d1, io::detail::alternate::none>();
                io::detail::pins::set_peripheral<d2, io::detail::alternate::none>();
                io::detail::pins::set_peripheral<d3, io::detail::alternate::none>();
            }
        }

    };
}

#endif