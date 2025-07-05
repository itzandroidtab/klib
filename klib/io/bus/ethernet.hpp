#ifndef KLIB_ETHERNET_HPP
#define KLIB_ETHERNET_HPP

#include <cstdint>
#include <array>

namespace klib::io::ethernet {
    /**
     * @brief Media independent interfaces
     * 
     */
    enum class mii {
        mii,
        rmii,
        rgmii,
    };

    /**
     * @brief Ethernet speeds
     * 
     */
    enum class speed {
        mbps_10,
        mbps_100,
        mbps_1000,
    };

    /**
     * @brief Link configuration
     * 
     */
    struct link_config {
        // the link speed
        speed link_speed;

        // flag if the link is full duplex
        bool full_duplex = true;
    };

    /**
     * @brief Using for a mac address
     * 
     */
    using mac_address = std::array<uint8_t, 6>;
}

#endif
