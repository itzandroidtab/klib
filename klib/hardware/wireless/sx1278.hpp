#ifndef KLIB_SX1278_HPP
#define KLIB_SX1278_HPP

#include <klib/delay.hpp>
#include <klib/irq_helper.hpp>

namespace klib::hardware::wireless {
    /**
     * @brief Driver for the sx1278. Currently only supports 
     * explicit header mode
     * 
     * @tparam Bus 
     * @tparam Cs 
     * @tparam RstPin 
     * @tparam IrqPin 
     */
    template <typename Bus, typename Cs, typename RstPin, typename IrqPin>
    class sx1278 {
    public:
        // using for the callbacks
        using interrupt_callback = void(*)();

        /**
         * @brief Available frequencies on the sx1278
         * 
         */
        enum class frequency {
            mhz_433 = 433'000'000,
            mhz_868 = 868'000'000,
            mhz_915 = 915'000'000,
        };

        /**
         * @brief Lna gain mode
         * 
         * g1 = 0dB (auto gain)
         * g2 = -6dB
         * g3 = -12dB
         * g4 = -24dB
         * g5 = -36dB
         * g6 = -48dB
         */
        enum class lna_gain {
            g1 = 0x01,
            g2 = 0x02,
            g3 = 0x03,
            g4 = 0x04,
            g5 = 0x05,
            g6 = 0x06
        };

        /**
         * @brief Available bandwidth modes
         * 
         */
        enum class bandwidth {
            khz_7_8 = 0b00,
            khz_10_4 = 0b01,
            khz_15_6 = 0b10,
            khz_20_8 = 0b11,
            khz_31_25 = 0b100,
            khz_41_7 = 0b101,
            khz_62_5 = 0b110,
            khz_125 = 0b111,
            khz_250 = 0b1000,
            khz_500 = 0b1001,
        };

        /**
         * @brief Available error coding rates
         * 
         */
        enum class coding_rate {
            rate_4_5 = 0b01,
            rate_4_6 = 0b10,
            rate_4_7 = 0b11,
            rate_4_8 = 0b111,
        };

        /**
         * @brief Available spreading factor rates
         * 
         */
        enum class spreading_factor {
            sf_6 = 6,
            sf_7 = 7,
            sf_8 = 8,
            sf_9 = 9,
            sf_10 = 10,
            sf_11 = 11,
            sf_12 = 12,
        };

    protected:
        // irq helper for the interrupt register
        static inline irq_helper_status<8> helper;

        // callbacks
        static inline interrupt_callback transmit_callback = nullptr;
        static inline interrupt_callback receive_callback = nullptr;

        /**
         * @brief All the registers available on the sx1278
         * 
         */
        enum class reg: uint8_t {
            fifo = 0x00,
            operating_mode = 0x01,
            rf_carrier_frequency_msb = 0x06,
            rf_carrier_frequency_mid = 0x07,
            rf_carrier_frequency_lsb = 0x08,
            pa_selection = 0x09,
            pa_ramp = 0x0a,
            over_current_protection = 0x0b,
            lna_settings = 0x0c,
            
            fifo_address_ptr = 0x0d,
            fifo_tx_base_address = 0x0e,
            fifo_rx_base_address = 0x0f,
            fifo_rx_start_address = 0x10,
            irq_flags_mask = 0x11,
            irq_flags = 0x12,
            rx_byte_count = 0x13,
            rx_header_count_msb = 0x14,
            rx_header_count_lsb = 0x15,
            rx_packet_count_msb = 0x16,
            rx_packet_count_lsb = 0x17,

            modem_status = 0x18,
            last_packet_snr = 0x19,
            last_packet_rssi = 0x1a,
            current_rssi = 0x1b,
            hop_channel = 0x1c,

            modem_config0 = 0x1d,
            modem_config1 = 0x1e,
            receiver_timeout = 0x1f,
            preamble_msb = 0x20,
            preamble_lsb = 0x21,
            payload_length = 0x22,
            max_payload_length = 0x23,
            
            hop_period = 0x24,
            fifo_rx_current_address = 0x25,
            modem_config2 = 0x26,
            est_frequency_error_msb = 0x28,
            est_frequency_error_mid = 0x29,
            est_frequency_error_lsb = 0x2a,

            wide_band_rssi = 0x2b,
            if_frequency0 = 0x2f,
            if_frequency1 = 0x30,
            detection_optimize = 0x31,
            invert_iq0 = 0x33,
            high_bw_optimize0 = 0x36,
            detection_threshold = 0x37,
            sync_word = 0x39,
            high_bw_optimize1 = 0x3a,
            invert_iq1 = 0x3b,

