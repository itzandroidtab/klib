#ifndef KLIB_ATMEL_ATSAM3X8E_PORT_HPP
#define KLIB_ATMEL_ATSAM3X8E_PORT_HPP

#include <cstdint>

#include <io/power.hpp>

namespace klib::core::atsam3x8e::io::detail::alternate {
    // alternate functions for all the gpio
    // default function (view reference manual for
    // default functions for every pin)
    struct none {};

    // alternate function 1
    struct func_1 {};

    // alternate function 2
    struct func_2 {};
}

namespace klib::core::atsam3x8e::io::detail::pins {
    // get the pin mask of a pin number
    template <typename Pin>
    constexpr uint32_t mask = 1U << Pin::number;

    /**
     * @brief Disable the write protect on a specific port
     *
     */
    template <typename Port>
    static void disable_write_protect() {
        // disable the pio write protection
        Port::port->WPMR = (0x50494f << 8);
    }

    /**
     * @brief Helper function to set a pin to a specific peripheral
     *
     * @tparam Pin
     * @tparam Periph
     */
    template <typename Pin, typename Periph>
    static void set_peripheral() {
        // disable the write protection on the port
        disable_write_protect<typename Pin::port>();

        // set the 2 function registers based on what alternate
        // function we want
        if constexpr (std::is_same_v<Periph, io::detail::alternate::none>) {
            // enable control using the PIO. Disables peripheral
            // control of the pin
            Pin::port::port->PER = mask<Pin>;
        }
        else if constexpr (std::is_same_v<Periph, io::detail::alternate::func_1>) {
            // disable control using the PIO. Enables peripheral
            // control of the pin
            Pin::port::port->PDR = mask<Pin>;

            // setup alternate function 1
            Pin::port::port->ABSR &= ~mask<Pin>;
        }
        else {
            // disable control using the PIO. Enables peripheral
            // control of the pin
            Pin::port::port->PDR = mask<Pin>;

            // setup alternate function 2
            Pin::port::port->ABSR |= mask<Pin>;
        }
    }
}

namespace klib::core::atsam3x8e::io {
    template <typename Pin>
    class pin_in {
    public:
        constexpr static void init() {
            // enable the power for the pio
            target::io::power_control::enable<typename Pin::port>();

            // clear all the alternate functions
            detail::pins::set_peripheral<Pin, io::detail::alternate::none>();

            // disable the output on the pin
            Pin::port::port->ODR |= detail::pins::mask<Pin>;
        }

        constexpr static bool get() {
            // get the status of the pin
            return Pin::port::port->PDSR & detail::pins::mask<Pin>;
        }

        template <bool Val>
        constexpr static void pullup_enable() {
            if constexpr (Val) {
                Pin::port::port->PUER = detail::pins::mask<Pin>;
            }
            else {
                Pin::port::port->PUDR = detail::pins::mask<Pin>;
            }
        }
    };

    template <typename Pin>
    class pin_out {
    public:
        constexpr static void init() {
            // enable the power for the pio
            target::io::power_control::enable<typename Pin::port>();

            // clear all the alternate functions
            detail::pins::set_peripheral<Pin, io::detail::alternate::none>();

            // enable the gpio output
            Pin::port::port->OER = detail::pins::mask<Pin>;
        }

        template <bool Val>
        constexpr static void set() {
            if constexpr (Val) {
                Pin::port::port->SODR = detail::pins::mask<Pin>;
            }
            else {
                Pin::port::port->CODR = detail::pins::mask<Pin>;
            }
        }

        constexpr static void set(const bool val) {
            if (val) {
                Pin::port::port->SODR = detail::pins::mask<Pin>;
            }
            else {
                Pin::port::port->CODR = detail::pins::mask<Pin>;
            }
        }
    };

    template <typename Pin>
    class pin_od {
    public:
        constexpr static void init() {
            // init the pin using the pin_out 
            pin_out<Pin>::init();

            // enable the multi-driver open drain
            Pin::port::port->MDER = detail::pins::mask<Pin>;
        }

        template <bool Val>
        constexpr static void set() {
            pin_out<Pin>::template set<Val>();
        }

        constexpr static void set(const bool val) {
            pin_out<Pin>::set(val);
        }
    };

    template <typename Pin>
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

        template <bool Val>
        constexpr static void pullup_enable() {
            pin_in<Pin>::template pullup_enable<Val>();
        }

        template <bool Val>
        constexpr static void set() {
            pin_out<Pin>::template set<Val>();
        }

        constexpr static void set(const bool val) {
            pin_out<Pin>::set(val);
        }
    };

    template <typename Pin>
    class pin_in_out_od {
    public:
        constexpr static void init() {
            // init using pin_od as we can read the pin anyway
            pin_od<Pin>::init();
        }

        constexpr static bool get() {
            // get the status of the pin
            return pin_in<Pin>::get();
        }

        template <bool Val>
        constexpr static void pullup_enable() {
            pin_in<Pin>::template pullup_enable<Val>();
        }

        template <bool Val>
        constexpr static void set() {
            pin_od<Pin>::template set<Val>();
        }

        constexpr static void set(const bool val) {
            pin_od<Pin>::set(val);
        }
    };
}

#endif