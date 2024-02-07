#ifndef KLIB_ST7789_HPP
#define KLIB_ST7789_HPP

#include <span>

#include <klib/delay.hpp>
#include <klib/vector2.hpp>
#include <klib/graphics/color.hpp>
#include <klib/graphics/display.hpp>
#include <klib/io/dma.hpp>

namespace klib::hardware::display {
    template <
        typename Bus, typename PinDC, typename PinRst, 
        graphics::mode Mode = graphics::mode::rgb565,
        uint32_t Width = 320, uint32_t Height = 240, 
        uint32_t XOffset = 0, uint32_t YOffset = 0
    >
    class st7789 {
        public:
            // width of the display
            constexpr static uint32_t width = Width;

            // height of the display
            constexpr static uint32_t height = Height;

            // mode used in the display
            constexpr static graphics::mode mode = Mode;

        protected:
            static_assert(
                (mode == graphics::mode::rgb565) ||
                (mode == graphics::mode::rgb666),
                "Display only supports rgb565 or rgb666"
            );

            // check if we have valid width and height
            static_assert((width != 0) || (height != 0), "Screen needs to have a valid size");

            // all the commands for the display
            enum class cmd: uint8_t {
                // system function command table 1
                nop = 0x00,
                swreset = 0x01,
                rddid = 0x04,
                rddst = 0x09,
                rddpm = 0x0a,
                rddmadctl = 0x0b,
                rddcolmod = 0x0c,
                rddim = 0x0d,
                rddsm = 0x0e,
                rddsdr = 0x0f,
                slpin = 0x10,
                slpout = 0x11,
                ptlon = 0x12,
                noron = 0x13,
                invoff = 0x20,
                invon = 0x21,
                gamset = 0x26,
                dispoff = 0x28,
                dispon = 0x29,
                caset = 0x2a,
                raset = 0x2b,
                ramwr = 0x2c,
                ramrd = 0x2e,
                ptlar = 0x30,
                vscrdef = 0x33,
                teoff = 0x34,
                teon = 0x35,
                madctl = 0x36,
                vscsad = 0x37,
                idmoff = 0x38,
                idmon = 0x39,
                colmod = 0x3a,
                wrmemc = 0x3c,
                rdmemc = 0x3e,
                ste = 0x44,
                gscan = 0x45,
                wrdisbv = 0x51,
                rddisbv = 0x52,
                wrctrld = 0x53,
                rdctrld = 0x54,
                wrcace = 0x55,
                rdcabc = 0x56,
                wrcabcmb = 0x5e,
                rdcabcmb = 0x5f,
                rdabcsdr = 0x68,
                rdid1 = 0xda,
                rdid2 = 0xdb,
                rdid3 = 0xdc,

                // system function command table 2
                ramctrl = 0xb0,
                rgbctrl = 0xb1,
                porctrl = 0xb2,
                frctrl1 = 0xb3,
                parctrl = 0xb5,
                gctrl = 0xb7,
                gtadj = 0xb8,
                dgmen = 0xba,
                vcoms = 0xbb,
                powsave = 0xbc,
                dlpoffsave = 0xbd,
                lcmctrl = 0xc0,
                idset = 0xc1,
                vdvvrhen = 0xc2,
                vrhs = 0xc3,
                vdvs = 0xc4,
                vcmofset = 0xc5,
                frctrl2 = 0xc6,
                cabcctrl = 0xc7,
                regsel1 = 0xc8,
                regsel2 = 0xca,
                pwmfrsel = 0xcc,
                pwctrl1 = 0xd0,
                vapvanen = 0xd2,
                cmd2en = 0xdf,
                pvgamctrl = 0xe0,
                nvgamctrl = 0xe1,
                dgmlutr = 0xe2,
                dgmlutb = 0xe3,
                gatectrl = 0xe4,
                spi2en = 0xe7,
                pwctrl2 = 0xe8,
                eqctrl = 0xe9,
                promctrl = 0xec,
                promen = 0xfa,
                nvmset = 0xfc,
                promact = 0xfe,             
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
                Bus::write({reinterpret_cast<const uint8_t *>(&command), sizeof(cmd)});