            dio_mapping0 = 0x40,
            dio_mapping1 = 0x41,
            version = 0x42,
            xtal_tcxo = 0x4b,
            pa_dac = 0x4d,
            former_temp = 0x5b,
            agc_reference = 0x61,
            agc_threshold0 = 0x62,
            agc_threshold1 = 0x63,
            agc_threshold2 = 0x64,
            pll = 0x70,
        };

        /**
         * @brief Available device operating modes
         * 
         */
        enum class device_mode {
            sleep = 0x0,
            standby = 0x1,
            tx_frequency_synthesis = 0x2,
            transmit = 0x3,
            rx_frequency_synthesis = 0x4,
            receive_continuous = 0x5,
            receive_single = 0x6,
            channel_activity_detection = 0x7
        };

        /**
         * @brief Write a register with data
         * 
         * @param command 
         * @param data 
         * @param size
         */
        static void write_reg(const reg command, const uint8_t* data, const uint8_t size) {
            // set the write bit
            const uint8_t c = static_cast<uint8_t>(command) | 0x80;

            // set the chipselect to write to the device
            Cs::template set<false>();

            // write the command to the display
            Bus::write(&c, sizeof(c));

            // write the data
            Bus::write(data, size);

            // unselect the device
            Cs::template set<true>();
        }  

        /**
         * @brief Write one byte to a register
         * 
         * @param command 
         * @param data 
         * @param size 
         */
        static void write_reg(const reg command, const uint8_t data) {
            // write using the array write
            write_reg(command, &data, sizeof(data));
        }  

        /**
         * @brief multiple bytes from a register
         * 
         * @param command 
         * @param data 
         * @param size 
         */
        static void read_reg(const reg command, uint8_t* data, const uint8_t size) {
            // set the chipselect to write to the device
            Cs::template set<false>();

            // write the command to the display
            Bus::write(reinterpret_cast<const uint8_t*>(&command), sizeof(command));
            
            // read the size amount of data in the data array
            Bus::write_read(nullptr, data, size);

            // unselect the device
            Cs::template set<true>();
        }  

        /**
         * @brief Read a single register
         * 
         * @param command 
         * @return uint8_t 
         */
        static uint8_t read_reg(const reg command) {
            uint8_t res;

            // read a single byte
            read_reg(command, &res, sizeof(res));

            return res;
        }

        /**
         * @brief Set the over current protection
         * 
         * @tparam MAmp 
         */
        template <uint8_t MAmp>
        static void set_over_current_protection() {
            // make sure the input is valid
            static_assert(MAmp <= 240, "Invalid over protection value set");

            if constexpr (!MAmp) {
                // disable the over current protection
                write_reg(reg::over_current_protection, 0x00);
            }
            else if constexpr (MAmp <= 120) {
                // set the over current protection
                write_reg(reg::over_current_protection, (MAmp + 45) / 5);
            }
            else {
                // set the over current protection
                write_reg(reg::over_current_protection, (MAmp + 30) / 10);
            }
        }

        static void on_receive(const uint32_t status, const uint32_t mask) {
            // check if we have a error
            if (status & (0x1 << 5)) {
                // we have a error. Skip this interrupt
                return;
            }

            // call the callback we have data
            if (receive_callback) {
                receive_callback();
            }
        }

        static void on_transmit(const uint32_t status, const uint32_t mask) {
            // we are done transmitting change the dio mapping
            // back to receive mode
            change_dio_mapping<true>();

            // change to continueous receive mode
            change_mode<device_mode::receive_continuous>();

            // check if we have a error
            if (status & (0x1 << 5)) {
                // we have a error. Skip this interrupt
                return;
            }

            // call the callback we are done transmitting
            if (transmit_callback) {
                transmit_callback();
            }
        }

        /**
         * @brief Interrupt handler. Calls the specific function
         * that handles the interrupt request using the irq 
         * helper
         * 
         */
        static void irq_handler() {
            // check what interrupt we are handling
            const uint8_t status = read_reg(reg::irq_flags);

            // clear the interrupt bits
            write_reg(reg::irq_flags, status);

            // handle the interrupts
            helper.handle_irq(status, 0xff);
        }

        /**
         * @brief Changes the sx1278 to a different mode
         * 
         * @tparam Mode 
         */
        template <device_mode Mode>
        static void change_mode() {
            // change to continueous receive mode
            write_reg(reg::operating_mode, (0x1 << 7) | static_cast<uint8_t>(Mode));
        }

