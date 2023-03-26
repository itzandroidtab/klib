#ifndef KLIB_COLOR_HPP
#define KLIB_COLOR_HPP

#include <cstdint>

#include <klib/math.hpp>

namespace klib::graphics {
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

namespace klib::graphics {
    /**
     * @brief Different display modes. Not all modes are supported for all displays
     * 
     */
    enum class mode {
        mono,
        rgb222,
        rgb232,
        rgb242,
        rgb444,
        rgb565,
        bgr565,
        rgb666,
        rgb888,
    };
}

namespace klib::graphics::detail {
    // using to determine the pixel type based on the template input
    template <mode Mode>
    struct pixel_conversion {
        using type = void;

        // amount of bits used in type
        constexpr static uint32_t bits = 16;
    };

    // specialization for mono
    template <>
    struct pixel_conversion<mode::mono> {
        using type = bool;

        // amount of bits used in type
        constexpr static uint32_t bits = 1;
    };

    // specialization for rgb222
    template <>
    struct pixel_conversion<mode::rgb222> {
        using type = uint8_t;

        // amount of bits used in type
        constexpr static uint32_t bits = 6;
    };

    // specialization for rgb232
    template <>
    struct pixel_conversion<mode::rgb232> {
        using type = uint8_t;

        // amount of bits used in type
        constexpr static uint32_t bits = 7;
    };

    // specialization for rgb242
    template <>
    struct pixel_conversion<mode::rgb242> {
        using type = uint8_t;

        // amount of bits used in type
        constexpr static uint32_t bits = 8;
    };

    // specialization for rgb444
    template <>
    struct pixel_conversion<mode::rgb444> {
        using type = uint16_t;

        // amount of bits used in type
        constexpr static uint32_t bits = 12;
    };

    // specialization for rgb565
    template <>
    struct pixel_conversion<mode::rgb565> {
        using type = uint16_t;

        // amount of bits used in type
        constexpr static uint32_t bits = 16;
    };

    // specialization for bgr565
    template <>
    struct pixel_conversion<mode::bgr565> {
        using type = uint16_t;

        // amount of bits used in type
        constexpr static uint32_t bits = 16;
    };
    
    // specialization for rgb666
    template <>
    struct pixel_conversion<mode::rgb666> {
        using type = uint32_t;

        // amount of bits used in type
        constexpr static uint32_t bits = 18;
    };

    // specialization for rgb888
    template <>
    struct pixel_conversion<mode::rgb888> {
        using type = uint32_t;

        // amount of bits used in type
        constexpr static uint32_t bits = 24;
    };

    /**
     * @brief Generic Conversion from a klib color to a display mode. Transparant 
     * pixels are not handled in this conversion
     * 
     * @tparam Mode 
     * @param col 
     * @return pixel_conversion<Mode>::type
     */
    template <mode Mode>
    constexpr pixel_conversion<Mode>::type color_to_raw(const klib::graphics::color &col) {
        // convert from klib color to the raw pixel format
        if constexpr (Mode == mode::mono) {
            return col.red || col.green || col.blue; 
        }
        else if constexpr (Mode == mode::rgb222) {
            return ((col.red >> 6) << 4) | ((col.green >> 6) << 2) | (col.blue >> 6);
        }
        else if constexpr (Mode == mode::rgb232) {
            return ((col.red >> 6) << 5) | ((col.green >> 5) << 2) | (col.blue >> 6);
        }
        else if constexpr (Mode == mode::rgb242) {
            return ((col.red >> 6) << 6) | ((col.green >> 4) << 2) | (col.blue >> 6);
        }
        else if constexpr (Mode == mode::rgb444) {
            return ((col.red >> 4) << 8) | ((col.green >> 4) << 4) | (col.blue >> 4);
        }
        else if constexpr (Mode == mode::rgb565) {
            return ((col.red >> 3) << 11) | ((col.green >> 2) << 5) | (col.blue >> 3);
        }
        else if constexpr (Mode == mode::bgr565) {
            return ((col.blue >> 3) << 11) | ((col.green >> 2) << 5) | (col.red >> 3);
        }
        else if constexpr (Mode == mode::rgb666) {
            return ((col.blue >> 2) << 12) | ((col.green >> 2) << 6) | (col.red >> 2);
        }
        else {
            // return rgb888 when not in other cases
            return (col.red << 16) | (col.green << 8) | (col.blue);
        }
    }

