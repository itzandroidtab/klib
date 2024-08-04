#ifndef KLIB_BU97960_HPP
#define KLIB_BU97960_HPP

#include <cstdint>

#include <klib/vector2.hpp>
#include <klib/graphics/color.hpp>

namespace klib::hardware::display {
    template <typename Bus, uint8_t Address = 0x3e, uint32_t Width = 18, uint32_t Height = 4, uint32_t XOffset = 0, uint32_t YOffset = 0>
    class bu97960 {
    public:
        // width of the display
        constexpr static uint32_t width = Width;

        // height of the display
        constexpr static uint32_t height = Height;

        // mode used in the display
        constexpr static graphics::mode mode = graphics::mode::mono;

    protected:
        // check if we have valid width and height
        static_assert((width != 0) || (height != 0), "Screen needs to have a valid size");
        static_assert(((width <= 15) && (height <= 8)) || (((width <= 18) && (height <= 4))), "Screen needs to have a valid size");

        // internal cursor for writing to the display buffer. Used in the framebuffer implementation
        static inline klib::vector2u start_pos = {};
        static inline klib::vector2u end_pos = {};
        static inline klib::vector2u cursor = {};

        /**
         * @brief All commands for the display. Command bit is already set for the
         * appropriate commands
         *
         */
        enum class cmd: uint8_t {
            adset = 0x00,
            evrset = 0xc0,
            disctl = 0xe2,
            icset = 0xf0,
            apctl = 0xf8,
            modeset = 0xfc
        };

        /**
         * @brief Available frequencies
         *
         * @details format of the value:
         * 0bx00 = p3 of disctl
         * 0b0x0 = p2 of disctl
         * 0b00x = FRSEL of modeset
         */
        enum class hz: uint8_t {
            hz_80 = 0b000,
            hz_71 = 0b010,
            hz_64 = 0b100,
            hz_50 = 0b110,
            hz_233 = 0b001,
            hz_197 = 0b011,
            hz_160 = 0b101,
            hz_122 = 0b111,
        };

        /**
         * @brief Available duty cycle and bias modes
         *
         */
        enum class duty_bias: bool {
            duty_1_8_bias_1_4 = 0,
            duty_1_4_bias_1_3 = 1,
        };

        // internal buffer to store all the pixels + the write display command. Display
        // has a internal 120 bit ddr ram
        static inline uint8_t framebuffer[(120 / 8) + 1] = {};

    public:
        template <hz Freq = hz::hz_233, duty_bias DutyBias = duty_bias::duty_1_4_bias_1_3>
        static void init() {
            // init commands
            const uint8_t init_cmd[] = {
                // ICSET reset the display
                (static_cast<uint8_t>(cmd::icset) | (0x1 << 1)),

                // DISCTL with option 0x00 for 233hz (80hz if FRSEL is
                // not set in MODESET)
                static_cast<uint8_t>(cmd::disctl) | (static_cast<uint8_t>(Freq) >> 1),

                // EVRSET with option 0x00
                static_cast<uint8_t>(cmd::evrset),

                // MODESET with option 200hz and 1/4 duty and 1/3 bias
                static_cast<uint8_t>(cmd::modeset) | ((static_cast<uint8_t>(Freq) & 0x1) << 1) |
                (static_cast<uint8_t>(DutyBias) & 0x1),

                // ADSET set the start address
                static_cast<uint8_t>(cmd::adset),

                // clear the display data
                0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
            };

            // send the reset sequence
            Bus::write(Address, init_cmd, sizeof(init_cmd));

            // enable command to turn on the display
            const uint8_t enable[] {
                // turn on the display
                (static_cast<uint8_t>(cmd::icset) | 0x1)
            };

            Bus::write(Address, enable, sizeof(enable));

            // clear the cursor
            cursor = {0, 0};
        }

        static void set_cursor(const klib::vector2u &start_pos, const klib::vector2u &end_pos) {
            // update the internal positions
            start_pos = start_pos;
            end_pos = end_pos;
        }

        static void start_write() {
            // do nothing
            cursor = start_pos;
        }

        static void raw_write(const uint8_t *const data, const uint32_t size) {
            // raw write still uses buffer. Write the data to the buffer first. Update the
            // display afterwards as we only have a max of 15 bytes. (+ 8 as the first byte
            // is the adset command)
            constexpr uint32_t offset = ((YOffset * width) + XOffset) + 8;

            // counter with the amount of pixels we have written. Using the size
            // to limit the amount of pixels written
            uint32_t count = 0;

            // write data to local framebuffer
            for (uint32_t i = 0; i < (size * 8); i++) {
                // get the current pixel
                const bool bit = (data[i / 8] >> (i % 8)) & 0x1;

                // get the framebuffer index
                uint32_t index = (cursor.y * width) + cursor.x;

                // set the data into the framebuffer
                if (bit) {
                    framebuffer[index] |= 0x1 << (index % 8);
                }
                else {
                    framebuffer[index] &= ~(0x1 << (index % 8));
                }

                // increment the cursor
                cursor.x++;

                if (cursor.x > end_pos.x) {
                    cursor.x = start_pos.x;
                    cursor.y++;

                    if (cursor.y > end_pos.y) {
                        cursor.y = start_pos.y;
                    }
                }
            }

            const uint32_t index = ((((start_pos.y * width) + start_pos.x) + 7) / 8);

            // move the adset command before the first data byte (- 1 to move before the first written index)
            framebuffer[index - 1] = static_cast<uint8_t>(cmd::adset);

            // amount of data we need to send over the bus (+ 1 for the command)
            const uint32_t tx_size = 1 + ((index * 8) + height * width + 7) / 8;

            // only write the pixels we have in this dislay
            Bus::write(Address, &framebuffer[index - 1], tx_size);
        }

        static void end_write() {
            // do nothing
        }

        constexpr static uint32_t position_to_buffer(const klib::vector2u &pos) {
            // convert the xy position to a position in a array
            return (pos.y * width) + pos.x;
        }
    };

    /**
     * @brief Using for the a TN 3x7 segment display
     *
     * @tparam Bus
     * @tparam Address
     */
    template <typename Bus, uint8_t Address = 0x3e>
    using bu97960_3x7segment = bu97960<Bus, Address, 6, 4, 2, 13>;
}

#endif