#ifndef KLIB_MAX32625_PIO_HPP
#define KLIB_MAX32625_PIO_HPP

#include <cstdint>

namespace klib::max32625::io::detail {
    // alternate ports for all the gpio
    struct pio0 {};

    // alternate functions for all the gpio
    // default function
    struct periph_func_none {};

    // alternate function 1
    struct periph_func_1 {};

    // alternate function 2
    struct periph_func_2 {};

    // alternate function 3
    struct periph_func_3 {};
}

#endif