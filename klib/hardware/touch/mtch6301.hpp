#ifndef KLIB_MTCH6301_HPP
#define KLIB_MTCH6301_HPP

#include <klib/math.hpp>
#include <klib/delay.hpp>

namespace klib::hardware::touch {   
    template <typename Bus, typename IrqPin, typename RstPin>
    class mtch6301 {
    public:
        /**
         * @brief Touch packet received from the mtch6301
         * 
         */
        struct touch_packet {
            // touch id (0 - 9)
            uint8_t touch_id:4;
            
            // pen state
            // 0xb0 = pen up
            // 0xb1 = pen down
            bool pen;
            
            // x coordinate
            uint16_t x:12;

            // y coordinate
            uint16_t y:12;

            /**
             * @brief Construct a new touch packet from seperate values
             * 
             * @param touch_id 
             * @param pen 
             * @param x 
             * @param y 
             */
            constexpr touch_packet(uint8_t touch_id, bool pen, uint16_t x, uint16_t y):
                touch_id(touch_id), pen(pen), x(x), y(y)
            {}

            /**
             * @brief Construct a new touch packet from a single uint64_t
             * 
             * @param packet 
             */
            constexpr touch_packet(const uint64_t packet):
                touch_packet((packet >> 3) & 0xf, packet & 0x1, 
                            ((packet >> 8) & 0x7f) | ((packet >> 16) & 0x1f),
                            ((packet >> 24) & 0x7f) | ((packet >> 32) & 0x1f))
            {}        
        };

        /**
         * @brief Gesture packet received from the mtch6301
         * 
         */
        struct gesture_packet {
            // touch id (0 - 9)
            uint8_t touch_id:4;

            /**
             * @brief Available gestures
             * 
             */
            enum class gest: uint8_t {
                single_tap = 0x10,
                single_tap_hold = 0x11,
                double_tap = 0x20,
                up_swipe = 0x31,
                up_swipe_hold = 0x32,
                right_swipe = 0x41,
                right_swipe_hold = 0x42,
                down_swipe = 0x51,
                down_swipe_hold = 0x52,
                left_swipe = 0x61,
                left_swipe_hold = 0x62
            };

            // gesture
            gest gesture;

            /**
             * @brief Construct a new gesture packet from seperate values
             * 
             * @param touch_id 
             * @param gesture 
             */
            gesture_packet(uint8_t touch_id, gest gesture):
                touch_id(touch_id), gesture(gesture)
            {}

            /**
             * @brief Construct a new gesture packet object
             * 
             * @param packet 
             */
            gesture_packet(uint16_t packet):
                gesture_packet((packet >> 3) & 0xf, static_cast<gest>((packet >> 8) & 0x7f)) 
            {}
        };

        /**
         * @brief Orientation of the sensor. see page 10 for more information
         * 
         */
        struct sensor_orientation {
            // swap
            // 0b0 = RX axis vertical, TX axis horizontal
            // 0b1 = RX axis horizontal, TX axis vertical
            bool swap;

            // tx flip
            // 0xb0 do not invert the tx axis
            // 0xb1 invert the tx axis
            bool tx_flip;

            // rx flip
            // 0xb0 do not invert the rx axis
            // 0xb1 invert the rx axis
            bool rx_flip;

            /**
             * @brief Get the orientation as a uint8_t
             * 
             * @return uint8_t 
             */
            constexpr uint8_t get() {
                return swap << 2 | tx_flip << 1 | rx_flip;
            }

            /**
             * @brief Construct a new sensor orientation object from bools
             * 
             * @param swap 
             * @param tx_flip 
             * @param rx_flip 
             */
            constexpr sensor_orientation(bool swap = false, bool tx_flip = false, bool rx_flip = false):
                swap(swap), tx_flip(tx_flip), rx_flip(rx_flip)
            {}

            /**
             * @brief Construct a new sensor orientation object from a uint8_t
             * 
             * @param orientation 
             */
            constexpr sensor_orientation(uint8_t orientation):
                sensor_orientation(orientation & (1 << 2), 
                                orientation & (1 << 1), 
                                orientation & 0x1)
            {}
        };

