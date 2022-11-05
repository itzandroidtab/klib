#ifndef KLIB_ST7735_HPP
#define KLIB_ST7735_HPP

#include <klib/wait.hpp>
#include <klib/vector2.hpp>
#include <klib/color.hpp>

namespace klib::hardware::display {
    template <typename Bus, typename PinDC, typename PinRst, uint32_t Width = 80, uint32_t Height = 160, uint32_t XOffset = 26, uint32_t YOffset = 1>
    class st7735 {
        public:
            // type for framebuffers
            using pixel_type = uint16_t;

            // width of the display
            constexpr static uint32_t width = Width;

            // height of the display
            constexpr static uint32_t height = Height;

        protected:
            // check if we have valid width and height
            static_assert((width != 0) || (height != 0), "Screen needs to have a valid size");

            // all the commands for the display
            enum class cmd: uint8_t {
                nop = 0x00,
                swreset = 0x01,
                rddid = 0x04,
                rddst = 0x09,
                slpin = 0x10,
                slpout = 0x11,
                ptlon = 0x12,
                noron = 0x13,
                invoff = 0x20,
                invon = 0x21,
                dispoff = 0x28,
                dispon = 0x29,
                caset = 0x2A,
                raset = 0x2B,
                ramwr = 0x2C,
                ramrd = 0x2E,
                ptlar = 0x30,
                colmod = 0x3A,
                madctl = 0x36,
                frmctr1 = 0xB1,
                frmctr2 = 0xB2,
                frmctr3 = 0xB3,
                invctr = 0xB4,
                disset5 = 0xB6,
                pwctr1 = 0xC0,
                pwctr2 = 0xC1,
                pwctr3 = 0xC2,
                pwctr4 = 0xC3,
                pwctr5 = 0xC4,
                vmctr1 = 0xC5,
                rdid1 = 0xDA,
                rdid2 = 0xDB,
                rdid3 = 0xDC,
                rdid4 = 0xDD,
                pwctr6 = 0xFC,
                gmctrp1 = 0xE0,
                gmctrn1 = 0xE1
            };

            /**
             * @brief Write display commands/data to the screen
             *
             * @param data
             * @param size
             */
            static void write_cmd(const cmd command, const uint8_t *const arguments, const uint32_t size) {
                // set display in command mode
                PinDC::template set<false>();

                // write the command
                Bus::write(reinterpret_cast<const uint8_t *>(&command), sizeof(cmd));

                // only write the data if we have arguments
                if (size) {
                    // set display in data mode
                    PinDC::template set<true>();

                    // write the data of the command
                    Bus::write(arguments, size);
                }
            }

            /**
             * @brief Write a command to the display
             * 
             * @param command 
             */
            static void write_cmd(const cmd command) {
                // write the command to the display
                write_cmd(command, nullptr, 0);
            }  

            /**
             * @brief Write a command with one or multiple parameters to the display
             * 
             * @details this function packs the aguments in a array to write to the display
             * 
             * @tparam Args 
             * @param command 
             * @param arg 
             * @param args 
             */
            template <typename... Args>
            static void write_cmd(const cmd command, const uint8_t arg, Args &&... args) {
                // create a array of the arguments
                const uint8_t arguments[] = {arg, static_cast<uint8_t>(args)...};

                // write the command and arguments to the display
                write_cmd(command, arguments, sizeof(arguments));
            }         

