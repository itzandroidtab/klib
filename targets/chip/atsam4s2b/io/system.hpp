#ifndef KLIB_ATSAM4S2B_SYSTEM_HPP
#define KLIB_ATSAM4S2B_SYSTEM_HPP

#include <atsam4s2b.hpp>

#include <targets/core/atmel/atsam4s/system.hpp>

namespace klib::atsam4s2b::io::system {
    // import all the static classes from the core
    // library in the current system namespace
    using namespace core::atsam4s::io::system;
}

#endif