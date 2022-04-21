#ifndef KLIB_MAX32660_GPIO_HPP
#define KLIB_MAX32660_GPIO_HPP

#include "pio.hpp"
#include "pins.hpp"

// peripheral pin mapping to alternate functions.
namespace klib::max32660::io::wlp {
    struct swd0_0 {
        // peripheral id (e.g swd0, swd1)
        constexpr static uint32_t id = 0;

        // configuration of the pins
        using dio = detail::swd::swd<pins::pc1, detail::swd::mode::dio, io::detail::periph_func_2>;
        using clk = detail::swd::swd<pins::pc2, detail::swd::mode::clk, io::detail::periph_func_2>;
    };

    struct swd0_1 {
        // peripheral id (e.g swd0, swd1)
        constexpr static uint32_t id = 0;

        // configuration of the pins
        using dio = detail::swd::swd<pins::pb4, detail::swd::mode::dio, io::detail::periph_func_2>;
        using clk = detail::swd::swd<pins::pb3, detail::swd::mode::clk, io::detail::periph_func_2>;
    };

    struct uart0 {
        // peripheral id (e.g uart0, uart1)
        constexpr static uint32_t id = 0;

        // configuration of the pins
        using txd = detail::uart::uart<pins::pd3, detail::uart::mode::txd, io::detail::periph_func_2>;
        using rxd = detail::uart::uart<pins::pd4, detail::uart::mode::rxd, io::detail::periph_func_2>;
        using rts = detail::uart::uart<pins::pc4, detail::uart::mode::rts, io::detail::periph_func_2>;
        using cts = detail::uart::uart<pins::pc3, detail::uart::mode::cts, io::detail::periph_func_2>;
    };

    struct uart1_0 {
        // peripheral id (e.g uart0, uart1)
        constexpr static uint32_t id = 1;

        // configuration of the pins
        using txd = detail::uart::uart<pins::pc1, detail::uart::mode::txd, io::detail::periph_func_3>;
        using rxd = detail::uart::uart<pins::pc2, detail::uart::mode::rxd, io::detail::periph_func_3>;
    };

    struct uart1_1 {
        // peripheral id (e.g uart0, uart1)
        constexpr static uint32_t id = 1;

        // configuration of the pins
        using txd = detail::uart::uart<pins::pc3, detail::uart::mode::txd, io::detail::periph_func_3>;
        using rxd = detail::uart::uart<pins::pc4, detail::uart::mode::rxd, io::detail::periph_func_3>;
    };

    struct spi0 {
        // peripheral id (e.g spi0, spi1)
        constexpr static uint32_t id = 0;

        // configuration of the pins 
        using mosi = detail::spi::spi<pins::pd4, detail::spi::mode::mosi, io::detail::periph_func_1>;
        using miso = detail::spi::spi<pins::pd3, detail::spi::mode::miso, io::detail::periph_func_1>;
        using sck = detail::spi::spi<pins::pc3, detail::spi::mode::sck, io::detail::periph_func_1>;
        using cs0 = detail::spi::spi<pins::pc4, detail::spi::mode::cs0, io::detail::periph_func_1>;
    };

    struct spi1 {
        // peripheral id (e.g spi0, spi1)
        constexpr static uint32_t id = 1;

        // configuration of the pins 
        using mosi = detail::spi::spi<pins::pc2, detail::spi::mode::mosi, io::detail::periph_func_2>;
        using miso = detail::spi::spi<pins::pc1, detail::spi::mode::miso, io::detail::periph_func_2>;
        using sck = detail::spi::spi<pins::pd1, detail::spi::mode::sck, io::detail::periph_func_2>;
        using cs0 = detail::spi::spi<pins::pd2, detail::spi::mode::cs0, io::detail::periph_func_2>;
    };

    struct i2c0 {
        // peripheral id (e.g i2c0, i2c1)
        constexpr static uint32_t id = 0;

        using sda = detail::i2c::i2c<pins::pb3, detail::i2c::mode::sda, io::detail::periph_func_1>;
        using scl = detail::i2c::i2c<pins::pb4, detail::i2c::mode::scl, io::detail::periph_func_1>;
    };

    struct i2c1 {
        // peripheral id (e.g i2c0, i2c1)
        constexpr static uint32_t id = 1;

        using sda = detail::i2c::i2c<pins::pd2, detail::i2c::mode::sda, io::detail::periph_func_1>;
        using scl = detail::i2c::i2c<pins::pd1, detail::i2c::mode::scl, io::detail::periph_func_1>;
    };

    struct tc0 {
        // peripheral id (e.g tc0, tc1)
        constexpr static uint32_t id = 0;

        using tmr = detail::tc::tc<pins::pd2, detail::tc::mode::tmr, io::detail::periph_func_3>;
    };
}

#endif