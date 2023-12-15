#ifndef KLIB_NXP_LPC178X_PORT_HPP
#define KLIB_NXP_LPC178X_PORT_HPP

#include <cstdint>

#include <klib/klib.hpp>

namespace klib::core::lpc178x::io::detail::alternate {
    // alternate functions for all the gpio
    // default function (view reference manual for 
    // default functions for every pin)
    struct none {
        // peripheral id (e.g func0, func1)
        constexpr static uint32_t id = 0;
    };

    // alternate function 1
    struct func_1 {
        // peripheral id (e.g func0, func1)
        constexpr static uint32_t id = 1;
    };

    // alternate function 2
    struct func_2 {
        // peripheral id (e.g func0, func1)
        constexpr static uint32_t id = 2;
    };

    // alternate function 3
    struct func_3 {
        // peripheral id (e.g func0, func1)
        constexpr static uint32_t id = 3;
    };

    // alternate function 4
    struct func_4 {
        // peripheral id (e.g func0, func1)
        constexpr static uint32_t id = 4;
    };

    // alternate function 5
    struct func_5 {
        // peripheral id (e.g func0, func1)
        constexpr static uint32_t id = 5;
    };

    // alternate function 6
    struct func_6 {
        // peripheral id (e.g func0, func1)
        constexpr static uint32_t id = 6;
    };

    // alternate function 7
    struct func_7 {
        // peripheral id (e.g func0, func1)
        constexpr static uint32_t id = 7;
    };
}

namespace klib::core::lpc178x::io::detail::pins {
    // get the pin mask of a pin number
    template <typename Pin>
    constexpr uint32_t mask = 1U << Pin::number;

    /**
     * @brief Get the pointer to the correct pin select register
     * 
     * @tparam Pin 
     * @return uint32_t* const 
     */
    template <typename Pin>
    constexpr uint32_t get_pinselect_offset() {
        // get the offset by checking the id of the port and the pin
        constexpr uint32_t offset = Pin::number + (Pin::port::id * 32);

        // validate the pin offset
        static_assert(offset < 164, "Invalid pin offset");

        // check what offset to return
        return offset;
    }

    /**
     * @brief Helper function to set a pin to a specific peripheral
     * 
     * @tparam Pin 
     * @tparam Periph 
     */
    template <typename Pin, typename Periph>    
    static void set_peripheral() {
        // get the register offset for the pin selection
        constexpr uint32_t pin_offset = get_pinselect_offset<Pin>();

        // validate the alternate function is valid
        static_assert(Periph::id <= alternate::func_7::id, "Invalid alternate function");

        // setup the alternate function
        IOCON->PINSEL[pin_offset] = (IOCON->PINSEL[pin_offset] & ~(0x7)) | Periph::id;
    }

    /**
     * @brief Available pin modes
     * 
     */
    enum class mode {
        pullup = 0b00,
        repeater = 0b01,
        none = 0b10,
        pulldown = 0b11
    };

    /**
     * @brief Helper function to set the mode of a pin. 
     * 
     * @tparam Pin 
     * @tparam Value 
     */
    template <typename Pin, mode Value>
    static void set_pinmode() {
        // get the register offset for the pin selection
        constexpr uint32_t pin_offset = get_pinselect_offset<Pin>();

        // clear the previous value and set the new value
        IOCON->PINSEL[pin_offset] = (
            (IOCON->PINSEL[pin_offset] & ~(0x7 << 3)) | (static_cast<uint32_t>(Value) << 3)
        );
    }

    /**
     * @brief Set the open drain of a pin
     * 
     * @tparam Pin 
     * @tparam OpenDrain 
     */
    template <typename Pin, bool OpenDrain>
    static void set_open_drain() {
        // get the register offset for the pin selection
        constexpr uint32_t pin_offset = get_pinselect_offset<Pin>();

        // set or clear based on the value
        if constexpr (OpenDrain) {
            IOCON->PINSEL[pin_offset] |= (0x1 << 10);
        }
        else {
            IOCON->PINSEL[pin_offset] &= ~(0x1 << 10);
        }
    }
}

namespace klib::core::lpc178x::io {
    template <typename Pin>
    class pin_in {
    public:
        constexpr static void init() {
            // clear all the alternate functions
            detail::pins::set_peripheral<Pin, io::detail::alternate::none>();

            // set the pin as a input (clear the pin int the direction mask)
            Pin::port::port->DIR &= ~(detail::pins::mask<Pin>);
        }

        constexpr static bool get() {
            // get the status of the pin
            return Pin::port::port->PIN & detail::pins::mask<Pin>;
        }

        template <bool Val>
        constexpr static void pullup_enable() {
            detail::pins::set_pinmode<Pin, 
                Val ? detail::pins::mode::pullup : detail::pins::mode::none
            >();
        }

        template <bool Val>
        constexpr static void pulldown_enable() {
            detail::pins::set_pinmode<Pin, 
                Val ? detail::pins::mode::pulldown : detail::pins::mode::none
            >();
        }        
    };
 
    template <typename Pin>
    class pin_out {
    public:
        constexpr static void init() {
            // clear all the alternate functions
            detail::pins::set_peripheral<Pin, io::detail::alternate::none>();

            // enable the gpio output
            Pin::port::port->DIR |= detail::pins::mask<Pin>;
        }

        template <bool Val>
        constexpr static void set() {
            if constexpr (Val) {
                Pin::port::port->SET = detail::pins::mask<Pin>;
            }
            else {
                Pin::port::port->CLR = detail::pins::mask<Pin>;
            }
        }

        constexpr static void set(const bool val) {
            if (val) {
                Pin::port::port->SET = detail::pins::mask<Pin>;
            }
            else {
                Pin::port::port->CLR = detail::pins::mask<Pin>;
            }
        }
    };
}

#endif