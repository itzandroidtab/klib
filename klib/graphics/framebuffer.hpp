#ifndef KLIB_FRAMEBUFFER_HPP
#define KLIB_FRAMEBUFFER_HPP

#include <cstdint>

#include <klib/vector2.hpp>

#include "color.hpp"

namespace klib {
    /**
     * @brief Framebuffer with a different start position in a another framebuffer
     * 
     * @tparam FrameBuffer 
     * @tparam XStart 
     * @tparam YStart 
     */
    template <typename FrameBuffer, uint32_t XStart, uint32_t YStart>
    class sub_framebuffer {
    protected:
        FrameBuffer &fb;

    public:
        using display = FrameBuffer::display;

        constexpr sub_framebuffer(FrameBuffer &fb):
            fb(fb)
        {}

        constexpr void init() {
            // call the framebuffer init
            fb.init();
        }

        constexpr void flush() {
            // call the framebuffer flush
            fb.flush();
        }

        constexpr void set_pixel(const klib::vector2u position, const display::pixel_type raw) {
            // move the position with the offset
            const klib::vector2u sub_position = (
                position + klib::vector2u{XStart, YStart}
            );

            fb.set_pixel(sub_position, raw);
        }

        constexpr void set_pixel(const klib::vector2u position, const klib::color &col) {
            // convert the color to raw
            const auto raw = display::color_to_raw(col);

            // set the pixel using the set_pixel
            set_pixel(position, raw);
        }

        constexpr void clear(const display::pixel_type raw) {
            // clear using raw value
            fb.clear(raw);
        }

        constexpr void clear(const klib::color &col) {
            // clear using color
            fb.clear(col);
        }
    };

    /**
     * @brief Framebuffer that scales input into multiple pixels to another framebuffer
     * 
     * @tparam FrameBuffer 
     * @tparam XScale 
     * @tparam YScale 
     */
    template <typename FrameBuffer, uint32_t XScale, uint32_t YScale>
    class scaled_framebuffer {
    protected:
        FrameBuffer &fb;

        // make sure we have a positive scale factor
        static_assert((XScale >= 1) && (YScale >= 1), "Framebuffer scale factor cannot be 0");

    public:
        using display = FrameBuffer::display;

        constexpr scaled_framebuffer(FrameBuffer &fb):
            fb(fb)
        {}

        constexpr void init() {
            // call the framebuffer init
            fb.init();
        }

        constexpr void flush() {
            // call the framebuffer flush
            fb.flush();
        }

        constexpr void set_pixel(const klib::vector2u position, const display::pixel_type raw) {
            // convert to the scaled position
            const klib::vector2u scaled_position = (
                position * klib::vector2u{XScale, YScale}
            );

            // scale every pixel scale amount of times
            for (uint32_t y = 0; y < YScale; y++) {
                for (uint32_t x = 0; x < XScale; x++) {
                    // set the pixel multiple times
                    fb.set_pixel(scaled_position + klib::vector2u{x, y}, raw);
                }
            }
        }

        constexpr void set_pixel(const klib::vector2u position, const klib::color &col) {
            // convert the color to raw
            const auto raw = display::color_to_raw(col);

            // set the pixel using the set_pixel
            set_pixel(position, raw);
        }

        constexpr void clear(const display::pixel_type raw) {
            // clear using raw value
            fb.clear(raw);
        }

        constexpr void clear(const klib::color &col) {
            // clear using color
            fb.clear(col);
        }
    };

    /**
     * @brief Framebuffer that can flip another framebuffer. Uses all the 
     * parameters from the original framebuffer. Can be flipped in the x-axis
     * and the y-axis independently.
     * 
     * @tparam FrameBuffer 
     * @tparam XMirror 
     * @tparam YMirror 
     */
    template <typename FrameBuffer, bool XMirror, bool YMirror>
    class flipped_framebuffer {
    protected:
        FrameBuffer &fb;

