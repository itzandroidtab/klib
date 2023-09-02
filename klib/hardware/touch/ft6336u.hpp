#ifndef KLIB_FT6336U_HPP
#define KLIB_FT6336U_HPP

#include <cstdint>
#include <klib/delay.hpp>

#include "touch.hpp"

namespace klib::hardware::touch {   
    template <typename TouchHelper, typename Bus, typename IrqPin, typename RstPin>
    class ft6336u {
    public:
        // maximum amount of touch points supported for the ft6336u
        constexpr static uint8_t max_touch_points = 2;

    protected:
        // 7-bit base address of the ft6336u
        constexpr static uint8_t address = 0x38;        

        /**
         * @brief Available registers on the ft6336u. TODO: add gesture registers
         * 
         */
        enum class cmd: uint8_t {
            // mode registers
            dev_mode = 0x00,
            monitor_mode = 0x86,
            operating_mode = 0xa5,
            interrupt_mode = 0xa4,
            factory_mode = 0xae,
            working_mode = 0xbc,

            touch_data = 0x02,

            // touch point 1
            p1_x_high = 0x03,
            p1_x_low = 0x04,
            p1_y_high = 0x05,
            p1_y_low = 0x06,
            p1_weight = 0x07,
            p1_misc = 0x08,

            // touch point 2
            p2_x_high = 0x09,
            p2_x_low = 0x0a,
            p2_y_high = 0x0b,
            p2_y_low = 0x0c,
            p2_weight = 0x0d,
            p2_misc = 0x0e,

            // settings
            touch_threshold = 0x80,
            point_threshold = 0x85,
            timeout = 0x87,
            active_report_rate = 0x88,
            monitor_report_rate = 0x89,
            charger_id = 0x8b,
            alpha_filter = 0x96,
            virtual_key_threshold = 0xa9,
            proximity_sensing = 0xb0,
            gesture = 0xd0,

            call_flag = 0xad,

            // chip information
            chip_designator_mid = 0x9f,
            chip_designator_low = 0xa0,
            lib_version_high = 0xa1,
            lib_version_low = 0xa2,
            chip_designator_high = 0xa3,
            firmware_id = 0xa6,
            vendor_id = 0xa8,
            release_code = 0xaf,
        };

        /**
         * @brief function to write commands to the i2c bus. length of the data is calculated automaticly
         * 
         * @tparam Args 
         * @param args 
         */
        template <typename... Args>
        static bool write_command(const cmd command, Args &&... args) {
            // convert all the parameters to an array with the length at the front
            const uint8_t data[] = {static_cast<uint8_t>(command), static_cast<uint8_t>(args)...};

            // send the array to the bus
            return Bus::write(address, data, sizeof(data));
        }

        /**
         * @brief 
         * 
         * @param command 
         */
        static void read(const cmd command, uint8_t *const rx, const uint8_t size) {
            // write the command
            if (!write_command(command)) {
                return;
            }

            // read the data
            Bus::read(address, rx, size);
        }

    public:
        static void init() {
            // reset the device
            RstPin::template set<false>();
            delay<busy_wait>(time::ms(5));
            RstPin::template set<true>();

            // wait until the interrupt pin is high
            while (!IrqPin::get()) {
                // do nothing
            }

            // change to working mode
            write_command(cmd::dev_mode, 0x00);

            // change the interrupt mode
            write_command(cmd::interrupt_mode, 0x00);
        }

        static void irq_handler() {
            uint8_t count;

            // read how many touches we have
            read(cmd::touch_data, &count, sizeof(count));

            // check if we have any test points and the amount is valid
            if (count > 2 || !count) {
                // unpress all the keys and exit
                TouchHelper::unpress_all();

                // do a early return
                return;
            }

            // update all the touch data
            for (uint32_t i = 0; i < max_touch_points; i++) {
                // check if the 
                if (i >= count) {
                    // unpress the touch point
                    TouchHelper::update(i);

                    // go to the next touch point
                    continue;
                }

                // buffer to store the xy coordinates
                uint8_t raw[4];

                // read all the touch data
                read((i == 0) ? cmd::p1_x_high : cmd::p2_x_high, raw, sizeof(raw));

                // get the x and y positions
                const uint16_t x = (raw[0] & 0xf) << 8 | raw[1];
                const uint16_t y = (raw[2] & 0xf) << 8 | raw[3];

                // set the data 
                TouchHelper::update(i, {x, y});
            }
        }
    };
}

#endif