    protected:
        // 7-bit base address of the mtch6301
        const uint8_t address = 0x25;

        // using for the callback function
        using interrupt_callback = void (*)(const touch_packet&);

        // callback when a touch packet happens
        interrupt_callback touch_callback = nullptr;

        // prefix for writing commands
        constexpr static uint8_t command_prefix = 0x55;

        /**
         * @brief Avaliable responses for a command
         * 
         */
        enum class res: uint8_t {
            success = 0x0,
            parameter_out_of_range = 0x80,
            missing_or_extra_parameter = 0xfc,
            invalid_parameter = 0xfd,
            timeout = 0xfe,
            unrecognized_cmd = 0xff
        };

        /**
         * @brief Command set
         * 
         */
        enum class cmd: uint8_t {
            enable_touch = 0x0,
            disable_touch = 0x1,
            scan_baseline = 0x14,
            write_register = 0x15,
            read_register = 0x16,
            write_nvram = 0x17,
            software_sleep = 0x18,
            erase_nvram = 0x19,
            manufacturing_test = 0x1a,
            divice_id = 0x83
        };

        // rx/tx scaling coefficients. see page 8 for more information
        constexpr static uint16_t coefficent[] = {
            0x5555, 0x5555, 0x5555, 0x4000, 
            0x3333, 0x2aaa, 0x2493, 0x2000, 
            0x1c71, 0x1999, 0x1745, 0x1555, 
            0x13b1, 0x1249, 0x1111, 0x1000, 
            0x0f0f, 0x0e38
        };

        // rx pin map for custom sensors. see page 8 for more information
        constexpr static uint8_t rx_pinmap[] = {
            8, 7, 6, 5, 4, 3, 2, 1, 0, 9, 10, 11, 12
        };

        // tx pin map for custom sensors. see page 8 for more information
        constexpr static uint8_t tx_pinmap[] = {
            13, 6, 3, 2, 4, 30, 29, 28, 7, 14, 15, 16, 5, 8, 34, 33, 32, 31
        };  

        /**
         * @brief function to write commands to the i2c bus. length of the data is calculated automaticly
         * 
         * @tparam Args 
         * @param args 
         */
        template <typename... Args>
        void write_command(Args &&... args) {
            // convert all the parameters to an array with the length at the front
            const uint8_t data[] = {command_prefix, static_cast<uint8_t>(sizeof...(Args)), static_cast<uint8_t>(args)...};

            // send the array to the bus
            Bus::write(address, data, sizeof(data));
        }

        /**
         * @brief enable or disable the touch input
         * 
         * @param enabled 
         */
        void enable_touch(bool enabled) {
            // enable/disable the touch based on the input
            write_command(enabled ? cmd::enable_touch : cmd::disable_touch);
        }

        /**
         * @brief instruct the sensor controller to scan for new sensor baseline immediately
         * 
         */
        void scan_baseline() {
            // send the scan baseline command
            write_command(cmd::scan_baseline);
        }

        // general functions
        /**
         * @brief Set the amount of active rx and tx channels
         * 
         * @param rx 
         * @param tx 
         */
        void set_active_channels(uint8_t rx, uint8_t tx) {
            // set the amount of rx channels
            // write the rx register at byte index 0x0, byte offset 0x1
            write_command(cmd::write_register, 0x0, 0x1, rx);

            // set the amount of tx channels
            // write the tx register at byte index 0x0, byte offset 0x2
            write_command(cmd::write_register, 0x0, 0x2, tx);

            // write the rx scaling coefficient
            write_command(cmd::write_register, 0x0, 0x4, coefficent[rx] & 0xff);
            write_command(cmd::write_register, 0x0, 0x5, (coefficent[rx] >> 8) & 0xff);

            // write the tx scaling coefficient
            write_command(cmd::write_register, 0x0, 0x6, coefficent[tx] & 0xff);
            write_command(cmd::write_register, 0x0, 0x7, (coefficent[tx] >> 8) & 0xff);
        }

