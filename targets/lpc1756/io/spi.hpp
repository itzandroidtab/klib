#ifndef KLIB_LPC1756_SPI_HPP
#define KLIB_LPC1756_SPI_HPP

#include <cstdint>

#include <lpc1756.hpp>

namespace klib::lpc1756::io::periph {
    struct spi0 {
        // peripheral id (e.g spi0, spi1)
        constexpr static uint32_t id = 0;

        // interrupt id (including the arm vector table)
        constexpr static uint32_t interrupt_id = 29;

        // port to the SPI hardware
        static inline SPI_Type *const port = SPI;
    };
}

#endif