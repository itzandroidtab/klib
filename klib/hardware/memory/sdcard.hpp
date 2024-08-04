#ifndef KLIB_SDCARD_HPP
#define KLIB_SDCARD_HPP

#include <cstdint>
#include <optional>
#include <type_traits>

#include <klib/math.hpp>

namespace klib::io {
    /**
     * @brief Sd card interface using spi. Needs to have
     * control over the chip select pin as it uses it to
     * send the spi mode command with the chip select pin
     * high
     *
     * credits to http://elm-chan.org/docs/mmc/mmc_e.html
     *
     * @tparam Bus
     * @tparam Cs
     */
    template <typename Bus, typename Cs>
    class sd {
    protected:
        /**
         * @brief Generic sd card commands
         *
         */
        enum class sd_cmd {
            // Go to idle state
            CMD0 = 0x00,
            // Initiate initialization process for mmc
            CMD1 = 0x01,
            // Initiate initialization process for SDC
            ACMD41 = 0x29,
            // check voltage range (only for SDC v2)
            CMD8 = 0x08,
            // read CSD register
            CMD9 = 0x09,
            // read CID register
            CMD10 = 0x0a,
            // stop reading data
            CMD12 = 0x0c,
            // set the block lenght
            CMD16 = 0x10,
            // read a block
            CMD17 = 0x11,
            // read multiple blocks
            CMD18 = 0x12,
            // set block count (only for mmc)
            CMD23 = 0x17,
            // define number of blocks to pre-erase next multi-
            // block write command (only for sdc)
            ACMD23 = 0x17,
            // write block
            CMD24 = 0x18,
            // write multiple blocks
            CMD25 = 0x19,
            // Leading command for ACMD commands
            CMD55 = 0x37,
            // read OCR
            CMD58 = 0x3a,
        };

        /**
         * @brief SD type detected during init
         *
         */
        enum class type {
            unknown,
            sd_v1,
            sd_v2,
            mmc_v3
        };

        #pragma pack(push, 1)
        /**
         * @brief command to send to the sd card
         *
         */
        struct command {
            // command number
            uint8_t cmd: 6;

            // header of the command
            uint8_t header: 2 = 0b01;

            // command argument
            uint32_t argument;

            // command footer
            bool footer: 1 = 0b1;

            // crc bits.
            uint8_t crc: 7;
        };

        /**
         * @brief Response 3/7 for CMD58 and CMD8
         *
         */
        struct response37 {
            // r1 response
            uint8_t r1;

            // trailing data
            uint32_t data;
        };
        #pragma pack(pop)

        // make sure the command is the right size
        static_assert(sizeof(command) == (48 / 8), "invalid command size");
        static_assert(sizeof(uint8_t) == sizeof(uint8_t), "invalid response size");

        static void write_bus(const uint8_t *const data, const uint16_t size) {
            // write the command to the spi bus
            Bus::write({data, size});
        }

        template <typename T, typename = std::enable_if<!std::is_pointer_v<T>>>
        static void write_bus(const T data) {
            return write_bus(reinterpret_cast<const uint8_t *const>(&data), sizeof(data));
        }

        /**
         * @brief Change the sd card to spi mode
         *
         */
        static void change_mode() {
            // buffer to send at least 74 clock pulses
            uint8_t buffer[10];

            // set the buffer to all 1's for the sd specification
            for (uint32_t i = 0; i < sizeof(buffer); i++) {
                buffer[i] = 0xff;
            }

            // send the clock pulses to the device to force the
            // spi mode
            Bus::write(buffer);
        }

        static uint8_t receive_response(uint16_t cycles = 10) {
            // data to send while waiting
            const uint8_t tx = 0xff;

            // response byte
            uint8_t r1 = 0xff;

            // make sure the in and out match in size
            static_assert(sizeof(tx) == sizeof(r1));

            // try to read or wait for a timeout
            while ((r1 & 0x80) && cycles) {
                // read a byte
                Bus::write_read({&tx, sizeof(tx)}, {&r1, sizeof(r1)});

                // decrement the timeout
                cycles--;
            }

            // return the last read data
            return r1;
        }

