#ifndef KLIB_COLOR_HPP
#define KLIB_COLOR_HPP

#include <cstdint>

namespace klib {
    /**
     * @brief 3 channel color struct
     * 
     * @tparam T 
     */
    struct color {
        // red color
        uint8_t red;

        // green color
        uint8_t green;

        // blue color
        uint8_t blue;
    };
}

#endif