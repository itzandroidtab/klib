#ifndef KLIB_SSD1306_HPP
#define KLIB_SSD1306_HPP

#include <cstdint>

#include <klib/vector2.hpp>

namespace klib::hardware::display {
    class ssd1306 {
    public:
        enum class command : uint8_t {
            set_contrast = 0x81,
            display_all_on_resume = 0xa4,
            display_all_on = 0xa5,
            normal_display = 0xa6,
            invert_display = 0xa7,
            display_off = 0xae,
            display_on = 0xaf,
            set_display_offset = 0xd3,
            set_compins = 0xda,
            set_vcom_detect = 0xdb,
            set_display_clock_div = 0xd5,
            set_precharge = 0xd9,
            set_multiplex = 0xa8,
            set_low_column = 0x00,
            set_high_column = 0x10,
            set_start_line = 0x40,
            memory_mode = 0x20,
            column_addr = 0x21,
            page_addr = 0x22,
            com_scan_inc = 0xc0,
            com_scan_dec = 0xc8,
            seg_remap = 0xa0,
            charge_pump = 0x8d,
            external_vcc = 0x01,
            switch_cap_vcc = 0x02,
            activate_scroll = 0x2f,
            deactivate_scroll = 0x2e,
            set_vertical_scroll_area = 0xa3,
            right_horizontal_scroll = 0x26,
            left_horizontal_scroll = 0x27,
            vertical_and_right_horizontal_scroll = 0x29,
            vertical_and_left_horizontal_scroll = 0x2A
        };
    };

    template <typename Bus>
    class ssd1306_i2c : public ssd1306 {
    protected:
        // value to send over i2c before a command
        constexpr static uint8_t cmd_prefix = 0x80;
        // value to send over i2c before data
        constexpr static uint8_t data_prefix = 0x40;

        // SSD1306 chip initialization
        constexpr static uint8_t initialization[] = {
            cmd_prefix,
            (uint8_t)command::display_off,
            cmd_prefix,
            (uint8_t)command::set_display_clock_div,
            0x80,
            cmd_prefix,
            (uint8_t)command::set_multiplex,
            0x3f,
            cmd_prefix,
            (uint8_t)command::set_display_offset,
            0x00,
            cmd_prefix,
            (uint8_t)command::set_start_line | 0x00,
            cmd_prefix,
            (uint8_t)command::charge_pump,
            0x14,
            cmd_prefix,
            (uint8_t)command::memory_mode,
            0x00,
            cmd_prefix,
            (uint8_t)command::seg_remap | 0x01,
            cmd_prefix,
            (uint8_t)command::com_scan_dec,
            cmd_prefix,
            (uint8_t)command::set_compins,
            0x12,
            cmd_prefix,
            (uint8_t)command::set_contrast,
            0xcf,
            cmd_prefix,
            (uint8_t)command::set_precharge,
            0xf1,
            cmd_prefix,
            (uint8_t)command::set_vcom_detect,
            0x40,
            cmd_prefix,
            (uint8_t)command::display_all_on_resume,
            cmd_prefix,
            (uint8_t)command::normal_display,
            cmd_prefix,
            (uint8_t)command::display_on
        };

        // The device address
        const uint8_t address;

        // The current cursor location in the controller
        vector2u cursor;

        /**
         * @brief Write a array of commands to the screen
         *
         * @tparam Args
         * @param args
         */
        void write_command(const uint8_t *const data, const uint32_t size) {
            // write every command to the bus
            for (uint32_t i = 0; i < size; i++) {
                const uint8_t buffer[] = {cmd_prefix, data[i]};

                // write all commands on the bus
                (void)Bus::write(address, buffer, sizeof(buffer));
            }
        }

        void write_data(const uint8_t *const data, const uint32_t size) {
            // write every command to the bus
            for (uint32_t i = 0; i < size; i++) {
                const uint8_t buffer[] = {data_prefix, data[i]};

                // write all commands on the bus
                (void)Bus::write(address, buffer, sizeof(buffer));
            }
        }

    public:
        // construct by providing the i2c channel

        /**
         * @brief Construct a ssd1306 i2c
         *
         * @details inits the cursor to an invalid address to force a update of the cursor
         *
         * @param address
         */
        ssd1306_i2c(const uint8_t &address):
            address(address), cursor({255, 255})
        {
            // write the initialization sequence to the screen
            write_command(initialization, sizeof(initialization));
        }

        /**
         * @brief Write pixel to the display
         *
         * @param location
         * @param data
         */
        void set(const vector2u location, uint8_t data) {
            // check if we need to update the current cursor of the screen
            if (location.x != cursor.x || cursor.y != cursor.y) {
                uint8_t column_cmd[] = {static_cast<uint8_t>(command::column_addr), location.x, 127};
                write_command(column_cmd, sizeof(column_cmd));

                uint8_t page_cmd[] = {static_cast<uint8_t>(command::page_addr), location.y, 7};
                write_command(page_cmd, sizeof(page_cmd));

                cursor = location;
            }

            // write data to the screen
            write_data(&data, 1);

            // update the local cursor
            cursor.x++; // TODO: cursor fallthrough
        }
    };

}

#endif