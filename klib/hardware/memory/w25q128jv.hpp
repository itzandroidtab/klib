#ifndef KLIB_W25Q128JV_HPP
#define KLIB_W25Q128JV_HPP

#include <cstdint>

#include "is25lq040b.hpp"

namespace klib::hardware::memory {
    /**
     * @brief Use the same protocol as the is25lq040 for the w25q128jv
     * 
     * @tparam Bus 
     * @tparam Cs 
     * @tparam BusyWait check the busy wait flag after enabling 
     * the write enable latch. This might be needed for slower 
     * memory devices for proper writes
     */
    template <typename Bus, typename Cs, bool BusyWait = false>
    class w25q128jv: public is25lq040b<Bus, Cs, BusyWait> {};
}

#endif