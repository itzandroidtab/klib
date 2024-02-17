#ifndef KLIB_TMPM373_SYSTEM_HPP
#define KLIB_TMPM373_SYSTEM_HPP

#include <tmpm373.hpp>

#include <targets/core/toshiba/tmpm37x/system.hpp>

namespace klib::tmpm373::io::system {
    // import all the static classes from the core
    // library in the current system namespace
    using namespace core::tmpm37x::io::system;
}

#endif