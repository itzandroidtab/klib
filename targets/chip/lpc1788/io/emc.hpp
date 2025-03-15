#ifndef KLIB_LPC1788_EMC_HPP
#define KLIB_LPC1788_EMC_HPP

#include <tuple>
#include <array>

#include <klib/io/peripheral.hpp>
#include <targets/core/nxp/lpc178x/emc.hpp>

#include "pins.hpp"

namespace klib::lpc1788::io::periph::detail::emc {
    enum class mode {
        address_bus,
        data_bus,
        clk,
        cke,
        cas,
        ras,
        dqm,
        oe,
        we,
        bls,
        cs,
    };

    template <typename Pin, mode Type, typename Periph>
    struct emc {
        // pin of the peripheral
        using pin = Pin;

        // type of the pin
        constexpr static mode type = Type;

        // alternate function
        using periph = Periph;
    };
}

namespace klib::lpc1788::io::periph::lqfp_208 {
    struct emc0 {
        // peripheral id (e.g emc0, emc1)
        constexpr static uint32_t id = 0;

        // peripheral clock bit position
        constexpr static uint32_t clock_id = 11;

        // port to the EMC hardware
        static inline EMC_Type *const port = EMC;

        // pin configuration for the emc
        using cas = detail::emc::emc<pins::package::lqfp_208::p87, detail::emc::mode::cas, core::lpc178x::io::detail::alternate::func_1>;
        using ras = detail::emc::emc<pins::package::lqfp_208::p95, detail::emc::mode::ras, core::lpc178x::io::detail::alternate::func_1>;
        using oe = detail::emc::emc<pins::package::lqfp_208::p183, detail::emc::mode::oe, core::lpc178x::io::detail::alternate::func_1>;
        using we = detail::emc::emc<pins::package::lqfp_208::p179, detail::emc::mode::we, core::lpc178x::io::detail::alternate::func_1>;

        // all the chip selects for non dynamic memory (sram, parallel flash, etc)
        using cs_pins = std::tuple<
            detail::emc::emc<pins::package::lqfp_208::p187, detail::emc::mode::cs, core::lpc178x::io::detail::alternate::func_1>, // cs0
            detail::emc::emc<pins::package::lqfp_208::p193, detail::emc::mode::cs, core::lpc178x::io::detail::alternate::func_1>, // cs1
            detail::emc::emc<pins::package::lqfp_208::p91, detail::emc::mode::cs, core::lpc178x::io::detail::alternate::func_1>, // cs2
            detail::emc::emc<pins::package::lqfp_208::p99, detail::emc::mode::cs, core::lpc178x::io::detail::alternate::func_1> // cs3
        >;
        
        // all the clock pins
        using clk_pins = std::tuple<
            detail::emc::emc<pins::package::lqfp_208::p59, detail::emc::mode::clk, core::lpc178x::io::detail::alternate::func_1>, // clk0
            detail::emc::emc<pins::package::lqfp_208::p67, detail::emc::mode::clk, core::lpc178x::io::detail::alternate::func_1> // clk1
        >;

        // all the sdram chip selects
        using dynamic_cs_pins = std::tuple<
            detail::emc::emc<pins::package::lqfp_208::p73, detail::emc::mode::cs, core::lpc178x::io::detail::alternate::func_1>, // cynamic_cs0
            detail::emc::emc<pins::package::lqfp_208::p81, detail::emc::mode::cs, core::lpc178x::io::detail::alternate::func_1>, // cynamic_cs1
            detail::emc::emc<pins::package::lqfp_208::p85, detail::emc::mode::cs, core::lpc178x::io::detail::alternate::func_1>, // cynamic_cs2
            detail::emc::emc<pins::package::lqfp_208::p64, detail::emc::mode::cs, core::lpc178x::io::detail::alternate::func_1> // cynamic_cs3
        >;

