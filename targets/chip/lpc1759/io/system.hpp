#ifndef KLIB_LPC1759_SYSTEM_HPP
#define KLIB_LPC1759_SYSTEM_HPP

#include <lpc1759.hpp>

#include <targets/core/nxp/lpc175x/system.hpp>
#include <targets/core/nxp/lpc17xx/system.hpp>

namespace klib::lpc1759::io::system {
    // import all the static classes from the core
    // library in the current system namespace
    using namespace core::lpc175x::io::system;

    // import the generic lpc17xx system drivers as
    // well
    using namespace core::lpc17xx::io::system;
}

#endif