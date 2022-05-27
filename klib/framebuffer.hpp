#ifndef KLIB_FRAMEBUFFER_HPP
#define KLIB_FRAMEBUFFER_HPP

#include <cstdint>

#include <klib/color.hpp>
#include <klib/vector2.hpp>

namespace klib {
    /**
     * @brief Direct framebuffer. Implements framebuffer without buffer 
     * and directly writes to the display.
     * 
     * Autoincrement causes a cursor to update to the next pixel. This removes 
     * the need to update the cursor for every pixel update. Should be disabled 
     * when the display does not support it.
     * 
     * Autoincrement direction should be in the x axis to the positive direction
     * 
     * @tparam Display 
     * @tparam AutoIncrement
     */
    template <typename Display, bool AutoIncrement = true>
    class direct_framebuffer {
    protected:
        // cache for the where the cursor is located
        klib::vector2<uint32_t> cursor;
    
    public:
        using display = Display;

        constexpr void init() {
            // update the position of the cursor in the display
            display::set_cursor({0, 0}, {display::width - 1, display::height - 1});

            // clear the cursor
            cursor = {0, 0};
        }

        constexpr void flush() {
            // not used in the direct framebuffer
        }

        constexpr void set_pixel(const klib::vector2<uint32_t> position, const display::pixel_type raw) {
            // limit the input to the display size
            const auto pos = klib::vector2<uint32_t>(
                klib::min(position.x, display::width - 1), klib::min(position.y, display::height - 1)
            );

            // check if we can optimize the set cursor command
            if constexpr (AutoIncrement) {
                // update the cursor on the display if it doesnt match
                if (cursor != pos) {
                    // update the cursor on the display
                    display::set_cursor(pos, {display::width - 1, display::height - 1});
                }
            }
            else {
                // set the position every time we send raw pixel data
                display::set_cursor(pos, {display::width - 1, display::height - 1});
            }

            // write the raw data to the screen
            display::raw_write_screen(&raw, 1);

            if constexpr (AutoIncrement) {
                // update the cursor on the screen
                cursor = pos + klib::vector2<uint32_t>(1, 0);
            }
        }

        constexpr void set_pixel(const klib::vector2<uint32_t> position, const klib::color &col) {
            // convert using the display conversion function
            const auto raw = display::color_to_raw(col);

            // set the raw value on position
            set_pixel(position, raw);
        }

        constexpr void clear(const display::pixel_type raw) {
            // write the raw value to every pixel
            for (uint32_t y = 0; y < display::height; y++) {
                for (uint32_t x = 0; x < display::width; x++) {
                    // set the raw value on position
                    set_pixel({x, y}, raw);
                }
            }
        }

        constexpr void clear(const klib::color &col) {
            // conver the color to raw data
            const auto raw = display::color_to_raw(col);

            // clear using the raw value
            clear(raw);
        }
    };

    /**
     * @brief Framebuffer. Buffers data before it is flushed to the display. 
     * Makes use of the feature to automaticly increment the cursor of the 
     * display after pixel data is written. This allows the spi bus to only
     * transmit pixel data without other data in between
     * 
     * @tparam display 
     */
    template <typename Display>
    class framebuffer {
    protected:
        // framebuffer to store the image
        Display::pixel_type buffer[Display::width * Display::height];

    public:
        using display = Display;

        constexpr void init() {
            // do nothing
        }

        constexpr void flush() {
            // set the cursor to the start of the display
            display::set_cursor(
                klib::vector2<uint32_t>{0, 0}, 
                klib::vector2<uint32_t>{display::width - 1, display::height - 1}
            );

            // write the buffer to the screen
            display::raw_write_screen(buffer, display::width * display::height);
        }

        constexpr void set_pixel(const klib::vector2<uint32_t> position, const display::pixel_type raw) {
            // limit the input to the display size
            const auto pos = klib::vector2<uint32_t>(
                klib::min(position.x, display::width - 1), klib::min(position.y, display::height - 1)
            );

            // set the data in the buffer based on the display position function
            buffer[display::position_to_buffer(pos)] = raw;
        }

        constexpr void set_pixel(const klib::vector2<uint32_t> position, const klib::color &col) {
            // convert using the display conversion function
            const auto raw = display::color_to_raw(col);

            // set the pixel using the raw value
            set_pixel(position, raw);
        }

        constexpr void clear(const display::pixel_type raw) {
            // get the amount of pixels we need to clear
            constexpr uint32_t c = display::width * display::height;

            // iterate on all the pixels
            for (uint32_t i = 0; i < c; i++) {
                // convert the color to a pixel
                buffer[i] = raw;
            }
        }

        constexpr void clear(const klib::color &col) {
            // conver the color to raw data
            const auto raw = display::color_to_raw(col);

            // clear using the raw value
            clear(raw);
        }
    };
}

#endif