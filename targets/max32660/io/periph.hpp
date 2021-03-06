#ifndef KLIB_MAX32660_PERIPH_HPP
#define KLIB_MAX32660_PERIPH_HPP

#include "pio.hpp"
#include "pins.hpp"

// peripheral pin mapping to alternate functions.
namespace klib::max32660::io::periph::wlp {
    struct swd0_0 {
        // peripheral id (e.g swd0, swd1)
        constexpr static uint32_t id = 0;

        // configuration of the pins
        using dio = detail::swd::swd<io::pins::package::wlp::pc1, detail::swd::mode::dio, io::detail::periph_func_1>;
        using clk = detail::swd::swd<io::pins::package::wlp::pc2, detail::swd::mode::clk, io::detail::periph_func_1>;
    };

    struct swd0_1 {
        // peripheral id (e.g swd0, swd1)
        constexpr static uint32_t id = 0;

        // configuration of the pins
        using dio = detail::swd::swd<io::pins::package::wlp::pb4, detail::swd::mode::dio, io::detail::periph_func_2>;
        using clk = detail::swd::swd<io::pins::package::wlp::pb3, detail::swd::mode::clk, io::detail::periph_func_2>;
    };

    struct uart0 {
        // peripheral id (e.g uart0, uart1)
        constexpr static uint32_t id = 0;

        // peripheral clock bit position
        constexpr static uint32_t clock_id = 9;

        // configuration of the pins
        using txd = detail::uart::uart<io::pins::package::wlp::pd3, detail::uart::mode::txd, io::detail::periph_func_2>;
        using rxd = detail::uart::uart<io::pins::package::wlp::pd4, detail::uart::mode::rxd, io::detail::periph_func_2>;
        using rts = detail::uart::uart<io::pins::package::wlp::pc4, detail::uart::mode::rts, io::detail::periph_func_2>;
        using cts = detail::uart::uart<io::pins::package::wlp::pc3, detail::uart::mode::cts, io::detail::periph_func_2>;
    };

    struct uart1_0 {
        // peripheral id (e.g uart0, uart1)
        constexpr static uint32_t id = 1;

        // peripheral clock bit position
        constexpr static uint32_t clock_id = 10;

        // configuration of the pins
        using txd = detail::uart::uart<io::pins::package::wlp::pc1, detail::uart::mode::txd, io::detail::periph_func_3>;
        using rxd = detail::uart::uart<io::pins::package::wlp::pc2, detail::uart::mode::rxd, io::detail::periph_func_3>;
    };

    struct uart1_1 {
        // peripheral id (e.g uart0, uart1)
        constexpr static uint32_t id = 1;

        // peripheral clock bit position
        constexpr static uint32_t clock_id = 10;

        // configuration of the pins
        using txd = detail::uart::uart<io::pins::package::wlp::pc3, detail::uart::mode::txd, io::detail::periph_func_3>;
        using rxd = detail::uart::uart<io::pins::package::wlp::pc4, detail::uart::mode::rxd, io::detail::periph_func_3>;
    };

    struct spi0 {
        // peripheral id (e.g spi0, spi1)
        constexpr static uint32_t id = 0;

        // peripheral clock bit position
        constexpr static uint32_t clock_id = 6;

        // configuration of the pins 
        using mosi = detail::spi::spi<io::pins::package::wlp::pd4, detail::spi::mode::mosi, io::detail::periph_func_1>;
        using miso = detail::spi::spi<io::pins::package::wlp::pd3, detail::spi::mode::miso, io::detail::periph_func_1>;
        using sck = detail::spi::spi<io::pins::package::wlp::pc3, detail::spi::mode::sck, io::detail::periph_func_1>;
        using cs0 = detail::spi::spi<io::pins::package::wlp::pc4, detail::spi::mode::cs0, io::detail::periph_func_1>;
    };

    struct spi1 {
        // peripheral id (e.g spi0, spi1)
        constexpr static uint32_t id = 1;

        // peripheral clock bit position
        constexpr static uint32_t clock_id = 7;

        // configuration of the pins 
        using mosi = detail::spi::spi<io::pins::package::wlp::pc2, detail::spi::mode::mosi, io::detail::periph_func_2>;
        using miso = detail::spi::spi<io::pins::package::wlp::pc1, detail::spi::mode::miso, io::detail::periph_func_2>;
        using sck = detail::spi::spi<io::pins::package::wlp::pd1, detail::spi::mode::sck, io::detail::periph_func_2>;
        using cs0 = detail::spi::spi<io::pins::package::wlp::pd2, detail::spi::mode::cs0, io::detail::periph_func_2>;
    };

    struct i2c0 {
        // peripheral id (e.g i2c0, i2c1)
        constexpr static uint32_t id = 0;

        // peripheral clock bit position
        constexpr static uint32_t clock_id = 13;

        using sda = detail::i2c::i2c<io::pins::package::wlp::pb3, detail::i2c::mode::sda, io::detail::periph_func_1>;
        using scl = detail::i2c::i2c<io::pins::package::wlp::pb4, detail::i2c::mode::scl, io::detail::periph_func_1>;
    };

