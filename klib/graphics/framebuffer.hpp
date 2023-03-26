#ifndef KLIB_FRAMEBUFFER_HPP
#define KLIB_FRAMEBUFFER_HPP

#include <cstdint>

#include <klib/vector2.hpp>
#include <klib/graphics/color.hpp>

namespace klib::graphics {
    /**
     * @brief Framebuffer with a different start position in a another framebuffer
     * 
     * @tparam FrameBuffer 
     * @tparam XStart 
     * @tparam YStart 
     */
    template <typename FrameBuffer, uint32_t XStart, uint32_t YStart>
    class sub_framebuffer {
    public:
        // mode for the framebuffer
        constexpr static graphics::mode mode = FrameBuffer::mode;

    protected:
        FrameBuffer &fb;

        // color mode with all parameters
        using color_mode = graphics::detail::pixel_conversion<mode>;

        // color type
        using color_type = color_mode::type;

    public:
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

        constexpr void set_pixel(const klib::vector2u position, const color_type raw) {
            // move the position with the offset
            const klib::vector2u sub_position = (
                position + klib::vector2u{XStart, YStart}
            );

            fb.set_pixel(sub_position, raw);
        }

        constexpr void set_pixel(const klib::vector2u position, const klib::graphics::color &col) {
            // convert the color to raw
            const auto raw = graphics::detail::color_to_raw<mode>(col);

            // set the pixel using the set_pixel
            set_pixel(position, raw);
        }

        constexpr void clear(const color_type raw) {
            // clear using raw value
            fb.clear(raw);
        }