        // all the clock enable pins
        using cke_pins = std::tuple<
            detail::emc::emc<pins::package::lqfp_208::p53, detail::emc::mode::cke, core::lpc178x::io::detail::alternate::func_1>, // cke0
            detail::emc::emc<pins::package::lqfp_208::p54, detail::emc::mode::cke, core::lpc178x::io::detail::alternate::func_1>, // cke1
            detail::emc::emc<pins::package::lqfp_208::p57, detail::emc::mode::cke, core::lpc178x::io::detail::alternate::func_1>, // cke2
            detail::emc::emc<pins::package::lqfp_208::p47, detail::emc::mode::cke, core::lpc178x::io::detail::alternate::func_1> // cke3
        >;

        // all the data mask pins
        using dqm_pins = std::tuple<
            detail::emc::emc<pins::package::lqfp_208::p49, detail::emc::mode::dqm, core::lpc178x::io::detail::alternate::func_1>, // dqm0
            detail::emc::emc<pins::package::lqfp_208::p43, detail::emc::mode::dqm, core::lpc178x::io::detail::alternate::func_1>, // dqm1
            detail::emc::emc<pins::package::lqfp_208::p31, detail::emc::mode::dqm, core::lpc178x::io::detail::alternate::func_1>, // dqm2
            detail::emc::emc<pins::package::lqfp_208::p39, detail::emc::mode::dqm, core::lpc178x::io::detail::alternate::func_1> // dqm3
        >;

        // all the byte lane select pins
        using bls_pins = std::tuple<
            detail::emc::emc<pins::package::lqfp_208::p119, detail::emc::mode::bls, core::lpc178x::io::detail::alternate::func_1>, // bls0
            detail::emc::emc<pins::package::lqfp_208::p139, detail::emc::mode::bls, core::lpc178x::io::detail::alternate::func_1>, // bls1
            detail::emc::emc<pins::package::lqfp_208::p170, detail::emc::mode::bls, core::lpc178x::io::detail::alternate::func_1>, // bls2
            detail::emc::emc<pins::package::lqfp_208::p176, detail::emc::mode::bls, core::lpc178x::io::detail::alternate::func_1> // bls3
        >;

