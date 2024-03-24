#ifndef KLIB_GRAPHICS_STRING_HPP
#define KLIB_GRAPHICS_STRING_HPP

#include <cstdint>

#include <klib/vector2.hpp>
#include <klib/string.hpp>

#include "color.hpp" 
#include "font.hpp"

namespace klib::graphics {
    template <typename Font>
    class string {
    public:
        // using so the width and height are exported
        using font = Font;

    protected:
        // using for color
        using color = klib::graphics::color;

    public:
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
        static void draw(Fb& framebuffer, const char ch, 
            const klib::vector2i& position = {}, 
            const color foreground = klib::graphics::black, 
            const color background = klib::graphics::transparent)         
        {
            // get the bitmap of the character
            const auto bitmap = Font::get_character(ch);

            // draw the bitmap at the target location
            bitmap.draw(framebuffer, position, foreground, background);
        }

        /**
         * @brief Draws a string to the framebuffer. Limited by the size
         * 
         * @tparam Fb 
         * @param framebuffer 
         * @param str 
         * @param size 
         * @param position 
         * @param foreground 
         * @param background 
         */
        template <typename Fb>
        static void draw(Fb& framebuffer, const char* str, 
            const uint32_t size, const klib::vector2i& position = {}, 
            const color foreground = klib::graphics::black, 
            const color background = klib::graphics::transparent) 
        {
            // make sure we have a valid string
            if (!str) {
                return;
            }

            // go through every character
            for (uint32_t i = 0; i < size; i++) {
                // draw every character
                draw(
                    framebuffer, str[i], position + klib::vector2i((Font::width * i), 0), 
                    foreground, background
                );
            }
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
        static void draw(Fb& framebuffer, const char* str, 
            const klib::vector2i& position = {}, 
            const color foreground = klib::graphics::black, 
            const color background = klib::graphics::transparent) 
        {
            // draw using the other implementation
            draw(framebuffer, str, klib::string::strlen(str), position, foreground, background);
        }
    };
}

#endif