        // sensor map functions
        /**
         * @brief Set the rx pinmap of the controller
         * 
         * @tparam Args 
         * @param args 
         */
        template <typename... Args>
        void set_rx_pinmap(Args &&... args) {
            // limit the input to length 13 
            static_assert((sizeof...(Args) <= 13), "To many arguments for the rx pinmap");

            // create an array with the arguments
            const uint8_t data[] = {
                static_cast<const uint8_t>(rx_pinmap[args])...
            };
    
            // send the rx pinmap to the controller on byte index 0x01, byte offset (0x0 - 0xc)
            for (uint8_t i = 0; i < sizeof(data); i++) {
                write_command(cmd::write_register, 0x01, i, data[i]);
            }
        }

        /**
         * @brief Set the tx pinmap of the controller
         * 
         * @tparam Args 
         * @param args 
         */
        template <typename... Args>
        void set_tx_pinmap(Args &&... args) {
            // limit the input to length 18 
            static_assert((sizeof...(Args) <= 18), "To many arguments for the tx pinmap");

            // create an array with the arguments
            const uint8_t data[] = {
                static_cast<const uint8_t>(rx_pinmap[args])...
            };
    
            // send the tx pinmap to the controller on byte index 0x02, byte offset (0x0 - 0xc)
            for (uint8_t i = 0; i < sizeof(data); i++) {
                write_command(cmd::write_register, 0x02, i, data[i]);
            }
        }



        // decoding functions 
        /**
         * @brief Set the sensor orientation
         * 
         * @param orientation 
         */
        void set_sensor_orientation(sensor_orientation &orientation) {
            // write the orientation register at byte index 0x30, byte offset 0x0 
            write_command(cmd::write_register, 0x30, 0x0, orientation.get());
        }

        /**
         * @brief Set the number of averages to use to get the touch coordinates
         * 
         * @param averages 
         */
        void set_averaging(uint8_t averages) {
            // write the average register at byte index 0x30, byte offset 0x1 
            write_command(cmd::write_register, 0x30, 0x1, averages & 0xf);        
        }

        /**
         * @brief Set the minimum distance allowed between two touch locations before 
         * suppressing the weaker touch location
         * 
         * @param distance 
         */
        void set_minimum_touch_distance(uint8_t distance) {
            // write the minimum touch distance register at byte index 0x30, byte offset 0x4 
            write_command(cmd::write_register, 0x30, 0x4, distance);
        }

        /**
         * @brief Set the number of scans before an up/down pen packet is notifyed
         * 
         * @warning input needs to be limited in range 0 - 10. no internal checks are performed
         * 
         * @param up_timer 
         * @param down_timer 
         */
        void set_pen(uint8_t up_timer, uint8_t down_timer) {
            // write the pen down register at byte index 0x30, byte offset 0x5
            write_command(cmd::write_register, 0x30, 0x5, down_timer & 0xf);        

            // write the pen up register at byte index 0x30, byte offset 0x6
            write_command(cmd::write_register, 0x30, 0x6, up_timer & 0xf);    
        }

        /**
         * @brief Set the max amount of touch points
         * 
         * @param max_points 
         */
        void set_max_touch_points(uint8_t max_points) {
            // set the max amount of touch points before they are suppressed
            // write the suppresion register at byte index 0x30, byte offset 0x7
            write_command(cmd::write_register, 0x30, 0x7, max_points);
        }
        


        // gesture functions
        /**
         * @brief Set the minimum swipe distance before the gesture is recognized
         * 
         * @warning value needs to stay in range 10 - 255. no internal checks are performed
         * 
         * @param rx_swipe 
         * @param tx_swipe 
         */
        void set_swipe_length(uint8_t rx_swipe, uint8_t tx_swipe) {
            // write the rx swipe length register at byte index 0x50, byte offset 0x0
            write_command(cmd::write_register, 0x50, 0x0, rx_swipe);

            // write the tx swipe length register at byte index 0x50, byte offset 0x1
            write_command(cmd::write_register, 0x50, 0x1, tx_swipe);
        }



        // configuration functions
        /**
         * @brief Set the sleep timeout object
         * 
         * @param timeout 
         */
        void set_sleep_timeout(uint32_t timeout) {
            // write all the timeout registers with the timeout value
            for (uint8_t i = 0; i < sizeof(timeout); i++) {
                // write the slp register at byte index 0xf0, byte offset (0x0 - 0x03)
                write_command(cmd::write_register, 0xf0, i, (timeout >> (i * 8)) & 0xff);
            }
        }