        // all the data pins
        using data_pins = std::tuple<
            detail::emc::emc<pins::package::lqfp_208::p197, detail::emc::mode::data_bus, core::lpc178x::io::detail::alternate::func_1>, // d0
            detail::emc::emc<pins::package::lqfp_208::p201, detail::emc::mode::data_bus, core::lpc178x::io::detail::alternate::func_1>, // d1
            detail::emc::emc<pins::package::lqfp_208::p207, detail::emc::mode::data_bus, core::lpc178x::io::detail::alternate::func_1>, // d2
            detail::emc::emc<pins::package::lqfp_208::p3, detail::emc::mode::data_bus, core::lpc178x::io::detail::alternate::func_1>, // d3
            detail::emc::emc<pins::package::lqfp_208::p13, detail::emc::mode::data_bus, core::lpc178x::io::detail::alternate::func_1>, // d4
            detail::emc::emc<pins::package::lqfp_208::p17, detail::emc::mode::data_bus, core::lpc178x::io::detail::alternate::func_1>, // d5
            detail::emc::emc<pins::package::lqfp_208::p23, detail::emc::mode::data_bus, core::lpc178x::io::detail::alternate::func_1>, // d6
            detail::emc::emc<pins::package::lqfp_208::p27, detail::emc::mode::data_bus, core::lpc178x::io::detail::alternate::func_1>, // d7
            detail::emc::emc<pins::package::lqfp_208::p191, detail::emc::mode::data_bus, core::lpc178x::io::detail::alternate::func_1>, // d8
            detail::emc::emc<pins::package::lqfp_208::p199, detail::emc::mode::data_bus, core::lpc178x::io::detail::alternate::func_1>, // d9
            detail::emc::emc<pins::package::lqfp_208::p205, detail::emc::mode::data_bus, core::lpc178x::io::detail::alternate::func_1>, // d10
            detail::emc::emc<pins::package::lqfp_208::p208, detail::emc::mode::data_bus, core::lpc178x::io::detail::alternate::func_1>, // d11
            detail::emc::emc<pins::package::lqfp_208::p1, detail::emc::mode::data_bus, core::lpc178x::io::detail::alternate::func_1>, // d12
            detail::emc::emc<pins::package::lqfp_208::p7, detail::emc::mode::data_bus, core::lpc178x::io::detail::alternate::func_1>, // d13
            detail::emc::emc<pins::package::lqfp_208::p21, detail::emc::mode::data_bus, core::lpc178x::io::detail::alternate::func_1>, // d14
            detail::emc::emc<pins::package::lqfp_208::p28, detail::emc::mode::data_bus, core::lpc178x::io::detail::alternate::func_1>, // d15
            detail::emc::emc<pins::package::lqfp_208::p137, detail::emc::mode::data_bus, core::lpc178x::io::detail::alternate::func_1>, // d16
            detail::emc::emc<pins::package::lqfp_208::p143, detail::emc::mode::data_bus, core::lpc178x::io::detail::alternate::func_1>, // d17
            detail::emc::emc<pins::package::lqfp_208::p151, detail::emc::mode::data_bus, core::lpc178x::io::detail::alternate::func_1>, // d18
            detail::emc::emc<pins::package::lqfp_208::p161, detail::emc::mode::data_bus, core::lpc178x::io::detail::alternate::func_1>, // d19
            detail::emc::emc<pins::package::lqfp_208::p167, detail::emc::mode::data_bus, core::lpc178x::io::detail::alternate::func_1>, // d20
            detail::emc::emc<pins::package::lqfp_208::p175, detail::emc::mode::data_bus, core::lpc178x::io::detail::alternate::func_1>, // d21
            detail::emc::emc<pins::package::lqfp_208::p195, detail::emc::mode::data_bus, core::lpc178x::io::detail::alternate::func_1>, // d22
            detail::emc::emc<pins::package::lqfp_208::p65, detail::emc::mode::data_bus, core::lpc178x::io::detail::alternate::func_1>, // d23
            detail::emc::emc<pins::package::lqfp_208::p58, detail::emc::mode::data_bus, core::lpc178x::io::detail::alternate::func_1>, // d24
            detail::emc::emc<pins::package::lqfp_208::p56, detail::emc::mode::data_bus, core::lpc178x::io::detail::alternate::func_1>, // d25
            detail::emc::emc<pins::package::lqfp_208::p55, detail::emc::mode::data_bus, core::lpc178x::io::detail::alternate::func_1>, // d26
            detail::emc::emc<pins::package::lqfp_208::p203, detail::emc::mode::data_bus, core::lpc178x::io::detail::alternate::func_1>, // d27
            detail::emc::emc<pins::package::lqfp_208::p5, detail::emc::mode::data_bus, core::lpc178x::io::detail::alternate::func_1>, // d28
            detail::emc::emc<pins::package::lqfp_208::p11, detail::emc::mode::data_bus, core::lpc178x::io::detail::alternate::func_1>, // d29
            detail::emc::emc<pins::package::lqfp_208::p19, detail::emc::mode::data_bus, core::lpc178x::io::detail::alternate::func_1>, // d30
            detail::emc::emc<pins::package::lqfp_208::p25, detail::emc::mode::data_bus, core::lpc178x::io::detail::alternate::func_1> // d31
        >;

