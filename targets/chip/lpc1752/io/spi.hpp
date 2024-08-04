#ifndef KLIB_LPC1752_SPI_HPP
#define KLIB_LPC1752_SPI_HPP

#include <cstdint>

#include <targets/core/nxp/lpc175x/spi.hpp>

#include "pins.hpp"

namespace klib::lpc1752::io::periph::detail::spi {
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

namespace klib::lpc1752::io::periph::lqfp_80 {
    struct spi0 {
        // peripheral id (e.g spi0, spi1)
        constexpr static uint32_t id = 0;

        // interrupt id (including the arm vector table)
        constexpr static uint32_t interrupt_id = 29;

        // peripheral clock bit position
        constexpr static uint32_t clock_id = 8;

        // port to the SPI hardware
        static inline SPI_Type *const port = SPI;

        // configuration of the pins
        using mosi = detail::spi::spi<pins::package::lqfp_80::p45, detail::spi::mode::mosi, core::lpc175x::io::detail::alternate::func_3>;
        using miso = detail::spi::spi<pins::package::lqfp_80::p46, detail::spi::mode::miso, core::lpc175x::io::detail::alternate::func_3>;
        using sck = detail::spi::spi<pins::package::lqfp_80::p47, detail::spi::mode::sck, core::lpc175x::io::detail::alternate::func_3>;
        using cs0 = detail::spi::spi<pins::package::lqfp_80::p48, detail::spi::mode::cs0, core::lpc175x::io::detail::alternate::func_3>;
    };
}

namespace klib::lpc1752::io {
    template <typename Spi>
    using spi = core::lpc175x::io::spi<Spi>;
}

#endif