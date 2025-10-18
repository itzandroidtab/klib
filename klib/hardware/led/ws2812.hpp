#ifndef KLIB_WS2812_HPP
#define KLIB_WS2812_HPP

#include <array>
#include <span>

#include <klib/vector2.hpp>
#include <klib/io/dma.hpp>
#include <klib/graphics/color.hpp>

namespace klib::hardware::led {
    /**
     * @brief Class to set colors on a string of ws2812 leds. This uses the spi 
     * peripheral in dma mode to get the correct timing without using the cpu. 
     * This uses 4 bits per pixel (e.g. 10 leds uses = (((24 * 10 * 4) / 8) + 1) 
     * 121 bytes) in the array and is expecting a 3.75mhz spi clock
     * 
     * Has support for 2 dma channels as some hardware requires a channel for 
     * reading the spi data as well. If not used it can be set to:
     * "klib::io::dma::none"
     * 
     * When enabling Zigzag the string of leds is expected to go:
     * >>>>
     * <<<<
     * >>>>
     * 
     * instead of:
     * >>>>
     * >>>>
     * >>>>
     * 
     * This is only for using the framebuffer mode
     * 
     * @details this simulates a klib "display". This allows us to use all the 
     * graphic functions on a framebuffer and write it to the leds
     * 
     * @tparam DmaTx 
     * @tparam DmaRx 
     * @tparam Width (only used in framebuffer mode)
     * @tparam Height (only used in framebuffer mode)
     * @tparam Zigzag (only used in framebuffer mode)
     */
    template <
        typename DmaTx, typename DmaRx, 
        uint32_t Width = 1, uint32_t Height = 1,
        bool Zigzag = false
    >
    class ws2812_dma {
    public:
        // width of the display
        constexpr static uint32_t width = Width;

        // height of the display
        constexpr static uint32_t height = Height;

        // mode needed for the led pixels
        constexpr static graphics::mode mode = graphics::mode::rgb888;

    protected:
        // color mode with all parameters
        using color_mode = graphics::detail::pixel_conversion<mode>;

        // color type
        using color_type = color_mode::type;

        // rx data for if we have a rx dma
        static inline uint32_t rx;

    public:
        static void init() {
            // do nothing
        }

        /**
         * @brief Write a array of colors to the leds. Uses dma in the 
         * background to transfer all the data. The data needs to be 
         * converted to grb888 beforehand as a byte stream using the
         * 'set_led' function
         * 
         * @param data 
         * @param size 
         */
        static void update_leds(const uint8_t *const data, const uint32_t size) {
            // write the led data to the leds using the raw write
            raw_write(data, size);
        }

        /**
         * @brief Set a color into the raw led array using the position as 
         * the pixel position. Converts the color to the color needed for
         * the ws2812
         * 
         * @param data 
         * @param position 
         * @param raw 
         */
        static void set_led(uint8_t *const data, uint32_t position, const graphics::color col) {
            // indexes of the colors. (Red, Green and Blue)
            const uint8_t positions[(color_mode::bits / 8)] = {8, 16, 0};

            // get the color in the correct raw format
            const uint32_t v = (col.green << 16) | (col.red << 8) | (col.blue);

            // values for the bits (calculated for 3.75 Mhz)
            const uint8_t low = 0b1000;
            const uint8_t high = 0b1110;

            // get the colors from the raw data. The data is at the 24 lsb bits.
            // We shift by 16, 8 and 0 to get these values. 
            for (uint32_t i = 0; i < color_mode::bits; i++) {
                // get the bit from the color (shift from the left for the msb bit)
                const bool bit = (v >> ((color_mode::bits - 1) - i)) & 0x1;

                // get the index we want to read/write to (we have 4 bits per led)
                const uint32_t index = position * ((color_mode::bits * 4) / 8) + (i / 2) + 1;

                // check if we need to set the low or high part of the byte
                const bool high_nible = !(i % 2);

                // ge tthe current value
                const uint8_t value = (
                    data[index] & (~(0b1111 << (4 * high_nible)))
                );

                // set the value in the buffer
                data[index] = value | ((bit ? high : low)) << (4 * high_nible);
            }
        }

    public:
        /**
         * @brief @Helper function for the framebuffers. If directly writing 
         * to the leds this is not needed and should not be used
         * 
         */

        static void raw_write(const uint8_t *const data, const uint32_t size) {
            // check if we have a receive dma channel
            if constexpr (!std::is_same_v<DmaRx, klib::io::dma::none>) {
                // read memory into the rx buffer. Do not increment as we
                // do not care about the result of what we are reading
                DmaRx::template read<false>(std::span<uint8_t>{
                    reinterpret_cast<uint8_t*>(&rx), size
                });
            }

            // write to the dma channel
            DmaTx::template write<true>(std::span<const uint8_t>{data, size});
        }

        static void set_cursor(uint16_t x_min, uint16_t y_min, uint16_t x_max, uint16_t y_max) {    
            // Nothing to do here
        }

        static void set_cursor(const klib::vector2u &start_pos, const klib::vector2u &end_pos) {
            // Nothing to do here
        }

        static void start_write() {
            // Nothing to do here
        }

        static void end_write() {
            // do nothing
        }

        constexpr static uint32_t position_to_buffer(const klib::vector2u &pos) {
            // check if we need to zigzag or not
            if constexpr (Zigzag) {
                // convert the xy position to a position in a array
                if (pos.y % 2 == 0) {
                    // going left to right
                    return (pos.y * width) + pos.x;
                } 
                else {
                    // going right to left
                    return (pos.y * width) + ((width - 1) - pos.x);
                }
            }
            else {
                // convert the xy position to a position in a array
                return (pos.y * width) + pos.x;
            }
        }
    };
}

#endif