    public:
        using display = FrameBuffer::display;

        constexpr flipped_framebuffer(FrameBuffer &fb):
            fb(fb)
        {}

        constexpr void init() {
            // call the framebuffer init
            fb.init();
        }

        constexpr void flush() {
            // call the framebuffer flush
            fb.flush();
        }

        constexpr void set_pixel(const klib::vector2u position, const display::pixel_type raw) {
            // mirror using the parameters
            if constexpr (XMirror && YMirror) {
                const klib::vector2u pos = (klib::vector2u(
                    (display::width - 1), (display::height - 1)) - position
                );

                fb.set_pixel(pos, raw);
            }
            else if constexpr (XMirror) {
                const klib::vector2u pos = (klib::vector2u(
                    (display::width - 1) - position.x, position.y)
                );

                fb.set_pixel(pos, raw);
            }
            else if constexpr (YMirror) {
                const klib::vector2u pos = (klib::vector2u(
                    position.x, (display::height - 1) - position.y)
                );

                fb.set_pixel(pos, raw);
            }
            else {
                fb.set_pixel(position, raw);
            }
        }

        constexpr void set_pixel(const klib::vector2u position, const klib::color &col) {
            // convert the color to raw
            const auto raw = display::color_to_raw(col);

            // set the pixel using the mirror set_pixel
            set_pixel(position, raw);
        }

        constexpr void clear(const display::pixel_type raw) {
            // clear using raw value
            fb.clear(raw);
        }

        constexpr void clear(const klib::color &col) {
            // clear using color
            fb.clear(col);
        }
    };

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
     * @tparam StartX 
     * @tparam StartY 
     * @tparam EndX 
     * @tparam EndY 
     */

    template <
        typename Display, bool AutoIncrement = true, 
        uint32_t StartX = 0, uint32_t StartY = 0, 
        uint32_t EndX = Display::width, 
        uint32_t EndY = Display::height
    >
    class direct_framebuffer {
    public:
        constexpr static uint32_t width = EndX - StartX;
        constexpr static uint32_t height = EndY - StartY;

        // using to export the display conversion functions
        using display = Display;

    protected:
        // cache for the where the cursor is located
        klib::vector2u cursor;
    
        // make sure the input is valid
        static_assert(EndX <= Display::width, "Framebuffer EndX cannot be bigger than the display width");
        static_assert(EndY <= Display::height, "Framebuffer EndY cannot be bigger than the display height");

    public:
        constexpr void init() {
            // update the position of the cursor in the display
            display::set_cursor({StartX, StartY}, {EndX - 1, EndY - 1});

            // clear the cursor
            cursor = {0, 0};
        }

        constexpr void flush() {
            // not used in the direct framebuffer
        }

        constexpr void set_pixel(const klib::vector2u position, const display::pixel_type raw) {
            // limit the input to the display size
            const auto pos = klib::vector2u(
                StartX + klib::min(position.x, width - 1), 
                StartY + klib::min(position.y, height - 1)
            );

            // check if we can optimize the set cursor command
            if constexpr (AutoIncrement) {
                // update the cursor on the display if it doesnt match
                if (cursor != pos) {
                    // update the cursor on the display
                    display::set_cursor(pos, {width - 1, height - 1});
                }
            }
            else {
                // set the position every time we send raw pixel data
                display::set_cursor(pos, {width - 1, height - 1});
            }

            // write the raw data to the screen
            display::raw_write_screen(&raw, 1);

            if constexpr (AutoIncrement) {
                // update the cursor on the screen
                if ((pos.x + 1) >= EndX) {
                    // move the y cursor
                    if ((pos.y + 1) >= EndY) {
                        // move the cursor back to the start
                        cursor = {StartX, StartY};
                    }
                    else {
                        // move the cursor back to the StartX'
                        cursor = {StartX, pos.y + 1};
                    }
                }
                else {
                    cursor = pos + klib::vector2u(1, 0);
                }
            }
        }

