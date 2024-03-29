#ifndef KLIB_DMA_HPP
#define KLIB_DMA_HPP

namespace klib::io::dma {
    // Helper struct to signal we are copying from/to memory
    struct memory {};

    // Helper struct to signal there is no channel
    struct none {};
}

#endif