        /**
         * @brief enable/disable the messages from the mtch6301
         * 
         * @param touch 
         * @param gesture 
         * @param status 
         */
        void setup_messages(bool touch, bool gesture, bool status) {
            // enable/disable the touch input packets
            // write the touch input enable at byte index 0xf0, byte offset 0x7
            write_command(cmd::write_register, 0xf0, 0x7, (touch ? 0x81 : 0x1));

            // enable/disable the gesture input packets
            // write the gesture input enable at byte index 0xf0, byte offset 0x9
            write_command(cmd::write_register, 0xf0, 0x9, (gesture ? 0x81 : 0x1));

            // enable/disable the status packets
            // write the status enable at byte index 0xf0, byte offset 0xa
            write_command(cmd::write_register, 0xf0, 0xa, (status ? 0x81 : 0x1));
        }

    public:
        /**
         * @brief Construct a mtch6301 object
         * 
         * @param touch_callback callback function when a touch packet arrives
         * @param address address of the mtch6301 on the bus
         * @param rx_channels amount of rx channels
         * @param tx_channels amount of tx channels
         */
        mtch6301(const interrupt_callback touch_callback = nullptr,
                 const uint8_t address = 0x25, 
                 const uint8_t rx_channels = 6, 
                 const uint8_t tx_channels = 6)
        :
            address(address), touch_callback(touch_callback)
        {
            // reset the device
            RstPin::template set<false>();

            // wait more than 5 microseconds
            klib::delay<klib::busy_wait>(klib::time::ms{1});

            // pull the resset high again to stop the reset
            RstPin::template set<true>();

            // wait until the interrupt pin is low again
            while (IrqPin::get()) {
                // do nothing (can take between 80 and 100 ms)
            }

            // send the enable touch message to wake up the controller
            enable_touch(true);

            // wait more than 350 microseconds
            klib::delay<klib::busy_wait>(klib::time::ms{1});

            // stop the touch input
            enable_touch(false);

            // set the amount of rx and tx channels
            set_active_channels(rx_channels, tx_channels);

            // set the max amount of touch points
            set_max_touch_points(10);

            // set the pen hold information
            set_pen(0x9, 0x9);

            // set the rx pinmap
            set_rx_pinmap(0, 1, 2, 3, 4, 5);

            // set the tx pinmap
            set_tx_pinmap(0, 1, 2, 3, 4, 5);

            // enable/disable different status messages
            setup_messages(true, false, false);

            // scan the baseline
            scan_baseline();        

            // enable the touch input
            enable_touch(true);   
        }

        /**
         * @brief IRQ handler to call when the mtch6301 interrupt pin gets triggered. 
         * User needs to call this function periodically or use a falling interrupt
         * on the irq pin.
         * 
         */
        void irq_handler() {
            // variable to store the data
            uint8_t length;

            // read data from the bus. If we fail we exit
            if (!Bus::template read<false>(address, &length, sizeof(length))) {
                // no valid response from target exit
                return;
            }

            // create a buffer for the packet
            uint8_t buffer[255];

            // request the rest of the message using the length
            bool data_valid = Bus::template read<true, true>(address, buffer, klib::min(length, sizeof(buffer)));

            // check if we received the amount of data we specified
            if (!data_valid) {
                // invalid read exit
                return;
            }

            // check if we received a touch or gesture packet
            bool packet_type = buffer[0] & (1 << 2); 

            // check if we need to call the callback
            if (touch_callback && (packet_type == 0)) {
                // copy the buffer to a uint64_t
                const uint64_t data = static_cast<uint64_t>(buffer[0]) | 
                                static_cast<uint64_t>(buffer[1]) << 8 | 
                                static_cast<uint64_t>(buffer[2]) << 16 | 
                                static_cast<uint64_t>(buffer[3]) << 24 | 
                                static_cast<uint64_t>(buffer[4]) << 32;

                // create a touch packet
                auto t = touch_packet(data);

                // call the callback
                touch_callback(t);
            }

            // TODO: gesture packets      
        }
    };
}

#endif
