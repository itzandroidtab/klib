#ifndef KLIB_MAX32660_EVSYS_PINS_HPP
#define KLIB_MAX32660_EVSYS_PINS_HPP

#include "../../io/pins.hpp"

namespace klib::max32660::io::pins::board::evsys {
    // using the tqfn_24 pins
    using d0 = package::tqfn_24::p5;
    using d1 = package::tqfn_24::p4;
    using d2 = package::tqfn_24::p21;
    using d3 = package::tqfn_24::p23;
    using d4 = package::tqfn_24::p24;
    using d5 = package::tqfn_24::p1;
    using d6 = package::tqfn_24::p2;

    using d7 = package::tqfn_24::p13;
    using d8 = package::tqfn_24::p14;
    using d9 = package::tqfn_24::p18;
    using d10 = package::tqfn_24::p19;
    using d11 = package::tqfn_24::p20;
    using d12 = package::tqfn_24::p16;
    using d13 = package::tqfn_24::p17;
}

namespace klib::max32660::io::periph {
    namespace tqfn_24 {};

    // move target package peripherals into current namespace
    using namespace tqfn_24;
}

#endif