#ifndef KLIB_PERIPHERAL_HPP
#define KLIB_PERIPHERAL_HPP

#include <cstdint>
#include <tuple>

namespace klib::io::peripheral::detail {
    /**
     * @brief Helper function to get a index to a pin in a peripheral tuple
     *
     * @tparam Index
     * @tparam Pin
     * @tparam ValidPins
     * @return constexpr uint32_t
     */
    template <uint32_t Index, typename Pin, typename ValidPins>
    constexpr uint32_t index() {
        // make sure we do not go out of the tuple bounds
        static_assert(Index < std::tuple_size<ValidPins>::value, "Pin is not supported in this peripheral");

        // get the valid pin from the tuple
        using valid = std::tuple_element_t<Index, ValidPins>::pin;

        // check if we have a match
        if constexpr(std::is_same<Pin, valid>::value) {
            return Index;
        }
        else{
            return index<Index + 1, Pin, ValidPins>();
        }
    }
}

namespace klib::io::peripheral {
    /**
     * @brief Peripheral helper function to verify and get the index in the peripheral tuple
     *
     * @tparam Pin
     * @tparam ValidPins
     * @return consteval
     */
    template <typename Pin, typename ValidPins>
    consteval uint32_t get_index() {
        // return the index of the pin
        return detail::index<0, Pin, ValidPins>();
    }
}

#endif