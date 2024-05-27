#ifndef KLIB_BITMAP_HPP
#define KLIB_BITMAP_HPP

#include <cstdint>

#include <klib/vector2.hpp>

#include "color.hpp"

namespace klib::graphics {
    /**
     * @brief Bitmap for colors. Colors with less than 8 bits use 8 bits
     * for every pixel
     * 
     * @tparam Width 
     * @tparam Height 
     * @tparam Mode 
     */
    template <uint32_t Width, uint32_t Height, mode Mode>
    class bitmap {
    protected:
        // color mode with all parameters
        using color_mode = graphics::detail::pixel_conversion<Mode>;

        // Size used for the array
        constexpr static uint32_t size = (Width * Height);

    public:
        // array to store the bitmap
        color_mode::type data[size];

        /**
         * @brief Get the pixel data of a position 
         * 
         * @param x 
         * @param y 
         * @return bool set or unset
         */
        constexpr color_mode::type get_pixel(const klib::vector2u position) const {
            // index to store the data
            const auto index = ((position.y * width) + position.x);

            // return the index
            return data[index];
        }

    public:
        // width and height of the icon
        constexpr static uint32_t width = Width;
        constexpr static uint32_t height = Height;

        /**
         * @brief Construct a new bitmap using a array
         * 
         * @param data 
         */
        template <typename... T> 
        constexpr bitmap(const klib::graphics::color arg, const T... args):
            data{detail::color_to_raw<Mode>(arg), detail::color_to_raw<Mode>(args)...}
        {
            // make sure the input size matches the amount of data we are expecting
            static_assert((sizeof...(T) + 1) == (Width * Height), "Parameter size does not match");
        }

        /**
         * @brief Construct a new bitmap using a array
         * 
         * @param data 
         */
        template <typename... T> 
        constexpr bitmap(const color_mode::type arg, const T... args):
            data{arg, args...}
        {
            // make sure the input size matches the amount of data we are expecting
            static_assert((sizeof...(T) + 1) == (Width * Height), "Parameter size does not match");
        }

        /**
         * @brief Construct a empty bitmap
         * 
         */
        constexpr bitmap(): data{} {}

        /**
         * @brief Draw the bitmap to the framebuffer
         * 
         * @tparam Fb 
         * @param framebuffer 
         * @param position 
         * @param foreground 
         * @param background 
         */
        template <typename Fb>
        void draw(Fb& framebuffer, const klib::vector2i& position) const {
            // draw every pixel in the bitmap
            for (int32_t y = 0; y < height; y++) {
                for (int32_t x = 0; x < width; x++) {
                    // make sure the position is inside the framebuffer
                    if (((position.y + y) < 0 || (position.y + y) >= static_cast<int32_t>(framebuffer.height)) || 
                        ((position.x + x) < 0 || (position.x + x) >= static_cast<int32_t>(framebuffer.width)))
                    {
                        // skip the pixel if it outside of the framebuffer
                        continue;
                    }

                    // check if the pixel is set or not
                    const auto pixel = get_pixel(klib::vector2i{x, y}.cast<uint32_t>());

                    // get the position
                    const auto pos = position + klib::vector2i{x, y};

                    // set the pixel
                    framebuffer.set_pixel(pos.cast<uint32_t>(), pixel);
                }
            }
        }
    };

    /**
     * @brief Bitmap for mono images (1 bit per pixel). Uses optimisation to 
     * reduce space. This adds overhead on runtime
     * 
     * @tparam Width 
     * @tparam Height 
     */
    template <uint32_t Width, uint32_t Height>
    class mono_bitmap {
    protected:
        // raw size. used for size of the array
        constexpr static uint32_t raw_size = ((Width * Height) + 7) / 8;

    public:
        // array to store the bitmap
        uint8_t data[raw_size];

        /**
         * @brief Get the pixel data of a position 
         * 
         * @param position
         * @return bool set or unset
         */
        constexpr bool get_pixel(const klib::vector2u& position) const {
            // calculate the main part of the index
            const uint32_t t = (position.y * Width + position.x);

            // get the index of the data
            const uint32_t index = t / 8;

            // get the bit position 
            const uint32_t pos = t % 8;

            // return if the bit is set in the bitmap
            return (data[index] >> (7 - pos)) & 0x1;
        }

    public:
        // width and height of the icon
        constexpr static uint32_t width = Width;
        constexpr static uint32_t height = Height;

        /**
         * @brief Construct a new bitmap using a array
         * 
         * @param data 
         */
        template <typename... T> 
        constexpr mono_bitmap(const uint8_t arg, const T... args):
            data{arg, static_cast<uint8_t>(args)...}
        {
            // make sure the input size matches the amount of data we are expecting
            static_assert((sizeof...(T) + 1) == raw_size, "Parameter size does not match");
        }

        /**
         * @brief Construct a empty bitmap
         * 
         */
        constexpr mono_bitmap(): data{} {}

        /**
         * @brief Draw the bitmap to the framebuffer
         * 
         * @tparam Fb 
         * @param framebuffer 
         * @param position 
         * @param foreground 
         * @param background 
         */
        template <typename Fb>
        void draw(Fb& framebuffer, const klib::vector2i& position, const klib::graphics::color foreground, const klib::graphics::color background) const {
            // skip drawing if both colors are transparant
            if (foreground.transparant && background.transparant) {
                return;
            }

            // draw every pixel in the bitmap
            for (int32_t y = 0; y < height; y++) {
                for (int32_t x = 0; x < width; x++) {
                    // make sure the position is inside the framebuffer
                    if (((position.y + y) < 0 || (position.y + y) >= static_cast<int32_t>(framebuffer.height)) || 
                        ((position.x + x) < 0 || (position.x + x) >= static_cast<int32_t>(framebuffer.width)))
                    {
                        // skip the pixel if it outside of the framebuffer
                        continue;
                    }

                    // check if the pixel is set or not
                    const bool is_set = get_pixel(klib::vector2i{x, y}.cast<uint32_t>());

                    // get the position
                    const auto pos = position + klib::vector2i{x, y};

                    // set the pixel
                    framebuffer.set_pixel(pos.cast<uint32_t>(), is_set ? foreground : background);
                }
            }
        }
    };
}

#endif