    struct i2c1 {
        // peripheral id (e.g i2c0, i2c1)
        constexpr static uint32_t id = 1;

        // peripheral clock bit position
        constexpr static uint32_t clock_id = 28;

        using sda = detail::i2c::i2c<io::pins::package::wlp::pd2, detail::i2c::mode::sda, io::detail::periph_func_1>;
        using scl = detail::i2c::i2c<io::pins::package::wlp::pd1, detail::i2c::mode::scl, io::detail::periph_func_1>;
    };

    struct tc0 {
        // peripheral id (e.g tc0, tc1)
        constexpr static uint32_t id = 0;

        // peripheral clock bit position
        constexpr static uint32_t clock_id = 15;

        using tmr = detail::tc::tc<io::pins::package::wlp::pd2, detail::tc::mode::tmr, io::detail::periph_func_3>;
    };

    struct tc1 {
        // peripheral id (e.g tc0, tc1)
        constexpr static uint32_t id = 1;

        // peripheral clock bit position
        constexpr static uint32_t clock_id = 16;
    };

    struct tc2 {
        // peripheral id (e.g tc0, tc1)
        constexpr static uint32_t id = 2;

        // peripheral clock bit position
        constexpr static uint32_t clock_id = 17;
    };

    struct dma0 {
        // peripheral id (e.g dma0, dma1)
        constexpr static uint32_t id = 0;

        // peripheral clock bit position
        constexpr static uint32_t clock_id = 5;
    };
}

namespace klib::max32660::io::periph::tqfn_24 {
    struct swd0_0 {
        // peripheral id (e.g swd0, swd1)
        constexpr static uint32_t id = 0;

        // configuration of the pins
        using dio = detail::swd::swd<io::pins::package::tqfn_24::p2, detail::swd::mode::dio, io::detail::periph_func_1>;
        using clk = detail::swd::swd<io::pins::package::tqfn_24::p1, detail::swd::mode::clk, io::detail::periph_func_1>;
    };

    struct swd0_1 {
        // peripheral id (e.g swd0, swd1)
        constexpr static uint32_t id = 0;

        // configuration of the pins
        using dio = detail::swd::swd<io::pins::package::tqfn_24::p14, detail::swd::mode::dio, io::detail::periph_func_2>;
        using clk = detail::swd::swd<io::pins::package::tqfn_24::p13, detail::swd::mode::clk, io::detail::periph_func_2>;
    };

    struct uart0 {
        // peripheral id (e.g uart0, uart1)
        constexpr static uint32_t id = 0;

        // peripheral clock bit position
        constexpr static uint32_t clock_id = 9;

        // configuration of the pins
        using txd = detail::uart::uart<io::pins::package::tqfn_24::p21, detail::uart::mode::txd, io::detail::periph_func_2>;
        using rxd = detail::uart::uart<io::pins::package::tqfn_24::p20, detail::uart::mode::rxd, io::detail::periph_func_2>;
        using rts = detail::uart::uart<io::pins::package::tqfn_24::p18, detail::uart::mode::rts, io::detail::periph_func_2>;
        using cts = detail::uart::uart<io::pins::package::tqfn_24::p19, detail::uart::mode::cts, io::detail::periph_func_2>;
    };

    struct uart1_0 {
        // peripheral id (e.g uart0, uart1)
        constexpr static uint32_t id = 1;

        // peripheral clock bit position
        constexpr static uint32_t clock_id = 10;

        // configuration of the pins
        using txd = detail::uart::uart<io::pins::package::tqfn_24::p14, detail::uart::mode::txd, io::detail::periph_func_2>;
        using rxd = detail::uart::uart<io::pins::package::tqfn_24::p13, detail::uart::mode::rxd, io::detail::periph_func_2>;
        using rts = detail::uart::uart<io::pins::package::tqfn_24::p3, detail::uart::mode::rts, io::detail::periph_func_2>;
        using cts = detail::uart::uart<io::pins::package::tqfn_24::p4, detail::uart::mode::cts, io::detail::periph_func_2>;
    };

    struct uart1_1 {
        // peripheral id (e.g uart0, uart1)
        constexpr static uint32_t id = 1;

        // peripheral clock bit position
        constexpr static uint32_t clock_id = 10;

        // configuration of the pins
        using txd = detail::uart::uart<io::pins::package::tqfn_24::p2, detail::uart::mode::txd, io::detail::periph_func_3>;
        using rxd = detail::uart::uart<io::pins::package::tqfn_24::p1, detail::uart::mode::rxd, io::detail::periph_func_3>;
        using rts = detail::uart::uart<io::pins::package::tqfn_24::p3, detail::uart::mode::rts, io::detail::periph_func_2>;
        using cts = detail::uart::uart<io::pins::package::tqfn_24::p4, detail::uart::mode::cts, io::detail::periph_func_2>;
    };

    struct uart1_2 {
        // peripheral id (e.g uart0, uart1)
        constexpr static uint32_t id = 1;

        // peripheral clock bit position
        constexpr static uint32_t clock_id = 10;

