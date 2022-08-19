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

        // is pixel is transparant
        bool transparant;
    };

    // some default colors
    [[maybe_unused]]
    constexpr color white = {0xff, 0xff, 0xff, false};

    [[maybe_unused]]
    constexpr color black = {0x00, 0x00, 0x00, false};
    
    [[maybe_unused]]
    constexpr color grey = {0xa0, 0xa0, 0xa0, false};

    [[maybe_unused]]
    constexpr color red = {0xff, 0x00, 0x00, false};

    [[maybe_unused]]
    constexpr color green = {0x00, 0xff, 0x00, false};

    [[maybe_unused]]
    constexpr color blue = {0x00, 0x00, 0xff, false};

    [[maybe_unused]]
    constexpr color yellow = {0xff, 0xff, 0x00, false};

    [[maybe_unused]]
    constexpr color transparent = {0x00, 0x00, 0x00, true};
}

#endif