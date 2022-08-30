#ifndef KLIB_QMC5883_HPP
#define KLIB_QMC5883_HPP

#include <cstdint>
#include <numbers>
#include <cmath>

#include <klib/vector3.hpp>

namespace klib::hardware::compass {
    template <typename Bus>
    class qmc5883 {
        protected:
            // address of the device
            const uint8_t address; 

            /**
             * @brief Available registers of the qmc5883
             * 
             */
            enum class register_loc: uint8_t {
                x_lsb = 0x00,
                x_msb = 0x01,
                y_lsb = 0x02,
                y_msb = 0x03,
                z_lsb = 0x04,
                z_msb = 0x05,
                status = 0x06,
                temperature_lsb = 0x07,
                temperature_msb = 0x08,
                control_1 = 0x09,
                control_2 = 0x0a,
                period = 0x0b
            };

        public:
            /**
             * @brief Operating modes
             * 
             */
            enum class control_mode {
                standby = 0x00,
                continuous = 0x01
            };

            /**
             * @brief Available data rates supported
             * 
             */
            enum class data_rate {
                hz_10 = 0x00,
                hz_50 = 0x01,
                hz_100 = 0x02,
                hz_200 = 0x03
            };

            /**
             * @brief The maximum scale of the data
             * 
             */
            enum class full_scale {
                gauss_2 = 0x00,
                gauss_8 = 0x01
            };

            /**
             * @brief Over sample ratio
             * 
             */
            enum class over_sample {
                sample_512 = 0x00,
                sample_256 = 0x01,
                sample_128 = 0x02,
                sample_64 = 0x03
            };

        protected:
            /**
             * @brief Write a value to a register
             * 
             * @param reg 
             * @param value 
             */
            bool write(const register_loc reg, const uint8_t value) {
                // create a array to write to the i2c bus
                const uint8_t buffer[] = {reg, value};

                // write the data to the register (with no repeated start)
                return Bus::write(address, buffer, sizeof(buffer));
            }

            /**
             * @brief Read data from a register. Data read will be stored in input array
             * 
             * @param reg 
             * @param data 
             * @param size 
             */
            bool read(const register_loc reg, uint8_t *const data, const uint8_t size) {
                // write the the address to the bus (with no repeated start)
                (void)Bus::write(address, static_cast<const uint8_t*>(&reg), sizeof(reg));

                // read the data from the device (with no repeated start)
                return Bus::read(address, data, size);
            }

        public:
            /**
             * @brief Construct a new qmc5883
             * 
             * @param address 
             */
            qmc5883(const uint8_t address = 0x0D):
                address(address)
            {}

            /**
             * @brief Set the reset period. Recommended value is 0x01
             * 
             * @param period 
             */
            void set_reset_period(const uint8_t period) {
                // write the set/reset period
                write(register_loc::period, period);
            };

            /**
             * @brief Execute a software reset. This resets all the register values 
             * to default.
             * 
             */
            void reset() {
                // reset the device using the software reset in the control register 2
                write(register_loc::control_2, 0x80);
            }

            /**
             * @brief Setup the qmc5883 with the specified settings
             * 
             * @param mode mode control
             * @param rate output data rate 
             * @param scale the full scale 
             * @param sample over sample ratio
             */
            void set_mode(const control_mode mode, const data_rate rate, 
                          const full_scale scale, const over_sample sample) {
                // combine all the settings into a temporary variable
                const uint8_t buffer = static_cast<uint8_t>(sample) << 6 | 
                                       static_cast<uint8_t>(scale) << 4 |
                                       static_cast<uint8_t>(rate) << 2 |
                                       static_cast<uint8_t>(mode);

                // write the new settings to the device
                write(register_loc::control_1, buffer);
            }

            /**
             * @brief enable/disable the advanced options.
             * 
             * @param interrupt enable/disable interrupt pin
             * @param roll enable/disable pointer roll mode when communicating with 
             * the device
             */
            void advanced_mode(const bool interrupt, const bool roll) {
                // combine all the settings into a temporary variable
                const uint8_t buffer = static_cast<uint8_t>(roll) << 6 |
                                       static_cast<uint8_t>(interrupt);

                // write the new settings to the device
                write(register_loc::control_2, buffer);                
            }

            /**
             * @brief Get the status register from the device
             * 
             * @return uint8_t all the status bits
             */
            uint8_t get_status() {
                // create a variable to store the data we want to read
                uint8_t status;

                // read from the device
                read(register_loc::status, &status, sizeof(status));

                // return the status
                return status;
            }

            /**
             * @brief Check if the data ready bit is set
             * 
             * @return true
             * @return false
             */
            bool is_ready() {
                // return the data ready bit only from the status
                return get_status() & 0x01; 
            }

            /**
             * @brief Get the raw output data from the sensor
             * 
             * @return vector3<uint16_t> 
             */
            vector3<uint16_t> get_raw() {
                // create a array to store the data we want to receive
                uint8_t buffer[6];

                // read from the device
                read(register_loc::x_lsb, buffer, sizeof(buffer));

                // combine all the data
                return {
                    buffer[0] | (static_cast<uint16_t>(buffer[1]) << 8),
                    buffer[2] | (static_cast<uint16_t>(buffer[3]) << 8),
                    buffer[4] | (static_cast<uint16_t>(buffer[5]) << 8),
                };
            }

            /**
             * @brief Get the heading of the x and y axis
             * 
             * @return float 
             */
            float get_heading() {
                // get the raw values
                auto raw = get_raw();

                // convert the value to an angle
                float angle = std::atan2(raw.x, raw.y);

                // convert the radians to a degree and limit between 
                // 0 and 360 degrees
                return std::fmod((angle * 180 / static_cast<float>(std::numbers::pi)), 360);
            }

            /**
             * @brief Get the temperature
             * 
             * @return uint16_t tempemperature in 100 / celsius
             */
            int16_t get_temp() {
                // create a array to store the data we want to receive
                uint8_t buffer[2];

                // read from the device
                read(register_loc::temperature_lsb, buffer, sizeof(buffer));

                // convert to a uint16_t
                return static_cast<int16_t>(buffer[0]) | 
                       static_cast<int16_t>(buffer[1]) << 8;
            }             
    };
}

#endif