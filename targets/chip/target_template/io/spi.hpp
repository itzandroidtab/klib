#ifndef KLIB_TARGET_TEMPLATE_SPI_HPP
#define KLIB_TARGET_TEMPLATE_SPI_HPP

#include "pins.hpp"

/**
 * @brief The peripheral namespaces are as follows:
 * 
 * // helper namespace where the details about the hardware can be stored
 * namespace klib::target_template::io::periph::detail::spi {
 * }
 * 
 * // global peripherals, not affected by chip package
 * namespace klib::target_template::io::periph {
 * }
 * 
 * // peripheral mapping affected by chip package
 * namespace klib::target_template::io::periph::lqfp_100 {
 * }
 */

namespace klib::target_template::io::periph::detail::spi {
    enum class mode {
        miso,
        mosi,
        sck,
        cs0
    };

    template <typename Pin, mode Type, typename Periph>
    struct spi {
        // pin of the peripheral
        using pin = Pin;

        // type of the pin
        constexpr static mode type = Type;

        // alternate function
        using periph = Periph;
    };
}

namespace klib::target_template::io::periph::tqfn_100 {
    /**
     * @brief Struct stores information about the spi0 
     * hardware.
     * 
     */
    struct spi0 {
        // peripheral id (e.g spi0, spi1)
        constexpr static uint32_t id = 0;

        // peripheral clock bit position
        constexpr static uint32_t clock_id = 6;

        // port to the spi peripheral. On hardware 
        // this should have the port to write/read 
        // the hardware with
        static inline void *const port = (nullptr);

        // configuration of the pins for the hardware
        using mosi = detail::spi::spi<pins::package::tqfn_100::p0, detail::spi::mode::mosi, io::detail::alternate::func_1>;
        using miso = detail::spi::spi<pins::package::tqfn_100::p0, detail::spi::mode::miso, io::detail::alternate::func_1>;
        using sck = detail::spi::spi<pins::package::tqfn_100::p0, detail::spi::mode::sck, io::detail::alternate::func_1>;
    };
}

namespace klib::target_template::io::spi {
    /**
     * @brief Class for the spi hardware
     * 
     * @tparam Spi 
     */
    template <typename Spi>
    class spi {
    public:
        /**
         * @brief Init the spi hardware including the gpio
         * 
         */
        constexpr static void init() {}
    }
}

#endif