        static uint8_t wait_till_ready(uint32_t timeout = (500'000 / 20)) {
            // data to send while waiting
            const uint8_t tx = 0xff;

            // clear the output response
            uint8_t rx = 0x00;

            // make sure the in and out match in size
            static_assert(sizeof(tx) == sizeof(rx));

            // try to read or wait for a timeout
            while (rx != 0xff && timeout) {
                // read a byte
                Bus::write_read({&tx, sizeof(tx)}, {&rx, sizeof(rx)});

                // decrement the timeout
                timeout--;
            }

            // return if we had a timeout or we
            // have received data
            return rx;
        }

        template <bool R1Only = true>
        static auto dummy_response() {
            if constexpr (R1Only) {
                return static_cast<uint8_t>(0xff);
            }
            else {
                return response37{0xff, 0x00};
            }
        }

        template <bool R1Only = true>
        static auto write_command(const command cmd) {
            // wait until the device is ready
            if (wait_till_ready() != 0xff) {
                // return a dummy response
                return dummy_response<R1Only>();
            }

            // send the command to the bus
            write_bus(cmd);

            // receive the response. Can take
            // up to 10 reads
            const uint8_t r1 = receive_response();

            // check if we need to read more data
            if constexpr (R1Only) {
                // return the result
                return r1;
            }
            else {
                // read 4 more bytes
                const uint8_t tx[4] = {0xff, 0xff, 0xff, 0xff};

                // r3/r7 trailing data
                uint32_t r37 = 0x00;

                // make sure the in and out match in size
                static_assert(sizeof(tx) == sizeof(r37));

                // read the data
                Bus::write_read(
                    tx, {reinterpret_cast<uint8_t*>(&r37), sizeof(r37)}
                );

                // return the response for r3/r7
                return response37{r1, r37};
            }
        }

        static uint8_t write_app_command(const command cmd, const uint16_t timeout = 150) {
            // create the leading app command (for ACMD commands)
            const command app = {
                .cmd = static_cast<uint8_t>(sd_cmd::CMD55),
                .argument = 0x00,
                .crc = 0x00
            };

            // write the leading app command
            const uint8_t r1 = write_command(app);

            // check if the result is valid
            if (r1 > 0x01) {
                return r1;
            }

            // write the app command
            return write_command(cmd);
        }

    protected:
        static bool app_init(const command cmd, const bool app_command) {
            // timeout based on 400K (20 us per byte) and 1 sec timeout
            uint32_t t = (1'000'000 / 20);
            uint8_t res;

            // wait for a timeout or the correct response
            for (; t > 0; t--) {
                // get the data from the function
                if (app_command) {
                    res = write_app_command(cmd);
                }
                else {
                    res = write_command(cmd);
                }

                // check if the device is initialized
                if (res == 0x0 || res != 0x1) {
                    // continue, the sd card has initialized
                    break;
                }
            }

            // check if the device is initialized
            if (t == 0 || res != 0x0) {
                // timeout reached stop
                return false;
            }

            // return init was success
            return true;
        }

        static bool change_blocksize(const uint32_t blocksize = block_size) {
            // set the block size
            const command set_block = {
                .cmd = static_cast<uint8_t>(sd_cmd::CMD16),
                .argument = klib::bswap(blocksize),
                .crc = 0x00
            };

            // write the command
            const uint8_t r1 = write_command(set_block);

            // return if the resonse is valid
            return r1 == 0x00;
        }

        static void discard_crc() {
            const uint8_t tx[] = {0xff, 0xff};
            uint8_t crc[2];

            // make sure the in and out match in size
            static_assert(sizeof(tx) == sizeof(crc));

            // read the crc and discard it
            Bus::write_read(tx, crc);
        }

        static bool read_block(uint8_t *const data, uint32_t timeout = (200'000 / 20)) {
            // data to send while waiting
            const uint8_t tx = 0xff;

            // prefill r1 to a response we are expecting
            uint8_t r1 = 0xff;

            // make sure the in and out match in size
            static_assert(sizeof(tx) == sizeof(r1));

            // try to read or wait for a timeout
            while (r1 == 0xff && timeout) {
                // read a byte (we only read 1 byte here)
                Bus::write_read({&tx, sizeof(tx)}, {&r1, sizeof(r1)});

                // decrement the timeout
                timeout--;
            }

            if (!timeout || r1 != 0xfe) {
                return false;
            }

            // create a array with all 0xff to transmit
            uint8_t buffer[64];

            // set the buffer to all 1's for the sd specification
            for (uint32_t i = 0; i < sizeof(buffer); i++) {
                buffer[i] = 0xff;
            }

            // read using the buffer size
            for (uint32_t i = 0; i < (block_size / sizeof(buffer)); i++) {
                Bus::write_read(
                    buffer, {&data[i * sizeof(buffer)], sizeof(buffer)}
                );
            }

            // read the crc and discard it
            discard_crc();

            return true;
        }

        static bool write_block(const uint8_t *const data, const uint8_t token) {
            // wait until the sd is ready
            if (wait_till_ready() != 0xff) {
                return false;
            }

            // transmite the single block token
            Bus::write({&token, sizeof(token)});

            // check if we are sending a stop
            if (token == 0xfd) {
                return true;
            }

            // write the block to the card
            Bus::write({data, block_size});

            // read the crc and discard it
            discard_crc();

            const uint8_t tx = 0xff;
            uint8_t rx = 0x00;

            // make sure the in and out match in size
            static_assert(sizeof(tx) == sizeof(rx));

            // wait for the data response
            for (uint32_t i = 0; i < 64; i++) {
                // read the data response
                Bus::write_read({&tx, sizeof(tx)}, {&rx, sizeof(rx)});

                // check if the data is accepted
                if ((rx & 0x1f) == 0x05) {
                    break;
                }
            }

            uint8_t r1 = 0x0;

            // make sure the in and out match in size
            static_assert(sizeof(tx) == sizeof(r1));

            // wait until the card is not busy anymore
            while (r1 == 0) {
                Bus::write_read({&tx, sizeof(tx)}, {&r1, sizeof(r1)});
            }

            return (rx & 0x1f) == 0x05;
        }

        static type init_impl() {
            // set the cs pin manually
            Cs::template set<true>();

            // change the device to spi mode
            change_mode();

            // set the cs pin manually
            Cs::template set<false>();

            // create a goto idle command
            const command go_idle = {
                .cmd = static_cast<uint8_t>(sd_cmd::CMD0),
                .argument = 0x00,
                .crc = 0x95 >> 1
            };

            // send the go to idle command
            if (write_command(go_idle) != 0x01) {
                // restore the cs pin
                Cs::template set<true>();

                // return a unknown type if it does not respond to a
                // go to idle command
                return type::unknown;
            }

            // create a goto idle command
            const command voltage_range = {
                .cmd = static_cast<uint8_t>(sd_cmd::CMD8),
                .argument = klib::bswap(static_cast<uint32_t>(0x1aa)),
                .crc = 0x87 >> 1
            };

            // check if the device supports the voltage command (SD v2)
            if (const response37 r37 = write_command<false>(voltage_range); r37.r1 == 0x1) {
                // check if the response is the same
                if (r37.data != voltage_range.argument) {
                    // restore the cs pin
                    Cs::template set<true>();

                    // voltage range is not correct return unknown card
                    return type::unknown;
                }

                // create a command to init the SD v2 card
                const command init_hs = {
                    .cmd = static_cast<uint8_t>(sd_cmd::ACMD41),
                    .argument = klib::bswap(static_cast<uint32_t>(0x40000000)),
                    .crc = 0x00
                };

                // init the v2 card
                if (!app_init(init_hs, true)) {
                    // restore the cs pin
                    Cs::template set<true>();

                    // could not init card return unknown
                    return type::unknown;
                }

                // check if the device is in block mode
                const command block_info = {
                    .cmd = static_cast<uint8_t>(sd_cmd::CMD58),
                    .argument = 0x00,
                    .crc = 0x00
                };

                // write the ocr command
                const auto r37_ocr = write_command<false>(block_info);

                // check if the command succeded
                if (r37_ocr.r1 != 0x00) {
                    // restore the cs pin
                    Cs::template set<true>();

                    return type::unknown;
                }

                // check if block mode is enabled
                if (!((r37_ocr.data >> 30) & 0x1)) {
                    // block mode not enabled force block size to 512 bytes for fat
                    if (!change_blocksize()) {
                        // restore the cs pin
                        Cs::template set<true>();

                        return type::unknown;
                    }
                }

                // restore the cs pin
                Cs::template set<true>();

                return type::sd_v2;
            }

            // card is not v2. Must be v1 or mmc. Create the init command for v1
            const command init_v1 = {
                .cmd = static_cast<uint8_t>(sd_cmd::ACMD41),
                .argument = klib::bswap(static_cast<uint32_t>(0x00)),
                .crc = 0x00
            };

            if (app_init(init_v1, true)) {
                // v1 card detected. change the block size to 512 for fat
                change_blocksize();

                // restore the cs pin manually
                Cs::template set<true>();

                // check if we have a valid v1 card
                return type::sd_v1;
            }

            // no v2 or v1 card. must be mmc. create the command to init
            // the mmc
            const command init_mmc = {
                .cmd = static_cast<uint8_t>(sd_cmd::CMD1),
                .argument = 0x00,
                .crc = 0x00
            };

            if (app_init(init_mmc, false)) {
                // mmc detected. change the block size to 512 for fat
                change_blocksize();

                // restore the cs pin manually
                Cs::template set<true>();

                // return mmc v3
                return type::mmc_v3;
            }

            // restore the cs pin manually
            Cs::template set<true>();

            return type::unknown;
        }

    public:
        // block size of this storage type
        constexpr static uint32_t block_size = 512;

        /**
         * @brief Init the sd card. Will return if sd has been initialized
         *
         * @return true
         * @return false
         */
        static bool init() {
            // return if we have a valid type
            return init_impl() != type::unknown;
        }

        /**
         * @brief Write 1 or more blocks to the sd card
         *
         * @param sector
         * @param data
         * @param blocks
         * @return true
         * @return false
         */
        static bool write(const uint32_t sector, const uint8_t *const data, const uint32_t blocks) {
            // set the cs pin manually
            Cs::template set<false>();

            // write using the single block write
            for (uint32_t i = 0; i < blocks; i++) {
                const command cmd = {
                    .cmd = static_cast<uint8_t>(sd_cmd::CMD24),
                    .argument = klib::bswap(static_cast<uint32_t>(sector + i)),
                    .crc = 0x00
                };

                // send the write single block command
                const uint8_t t = write_command(cmd);

                if (t != 0x00) {
                   // clear the cs pin manually
                    Cs::template set<true>();

                    // return error while writing
                    return false;
                }

                // read a block from the card
                if (!write_block(&data[i * block_size], 0xfe)) {
                    // clear the cs pin manually
                    Cs::template set<true>();

                    return false;
                }
            }

            // clear the cs pin manually
            Cs::template set<true>();

            return true;
        }

        /**
         * @brief Read 1 or more blocks from the sd card
         *
         * @param sector
         * @param data
         * @param blocks
         * @return true
         * @return false
         */
        static bool read(const uint32_t sector, uint8_t *const data, const uint32_t blocks) {
            // set the cs pin manually
            Cs::template set<false>();

            // read by using the single block read command
            for (uint32_t i = 0; i < blocks; i++) {
                const command cmd = {
                    .cmd = static_cast<uint8_t>(sd_cmd::CMD17),
                    .argument = klib::bswap(static_cast<uint32_t>(sector + i)),
                    .crc = 0x00
                };

                // send the read single block command
                const uint8_t t = write_command(cmd);

                if (t != 0x00) {
                   // clear the cs pin manually
                    Cs::template set<true>();

                    // return error while writing
                    return false;
                }

                // read a block from the card
                if (!read_block(&data[i * block_size])) {
                    // clear the cs pin manually
                    Cs::template set<true>();

                    return false;
                }
            }

            // clear the cs pin manually
            Cs::template set<true>();

            return true;
        }
    };
}

#endif