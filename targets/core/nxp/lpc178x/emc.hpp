#ifndef KLIB_NXP_LPC178X_EMC_HPP
#define KLIB_NXP_LPC178X_EMC_HPP

#include <tuple>

#include <klib/delay.hpp>
#include <klib/io/core_clock.hpp>

#include <io/power.hpp>
#include <io/port.hpp>

namespace klib::core::lpc178x::io {
    template <typename Emc, uint8_t AddressBits, uint8_t DataBits, bool HalfCPUClock = false>
    class emc {
    public:

    };
}

#endif