        /**
         * @brief Change the current mode of the sx1278
         * 
         * @tparam RecieveMode 
         */
        template <bool RecieveMode>
        static void change_dio_mapping() {
            // different modes for dio0
            enum {
                rx_done = 0x0,
                tx_done = 0x1,
                cad_done = 0x2,
            };

            // unfortunaly we cannot set all the bits
            // so we need to switch between them based
            // on the current mode
            if constexpr (RecieveMode) {
                // set the receive done interrupt for the 
                // dio mapping
                write_reg(reg::dio_mapping0, rx_done << 6);
            }
            else {
                // set the tx done mode
                write_reg(reg::dio_mapping0, tx_done << 6);
            }
        }

    public:
        /**
         * @brief Init the sx1278
         * 
         * @tparam Frequency 
         */
        template <
            frequency Frequency, 
            bandwidth Bandwidth = bandwidth::khz_125, 
            coding_rate ErrorCodingRate = coding_rate::rate_4_5,
            spreading_factor SpreadingFactor = spreading_factor::sf_8,
            bool Crc = false,
            bool LowPowerPin = false
        >
        static void init(interrupt_callback receive = nullptr, interrupt_callback transmit = nullptr) {
            using namespace klib::time;

            // update the interrupt callbacks
            receive_callback = receive;
            transmit_callback = transmit;

            // lower the chip select to select the device
            Cs::template set<false>();

            // reset the sx1278
            RstPin::template set<false>();
            klib::delay(10_ms);

            // wait at least 10ms at the end of a Power on 
            // Reset cycle before communicating with the
            // sx1278
            RstPin::template set<true>();
            klib::delay(10_ms);

            Cs::template set<true>();

            // enable lora mode and set the device to sleep 
            // mode so we can change the frequency
            change_mode<device_mode::sleep>();

            // get the frequency to store
            constexpr static uint32_t freq = (static_cast<uint64_t>(Frequency) << 19) / 32000000;

            // set the frequency
            write_reg(reg::rf_carrier_frequency_msb, (freq >> 16) & 0xff);
            write_reg(reg::rf_carrier_frequency_mid, (freq >> 8) & 0xff);
            write_reg(reg::rf_carrier_frequency_lsb, freq & 0xff);

            // set to explict header mode and the provided bandwidth
            write_reg(reg::modem_config0, (
                (static_cast<uint8_t>(Bandwidth) << 4) | 
                (static_cast<uint8_t>(ErrorCodingRate) << 4))
            );

            // set the spreading factor and the CRC flag
            write_reg(reg::modem_config1, 
                (static_cast<uint8_t>(SpreadingFactor) << 4) | 
                (static_cast<uint8_t>(Crc) << 2)
            );

            // set the detection thresholds based on the spreading factor
            if constexpr (SpreadingFactor == spreading_factor::sf_6) {
                write_reg(reg::detection_optimize, 0x3 | (0xc << 3));
                write_reg(reg::detection_threshold, 0x0c);
            }
            else {
                write_reg(reg::detection_optimize, 0x5 | (0xc << 3));
                write_reg(reg::detection_threshold, 0x0a);
            }

            // set the bandwidth registers based on the errata file
            if constexpr (Bandwidth == bandwidth::khz_500) {
                // set the correct values for 433mhz and the rest
                if constexpr (Frequency == frequency::mhz_433) {
                    write_reg(reg::high_bw_optimize0, 0x02);
                    write_reg(reg::high_bw_optimize1, 0x64);
                }
                else {
                    write_reg(reg::high_bw_optimize0, 0x02);
                    write_reg(reg::high_bw_optimize1, 0x7f);
                }
            }
            else {
                // all other combinations should write 0x03 to the
                // register to automaticly configure 
                // high_bw_optimize1
                write_reg(reg::high_bw_optimize0, 0x03);
            }

            // allow full access to the fifo on rx and tx
            write_reg(reg::fifo_tx_base_address, 0x00);
            write_reg(reg::fifo_rx_base_address, 0x00);

            // set the lna gain to max (with auto gain) and enable boost
            set_gain<lna_gain::g1, true>();

            // set the output power to the max for the current power pin
            set_tx_power<LowPowerPin, (LowPowerPin ? 14 : 20)>();

            // change to standby
            change_mode<device_mode::standby>();

            // clear all the interrupt bits
            write_reg(reg::irq_flags, 0xff);

            // add our interrupt handles
            helper.register_irq<6>(on_receive);
            helper.register_irq<3>(on_transmit);

            // register our interrupt handler
            IrqPin::template init<IrqPin::edge::rising>(irq_handler);

            // change to receive mode
            change_dio_mapping<true>();

            // change to receive mod
            change_mode<device_mode::receive_continuous>();
        }

