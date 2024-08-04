#ifndef KLIB_LPC1788_SYSTEM_HPP
#define KLIB_LPC1788_SYSTEM_HPP

#include <targets/core/nxp/lpc178x/system.hpp>
#include <targets/core/nxp/lpc17xx/system.hpp>

namespace klib::lpc1788::io::system {
    // import all the static classes from the core
    // library in the current system namespace
    using namespace core::lpc178x::io::system;

    // import the generic lpc17xx system drivers as
    // well
    using namespace core::lpc17xx::io::system;
}

#endif