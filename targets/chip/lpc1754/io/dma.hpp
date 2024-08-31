#ifndef KLIB_LPC1754_DMA_HPP
#define KLIB_LPC1754_DMA_HPP

#include <array>

#include <targets/core/nxp/lpc175x/dma.hpp>

namespace klib::lpc1754::io::periph {
    struct dma0 {
        // peripheral id (e.g dma0, dma1)
        constexpr static uint32_t id = 0;

        // interrupt id (including the arm vector table)
        constexpr static uint32_t interrupt_id = 42;

        // peripheral clock bit position
        constexpr static uint32_t clock_id = 29;

        // port to the GPDMA hardware
        static inline GPDMA_Type *const port = GPDMA;

        // available channels in the dma
        constexpr static uint32_t max_channels = 8;
    };
}

namespace klib::lpc1754::io {
    template <typename Dma>
    using dma = core::lpc175x::io::dma<Dma>;

    template <typename Dma, uint8_t Channel, typename Source, typename Destination>
    using dma_channel = core::lpc175x::io::dma_channel<Dma, Channel, Source, Destination>;
}

#endif