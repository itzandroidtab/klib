#ifndef KLIB_BITMAP_HPP
#define KLIB_BITMAP_HPP

#include <cstdint>

namespace klib::graphics {
    template <uint32_t Width, uint32_t Height>
    class mono_bitmap {
    protected:
        // raw size. used for size of the array
        constexpr static uint32_t raw_size = ((Width * Height) + 7) / 8;

    public:
        // array to store the bitmap (const as the image cannot change)
        const uint8_t data[raw_size];

        /**
         * @brief Get the pixel data of a position 
         * 
         * @param x 
         * @param y 
         * @return bool set or unset
         */
        constexpr bool get_pixel(const uint32_t x, const uint32_t y) const {
            // calculate the main part of the index
            const uint32_t t = (y * Width + x);

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
                    if (((position.y + y) < 0 || (position.y + y) > static_cast<int32_t>(framebuffer.height)) || 
                        ((position.x + x) < 0 || (position.x + x) > static_cast<int32_t>(framebuffer.width)))
                    {
                        // skip the pixel if it outside of the framebuffer
                        continue;
                    }

                    // check if the pixel is set or not
                    const bool is_set = get_pixel(x, y);

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