#ifndef KLIB_NXP_LPC178X_EMAC_HPP
#define KLIB_NXP_LPC178X_EMAC_HPP

#include <klib/delay.hpp>

#include <klib/multispan.hpp>
#include <klib/io/core_clock.hpp>
#include <klib/io/bus/ethernet.hpp>

#include <io/port.hpp>
#include <io/clocks.hpp>
#include <io/power.hpp>

namespace klib::core::lpc178x::io {
    template <typename Emac, klib::io::ethernet::mii Mii, uint32_t MaxFrameLength = 1536>
    class emac {
    protected:
        // make sure we have a valid media independent interface we support
        static_assert(
            (Mii == klib::io::ethernet::mii::mii) || (Mii == klib::io::ethernet::mii::rmii), 
            "Invalid media independent interface requested"
        );

        static_assert(
            MaxFrameLength <= 0xffff, "Invalid max frame length"
        );

        constexpr static uint32_t get_clk_divider(uint32_t clock) {
            // all the available dividers and frequencies
            constexpr std::array<std::array<uint8_t, 2>, 15> dividers = {{
                {160, 0b1111},
                {150, 0b1110},
                {140, 0b1101},
                {130, 0b1100},
                {120, 0b1011},
                {100, 0b1010},
                {90, 0b1001},
                {80, 0b1000},
                {70, 0b0111},
                {50, 0b0110},
                {35, 0b0101},
                {25, 0b0100},
                {20, 0b0011},
                {15, 0b0010},
                {10, 0b0000}
            }};

            // get the frequency in mhz
            const uint32_t mhz = clock / 1'000'000;

            // search if the frequency in the table is higher than the current frequency
            for (const auto& pair : dividers) {
                if (mhz <= pair[0]) {
                    return pair[1];
                }
            }

            // return the highest divider if we cannot find it
            return dividers[0][1];
        }

        /**
         * @brief Wait until the phy is done with our request or until
         * the timeout has passed
         * 
         * @param timeout 
         * @return true 
         * @return false 
         */
        static bool phy_wait_not_busy(const klib::time::ms timeout) {
            const auto time = klib::io::systick<>::get_runtime();
            
            // wait until the read is done or a timeout
            while ((klib::io::systick<>::get_runtime() - time) < timeout) {
                // check if the read is done
                if (!(Emac::port->MIND & 0x1)) {
                    return false;
                }
            }

            return true;
        }

