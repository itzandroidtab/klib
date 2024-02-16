#ifndef KLIB_NXP_TMPM37X_PORT_HPP
#define KLIB_NXP_TMPM37X_PORT_HPP

#include <cstdint>

#include <klib/klib.hpp>

namespace klib::core::tmpm37x::io::detail::alternate {
    // alternate functions for all the gpio
    // default function (view reference manual for 
    // default functions for every pin)
    struct none {};

    // alternate function 1
    struct func_1 {};

    // alternate function 2
    struct func_2 {};

    // alternate function 3
    struct func_3 {};
}

namespace klib::core::tmpm37x::io::detail::pins {
    // get the pin mask of a pin number
    template <typename Pin>
    constexpr uint32_t mask = 1U << Pin::number;

    template <typename Pin, bool fr1, bool fr2, bool fr3, bool v1, bool v2, bool v3>
    static void set_peripheral_impl() {
        if constexpr (fr1) {
            Pin::port::port->FR1 = v1;
        }
        if constexpr (fr2) {
            Pin::port::port->FR2 = v2;
        }
        if constexpr (fr3) {
            Pin::port::port->FR3 = v3;
        }        
    }

    /**
     * @brief Helper function to set a pin to a specific peripheral
     * 
     * @tparam Pin 
     * @tparam Periph 
     */
    template <typename Pin, typename Periph>    
    static void set_peripheral() {
        // check how many PxFRn registers we have
        constexpr static bool fr1 = requires() {
            Pin::port::port->FR1 = 0xdeadbeef;
        };
        constexpr static bool fr2 = requires() {
            Pin::port::port->FR2 = 0xdeadbeef;
        };
        constexpr static bool fr3 = requires() {
            Pin::port::port->FR3 = 0xdeadbeef;
        };

        // set the 3 function registers
        if constexpr (std::is_same_v<Periph, io::detail::alternate::func_1>) {
            // setup alternate function 1
            set_peripheral_impl<Pin, fr1, fr2, fr3, true, false, false>();
        }
        else if constexpr (std::is_same_v<Periph, io::detail::alternate::func_2>) {
            // setup alternate function 2
            set_peripheral_impl<Pin, fr1, fr2, fr3, false, true, false>();
        }
        else if constexpr (std::is_same_v<Periph, io::detail::alternate::func_3>) {
            // setup alternate function 3
            set_peripheral_impl<Pin, fr1, fr2, fr3, false, false, true>();
        }
        else {
            // setup normal gpio function
            set_peripheral_impl<Pin, fr1, fr2, fr3, false, false, false>();
        }
    }
}

namespace klib::core::tmpm37x::io {
    template <typename Pin>
    class pin_in {
    public:
        constexpr static void init() {
            // clear all the alternate functions
            detail::pins::set_peripheral<Pin, io::detail::alternate::none>();

            // set the pin as a input (set the input enable)
            Pin::port::port->IE |= (detail::pins::mask<Pin>);
        }

        constexpr static bool get() {
            // get the status of the pin
            return Pin::port::port->DATA & detail::pins::mask<Pin>;
        }

        template <bool Val>
        constexpr static void pullup_enable() {
            if constexpr (Val) {
                Pin::port::port->PUP |= detail::pins::mask<Pin>;
            }
            else {
                Pin::port::port->PUP &= detail::pins::mask<Pin>;
            }
        }

        template <bool Val>
        constexpr static void pulldown_enable() {
            if constexpr (Val) {
                Pin::port::port->PDN |= detail::pins::mask<Pin>;
            }
            else {
                Pin::port::port->PDN &= detail::pins::mask<Pin>;
            }
        }        
    };
 
    template <typename Pin>
    class pin_out {
    public:
        constexpr static void init() {
            // clear all the alternate functions
            detail::pins::set_peripheral<Pin, io::detail::alternate::none>();

            // enable the gpio output
            Pin::port::port->CR |= detail::pins::mask<Pin>;
        }

        template <bool Val>
        constexpr static void set() {
            if constexpr (Val) {
                Pin::port::port->DATA |= detail::pins::mask<Pin>;
            }
            else {
                Pin::port::port->DATA &= (~detail::pins::mask<Pin>);
            }
        }

        constexpr static void set(const bool val) {
            if (val) {
                Pin::port::port->DATA |= detail::pins::mask<Pin>;
            }
            else {
                Pin::port::port->DATA &= (~detail::pins::mask<Pin>);
            }
        }
    };
}

#endif