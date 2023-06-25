#ifndef KLIB_LPC1756_DMA_HPP
#define KLIB_LPC1756_DMA_HPP

#include <lpc1756.hpp>

namespace klib::lpc1756::io::periph {
    struct dma0 {
        // peripheral id (e.g dma0, dma1)
        constexpr static uint32_t id = 0;

        // interrupt id (including the arm vector table)
        constexpr static uint32_t irq_id = 42;

        // peripheral clock bit position
        constexpr static uint32_t clock_id = 29;

        // port to the GPDMA hardware
        static inline GPDMA_Type *const port = GPDMA;

        // available channels in the dma
        constexpr static uint32_t max_channels = 8;
    };
}

#endif