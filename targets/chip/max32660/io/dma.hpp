#ifndef KLIB_MAX32660_DMA_HPP
#define KLIB_MAX32660_DMA_HPP

#include <cstdint>

// global peripherals, not affected by chip package
namespace klib::max32660::io::periph {
    struct dma0 {
        // peripheral id (e.g dma0, dma1)
        constexpr static uint32_t id = 0;

        // peripheral clock bit position
        constexpr static uint32_t clock_id = 5;

        // peripheral interrupt position
        constexpr static uint32_t interrupt_id = 44;
    };
}

#endif