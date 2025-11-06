#ifndef KLIB_ADXL345_HPP
#define KLIB_ADXL345_HPP

#include <cstdint>

#include <klib/vector3.hpp>
#include <klib/multispan.hpp>

namespace klib::hardware::accelerometer::detail {
    class adxl345_base {
    public:
        /**
         * @brief Wakeup rate options for POWER_CTL register (0x2D)
         * 
         */
        enum class wakeup : uint8_t {
            hz_8 = 0x00,
            hz_4 = 0x01,
            hz_2 = 0x02,
            hz_1 = 0x03
        };

        /**
         * @brief Interrupt output mapping for INT_MAP register (0x2F)
         * 
         */
        enum class output_map {
            int1 = 0x00,
            int2 = 0x01
        };

        // Data rate options for BW_RATE register (0x2C)
        enum class data_rate : uint8_t {
            hz_3200 = 0x0F,
            hz_1600 = 0x0E,
            hz_800 = 0x0D,
            hz_400 = 0x0C,
            hz_200 = 0x0B,
            hz_100 = 0x0A,
            hz_50 = 0x09,
            hz_25 = 0x08,
            hz_12_5 = 0x07,
            hz_6_25 = 0x06,
            hz_3_13 = 0x05,
            hz_1_56 = 0x04,
            hz_0_78 = 0x03,
            hz_0_39 = 0x02,
            hz_0_20 = 0x01,
            hz_0_10 = 0x00
        };

        // Data format options for DATA_FORMAT register (0x31)
        enum class range : uint8_t {
            g2 = 0x00,
            g4 = 0x01,
            g8 = 0x02,
            g16 = 0x03
        };

        // FIFO control options for FIFO_CTL register (0x38)
        enum class fifo_ctl : uint8_t {
            mode_bypass     = 0x00,
            mode_fifo       = 0x40,
            mode_stream     = 0x80,
            mode_trigger    = 0xC0,
            samples_mask    = 0x1F
        };

    protected:
        // Register addresses for ADXL345
        enum class reg : uint8_t {
            DEVID          = 0x00,
            THRESH_TAP     = 0x1D,
            OFSX           = 0x1E,
            OFSY           = 0x1F,
            OFSZ           = 0x20,
            DUR            = 0x21,
            LATENT         = 0x22,
            WINDOW         = 0x23,
            THRESH_ACT     = 0x24,
            THRESH_INACT   = 0x25,
            TIME_INACT     = 0x26,
            ACT_INACT_CTL  = 0x27,
            THRESH_FF      = 0x28,
            TIME_FF        = 0x29,
            TAP_AXES       = 0x2A,
            ACT_TAP_STATUS = 0x2B,
            BW_RATE        = 0x2C,
            POWER_CTL      = 0x2D,
            INT_ENABLE     = 0x2E,
            INT_MAP        = 0x2F,
            INT_SOURCE     = 0x30,
            DATA_FORMAT    = 0x31,
            DATAX0         = 0x32,
            DATAX1         = 0x33,
            DATAY0         = 0x34,
            DATAY1         = 0x35,
            DATAZ0         = 0x36,
            DATAZ1         = 0x37,
            FIFO_CTL       = 0x38,
            FIFO_STATUS    = 0x39
        };
    };
}

namespace klib::hardware::accelerometer {
    /**
     * @brief SPI driver for the adxl345 accelerometer
     * 
     * @details The adxl345 has a maximum spi clock speed of 5 MHz
     * 
     * @tparam Bus 
     */
    template <typename Bus>
    class adxl345_spi : public detail::adxl345_base {
    protected:
        // get a alias for easier access to registers
        using reg = detail::adxl345_base::reg;

    public:
        /**
         * @brief Init the ADXL345 sensor
         * 
         */
        void init(const detail::adxl345_base::data_rate rate, const detail::adxl345_base::range range = detail::adxl345_base::range::g2) {
            // set the range. This also makes sure the data format register is 
            // set correctly for 4-wire spi mode.
            set_range(range);

            // set the data rate
            set_data_rate(rate);

            // disable all the interrupts
            set_interrupts(false, false, false, false, false, false, false, false);

            // disable the fifo
            write_reg(reg::FIFO_CTL, static_cast<uint8_t>(fifo_ctl::mode_bypass));

            // set the device to measurement mode
            power_control(true, false, true, false, detail::adxl345_base::wakeup::hz_8);
        }

        /**
         * @brief Set the data rate of the sensor
         * 
         * @param rate 
         */
        void set_data_rate(const detail::adxl345_base::data_rate rate) {
            write_reg(reg::BW_RATE, static_cast<uint8_t>(rate));
        }

        /**
         * @brief Write to the POWER_CTL register to control power settings
         * 
         * @param link 
         * @param auto_sleep 
         * @param measure 
         * @param sleep 
         * @param wakeup 
         */
        void power_control(
            const bool link = false, const bool auto_sleep = false, const bool measure = true, 
            const bool sleep = false, const detail::adxl345_base::wakeup wakeup = false) 
        {
            const uint8_t ctl = (
                (link << 5) | (auto_sleep << 4) | (measure << 3) | (sleep << 2) | static_cast<uint8_t>(wakeup)
            );

            write_reg(reg::POWER_CTL, ctl);
        }

