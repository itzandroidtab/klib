#ifndef KLIB_TARGET_TEMPLATE_EXAMPLE_PINS_HPP
#define KLIB_TARGET_TEMPLATE_EXAMPLE_PINS_HPP

#include <io/pins.hpp>

namespace klib::target_template::pins::board::example {
    // using the tqfn_100 pins
    using d0 = package::tqfn_100::p0;
}

namespace klib::target_template::io::periph {
    namespace tqfn_100 {};

    // move target package peripherals into current namespace
    using namespace tqfn_100;
}

#endif