    public:
        static void init(const klib::io::ethernet::mac_address& mac) {
            // enable power to the peripheral
            target::io::power_control::enable<Emac>();

            // setup all the io pins based on the media independent interface
            if constexpr (Mii == klib::io::ethernet::mii::mii) {
                io::detail::pins::set_peripheral<typename Emac::mii::txd0::pin, typename Emac::mii::txd0::periph>();
                io::detail::pins::set_peripheral<typename Emac::mii::txd1::pin, typename Emac::mii::txd1::periph>();
                io::detail::pins::set_peripheral<typename Emac::mii::txd2::pin, typename Emac::mii::txd2::periph>();
                io::detail::pins::set_peripheral<typename Emac::mii::txd3::pin, typename Emac::mii::txd3::periph>();
                io::detail::pins::set_peripheral<typename Emac::mii::tx_en::pin, typename Emac::mii::tx_en::periph>();
                io::detail::pins::set_peripheral<typename Emac::mii::tx_er::pin, typename Emac::mii::tx_er::periph>();
                io::detail::pins::set_peripheral<typename Emac::mii::tx_clk::pin, typename Emac::mii::tx_clk::periph>();
                io::detail::pins::set_peripheral<typename Emac::mii::col::pin, typename Emac::mii::col::periph>();
                io::detail::pins::set_peripheral<typename Emac::mii::crs::pin, typename Emac::mii::crs::periph>();
                io::detail::pins::set_peripheral<typename Emac::mii::rxd0::pin, typename Emac::mii::rxd0::periph>();
                io::detail::pins::set_peripheral<typename Emac::mii::rxd1::pin, typename Emac::mii::rxd1::periph>();
                io::detail::pins::set_peripheral<typename Emac::mii::rxd2::pin, typename Emac::mii::rxd2::periph>();
                io::detail::pins::set_peripheral<typename Emac::mii::rxd3::pin, typename Emac::mii::rxd3::periph>();
                io::detail::pins::set_peripheral<typename Emac::mii::rx_dv::pin, typename Emac::mii::rx_dv::periph>();
                io::detail::pins::set_peripheral<typename Emac::mii::rx_er::pin, typename Emac::mii::rx_er::periph>();
                io::detail::pins::set_peripheral<typename Emac::mii::rx_clk::pin, typename Emac::mii::rx_clk::periph>();
            }
            else {
                io::detail::pins::set_peripheral<typename Emac::rmii::txd0::pin, typename Emac::rmii::txd0::periph>();
                io::detail::pins::set_peripheral<typename Emac::rmii::txd1::pin, typename Emac::rmii::txd1::periph>();
                io::detail::pins::set_peripheral<typename Emac::rmii::tx_en::pin, typename Emac::rmii::tx_en::periph>();
                io::detail::pins::set_peripheral<typename Emac::rmii::crs::pin, typename Emac::rmii::crs::periph>();
                io::detail::pins::set_peripheral<typename Emac::rmii::rxd0::pin, typename Emac::rmii::rxd0::periph>();
                io::detail::pins::set_peripheral<typename Emac::rmii::rxd1::pin, typename Emac::rmii::rxd1::periph>();
                io::detail::pins::set_peripheral<typename Emac::rmii::rx_er::pin, typename Emac::rmii::rx_er::periph>();
                io::detail::pins::set_peripheral<typename Emac::rmii::ref_clk::pin, typename Emac::rmii::ref_clk::periph>();
            }

            // setup the miim io
            io::detail::pins::set_peripheral<typename Emac::mdc::pin, typename Emac::mdc::periph>();
            io::detail::pins::set_peripheral<typename Emac::mdio::pin, typename Emac::mdio::periph>();

            // reset the rx and tx side in the mac register
            Emac::port->MAC1 = (
                (0x1 << 8) | (0x1 << 9) | (0x1 << 10) | 
                (0x1 << 11) | (0x1 << 14) | (0x1 << 15)
            );

            // reset the rx and tx datapath in the command register
            Emac::port->COMMAND |= (0b111 << 3) | (0x1 << 6);

            // wait a little bit
            klib::delay<klib::busy_wait>(klib::time::us(100));

            // pass all receive frames
            Emac::port->MAC1 = (0x1 << 1);
            
            // enable crc and pad out frames
            Emac::port->MAC2 = (0x1 << 4) | (0x1 << 5);

            // set the maximum frame length
            Emac::port->MAXF = MaxFrameLength;

            // set the default collision windows and retry register
            Emac::port->CLRT = 0x370f;

            // set the recommended back to back inter packet gap
            Emac::port->IPGR = 0x12;

            // configure the miim 
            // check what divider to use for the current frequency
            const uint32_t divider = get_clk_divider(klib::io::clock::get());

            // reset the miim interface
            Emac::port->MCFG = (0x1 << 15) | (divider << 2);

            // give the miim some time to reset
            klib::delay<klib::busy_wait>(klib::time::us(100));

            // enable the miim interface
            Emac::port->MCFG = (divider << 2);

            // configure the RMII or MII interface
            if constexpr (Mii == klib::io::ethernet::mii::mii) {
                // TODO: add support for MII
            }
            else {
                // set the mac command register to enable RMII and prevent 
                // runt frames being filtered out
                Emac::port->COMMAND = (0x1 << 9) | (0x1 << 6) | (0x1 << 7);   
            }

            // configure the mac address
            Emac::port->SA0 = mac[0] << 8 | mac[1];
            Emac::port->SA1 = mac[2] << 8 | mac[3];
            Emac::port->SA2 = mac[4] << 8 | mac[5];

            // TODO: add descriptor initalisation
        }

        /**
         * @brief Configure the emac after the link status has changed
         * 
         * @param config 
         */
        static void configure(const klib::io::ethernet::link_config config) {
            // configure the modes we received from the phy
            if (config.full_duplex) {
                // enable full duplex on the MAC
                Emac::port->MAC2 |= 0x1;

                // enable full duplex in the command register
                Emac::port->COMMAND |= (0x1 << 10);

                // change the packet to packet gap register
                Emac::port->IPGT = 0x15;
            }
            else {
                // enable full duplex on the MAC
                Emac::port->MAC2 &= (~0x1);

                // enable full duplex in the command register
                Emac::port->COMMAND &= (~(0x1 << 10));

                // change the packet to packet gap register
                Emac::port->IPGT = 0x12;
            }
            
            // configure the speed
            Emac::port->SUPP = (
                (config.link_speed == klib::io::ethernet::speed::mbps_10) ?
                0x0 : (0x1 << 8)
            );
        }

    public:
        // miim interface to write to the phy
        class miim {
        public:
            /**
             * @brief Read a uint16_t from the phy using the phy address and the register address
             * 
             * @param phy 
             * @param reg 
             * @param timeout 
             * @return uint16_t 
             */
            static uint16_t read(const uint8_t phy, const uint16_t reg, const klib::time::ms timeout = {50}) {
                // write the register we want to access on the phy
                Emac::port->MADR = ((phy & 0x1f) << 8) | (reg & 0x1f);

                // trigger a read on the phy
                Emac::port->MCMD = 0x1;

                // wait until the read is done or a 50 ms timeout
                phy_wait_not_busy(timeout);

                // stop any read that is still active
                Emac::port->MCMD = 0x0;

                // return the result
                return Emac::port->MRDD;
            }

            /**
             * @brief Write a uint16_t to the phy using the phy address and the register address
             * 
             * @param phy 
             * @param reg 
             * @param value 
             * @param timeout 
             */
            static void write(const uint8_t phy, const uint16_t reg, const uint16_t value, const klib::time::ms timeout = {50}) {
                // write the register we want to access on the phy
                Emac::port->MADR = ((phy & 0x1f) << 8) | (reg & 0x1f);

                // write the value to the phy
                Emac::port->MWTD = value;
                
                // wait until the read is done or a 50 ms timeout
                phy_wait_not_busy(timeout);
            }
        };
    };
}

#endif