#ifndef KLIB_MAX32625_PINS_HPP
#define KLIB_MAX32625_PINS_HPP

#include <type_traits>

#include <max32625.hpp>

#include "pio.hpp"

namespace klib::max32625::io::pins::detail {
    // get the pin mask of a pin number
    template<typename Pin>
    constexpr uint32_t mask = 1U << Pin::number;

    // default type when using the port
    template<typename Pin>
    const GPIO_Type *port = nullptr;

    // port when using the pio0
    template<>
    GPIO_Type *const port<io::detail::pio0> = GPIO;

    /**
     * @brief Convert a peripheral to a function selection.
     * 
     * @tparam Periph 
     * @return constexpr uint32_t 
     */
    template <typename Periph>
    constexpr static uint32_t peripheral_to_function_selection() {
        if constexpr (std::is_same_v<Periph, io::detail::periph_func_1>) {
            return 1;
        }
        else if constexpr (std::is_same_v<Periph, io::detail::periph_func_2>) {
            return 2;
        }
        else if constexpr (std::is_same_v<Periph, io::detail::periph_func_3>) {
            return 3;
        }
        else {
            return 0;
        }
    }
}

namespace klib::max32625::io::detail {
    /**
     * @brief Set the peripheral of a pin
     * 
     * @tparam Pin 
     * @tparam Periph 
     */
    template<typename Pin, typename Periph>    
    static void set_peripheral() {
        // get the index the pin is on (4 bits per pin and 32 bits in a registre)
        constexpr uint32_t index = Pin::number / ((sizeof(uint32_t) * 8) / 4);

        // get the position in the register of the index
        constexpr uint32_t position = Pin::number % ((sizeof(uint32_t) * 8) / 4);

        // get the value from the peripheral
        constexpr uint32_t value = pins::detail::peripheral_to_function_selection<Periph>();

        // calculate the offset of the value (4 bits per io pin)
        constexpr uint32_t offset = (position * 4);

        // set the value in the right index
        if constexpr (index == 0) {
            // clear the previous value
            pins::detail::port<typename Pin::port>->FUNC_SEL_P0 &= ((~0xf) << offset);
            // set the new value
            pins::detail::port<typename Pin::port>->FUNC_SEL_P0 |= (value << offset);
        }
        else if constexpr (index == 1) {
            // clear the previous value
            pins::detail::port<typename Pin::port>->FUNC_SEL_P1 &= ((~0xf) << offset);;
            // set the new value
            pins::detail::port<typename Pin::port>->FUNC_SEL_P1 |= (value << offset);
        }
        else if constexpr (index == 2) {
            // clear the previous value
            pins::detail::port<typename Pin::port>->FUNC_SEL_P2 &= ((~0xf) << offset);;
            // set the new value
            pins::detail::port<typename Pin::port>->FUNC_SEL_P2 |= (value << offset);
        }
        else if constexpr (index == 3) {
            // clear the previous value
            pins::detail::port<typename Pin::port>->FUNC_SEL_P3 &= ((~0xf) << offset);;
            // set the new value
            pins::detail::port<typename Pin::port>->FUNC_SEL_P3 |= (value << offset);
        }
        else if constexpr (index == 4) {
            // clear the previous value
            pins::detail::port<typename Pin::port>->FUNC_SEL_P4 &= ((~0xf) << offset);;
            // set the new value
            pins::detail::port<typename Pin::port>->FUNC_SEL_P4 |= (value << offset);
        }
        else if constexpr (index == 5) {
            // clear the previous value
            pins::detail::port<typename Pin::port>->FUNC_SEL_P5 &= ((~0xf) << offset);;
            // set the new value
            pins::detail::port<typename Pin::port>->FUNC_SEL_P5 |= (value << offset);
        }
        else if constexpr (index == 6) {
            // clear the previous value
            pins::detail::port<typename Pin::port>->FUNC_SEL_P6 &= ((~0xf) << offset);;
            // set the new value
            pins::detail::port<typename Pin::port>->FUNC_SEL_P6 |= (value << offset);
        }
        else if constexpr (index == 7) {
            // clear the previous value
            pins::detail::port<typename Pin::port>->FUNC_SEL_P7 &= ((~0xf) << offset);;
            // set the new value
            pins::detail::port<typename Pin::port>->FUNC_SEL_P7 |= (value << offset);
        }
        else if constexpr (index == 1) {
            // clear the previous value
            pins::detail::port<typename Pin::port>->FUNC_SEL_P8 &= ((~0xf) << offset);;
            // set the new value
            pins::detail::port<typename Pin::port>->FUNC_SEL_P8 |= (value << offset);
        }
    }
}

namespace klib::max32625::io::pins::package::wlp {

}

#endif