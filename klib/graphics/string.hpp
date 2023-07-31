#ifndef KLIB_GRAPHICS_STRING_HPP
#define KLIB_GRAPHICS_STRING_HPP

#include <cstdint>

#include <klib/vector2.hpp>

#include "color.hpp" 
#include "font.hpp"

namespace klib::graphics {
    template <typename F>
    class string {
    protected:
        // using for color
        using color = klib::graphics::color;

        // the font the string uses to display text
        const F& font;

    public:
        /**
         * @brief Construct a graphics string object
         * 
         * @param font 
         */
        string(const F& font):
            font(font)
        {}

        /**
         * @brief Draws a single character to the framebuffer
         * 
         * @tparam Fb 
         * @param ch 
         * @param position 
         * @param foreground 
         * @param background 
         */
        template <typename Fb>
        void draw(Fb& framebuffer, const char ch, 
            const klib::vector2i& position = {}, 
            const color foreground = klib::graphics::black, 
            const color background = klib::graphics::transparent) const         
        {
            // get the bitmap of the character
            const auto bitmap = font.get_character(ch);

            // draw the bitmap at the target location
            bitmap.draw(framebuffer, position, foreground, background);
        }

        /**
         * @brief Draws a string to the framebuffer
         * 
         * @tparam Fb 
         * @param framebuffer 
         * @param str 
         * @param position 
         * @param foreground 
         * @param background 
         */
        template <typename Fb>
        void draw(Fb& framebuffer, const char* str = nullptr, 
            const klib::vector2i& position = {}, 
            const color foreground = klib::graphics::black, 
            const color background = klib::graphics::transparent) const 
        {
            // make sure we have a valid string
            if (!str) {
                return;
            }

            // go through every character
            for (uint32_t i = 0; str[i] != 0x00; i++) {
                // draw every character
                draw(
                    framebuffer, str[i], position + klib::vector2i((font.width * i), 0), 
                    foreground, background
                );
            }
        }
    };
}

#endif