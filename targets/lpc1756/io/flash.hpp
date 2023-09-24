#ifndef KLIB_LPC1756_FLASH_HPP
#define KLIB_LPC1756_FLASH_HPP

#include <cstdint>
#include <klib/io/core_clock.hpp>

#include <lpc1756.hpp>

namespace klib::lpc1756::io {
    /**
     * @brief Driver for the lpc17xx flash. Uses IAP calls to write flash. The 
     * IAP calls use a maximum of 128 bytes of user ram and the top 32 bytes of 
     * memory. The user should not use the top bytes of memory or they will be 
     * overwritten.
     * 
     * @details The IAP uses some undocumented registers to remap the flash to
     * something else. Reverse engineering this is a bit of a pain. Might look
     * at it later to see if this can be optimized. (This means if I figure out
     * how it works it would require ram as that is not changed. Not sure if
     * this is worth the tradeof)
     * 
     */
    class flash {
    protected:
        /**
         * @brief All IAP commands available in the lpc17xx bootloader
         * 
         */
        class iap_cmd {
        public:
            enum {
                prepare_sector_for_write = 50,
                copy_ram_to_flash = 51,
                erase_sector = 52,
                blank_check_sector = 53,
                read_part_id = 54,
                read_boot_version = 55,
                compare = 56,
                reinvoke_isp = 57,
            };
        };

        /**
         * @brief IAP result codes
         * 
         */
        enum class iap_result {
            success = 0,
            invalid_command = 1,
            source_address_error = 2,
            destination_address_error = 3,
            source_addres_not_mapped = 4,
            destination_addres_not_mapped = 5,
            count_error = 6,
            invalid_sector = 7,
            sector_not_blank = 8,
            sector_not_prepared = 9,
            compare_error = 10,
            busy = 11,
        };

        static uint32_t get_clock_khz() {
            // get the clock in khz
            return klib::io::clock::get() / 1'000;
        }

        /**
         * @brief do a IAP call
         * 
         * @param cmd 
         * @param result 
         */
        static void iap_call(const uint32_t* cmd, uint32_t* result) {
            // disable interrupts as the flash is remapped
            disable_irq();

            // call the iap
            (*reinterpret_cast<void(*)(const uint32_t*, uint32_t*)>(0x1fff1ff1))(cmd, result);

            // reenable the interrupts again
            enable_irq();
        }

        static iap_result prepare_for_write(const uint32_t start, const uint32_t end) {
            // create the command for the iap call
            const uint32_t buffer[3] = {
                iap_cmd::prepare_sector_for_write,
                start, end
            };

            // return status
            uint32_t status;

            iap_call(buffer, &status);

            // return the status
            return static_cast<iap_result>(status);
        }

    public:
        /**
         * @brief Available erase modes
         * 
         */
        enum class erase_mode {
            sector,
        };

        /**
         * @brief Get the sector the address is located in
         * 
         * 
         * @return uint32_t 
         */
        static uint32_t address_to_sector(const uint32_t address) {
            // the lpc17xx chips have 16x 4k sectors at the start. 
            // After the 4k sectors it has 32k sectors
            if (address < (16 * (4 * 1024))) {
                return address / (4 * 1024);
            }
            else {
                return 16 + ((address - (16 * (4 * 1024))) / (32 * 1024));
            }
        }

        /**
         * @brief Get the start address of a sector
         * 
         * @param address 
         * @return sector 
         */
        static uint32_t sector_to_address(const uint32_t sector) {
            if (sector < 16) {
                return sector * (4 * 1024);
            }
            else {
                return (16 * (4 * 1024)) + ((sector - 16) * (32 * 1024));
            }
        }

        /**
         * @brief Check if a sector is blank
         * 
         * @param address 
         * @return true 
         * @return false 
         */
        static bool is_blank(const uint32_t address) {
            // get the sector number from the address
            const uint32_t sector = address_to_sector(address);

            // create the command for the iap call
            const uint32_t buffer[4] = {
                iap_cmd::blank_check_sector,
                sector, sector
            };

            // return status
            uint32_t status[3];

            iap_call(buffer, status);

            // return if the iap call was successfull
            return static_cast<iap_result>(status[0]) == iap_result::success;
        }

        /**
         * @brief Erase using the mode
         * 
         * @param mode 
         * @param address 
         * @return true 
         * @return false 
         */
    	static bool erase(const erase_mode mode, const uint32_t address) {
            // get the sector number from the address
            const uint32_t sector = address_to_sector(address);

            // prepare the sector for a write
            if (prepare_for_write(sector, sector) != iap_result::success) {
                return false;
            }

            // create the command for the iap call
            const uint32_t buffer[4] = {
                iap_cmd::erase_sector,
                sector, sector,
                get_clock_khz()
            };

            // return status
            uint32_t status;

            iap_call(buffer, &status);

            // return if the iap call was successfull
            return static_cast<iap_result>(status) == iap_result::success;
        }

        /**
         * @brief Write to the address with data
         * 
         * @param address 
         * @param data 
         * @param size 
         */
        static bool write(const uint32_t address, const uint8_t *const data, const uint16_t size) {
            // get the sector number from the address
            const uint32_t start_sector = address_to_sector(address);
            const uint32_t end_sector = address_to_sector(address + size);

            // prepare the sector for a write
            if (prepare_for_write(start_sector, end_sector) != iap_result::success) {
                return false;
            }

            // create the command for the iap call
            const uint32_t buffer[5] = {
                iap_cmd::copy_ram_to_flash,
                address, reinterpret_cast<uint32_t>(data), 
                size, get_clock_khz()
            };

            // return status
            uint32_t status;

            iap_call(buffer, &status);

            // return if the iap call was successfull
            return static_cast<iap_result>(status) == iap_result::success;
        }

        /**
         * @brief Erase the whole chip
         * 
         */
        static void chip_erase() {
            // try to erase every sector
            for (uint32_t i = 0; i < 30; i++) {
                // get the start address of the sector
                const uint32_t address = sector_to_address(i);

                // erase the sector
                erase(erase_mode::sector, address);
            }
        }
    };
}

#endif
