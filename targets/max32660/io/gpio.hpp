#ifndef KLIB_MAX32660_GPIO_HPP
#define KLIB_MAX32660_GPIO_HPP

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
            return pins::detail::port<typename Pin::port>->IN & pins::detail::mask<Pin>;
        }

        constexpr static void pullup_enable() {
            // get the status of the pin
            pins::detail::port<typename Pin::port>->PS |= pins::detail::mask<Pin>;
        }

        constexpr static void pullup_disable() {
            // get the status of the pin
            pins::detail::port<typename Pin::port>->PS &= ~pins::detail::mask<Pin>;
        }
    };

    template<typename Pin>
    class pin_out {
    public:
        constexpr static void init() {
            // clear all the alternate functions
            set_peripheral<Pin, io::detail::periph_func_none>();

            // enable the gpio output
            pins::detail::port<typename Pin::port>->OUT_EN_SET = pins::detail::mask<Pin>;
        }

        template<bool Val>
        constexpr static void set() {
            if constexpr (Val) {
                pins::detail::port<typename Pin::port>->OUT_SET = pins::detail::mask<Pin>;
            }
            else {
                pins::detail::port<typename Pin::port>->OUT_CLR = pins::detail::mask<Pin>;
            }
        }

        constexpr static void set(const bool val) {
            if (val) {
                pins::detail::port<typename Pin::port>->OUT_SET = pins::detail::mask<Pin>;
            }
            else {
                pins::detail::port<typename Pin::port>->OUT_CLR = pins::detail::mask<Pin>;
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