        // all the address pins
        using address_pins = std::tuple<
            detail::emc::emc<pins::package::lqfp_208::p75, detail::emc::mode::address_bus, core::lpc178x::io::detail::alternate::func_1>, // a0
            detail::emc::emc<pins::package::lqfp_208::p79, detail::emc::mode::address_bus, core::lpc178x::io::detail::alternate::func_1>, // a1
            detail::emc::emc<pins::package::lqfp_208::p83, detail::emc::mode::address_bus, core::lpc178x::io::detail::alternate::func_1>, // a2
            detail::emc::emc<pins::package::lqfp_208::p97, detail::emc::mode::address_bus, core::lpc178x::io::detail::alternate::func_1>, // a3
            detail::emc::emc<pins::package::lqfp_208::p103, detail::emc::mode::address_bus, core::lpc178x::io::detail::alternate::func_1>, // a4
            detail::emc::emc<pins::package::lqfp_208::p107, detail::emc::mode::address_bus, core::lpc178x::io::detail::alternate::func_1>, // a5
            detail::emc::emc<pins::package::lqfp_208::p113, detail::emc::mode::address_bus, core::lpc178x::io::detail::alternate::func_1>, // a6
            detail::emc::emc<pins::package::lqfp_208::p121, detail::emc::mode::address_bus, core::lpc178x::io::detail::alternate::func_1>, // a7
            detail::emc::emc<pins::package::lqfp_208::p127, detail::emc::mode::address_bus, core::lpc178x::io::detail::alternate::func_1>, // a8
            detail::emc::emc<pins::package::lqfp_208::p131, detail::emc::mode::address_bus, core::lpc178x::io::detail::alternate::func_1>, // a9
            detail::emc::emc<pins::package::lqfp_208::p135, detail::emc::mode::address_bus, core::lpc178x::io::detail::alternate::func_1>, // a10
            detail::emc::emc<pins::package::lqfp_208::p145, detail::emc::mode::address_bus, core::lpc178x::io::detail::alternate::func_1>, // a11
            detail::emc::emc<pins::package::lqfp_208::p149, detail::emc::mode::address_bus, core::lpc178x::io::detail::alternate::func_1>, // a12
            detail::emc::emc<pins::package::lqfp_208::p155, detail::emc::mode::address_bus, core::lpc178x::io::detail::alternate::func_1>, // a13
            detail::emc::emc<pins::package::lqfp_208::p159, detail::emc::mode::address_bus, core::lpc178x::io::detail::alternate::func_1>, // a14
            detail::emc::emc<pins::package::lqfp_208::p173, detail::emc::mode::address_bus, core::lpc178x::io::detail::alternate::func_1>, // a15
            detail::emc::emc<pins::package::lqfp_208::p101, detail::emc::mode::address_bus, core::lpc178x::io::detail::alternate::func_1>, // a16
            detail::emc::emc<pins::package::lqfp_208::p104, detail::emc::mode::address_bus, core::lpc178x::io::detail::alternate::func_1>, // a17
            detail::emc::emc<pins::package::lqfp_208::p105, detail::emc::mode::address_bus, core::lpc178x::io::detail::alternate::func_1>, // a18
            detail::emc::emc<pins::package::lqfp_208::p111, detail::emc::mode::address_bus, core::lpc178x::io::detail::alternate::func_1>, // a19
            detail::emc::emc<pins::package::lqfp_208::p109, detail::emc::mode::address_bus, core::lpc178x::io::detail::alternate::func_1>, // a20
            detail::emc::emc<pins::package::lqfp_208::p115, detail::emc::mode::address_bus, core::lpc178x::io::detail::alternate::func_1>, // a21
            detail::emc::emc<pins::package::lqfp_208::p123, detail::emc::mode::address_bus, core::lpc178x::io::detail::alternate::func_1>, // a22
            detail::emc::emc<pins::package::lqfp_208::p129, detail::emc::mode::address_bus, core::lpc178x::io::detail::alternate::func_1>, // a23
            detail::emc::emc<pins::package::lqfp_208::p9, detail::emc::mode::address_bus, core::lpc178x::io::detail::alternate::func_1>, // a24
            detail::emc::emc<pins::package::lqfp_208::p30, detail::emc::mode::address_bus, core::lpc178x::io::detail::alternate::func_1> // a25
        >;

        // static memory regions
        constexpr static std::array<uint32_t, 4> static_addresses = {
            0x80000000,
            0x90000000,
            0x98000000,
            0x9c000000,
        };

        // dynamic memory regions
        constexpr static std::array<uint32_t, 4> dynamic_addresses = {
            0xa0000000,
            0xb0000000,
            0xc0000000,
            0xd0000000,
        };
    };
}

namespace klib::lpc1788::io {
    template <typename Emc, uint8_t AddressBits, uint8_t DataBits, bool HalfCPUClock = false>
    using emc = klib::core::lpc178x::io::emc<Emc, AddressBits, DataBits, HalfCPUClock>;
}

#endif