        // configuration of the pins
        using txd = detail::uart::uart<io::pins::package::tqfn_24::p19, detail::uart::mode::txd, io::detail::periph_func_3>;
        using rxd = detail::uart::uart<io::pins::package::tqfn_24::p18, detail::uart::mode::rxd, io::detail::periph_func_3>;
        using rts = detail::uart::uart<io::pins::package::tqfn_24::p3, detail::uart::mode::rts, io::detail::periph_func_2>;
        using cts = detail::uart::uart<io::pins::package::tqfn_24::p4, detail::uart::mode::cts, io::detail::periph_func_2>;
    };

    struct spi0 {
        // peripheral id (e.g spi0, spi1)
        constexpr static uint32_t id = 0;

        // peripheral clock bit position
        constexpr static uint32_t clock_id = 6;

        // configuration of the pins 
        using mosi = detail::spi::spi<io::pins::package::tqfn_24::p20, detail::spi::mode::mosi, io::detail::periph_func_1>;
        using miso = detail::spi::spi<io::pins::package::tqfn_24::p21, detail::spi::mode::miso, io::detail::periph_func_1>;
        using sck = detail::spi::spi<io::pins::package::tqfn_24::p19, detail::spi::mode::sck, io::detail::periph_func_1>;
        using cs0 = detail::spi::spi<io::pins::package::tqfn_24::p18, detail::spi::mode::cs0, io::detail::periph_func_1>;
    };

    struct spi1_0 {
        // peripheral id (e.g spi0, spi1)
        constexpr static uint32_t id = 1;

        // peripheral clock bit position
        constexpr static uint32_t clock_id = 7;

        // configuration of the pins 
        using mosi = detail::spi::spi<io::pins::package::tqfn_24::p16, detail::spi::mode::mosi, io::detail::periph_func_1>;
        using miso = detail::spi::spi<io::pins::package::tqfn_24::p17, detail::spi::mode::miso, io::detail::periph_func_1>;
        using sck = detail::spi::spi<io::pins::package::tqfn_24::p5, detail::spi::mode::sck, io::detail::periph_func_1>;
        using cs0 = detail::spi::spi<io::pins::package::tqfn_24::p4, detail::spi::mode::cs0, io::detail::periph_func_1>;
    };

    struct spi1_1 {
        // peripheral id (e.g spi0, spi1)
        constexpr static uint32_t id = 1;

        // peripheral clock bit position
        constexpr static uint32_t clock_id = 7;

        // configuration of the pins 
        using mosi = detail::spi::spi<io::pins::package::tqfn_24::p1, detail::spi::mode::mosi, io::detail::periph_func_2>;
        using miso = detail::spi::spi<io::pins::package::tqfn_24::p2, detail::spi::mode::miso, io::detail::periph_func_2>;
        using sck = detail::spi::spi<io::pins::package::tqfn_24::p24, detail::spi::mode::sck, io::detail::periph_func_2>;
        using cs0 = detail::spi::spi<io::pins::package::tqfn_24::p23, detail::spi::mode::cs0, io::detail::periph_func_2>;
    };

    struct i2c0 {
        // peripheral id (e.g i2c0, i2c1)
        constexpr static uint32_t id = 0;

        // peripheral clock bit position
        constexpr static uint32_t clock_id = 13;

        using sda = detail::i2c::i2c<io::pins::package::tqfn_24::p13, detail::i2c::mode::sda, io::detail::periph_func_1>;
        using scl = detail::i2c::i2c<io::pins::package::tqfn_24::p14, detail::i2c::mode::scl, io::detail::periph_func_1>;
    };

    struct i2c1 {
        // peripheral id (e.g i2c0, i2c1)
        constexpr static uint32_t id = 1;

        // peripheral clock bit position
        constexpr static uint32_t clock_id = 28;

        using sda = detail::i2c::i2c<io::pins::package::tqfn_24::p23, detail::i2c::mode::sda, io::detail::periph_func_1>;
        using scl = detail::i2c::i2c<io::pins::package::tqfn_24::p24, detail::i2c::mode::scl, io::detail::periph_func_1>;
    };

    struct tc0 {
        // peripheral id (e.g tc0, tc1)
        constexpr static uint32_t id = 0;

        // peripheral clock bit position
        constexpr static uint32_t clock_id = 15;

        using tmr = detail::tc::tc<io::pins::package::tqfn_24::p23, detail::tc::mode::tmr, io::detail::periph_func_3>;
    };

    struct tc1 {
        // peripheral id (e.g tc0, tc1)
        constexpr static uint32_t id = 1;

        // peripheral clock bit position
        constexpr static uint32_t clock_id = 16;
    };

    struct tc2 {
        // peripheral id (e.g tc0, tc1)
        constexpr static uint32_t id = 2;

        // peripheral clock bit position
        constexpr static uint32_t clock_id = 17;
    };

    struct dma0 {
        // peripheral id (e.g dma0, dma1)
        constexpr static uint32_t id = 0;

        // peripheral clock bit position
        constexpr static uint32_t clock_id = 5;
    }; 
}

#endif