#ifndef KLIB_NXP_LPC178X_EMC_HPP
#define KLIB_NXP_LPC178X_EMC_HPP

#include <tuple>
#include <bit>

#include <klib/klib.hpp>
#include <klib/units.hpp>
#include <klib/delay.hpp>
#include <klib/io/peripheral.hpp>
#include <klib/io/port.hpp>
#include <klib/io/core_clock.hpp>

#include <io/power.hpp>

namespace klib::core::lpc178x::io {
    template <typename Emc, uint8_t AddressBits, uint8_t DataBits, bool HalfCPUClock = false>
    class emc {
    public:

    };
}

#endif