        /**
         * @brief Set the gain mode
         * 
         * @tparam gain 
         * @tparam boost 
         */
        template <lna_gain Gain, bool Boost>
        static void set_gain() {
            // get the correct bits to set when using
            // boost
            const uint8_t result = Boost ? 0x3 : 0x0;

            // check if we have gain enabled
            if constexpr (Gain == lna_gain::g1) {
                // enable auto gain (and keep the other register
                // values)
                write_reg(reg::modem_config2, 0x04 | read_reg(reg::modem_config2));

                // set the lna settings
                write_reg(reg::lna_settings, result);
            }
            else {
                // disable auto gain (and keep the other register
                // values)
                write_reg(reg::modem_config2, 
                    read_reg(reg::modem_config2) & (~(0x1 << 2))
                );

                // set the lna settings
                write_reg(reg::lna_settings, 
                    result | static_cast<uint8_t>(Gain) << 5
                );
            }
        }

        /**
         * @brief Set the tx power
         * 
         * @tparam LowPowerPin (flag if RFO_LF or PA_BOOST pin is used)
         * @tparam Power power level
         */
        template <bool LowPowerPin, uint8_t Power>
        static void set_tx_power() {
            // check if the boost pin is used
            if constexpr (LowPowerPin) {
                // we are using the low power pin (RFO_LF). The
                // limit on this pin is 14
                static_assert(Power <= 14, "Invalid power value for given pin");

                // write the power level (and set the max power level to the max)
                write_reg(reg::pa_selection, Power | (0x7 << 4));
            }
            else {
                static_assert(Power <= 20, "Invalid power value for given pin");

                // configure the over current protection
                set_over_current_protection<240>();

                // check if we need to enable the 20dBm boost
                if constexpr (Power > 17) {
                    // enable the boost
                    write_reg(reg::pa_dac, 0x87);

                    // write the power level (and set the max power level to the max)
                    write_reg(reg::pa_selection, (
                        (0x1 << 7) | (0x7 << 4) | (Power - 5))
                    );
                }
                else {
                    // disable the boost
                    write_reg(reg::pa_dac, 0x84);

                    // write the power level (and set the max power level to the max)
                    write_reg(reg::pa_selection, (
                        (0x1 << 7) | (0x7 << 4) | (klib::max(2, Power) - 2))
                    );
                }
            }
        }

        /**
         * @brief Write data to the sx1278
         * 
         * @tparam Async 
         * @param data 
         * @param size 
         */
        template <bool Async = false>
        static void write(const uint8_t* data, const uint8_t size) {
            // change the chip to standby
            change_mode<device_mode::standby>();

            // reset the fifo address
            write_reg(reg::fifo_address_ptr, 0x00);

            // write all the data we have in the fifo
            for (uint8_t i = 0; i < size; i++) {
                write_reg(reg::fifo, data[i]);
            }

            // write the amount of bytes we wrote
            write_reg(reg::payload_length, size);

            // change to transmit mode
            change_dio_mapping<false>();

            // change to transmit mode
            change_mode<device_mode::transmit>();

            // check if we should wait until we are done transmitting
            if constexpr (!Async) {
                // wait until the chip is done transmitting
                while ((read_reg(reg::operating_mode) & 0x7) == static_cast<uint8_t>(device_mode::transmit)) {
                    // do nothing
                }
            }
        }

        /**
         * @brief Read data in the provided array
         * 
         * @param data 
         * @return uint8_t 
         */
        static uint8_t read(uint8_t *const data, const uint8_t max_size) {
            // read the packet length
            const uint32_t size = read_reg(reg::rx_byte_count);

            // check if we have data available
            if (!size) {
                return 0;
            }

            // change the current address to the location the last packet has 
            // been received is stored
            write_reg(reg::fifo_address_ptr, read_reg(reg::fifo_rx_start_address));

            // read the whole packet length into the buffer
            for (uint32_t i = 0; i < size && i < max_size; i++) {
                // read the fifo in the buffer
                data[i] = read_reg(reg::fifo);
            }
            
            // reset the fifo address back to the start
            write_reg(reg::fifo_address_ptr, 0x00);

            // return the smallest of the two sizes
            return klib::min(max_size, size);
        }
    };
}

#endif