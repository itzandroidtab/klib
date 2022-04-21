#ifndef KLIB_MAX32660_GPIO_HPP
#define KLIB_MAX32660_GPIO_HPP

#include "max32660.h"

#include "pins.hpp"
#include "pio.hpp"

namespace klib::max32660::io {
    template<typename Pin>
    class pin_in {
    public:
        constexpr static void init() {
            // clear all the alternate functions
            set_peripheral<Pin, io::detail::periph_func_none>();
        }

        constexpr static bool get() {
            // get the status of the pin
            return io::pins::port<typename Pin::port>->IN & io::pins::mask<Pin>;
        }

        constexpr static void pullup_enable() {
            // get the status of the pin
            io::pins::port<typename Pin::port>->PS |= io::pins::mask<Pin>;
        }

        constexpr static void pullup_disable() {
            // get the status of the pin
            io::pins::port<typename Pin::port>->PS &= ~io::pins::mask<Pin>;
        }
    };

    template<typename Pin>
    class pin_out {
    public:
        constexpr static void init() {
            // clear all the alternate functions
            set_peripheral<Pin, io::detail::periph_func_none>();

            // enable the gpio output
            io::pins::port<typename Pin::port>->OUT_EN_SET = io::pins::mask<Pin>;
        }

        template<bool Val>
        constexpr static void set() {
            if constexpr (Val) {
                io::pins::port<typename Pin::port>->OUT_SET = io::pins::mask<Pin>;
            }
            else {
                io::pins::port<typename Pin::port>->OUT_CLR = io::pins::mask<Pin>;
            }
        }

        constexpr static void set(const bool val) {
            if (val) {
                io::pins::port<typename Pin::port>->OUT_SET = io::pins::mask<Pin>;
            }
            else {
                io::pins::port<typename Pin::port>->OUT_CLR = io::pins::mask<Pin>;
            }
        }
    };

    template<typename Pin>
    class pin_in_out {
    public:
        constexpr static void init() {
            // init using pin_out as we can read the pin anyway
            pin_out<Pin>::init();
        }

        constexpr static bool get() {
            // get the status of the pin
            return pin_in<Pin>::get();
        }

        constexpr static void pullup_enable() {
            pin_in<Pin>::pullup_enable();
        }

        constexpr static void pullup_disable() {
            pin_in<Pin>::pullup_disable();
        }

        template<bool Val>
        constexpr static void set() {
            pin_out<Pin>::template set<Val>();
        }

        constexpr static void set(const bool val) {
            pin_out<Pin>::set(val);
        }
    };   
}

#endif