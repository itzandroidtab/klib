#ifndef KLIB_SPI_HPP
#define KLIB_SPI_HPP

namespace klib::io::spi {
    /**
     * @brief Available spi modes
     * 
     */
    enum class mode {
        mode0 = 0,
        mode1 = 1,
        mode2 = 2,
        mode3 = 3
    };

    /**
     * @brief Amount of bits used in a spi transaction
     * 
     */
    enum class bits {
        bit_1 = 1,
        bit_2 = 2,
        bit_3 = 3,
        bit_4 = 4,
        bit_5 = 5,
        bit_6 = 6,
        bit_7 = 7,
        bit_8 = 8,
        bit_9 = 9,
        bit_10 = 10,
        bit_11 = 11,
        bit_12 = 12,
        bit_13 = 13,
        bit_14 = 14,
        bit_15 = 15,
        bit_16 = 16
    };

    /**
     * @brief Get the value of CPHA based on the input mode
     * 
     * @tparam Mode 
     * @return bool
     */
    template <mode Mode>
    constexpr bool get_cpha() {
        return Mode == mode::mode0 || Mode == mode::mode2;
    }

    /**
     * @brief Get the value of CPOL based on the input mode
     * 
     * @tparam Mode 
     * @return bool
     */
    template <mode Mode>
    constexpr bool get_cpol() {
        return Mode == mode::mode0 || Mode == mode::mode1;
    }
}

#endif