        constexpr void set_pixel(const klib::vector2u position, const klib::color &col) {
            // check if the pixel is transparant. Skip if it is
            if (col.transparant) {
                return;
            }
            
            // convert using the display conversion function
            const auto raw = display::color_to_raw(col);

            // set the raw value on position
            set_pixel(position, raw);
        }

        constexpr void clear(const display::pixel_type raw) {
            // write the raw value to every pixel
            for (uint32_t y = 0; y < height; y++) {
                for (uint32_t x = 0; x < width; x++) {
                    // set the raw value on position
                    set_pixel({x, y}, raw);
                }
            }
        }

        constexpr void clear(const klib::color &col) {
            // check if the pixel is transparant. Skip if it is
            if (col.transparant) {
                return;
            }

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
     * @tparam Display 
     * @tparam StartX 
     * @tparam StartY 
     * @tparam EndX 
     * @tparam EndY 
     */
    template <
        typename Display, uint32_t StartX = 0, uint32_t StartY = 0, 
        uint32_t EndX = Display::width, uint32_t EndY = Display::height
    >
    class framebuffer {
    public:
        constexpr static uint32_t width = EndX - StartX;
        constexpr static uint32_t height = EndY - StartY;

        // using to export the display conversion functions
        using display = Display;

    protected:
        // framebuffer to store the image
        Display::pixel_type buffer[width * height];

        // make sure the input is valid
        static_assert(EndX <= Display::width, "Framebuffer EndX cannot be bigger than the display width");
        static_assert(EndY <= Display::height, "Framebuffer EndY cannot be bigger than the display height");

    public:
        /**
         * @brief Init the framebuffer. Not used in 
         * the buffered framebuffer
         * 
         */
        constexpr void init() {
            // do nothing
        }

        /**
         * @brief Flush the buffer to the display
         * 
         */
        constexpr void flush() {
            // set the cursor to the start of the display
            display::set_cursor(
                klib::vector2u{StartX, StartY}, 
                klib::vector2u{EndX - 1, EndY - 1}
            );

            // write the buffer to the screen
            display::raw_write_screen(buffer, width * height);
        }

        /**
         * @brief Set a pixel in the framebuffer (raw pixel data). 
         * 
         * @param position 
         * @param raw 
         */
        constexpr void set_pixel(const klib::vector2u position, const Display::pixel_type raw) {
            // limit the input to the display size
            const auto pos = klib::vector2u(
                klib::min(position.x, width - 1), 
                klib::min(position.y, height - 1)
            );

            // set the data in the buffer based on the display position function
            buffer[Display::position_to_buffer(pos)] = raw;
        }

        /**
         * @brief Set a pixel in the framebuffer (klib color)
         * 
         * @param position 
         * @param col 
         */
        constexpr void set_pixel(const klib::vector2u position, const klib::color &col) {
            // check if the pixel is transparant. Skip if it is
            if (col.transparant) {
                return;
            }
            
            // convert using the display conversion function
            const auto raw = Display::color_to_raw(col);

            // set the pixel using the raw value
            set_pixel(position, raw);
        }

        /**
         * @brief Clear the framebuffer with the specified color (raw pixel data)
         * 
         * @param raw 
         */
        constexpr void clear(const Display::pixel_type raw) {
            // get the amount of pixels we need to clear
            constexpr uint32_t c = width * height;

            // iterate on all the pixels
            for (uint32_t i = 0; i < c; i++) {
                // convert the color to a pixel
                buffer[i] = raw;
            }
        }

        /**
         * @brief Clear the framebuffer with the specified color (klib color)
         * 
         * @param raw 
         */
        constexpr void clear(const klib::color &col) {
            // check if the pixel is transparant. Skip if it is
            if (col.transparant) {
                return;
            }

            // conver the color to raw data
            const auto raw = Display::color_to_raw(col);

            // clear using the raw value
            clear(raw);
        }
    };
}

#endif