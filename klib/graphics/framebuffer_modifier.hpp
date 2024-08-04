#ifndef KLIB_FRAMEBUFFER_MODIFIER_HPP
#define KLIB_FRAMEBUFFER_MODIFIER_HPP

#include <cstdint>

#include <klib/vector2.hpp>
#include <klib/graphics/color.hpp>

#include "framebuffer.hpp"

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
     * @brief Movable version of the framebuffer. Only difference is that the
     * flush function allows overwriting the start position
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
        uint32_t StartX = 0,
        uint32_t StartY = 0,
        uint32_t EndX = Display::width,
        uint32_t EndY = Display::height,
        std::endian Endian = std::endian::native
    >
    class movable_framebuffer: public framebuffer<Display, Mode, StartX, StartY, EndX, EndY, Endian> {
    public:
        constexpr void flush(const klib::vector2u position) {
            // set the cursor to the start of the display
            Display::set_cursor(
                position + klib::vector2u{StartX, StartY},
                position + klib::vector2u{EndX - 1, EndY - 1}
            );

            // start the display write
            Display::start_write();

            // call the flush implementation
            framebuffer<Display, Mode, StartX, StartY, EndX, EndY, Endian>::flush_impl();

            // stop the write to the display
            Display::end_write();
        }
    };
}

#endif