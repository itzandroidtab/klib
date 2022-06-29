#ifndef KLIB_MAX32625_PERIPH_HPP
#define KLIB_MAX32625_PERIPH_HPP

#include "pio.hpp"
#include "pins.hpp"

// peripheral pin mapping to alternate functions.
namespace klib::max32625::io::periph::wlp {
    struct usb0 {
        // peripheral id (e.g usb0, usb1)
        constexpr static uint32_t id = 0;

        // interrupt id (including the arm vector table)
        constexpr static uint32_t interrupt_id = 24;
    };
}

#endif