        constexpr void clear(const klib::graphics::color &col) {
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
    public:
        // mode for the framebuffer
        constexpr static graphics::mode mode = FrameBuffer::mode;

    protected:
        FrameBuffer &fb;

        // make sure we have a positive scale factor
        static_assert((XScale >= 1) && (YScale >= 1), "Framebuffer scale factor cannot be 0");

        // color mode with all parameters
        using color_mode = graphics::detail::pixel_conversion<mode>;

        // color type
        using color_type = color_mode::type;

    public:
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

        constexpr void set_pixel(const klib::vector2u position, const color_type raw) {
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

        constexpr void set_pixel(const klib::vector2u position, const klib::graphics::color &col) {
            // convert the color to raw
            const auto raw = graphics::detail::color_to_raw<mode>(col);

            // set the pixel using the set_pixel
            set_pixel(position, raw);
        }

        constexpr void clear(const color_type raw) {
            // clear using raw value
            fb.clear(raw);
        }

        constexpr void clear(const klib::graphics::color &col) {
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
    public:
        // mode for the framebuffer
        constexpr static graphics::mode mode = FrameBuffer::mode;

    protected:
        FrameBuffer &fb;

        // color mode with all parameters
        using color_mode = graphics::detail::pixel_conversion<mode>;

        // color type
        using color_type = color_mode::type;

    public:
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

        constexpr void set_pixel(const klib::vector2u position, const color_type raw) {
            // mirror using the parameters
            if constexpr (XMirror && YMirror) {
                const klib::vector2u pos = (klib::vector2u(
                    (FrameBuffer::width - 1), (FrameBuffer::height - 1)) - position
                );

                fb.set_pixel(pos, raw);
            }
            else if constexpr (XMirror) {
                const klib::vector2u pos = (klib::vector2u(
                    (FrameBuffer::width - 1) - position.x, position.y)
                );

                fb.set_pixel(pos, raw);
            }
            else if constexpr (YMirror) {
                const klib::vector2u pos = (klib::vector2u(
                    position.x, (FrameBuffer::height - 1) - position.y)
                );

                fb.set_pixel(pos, raw);
            }
            else {
                fb.set_pixel(position, raw);
            }
        }

        constexpr void set_pixel(const klib::vector2u position, const klib::graphics::color &col) {
            // convert the color to raw
            const auto raw = graphics::detail::color_to_raw<mode>(col);

            // set the pixel using the mirror set_pixel
            set_pixel(position, raw);
        }

        constexpr void clear(const color_type raw) {
            // clear using raw value
            fb.clear(raw);
        }

        constexpr void clear(const klib::graphics::color &col) {
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

        // mode for the framebuffer
        constexpr static graphics::mode mode = Display::mode;

    protected:
        // cache for the where the cursor is located
        klib::vector2u cursor;
    
        // make sure the input is valid
        static_assert(EndX <= Display::width, "Framebuffer EndX cannot be bigger than the display width");
        static_assert(EndY <= Display::height, "Framebuffer EndY cannot be bigger than the display height");

        // color mode with all parameters
        using color_mode = graphics::detail::pixel_conversion<mode>;

        // color type
        using color_type = color_mode::type;

    public:
        constexpr void init() {
            // update the position of the cursor in the display
            Display::set_cursor({StartX, StartY}, {EndX - 1, EndY - 1});

            // clear the cursor
            cursor = {0, 0};
        }

        constexpr void flush() {
            // not used in the direct framebuffer
        }

        constexpr void set_pixel(const klib::vector2u position, const color_type raw) {
            // limit the input to the display size
            const auto pos = klib::vector2u(
                StartX + klib::min(position.x, width - 1), 
                StartY + klib::min(position.y, height - 1)
            );

            // check if we can optimize the set cursor command
            if constexpr (AutoIncrement) {
                // update the cursor on the display if it doesnt match
                if (cursor != pos) {
                    // end the write before setting the cursor
                    Display::end_write();

                    // update the cursor on the display
                    Display::set_cursor(pos, {width - 1, height - 1});

                    // start the write after we change the cursor
                    Display::start_write();
                }
            }
            else {
                // end the write before setting the cursor
                Display::end_write();

                // set the position every time we send raw pixel data
                Display::set_cursor(pos, {width - 1, height - 1});

                // start the write after we change the cursor
                Display::start_write();
            }

            // write the raw data to the screen
            Display::raw_write(reinterpret_cast<const uint8_t*>(&raw), sizeof(raw));

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

        constexpr void set_pixel(const klib::vector2u position, const klib::graphics::color &col) {
            // check if the pixel is transparant. Skip if it is
            if (col.transparant) {
                return;
            }
            
            // convert using the display conversion function
            const auto raw = graphics::detail::color_to_raw<mode>(col);

            // set the raw value on position
            set_pixel(position, raw);
        }

        constexpr void clear(const color_type raw) {
            // write the raw value to every pixel
            for (uint32_t y = 0; y < height; y++) {
                for (uint32_t x = 0; x < width; x++) {
                    // set the raw value on position
                    set_pixel({x, y}, raw);
                }
            }
        }

        constexpr void clear(const klib::graphics::color &col) {
            // check if the pixel is transparant. Skip if it is
            if (col.transparant) {
                return;
            }

            // conver the color to raw data
            const auto raw = graphics::detail::color_to_raw<mode>(col);

            // clear using the raw value
            clear(raw);
        }
    };

    /**
     * @brief Framebuffer. Buffers data before it is flushed to the display. 
     * Converts the the color mode from the framebuffer to the display color 
     * mode. The speed is decreased when using a different color mode than 
     * the display color mode.
     * 
     * @details Makes use of the feature to automaticly increment the cursor 
     * of the display after pixel data is written. This allows the spi bus to 
     * only transmit pixel data without other data in between.
     * 
     * @tparam Display 
     * @tparam Mode 
     * @tparam StartX 
     * @tparam StartY 
     * @tparam EndX 
     * @tparam EndY 
     */
    template <
        typename Display, graphics::mode Mode,
        uint32_t StartX = 0, uint32_t StartY = 0, 
        uint32_t EndX = Display::width, 
        uint32_t EndY = Display::height
    >
    class framebuffer {
    public:
        constexpr static uint32_t width = EndX - StartX;
        constexpr static uint32_t height = EndY - StartY;

        // mode used in the framebuffer.
        constexpr static graphics::mode mode = Mode;

    protected:
        // color mode with all parameters
        using color_mode = graphics::detail::pixel_conversion<Mode>;

        // color type
        using color_type = color_mode::type;

        // get the amount of bytes for the screen buffer
        constexpr static uint32_t byte_count = ((width * height) * color_mode::bits + 7) / 8;

        // buffer to store all the pixel data
        uint8_t buffer[byte_count];

        // flag if we are in native mode
        constexpr static bool native_mode = (Mode == Display::mode);

        // make sure the input is valid
        static_assert(EndX <= Display::width, "Framebuffer EndX cannot be bigger than the display width");
        static_assert(EndY <= Display::height, "Framebuffer EndY cannot be bigger than the display height");

        // make sure the type has at least 1 bit and at max 32 bits
        static_assert(color_mode::bits > 0, "Framebuffer only supports modes with at least 1 bit");
        static_assert(color_mode::bits <= 32, "Framebuffer only supports modes a maximum of 32 bits");

        // if the amount of bits is not a modulo of 8. We only support 
        // native mode. If not in native mode we only support output 
        // formats that have a modulo of 8. If we do not block this we
        // will write empty data in the remainder of the data in the pixel
        static_assert(
            (native_mode && ((graphics::detail::pixel_conversion<Display::mode>::bits % 8) != 0)) || 
            ((graphics::detail::pixel_conversion<Display::mode>::bits % 8) == 0), 
            "Framebuffer does not support this output mode for the input mode"
        );

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
            Display::set_cursor(
                klib::vector2u{StartX, StartY}, 
                klib::vector2u{EndX - 1, EndY - 1}
            );

            // start the display write
            Display::start_write();

            // write using the mode we have
            if constexpr (native_mode) {
                // we should be able to write the native stream 
                // to the display
                Display::raw_write(buffer, sizeof(buffer));
            }
            else {
                constexpr auto mode_bits = graphics::detail::pixel_conversion<Display::mode>::bits;

                // get the amount of bytes needed for the pixel
                constexpr uint32_t step = ((mode_bits + 7) / 8);

                // we need to convert all the colors by getting all the data 
                // first. We need to shift to get all the data
                for (uint32_t i = 0; i < (width * height); i++) {
                    // place to store the raw data
                    color_type buffer_raw = 0;

                    // check if we can get all the bits in a single read. This
                    // allows us to optimize getting the data as we do not have
                    // to shift
                    if constexpr ((color_mode::bits % 8) == 0) {
                        // get the amount of bytes needed for the pixel
                        constexpr uint32_t step = ((color_mode::bits + 7) / 8);

                        // merge the values into a single variable
                        for (uint32_t j = 0; j < step; j++) {
                            // add the buffer data
                            buffer_raw |= buffer[i * step + j] << (((step - 1) * 8) - (j * 8));
                        }
                    }
                    else {
                        // get the amount of bits before the current pixel
                        const uint32_t bits = i * color_mode::bits;

                        // get all the bytes
                        for (uint32_t j = 0; j < color_mode::bits; ) {
                            const uint32_t index = ((bits + j) / 8);
                            const uint32_t shift = ((bits + j) % 8);

                            // check how many bits we can update
                            const uint32_t update = klib::min(8 - shift, (color_mode::bits - j));

                            // get the mask where we want to write
                            const uint32_t mask = (klib::exp2(update) - 1);

                            // shift the data
                            buffer_raw |= ((buffer[index] >> (8 - (shift + update))) & mask) << (color_mode::bits - j - update);

                            // move with the amount of bits we have read
                            j += update;
                        }
                    }

                    // we are not in native mode. Convert the color to the 
                    // display color
                    const klib::graphics::color col = (
                        graphics::detail::raw_to_color<Mode>(buffer_raw)
                    );

                    // convert the color to the native color
                    const auto raw = (
                        graphics::detail::color_to_raw<Display::mode>(col)
                    );

                    uint8_t data[step];

                    // reorder the data to match the stream orientation
                    for (uint32_t i = 0; i < step; i++) {
                        data[(step - 1) - i] = (reinterpret_cast<const uint8_t *const>(&raw))[i];
                    }

                    // write the data to the display
                    Display::raw_write(data, step);
                }
            }

            // stop the write to the display
            Display::end_write();
        }

        /**
         * @brief Set a pixel in the framebuffer (raw pixel data). 
         * 
         * @param position 
         * @param raw 
         */
        constexpr void set_pixel(const klib::vector2u position, const color_type raw) {
            // set the pixel in the native format. The native format on most 
            // displays as follows: MSB -> LSB
            // as we are storing in little endian format we need to convert
            // the data to the correct stream format.

            // for example a 12 bit color format should be formatted like this:
            // [12, 11, 10, 9, 8, 7, 6, 5], [4, 3, 2, 1, x, x, x, x]

            // check if we can optimize the setting of the data
            if constexpr ((color_mode::bits % 8) == 0) {
                constexpr uint32_t bytes = ((color_mode::bits + 7) / 8);

                // index to store the data
                const auto index = ((position.y * width) + position.x) * bytes;

                // store the data in big endian format. We do this so we can send 
                // the data directly to the display in the native format
                for (uint32_t i = 0; i < bytes; i++) {
                    buffer[index + i] = (raw >> (((bytes - 1) * 8) - (i * 8))) & 0xff;
                }
            }
            else {
                // get the amount of bits before the current pixel
                const uint32_t bits = ((position.y * width) + position.x) * color_mode::bits;

                // set all the bits
                for (uint32_t i = 0; i < color_mode::bits; ) {
                    const uint32_t index = ((bits + i) / 8);
                    const uint32_t shift = ((bits + i) % 8);

                    // check how many bits we can update
                    const uint32_t update = klib::min(8 - shift, (color_mode::bits - i));

                    // get the mask where we want to write
                    const uint32_t mask = (klib::exp2(update) - 1);

                    // clear the bits
                    buffer[index] &= ~(mask << (8 - (shift + update)));

                    // set the bits
                    buffer[index] |= ((raw >> ((color_mode::bits - i) - update)) & mask) << (8 - (shift + update));

                    // update the bit index
                    i += update;
                }
            }
        }

        /**
         * @brief Set a pixel in the framebuffer (klib color)
         * 
         * @param position 
         * @param col 
         */
        constexpr void set_pixel(const klib::vector2u position, const klib::graphics::color &col) {
            // check if the pixel is transparant. Skip if it is
            if (col.transparant) {
                return;
            }

            // set the pixel value using the raw value
            set_pixel(position, graphics::detail::color_to_raw<Mode>(col));
        }

        /**
         * @brief Clear the framebuffer with the specified color (raw pixel data)
         * 
         * @param raw 
         */
        constexpr void clear(const color_type raw) {
            // write the data to every pixel
            for (uint32_t y = 0; y < height; y++) {
                for (uint32_t x = 0; x < width; x++) {
                    set_pixel({x, y}, raw);
                }
            }
        }

        /**
         * @brief Clear the framebuffer with the specified color (klib color)
         * 
         * @param raw 
         */
        constexpr void clear(const klib::graphics::color &col) {
            // check if the pixel is transparant. Skip if it is
            if (col.transparant) {
                return;
            }

            // clear the screen using the raw value
            clear(graphics::detail::color_to_raw<Mode>(col));
        }
    };
}

#endif