    template <mode Mode>
    constexpr klib::graphics::color raw_to_color(const typename pixel_conversion<Mode>::type raw) {
        // convert from klib color to the raw pixel format
        if constexpr (Mode == mode::mono) {
            // convert the input to either white or black
            return (raw ? klib::graphics::color{0xff, 0xff, 0xff, false} : klib::graphics::color{0x00, 0x00, 0x00, false});
        }
        else if constexpr (Mode == mode::rgb222) {
            // convert the input back to a klib color
            return {
                .red = klib::map(static_cast<uint8_t>((raw >> 4) & 0x3), 0, 0x3, 0, 255),
                .green = klib::map(static_cast<uint8_t>((raw >> 2) & 0x3), 0, 0x3, 0, 255),
                .blue = klib::map(static_cast<uint8_t>(raw & 0x3), 0, 0x3, 0, 255),
                .transparant = false
            };
        }
        else if constexpr (Mode == mode::rgb232) {
            // convert the input back to a klib color
            return {
                .red = klib::map(static_cast<uint8_t>((raw >> 5) & 0x3), 0, 0x3, 0, 255),
                .green = klib::map(static_cast<uint8_t>((raw >> 2) & 0x7), 0, 0x7, 0, 255),
                .blue = klib::map(static_cast<uint8_t>(raw & 0x3), 0, 0x3, 0, 255),
                .transparant = false
            };
        }
        else if constexpr (Mode == mode::rgb242) {
            // convert the input back to a klib color
            return {
                .red = klib::map(static_cast<uint8_t>((raw >> 6) & 0x3), 0, 0x3, 0, 255),
                .green = klib::map(static_cast<uint8_t>((raw >> 2) & 0xf), 0, 0xf, 0, 255),
                .blue = klib::map(static_cast<uint8_t>(raw & 0x3), 0, 0x3, 0, 255),
                .transparant = false
            };
        }
        else if constexpr (Mode == mode::rgb444) {
            return {
                .red = klib::map(static_cast<uint8_t>((raw >> 8) & 0xf), 0, 0xf, 0, 255),
                .green = klib::map(static_cast<uint8_t>((raw >> 4) & 0xf), 0, 0xf, 0, 255),
                .blue = klib::map(static_cast<uint8_t>(raw & 0xf), 0, 0xf, 0, 255),
                .transparant = false
            };
        }
        else if constexpr (Mode == mode::rgb565) {
            return {
                .red = klib::map(static_cast<uint8_t>((raw >> 11) & 0x1f), 0, 0x1f, 0, 255),
                .green = klib::map(static_cast<uint8_t>((raw >> 5) & 0x3f), 0, 0x3f, 0, 255),
                .blue = klib::map(static_cast<uint8_t>(raw & 0x1f), 0, 0x1f, 0, 255),
                .transparant = false
            };
        }
        else if constexpr (Mode == mode::bgr565) {
            return {
                .red = klib::map(static_cast<uint8_t>(raw & 0x1f), 0, 0x1f, 0, 255),
                .green = klib::map(static_cast<uint8_t>((raw >> 5) & 0x3f), 0, 0x3f, 0, 255),
                .blue = klib::map(static_cast<uint8_t>((raw >> 11) & 0x1f), 0, 0x1f, 0, 255),
                .transparant = false
            };
        }
        else if constexpr (Mode == mode::rgb666) {
            return {
                .red = klib::map(static_cast<uint8_t>((raw >> 12) & 0x3f), 0, 0x3f, 0, 255),
                .green = klib::map(static_cast<uint8_t>((raw >> 6) & 0x3f), 0, 0x3f, 0, 255),
                .blue = klib::map(static_cast<uint8_t>(raw & 0x3f), 0, 0x3f, 0, 255),
                .transparant = false
            };
        }
        else {
            // return rgb888 when not in other cases
            return {
                .red = (static_cast<uint8_t>((raw >> 16) & 0xff)),
                .green = (static_cast<uint8_t>((raw >> 8) & 0xff)),
                .blue = (static_cast<uint8_t>(raw & 0xff)),
                .transparant = false
            };
        }
    }

    template <mode Input, mode Output>
    constexpr pixel_conversion<Output>::type raw_to_raw(const typename pixel_conversion<Input>::type raw) {
        // convert from klib color to the raw pixel format
        if constexpr (Input == mode::mono) {
            // convert the input to either white or black. Let the compiler convert 
            // it to the correct type
            return (raw ? static_cast<pixel_conversion<Input>::type>(
                0xffffff & (klib::exp2(pixel_conversion<Input>::bits) - 1)) : 0x0);
        }
        else {
            // convert the input back to a klib color and convert back to raw
            return color_to_raw<Output>(raw_to_color<Input>(raw));
        }
    }
}

#endif