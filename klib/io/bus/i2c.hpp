#ifndef KLIB_I2C_HPP
#define KLIB_I2C_HPP

#include <cstdint>

namespace klib::io::i2c {
    /**
     * @brief Official i2c speed modes.
     *
     * @details
     * standard = 100kbps
     * fast = 400kbps
     * fast plus = 1Mbps
     * high = 3.4Mbps
     */
    enum class speed {
        standard = 100'000,
        fast = 400'000,
        fast_plus = 1'000'000,
        high = 3'400'000,
    };
}

#endif