#ifndef KLIB_LPC1756_SSP_HPP
#define KLIB_LPC1756_SSP_HPP

#include <cstdint>

#include <lpc1756.hpp>

namespace klib::lpc1756::io::periph {
    struct ssp0 {
        // peripheral id (e.g ssp0, ssp1)
        constexpr static uint32_t id = 0;

        // interrupt id (including the arm vector table)
        constexpr static uint32_t interrupt_id = 30;

        // port to the SSP hardware
        SSP0_Type *const port = SSP0;
    };

    struct ssp1 {
        // peripheral id (e.g ssp0, ssp1)
        constexpr static uint32_t id = 1;

        // interrupt id (including the arm vector table)
        constexpr static uint32_t interrupt_id = 31;

        // port to the SSP hardware
        SSP0_Type *const port = SSP1;
    };
}

#endif