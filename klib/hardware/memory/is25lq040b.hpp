#ifndef KLIB_IS25LQ040B_HPP
#define KLIB_IS25LQ040B_HPP

#include <cstdint>

namespace klib::hardware::memory {
    /**
     * @brief Driver for the is25lq040
     * 
     * @tparam Bus 
     * @tparam Cs 
     * @tparam BusyWait check the busy wait flag after enabling 
     * the write enable latch. This might be needed for slower 
     * memory devices for proper writes
     */
    template <typename Bus, typename Cs, bool BusyWait = false>
    class is25lq040b {
    protected:
        /**
         * @brief Some of the commands available 
         * on the is25lq040b
         * 
         */
        enum class cmd: uint8_t {
            // read command
            read_fast = 0x0b,

            // write command
            write = 0x02,

            // erase commands
            sector_erase = 0x20,
            block_erase_32k = 0x52,
            block_erase_64k = 0xd8,
            chip_erase = 0x60,

            // other commands
            read_status = 0x05,
            write_enable = 0x06,
            write_status = 0x01,
            reset_enable = 0x66,
            reset = 0x99,
        };
        
        /**
         * @brief Set the write enable bit to enable write 
         * and erase commands
         * 
         */
        static void write_enable() {
            // select the device
            Cs::template set<false>();

            // create a array for the command to erase
            const uint8_t command[] = {
                static_cast<uint8_t>(cmd::write_enable),
            };

            // write the command
            Bus::write(command);

            // clear the chip select
            Cs::template set<true>();

            // check if we should wait on the busy wait flag
            if constexpr (BusyWait) {
                // wait until the busy flag is cleared
                while (is_busy<true>()) {
                    // wait
                }
            }
        }

        /**
         * @brief Get the status register of the device 
         * 
         * @return uint8_t 
         */
        static uint8_t get_status() {
            // select the device
            Cs::template set<false>();

            // create a array for the command (with a dummy byte)
            const uint8_t command[] = {
                static_cast<uint8_t>(cmd::read_status), 
            };

            // write the command to fast read
            Bus::write(command);

            uint8_t status = 0x00;

            // write the data we want to write
            Bus::write_read({}, {&status, sizeof(status)});

            // clear the chip select
            Cs::template set<true>();

            return status;
        }

    public:
        /**
         * @brief Available erase modes
         * 
         */
        enum class erase_mode: uint8_t {
            sector = static_cast<uint8_t>(cmd::sector_erase),
            block_32k = static_cast<uint8_t>(cmd::block_erase_32k),
            block_64k = static_cast<uint8_t>(cmd::block_erase_64k),
        };

        /**
         * @brief Reset the memory device
         * 
         */
        static void reset() {
            // select the device
            Cs::template set<false>();

            // create a array for the command
            const uint8_t command_enable[] = {
                static_cast<uint8_t>(cmd::reset_enable)
            };

            // write the command
            Bus::write(command_enable);

            // clear the chip select
            Cs::template set<true>();

            // select the device
            Cs::template set<false>();

            const uint8_t command[] = {
                static_cast<uint8_t>(cmd::reset)
            };

            // write the reset command
            Bus::write(command);

            // clear the chip select
            Cs::template set<true>();
        }

        /**
         * @brief Init the memory device
         * 
         */
        static void init() {
            // reset the device
            reset();

            // enable the write bit
            write_enable();

            // select the device
            Cs::template set<false>();

            // create a array for the command (with a dummy byte)
            const uint8_t command[] = {
                static_cast<uint8_t>(cmd::write_status),
                0x00
            };

            // write the command to set the write status
            Bus::write(command);

            // clear the chip select
            Cs::template set<true>();
        }

        /**
         * @brief Check if the device is still busy writing/erasing
         * 
         * @tparam BusyFlagOnly when this function will only check 
         * the busy flag instead of also checking the write enable
         * latch
         * @return true 
         * @return false 
         */
        template <bool BusyFlagOnly = false>
        static bool is_busy() {
            // get the status register
            const uint8_t status = get_status();

            // check if we should wait only on the busy
            // flag
            if constexpr (BusyFlagOnly) {
                // return if the write busy bit is set
                return (status & 0x1);
            }
            else {
                // return if the write busy bit is set or if the 
                // write enable latch is still latched
                return (status & 0x3);
            }
        }

        /**
         * @brief Read data from the memory device
         * 
         * @param address 
         * @param data 
         * @param size 
         */
        static void read(const uint32_t address, uint8_t *const data, const uint32_t size) {
            // select the device
            Cs::template set<false>();

            // create a array for the command (with a dummy byte)
            const uint8_t command[] = {
                static_cast<uint8_t>(cmd::read_fast), 
                static_cast<uint8_t>((address >> 16) & 0xff), 
                static_cast<uint8_t>((address >> 8) & 0xff), 
                static_cast<uint8_t>(address & 0xff), 0x00
            };

            // write the command to fast read
            Bus::write(command);

            // write the data we want to write
            Bus::write_read({}, {data, size});

            // clear the chip select
            Cs::template set<true>();
        }

        /**
         * @brief Write to the address with data
         * 
         * @param address 
         * @param data 
         * @param size 
         */
        static void write(const uint32_t address, const uint8_t *const data, const uint16_t size) {
            // enable the write bit
            write_enable();

            // select the device
            Cs::template set<false>();

            const uint8_t command[] = {
                static_cast<uint8_t>(cmd::write), 
                static_cast<uint8_t>((address >> 16) & 0xff), 
                static_cast<uint8_t>((address >> 8) & 0xff), 
                static_cast<uint8_t>(address & 0xff)
            };

            // write the command and address
            Bus::write(command);

            // write the data
            Bus::write({data, size});

            // clear the chip select
            Cs::template set<true>();
        }

        /**
         * @brief Erase using the mode
         * 
         * @param mode 
         * @param address 
         */
        static void erase(const erase_mode mode, const uint32_t address) {
            // enable the write bit
            write_enable();

            // select the device
            Cs::template set<false>();

            // create a array for the command to erase
            const uint8_t command[] = {
                static_cast<uint8_t>(mode), 
                static_cast<uint8_t>((address >> 16) & 0xff), 
                static_cast<uint8_t>((address >> 8) & 0xff), 
                static_cast<uint8_t>(address & 0xff)
            };

            // write the command
            Bus::write(command);

            // clear the chip select
            Cs::template set<true>();
        }

        /**
         * @brief Erase the whole chip
         * 
         */
        static void chip_erase() {
            // enable the write bit
            write_enable();

            // select the device
            Cs::template set<false>();

            // create a array for the command to erase 
            // the whole chip
            const uint8_t command[] = {
                static_cast<uint8_t>(cmd::chip_erase)
            };

            // write the command
            Bus::write(command);

            // clear the chip select
            Cs::template set<true>();
        }
    };
}

#endif