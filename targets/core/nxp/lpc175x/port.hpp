#ifndef KLIB_NXP_LPC175X_PORT_HPP
#define KLIB_NXP_LPC175X_PORT_HPP

#include <cstdint>

#include <klib/klib.hpp>

namespace klib::core::lpc175x::io::detail::alternate {
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

namespace klib::core::lpc175x::io::detail::pins {
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
        // get the offset by checking the id of the port and if the pin number is above 15
        constexpr uint32_t offset = (Pin::port::id * 2) + (Pin::number >= (32 / 2));

        // make sure the offset is valid
        static_assert(
            offset == 0 || offset == 1 || offset == 2 || 
            offset == 3 || offset == 4 || offset == 7 || 
            offset == 9,
            "Pin offset is not correct" 
        );

        // check what pointer to return
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

        // set the 3 function registers
        if constexpr (std::is_same_v<Periph, io::detail::alternate::func_1>) {
            // setup alternate function 1
            PINCONNECT->PINSEL[pin_offset] = (
                PINCONNECT->PINSEL[pin_offset] & ~(0b11 << ((Pin::number * 2)) % 32)) | (0b01 << ((Pin::number * 2)) % 32
            );
        }
        else if constexpr (std::is_same_v<Periph, io::detail::alternate::func_2>) {
            // setup alternate function 2
            PINCONNECT->PINSEL[pin_offset] = (
                PINCONNECT->PINSEL[pin_offset] & ~(0b11 << ((Pin::number * 2)) % 32)) | (0b10 << ((Pin::number * 2)) % 32
            );
        }
        else if constexpr (std::is_same_v<Periph, io::detail::alternate::func_3>) {
            // setup alternate function 3
            PINCONNECT->PINSEL[pin_offset] |= (0b11 << ((Pin::number * 2)) % 32);
        }
        else {
            // setup normal gpio function
            PINCONNECT->PINSEL[pin_offset] &= ~(0b11 << ((Pin::number * 2)) % 32);
        }
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
        PINCONNECT->PINMODE[pin_offset] = (PINCONNECT->PINMODE[pin_offset] & ~(
            (static_cast<uint8_t>(Value) & 0b11) << ((Pin::number * 2)) % 32)) | 
            ((static_cast<uint8_t>(Value) & 0b11) << ((Pin::number * 2)
        ) % 32);
    }

    /**
     * @brief Set the open drain of a pin
     * 
     * @tparam Pin 
     * @tparam OpenDrain 
     */
    template <typename Pin, bool OpenDrain>
    static void set_open_drain() {        
        // set or clear based on the value
        if constexpr (OpenDrain) {
            PINCONNECT->PINMODE[Pin::port::id] |= mask<Pin>;
        }
        else {
            PINCONNECT->PINMODE[Pin::port::id] &= ~(mask<Pin>);
        }
    }
}

namespace klib::core::lpc175x::io {
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