#ifndef KLIB_LAN8720_HPP
#define KLIB_LAN8720_HPP

#include <cstdint>

#include <klib/io/systick.hpp>
#include <klib/io/bus/ethernet.hpp>

namespace klib::hardware::ethernet::phy {
    template <typename Miim, uint8_t PhyAddress = 0x0>
    class lan8720a {
    public:
        // the phy address. Note the last 4 bits are the revision
        // number and might differ based on the hardware
        constexpr static uint32_t phy_id = 0x7c0f0;

        /**
         * @brief Available speeds on the lan8720a when configuring
         * 
         */
        enum class speed {
            automatic,
            mbps_10,
            mbps_100,
        };

    protected:
        /**
         * @brief Available registers on the lan8720a
         * 
         */
        enum class cmd {
            // basic registers
            basic_control = 0x00,
            basic_status = 0x01,

            // extended registers
            phy_id_0 = 0x02,
            phy_id_1 = 0x03,
            auto_advertisement = 0x04,
            auto_link = 0x05,
            auto_expansion = 0x06,

            // vendor specific registers
            mode_control_status = 0x11,
            special_modes = 0x12,
            symbol_error_counter = 0x1a,
            control_status_indication = 0x1b,
            interrupt_source = 0x1d,
            interrupt_mask = 0x1e,
            special_control_status = 0x1f
        };

        /**
         * @brief Helper function to write to a register on the phy
         * 
         * @param reg 
         * @param value 
         */
        static void write(cmd reg, const uint16_t value) {
            Miim::write(PhyAddress, static_cast<uint8_t>(reg), value);
        }

        /**
         * @brief Helper function to read from the phy
         * 
         * @param reg 
         * @return uint16_t 
         */
        static uint16_t read(cmd reg) {
            return Miim::read(PhyAddress, static_cast<uint8_t>(reg));
        }

    public:
        /**
         * @brief Read the phy id from the Miim interface
         * 
         * @return uint32_t 
         */
        static uint32_t id() {
            // return the combined top and bottom id register
            return (read(cmd::phy_id_0) << 16) | read(cmd::phy_id_1);
        }

        /**
         * @brief Init the LAN8720A
         * 
         * @tparam Speed 
         * @tparam FullDuplex 
         * @tparam Loopback 
         * @tparam Isolate 
         * @return status
         */
        template <speed Speed = speed::automatic, bool FullDuplex = true, bool Loopback = false, bool Isolate = false>
        static bool init() {
            // do a software reset
            write(cmd::basic_control, (0x1 << 15));

            // wait until the reset is done or a 250ms timeout
            const auto time = klib::io::systick<>::get_runtime();
            
            // wait until the software reset is done or a timeout
            while (true) {
                // check if the software reset is done
                if (!(read(cmd::basic_control) & (0x1 << 15))) {
                    break;
                }

                // check for a timeout
                if ((klib::io::systick<>::get_runtime() - time) >= klib::time::ms(500)) {
                    // lan8720 did not reset within our timeout time
                    return false;
                }
            }

            // make sure we have the correct phy (we ignore the last 
            // 4 bits, the revision bits)
            if ((id() & (~0xf)) != phy_id) {
                // not a lan8720a phy
                return false;
            }

            // write the configuration to the phy
            if constexpr (Speed == speed::automatic) {
                // in automatic mode we ignore the full duplex mode 
                // (also overrides the speed)
                write(cmd::basic_control, (
                    (0x1 << 12) | (Loopback << 14) | (Isolate << 10)
                ));
            }
            else {
                write(cmd::basic_control, (
                    ((Speed == speed::mbps_100) << 13) | (Loopback << 14) | 
                    (Isolate << 10) | (FullDuplex << 8)
                ));
            }

            // return we successfully configured the lan8720a
            return true;
        }

        /**
         * @brief Returns if the link is up
         * 
         * @return true 
         * @return false 
         */
        static bool has_link() {
            return read(cmd::basic_status) & (0x1 << 2);
        }

        /**
         * @brief Returns the link configuration
         * 
         * @note Only valid if a linked
         * 
         * @return link_config 
         */
        static klib::io::ethernet::link_config get_link_config() {
            // read the special control status
            const uint16_t val = read(cmd::special_control_status);

            // get the link speed from the 
            const klib::io::ethernet::link_config ret = {
                .link_speed = (
                    (val & (0x1 << 2)) ?
                    klib::io::ethernet::speed::mbps_10 :
                    klib::io::ethernet::speed::mbps_100
                ),
                .full_duplex = static_cast<bool>(val & (0x1 << 4))
            };

            return ret;
        }
    };
}

#endif