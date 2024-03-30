#ifndef KLIB_TARGET_TEMPLATE_PINS_HPP
#define KLIB_TARGET_TEMPLATE_PINS_HPP

#include "port.hpp"

namespace klib::target_template::pins::package::tqfn_100 {
    /**
     * @brief Pin 0 of the tqfn package. Stores information about the 
     * port and bit number of the io.
     * 
     */
    struct p0 {
        // P{port}.{bit numer}
        using port = io::periph::gpio0;

        // bit number in the port
        constexpr static uint32_t number = 0;
    };
}

#endif