                // only write the data if we have arguments
                if (size) {
                    // set display in data mode
                    PinDC::template set<true>();

                    // write the data of the command
                    Bus::write({arguments, size});
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
             * @tparam InvertedColors 
             * @tparam RGBMode 
             * @tparam Orientation 
             * @tparam XMirror 
             * @tparam YMirror 
             */
            template <
                bool InvertedColors = true, bool RGBMode = true,
                graphics::orientation Orientation = graphics::orientation::portrait,
                bool XMirror = false, bool YMirror = false
            >
            static void init() {
                // using for the time
                using namespace klib::time;

                // reset the display
                PinRst::template set<true>();
                klib::delay<klib::busy_wait>(5_ms);
                PinRst::template set<false>();
                klib::delay<klib::busy_wait>(5_ms);
                PinRst::template set<true>();
                klib::delay<klib::busy_wait>(5_ms);

                // turn the display off
                write_cmd(cmd::dispoff);

                // turn off the sleep mode
                write_cmd(cmd::slpout);
                klib::delay<klib::busy_wait>(50_ms);

                // set screen based on the input mode
                switch (Mode) {
                    case graphics::mode::rgb666:
                        write_cmd(cmd::colmod, 0x66);
                        break;
                    default:
                    case graphics::mode::rgb565:
                        write_cmd(cmd::colmod, 0x55);
                        break;
                }

                // display inversion configuration
                if constexpr (InvertedColors) {
                    write_cmd(cmd::invon);
                }
                else {
                    write_cmd(cmd::invoff);
                }

                // memory direction control
                write_cmd(cmd::madctl, 
                    (RGBMode << 3) | ((Orientation == graphics::orientation::landscape) << 5) |
                    (XMirror << 6) | (YMirror << 7)
                );

                // frame rate control in idle and partial mode (set 
                // it so we use the values from frctrl2)
                write_cmd(cmd::frctrl1, 0x00, 0x0f, 0x0f);

                // frame rate control normal mode
                write_cmd(cmd::frctrl2, 0x0f);

                // set the porch control (only enable the minimum amount for now)
                write_cmd(cmd::porctrl, 0x01, 0x01, 0x00, 0x01, 0x01);

                // power control settings
                write_cmd(cmd::pwctrl1, 0xa4, 0xa1);
                write_cmd(cmd::pwctrl2, 0x93);

                // set gamma adjustment + polarity
                write_cmd(
                    cmd::pvgamctrl, 0xd0, 0x04, 0x0d, 
                    0x11, 0x13, 0x2b, 0x3f, 0x54, 0x4c, 
                    0x18, 0x0d, 0x0b, 0x1f, 0x23
                );

                // set gamma adjustment - polarity
                write_cmd(
                    cmd::nvgamctrl, 0xd0, 0x04, 0x0c, 
                    0x11, 0x13, 0x2c, 0x3f, 0x44, 0x51, 
                    0x2f, 0x1f, 0x1f, 0x20, 0x23
                );

                // normal display on
                write_cmd(cmd::noron);

                // screen on
                write_cmd(cmd::dispon);
                klib::delay<klib::busy_wait>(20_ms);
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

            static void start_write() {
                // write raw screendata to the display
                write_cmd(cmd::ramwr);

                // set display in data mode
                PinDC::template set<true>();
            }

            static void raw_write(const uint8_t *const data, const uint32_t size) {
                // write the data of the command
                Bus::write(data, size);
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
     * @brief Display write override with dma. Has support for 2 
     * channels as some hardware requires a channel for reading the 
     * spi data as well. If not used it can be set to:
     * "klib::io::dma::none"
     * 
     * @tparam DmaTx 
     * @tparam DmaRx 
     * @tparam Bus 
     * @tparam PinDC 
     * @tparam PinRst 
     * @tparam Mode 
     * @tparam Width 
     * @tparam Height 
     * @tparam XOffset 
     * @tparam YOffset 
     */
    template <
        typename DmaTx, typename DmaRx,
        typename Bus, typename PinDC, typename PinRst, 
        graphics::mode Mode = graphics::mode::rgb565,
        uint32_t Width = 320, uint32_t Height = 240, 
        uint32_t XOffset = 0, uint32_t YOffset = 0
    >
    class st7789_dma: public st7789<Bus, PinDC, PinRst, Mode, Width, Height, XOffset, YOffset> {
    protected:
        // rx data for if we have a rx dma
        static inline uint32_t rx;

    public:
        /**
         * @brief Do a raw write using the dma
         * 
         * @warning DMA channels should be initialized before 
         * calling this function
         * 
         * @param data 
         * @param size 
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
    };
}

#endif
