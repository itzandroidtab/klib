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
     */
    template <typename Bus, typename Cs>
    class w25q128jv: public is25lq040b<Bus, Cs> {};
}

#endif