        /**
         * @brief Enable/Disable interrupt sources
         * 
         * @param data_ready 
         * @param single_tap 
         * @param double_tap 
         * @param activity 
         * @param inactivity 
         * @param freefall 
         * @param watermark 
         * @param overrun 
         */
        void set_interrupts(const bool data_ready, const bool single_tap = false, const bool double_tap = false,
            const bool activity = false, const bool inactivity = false, const bool freefall = false,
            const bool watermark = false, const bool overrun = false)
        {
            const uint8_t int_enable = (
                (overrun << 7) | (watermark << 6) | (freefall << 5) | (inactivity << 4) |
                (activity << 3) | (double_tap << 2) | (single_tap << 1) | data_ready
            );

            write_reg(reg::INT_ENABLE, int_enable);
        }

        /**
         * @brief Set the interrupt mapping for each interrupt source
         * 
         * @param data_ready 
         * @param single_tap 
         * @param double_tap 
         * @param activity 
         * @param inactivity 
         * @param freefall 
         * @param watermark 
         * @param overrun 
         */
        void set_interrupt_map(
            const detail::adxl345_base::output_map data_ready, const detail::adxl345_base::output_map single_tap = false, 
            const detail::adxl345_base::output_map double_tap = false, const detail::adxl345_base::output_map activity = false, 
            const detail::adxl345_base::output_map inactivity = false, const detail::adxl345_base::output_map freefall = false,
            const detail::adxl345_base::output_map watermark = false, const detail::adxl345_base::output_map overrun = false)
        {
            const uint8_t int_map = (
                (static_cast<uint8_t>(overrun) << 7) | (static_cast<uint8_t>(watermark) << 6) | 
                (static_cast<uint8_t>(freefall) << 5) | (static_cast<uint8_t>(inactivity) << 4) |
                (static_cast<uint8_t>(activity) << 3) | (static_cast<uint8_t>(double_tap) << 2) | 
                (static_cast<uint8_t>(single_tap) << 1) | static_cast<uint8_t>(data_ready)
            );

            write_reg(reg::INT_MAP, int_map);   
        }

        /**
         * @brief Set the range of the accelerometer
         * 
         * @param range 
         */
        void set_range(const detail::adxl345_base::range range) {
            // we need to be sure the data_format register is set correctly. We only 
            // support 4-wire spi mode. We also turn on full resolution mode.
            const uint8_t data_format = (
                static_cast<uint8_t>(range) | (0x1 << 3)
            );

            write_reg(reg::DATA_FORMAT, data_format);
        }

        /**
         * @brief Get the range setting of the accelerometer
         * 
         * @return detail::adxl345_base::range 
         */
        detail::adxl345_base::range get_range() {
            // read the data format register
            const uint8_t reg = static_cast<uint8_t>(reg::DATA_FORMAT) | (0b10 << 6);
            uint8_t data_format = 0;

            // read from the bus
            Bus::write_read(
                std::span{&reg, sizeof(reg)}, std::span<uint8_t>{&data_format, sizeof(data_format)}
            );

            // extract the range bits and return
            return static_cast<detail::adxl345_base::range>(data_format & 0x03);
        }

        /**
         * @brief Read all the axis data and convert to mg
         * 
         * @return klib::vector3i
         */
        klib::vector3i read_axis() const {
            // return the axis data in mg (we multipyly by 39 and divide by 10 to 
            // get the 3.9 mg/LSB from the datasheet)
            return ((raw_axis() * 39) / 10);
        }

        /**
         * @brief Read all the axis and convert to Gs
         * 
         * @return klib::vector3<float> 
         */
        klib::vector3<float> read_axis_g() const {
            // return the axis data in g (we divide by 256 to 
            // get the 1 g/LSB from the datasheet)
            return (raw_axis().template cast<float>() / 256.f);
        }

        /**
         * @brief Read all the axis data in raw format
         * 
         * @return klib::vector3i 
         */
        klib::vector3i raw_axis() const {
            // Multi-byte read: set MB (bit 6) and read (bit 7) in address
            const uint8_t reg = static_cast<uint8_t>(reg::DATAX0) | (0b11 << 6);

            // buffer to hold the read data. Note the first byte is dummy for the write
            uint8_t buffer[7] = {};

            // Write register address, then read 6 bytes
            Bus::write_read(std::span{&reg, sizeof(reg)}, buffer);

            int16_t x = static_cast<int16_t>((buffer[2] << 8) | buffer[1]);
            int16_t y = static_cast<int16_t>((buffer[4] << 8) | buffer[3]);
            int16_t z = static_cast<int16_t>((buffer[6] << 8) | buffer[5]);

            return klib::vector3i(x, y, z);
        }

    protected:
        /**
         * @brief Write to a register over SPI
         * 
         * @param reg_addr 
         * @param value 
         */
        void write_reg(reg reg_addr, uint8_t value) const {
            // combine the register address and value to a buffer. Note
            // the first bit of the register address is 0 for write
            const uint8_t buffer[2] = {static_cast<uint8_t>(reg_addr), value};

            // write the buffer to the bus
            Bus::write(buffer);
        }
    };
};

#endif