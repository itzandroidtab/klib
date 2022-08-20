#ifndef KLIB_MAX32660_TEMP_CAMERA_PINS_HPP
#define KLIB_MAX32660_TEMP_CAMERA_PINS_HPP

#include <io/pins.hpp>

namespace klib::max32660::io::pins::board::temp_camera {
    // using the tqfn_24 pins

    // display pins
    using display_rst = package::tqfn_24::p4;
    using display_dc = package::tqfn_24::p24;
    using display_cs = package::tqfn_24::p18;
    using display_sck = package::tqfn_24::p19;
    using display_mosi = package::tqfn_24::p20;
    using display_blk = package::tqfn_24::p23;

    // buttons
    using button_shutter = package::tqfn_24::p17;
    using button_ok = package::tqfn_24::p5;

    // i2c pins
    using sda = package::tqfn_24::p13;
    using scl = package::tqfn_24::p14;

    // battery charging detection
    using charging_detect = package::tqfn_24::p16;
}

namespace klib::max32660::io::periph {
    namespace tqfn_24 {};

    // move target package peripherals into current namespace
    using namespace tqfn_24;
}

#endif