        public:
            /**
             * @brief inits the display
             *
             */
            static void init() {
                // using for the time
                using namespace klib::time;

                // reset the display
                PinRst::template set<true>();
                klib::delay_busy(5_ms);
                PinRst::template set<false>();
                klib::delay_busy(5_ms);
                PinRst::template set<true>();
                klib::delay_busy(5_ms);

                // do a software reset and stop sleep mode, voltage booster on
                write_cmd(cmd::swreset);
                klib::delay_busy(150_ms);

                write_cmd(cmd::dispoff);
                klib::delay_busy(10_ms);

                write_cmd(cmd::slpout);
                klib::delay_busy(100_ms);

                // frame rate control normal mode
                write_cmd(cmd::frmctr1, 0x01, 0x2C, 0x2D);

                // frame rate control idle mode
                write_cmd(cmd::frmctr2, 0x01, 0x2C, 0x2D);

                // frame rate control partial mode
                write_cmd(cmd::frmctr3, 0x01, 0x2C, 0x2D, 0x01, 0x2C, 0x2D);

                klib::delay_busy(20_ms);

                // display invertion
                write_cmd(cmd::invctr, 0x07);

                // power control settings
                write_cmd(cmd::pwctr1, 0xA2, 0x02, 0x84);
                write_cmd(cmd::pwctr2, 0xC5);
                write_cmd(cmd::pwctr3, 0xA0, 0x00);
                write_cmd(cmd::pwctr4, 0x8A, 0x2A);
                write_cmd(cmd::pwctr5, 0x8A, 0xEE);

                // vcomh voltage control
                write_cmd(cmd::vmctr1, 0x0E);

                // display inversion off, memory direction control
                write_cmd(cmd::invon);
                write_cmd(cmd::madctl, 0b1 << 3);

                klib::delay_busy(20_ms);

                // set screen in 8 bit bus mode with 16 bit color
                write_cmd(cmd::colmod, 0x05);
                klib::delay_busy(10_ms);

                // set gamma adjustment + polarity
                write_cmd(
                    cmd::gmctrp1, 0x02, 0x1C, 0x07, 0x12, 
                    0x37, 0x32, 0x29, 0x2D, 0x29, 0x25, 
                    0x2B, 0x39, 0x00, 0x01, 0x03, 0x10
                );

                // set gamma adjustment - polarity
                write_cmd(
                    cmd::gmctrn1, 0x03, 0x1D, 0x07, 0x06, 
                    0x2E, 0x2C, 0x29, 0x2D, 0x2E, 0x2E, 
                    0x37, 0x3F, 0x00, 0x00, 0x02, 0x10
                );

                // set the cursor to the maximum of the screen
                set_cursor(0x00, 0x00, width - 1, height - 1);

                // normal display on
                write_cmd(cmd::noron);
                klib::delay_busy(10_ms);

                // screen on
                write_cmd(cmd::dispon);
                klib::delay_busy(100_ms);
            }

           /**
             * @brief Set the cursor object
             *
             * @param x_min
             * @param y_min
             * @param x_max
             * @param y_max
             */
            static void set_cursor(uint16_t x_min, uint16_t y_min, uint16_t x_max, uint16_t y_max) {
                // add the offset
                x_min += XOffset;
                x_max += XOffset;
                y_min += YOffset;
                y_max += YOffset;

                // set the min and max row
                write_cmd(
                    cmd::caset, static_cast<uint8_t>(x_min >> 8), static_cast<uint8_t>(x_min),
                    static_cast<uint8_t>(x_max >> 8), static_cast<uint8_t>(x_max)
                );

                // set the min and max column
                write_cmd(
                    cmd::raset, static_cast<uint8_t>(y_min >> 8), static_cast<uint8_t>(y_min),
                    static_cast<uint8_t>(y_max >> 8), static_cast<uint8_t>(y_max)
                );
            }

            static void set_cursor(const klib::vector2u &start_pos, const klib::vector2u &end_pos) {
                // set cursor on correct position
                set_cursor(start_pos.x, start_pos.y, end_pos.x, end_pos.y);
            }

            static void raw_write_screen(const pixel_type* data, const uint32_t size) {
                // write raw screendata to the display
                write_cmd(cmd::ramwr, reinterpret_cast<const uint8_t *const>(data), (size * sizeof(pixel_type)));
            }

            constexpr static pixel_type color_to_raw(const klib::color &col) {
                // conver the color to raw data 
                const pixel_type data = (((static_cast<pixel_type>(col.red) * 0x1F) / 0xFF) << 11) | 
                                        (((static_cast<pixel_type>(col.green) * 0x3F) / 0xFF) << 5) |
                                        ((static_cast<pixel_type>(col.blue) * 0x1F) / 0xFF);

                // reverse the byte order of the data
                return klib::bswap(data); 
            }

            constexpr static uint32_t position_to_buffer(const klib::vector2u &pos) {
                // convert the xy position to a position in a array
                return (pos.y * width) + pos.x;
            }
    };

    /**
     * @brief Using for the st7735 mini display with the resolution 80 x 160
     * 
     * @tparam Bus 
     * @tparam PinDC 
     * @tparam PinRst 
     */
    template <typename Bus, typename PinDC, typename PinRst>
    using st7735_160x80 = st7735<Bus, PinDC, PinRst, 80, 160, 26, 1>;
}

#endif
