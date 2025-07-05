#ifndef KLIB_LPC1788_EMAC_HPP
#define KLIB_LPC1788_EMAC_HPP

#include <klib/io/peripheral.hpp>
#include <targets/core/nxp/lpc178x/emac.hpp>
#include "pins.hpp"

namespace klib::lpc1788::io::periph::detail::emac {
    enum class mode {
        txd0,
        txd1,
        txd2,
        txd3,

        rxd0,
        rxd1,
        rxd2,
        rxd3,

        crs,
        col,
        tx_en,
        tx_er,
        rx_dv,
        rx_er,

        tx_clk,
        rx_clk,
        ref_clk,

        mdc,
        mdio
    };

    template <typename Pin, mode Type, typename Periph>
    struct emac {
        // pin of the peripheral
        using pin = Pin;

        // type of the pin
        constexpr static mode type = Type;

        // alternate function
        using periph = Periph;
    };
}

namespace klib::lpc1788::io::periph::lqfp_208 {
    struct emac0 {
        // peripheral id (e.g emac0, emac1)
        constexpr static uint32_t id = 0;

        // interrupt id (including the arm vector table)
        constexpr static uint32_t interrupt_id = (16 + 28);

        // peripheral clock bit position
        constexpr static uint32_t clock_id = 30;

        // base dma id
        constexpr static uint32_t dma_id = 0;

        // port to the EMAC hardware
        static inline ETHERNET_Type *const port = ETHERNET;

        struct mii {
            // configuration of the pins
            using txd0 = detail::emac::emac<pins::package::lqfp_208::p196, detail::emac::mode::txd0, core::lpc178x::io::detail::alternate::func_1>;
            using txd1 = detail::emac::emac<pins::package::lqfp_208::p194, detail::emac::mode::txd1, core::lpc178x::io::detail::alternate::func_1>;
            using txd2 = detail::emac::emac<pins::package::lqfp_208::p185, detail::emac::mode::txd2, core::lpc178x::io::detail::alternate::func_1>;
            using txd3 = detail::emac::emac<pins::package::lqfp_208::p177, detail::emac::mode::txd3, core::lpc178x::io::detail::alternate::func_1>;

            using tx_en = detail::emac::emac<pins::package::lqfp_208::p192, detail::emac::mode::tx_en, core::lpc178x::io::detail::alternate::func_1>;
            using tx_er = detail::emac::emac<pins::package::lqfp_208::p156, detail::emac::mode::tx_er, core::lpc178x::io::detail::alternate::func_1>;
            using tx_clk = detail::emac::emac<pins::package::lqfp_208::p171, detail::emac::mode::tx_clk, core::lpc178x::io::detail::alternate::func_1>;

            using col = detail::emac::emac<pins::package::lqfp_208::p153, detail::emac::mode::col, core::lpc178x::io::detail::alternate::func_1>;
            using crs = detail::emac::emac<pins::package::lqfp_208::p190, detail::emac::mode::crs, core::lpc178x::io::detail::alternate::func_1>;

            using rxd0 = detail::emac::emac<pins::package::lqfp_208::p188, detail::emac::mode::rxd0, core::lpc178x::io::detail::alternate::func_1>;
            using rxd1 = detail::emac::emac<pins::package::lqfp_208::p186, detail::emac::mode::rxd1, core::lpc178x::io::detail::alternate::func_1>;
            using rxd2 = detail::emac::emac<pins::package::lqfp_208::p163, detail::emac::mode::rxd2, core::lpc178x::io::detail::alternate::func_1>;
            using rxd3 = detail::emac::emac<pins::package::lqfp_208::p157, detail::emac::mode::rxd3, core::lpc178x::io::detail::alternate::func_1>;

            using rx_dv = detail::emac::emac<pins::package::lqfp_208::p147, detail::emac::mode::rx_dv, core::lpc178x::io::detail::alternate::func_1>;
            using rx_er = detail::emac::emac<pins::package::lqfp_208::p184, detail::emac::mode::rx_er, core::lpc178x::io::detail::alternate::func_1>;
            using rx_clk = detail::emac::emac<pins::package::lqfp_208::p182, detail::emac::mode::rx_clk, core::lpc178x::io::detail::alternate::func_1>;
        };

        struct rmii {
            // configuration of the pins
            using txd0 = detail::emac::emac<pins::package::lqfp_208::p196, detail::emac::mode::txd0, core::lpc178x::io::detail::alternate::func_1>;
            using txd1 = detail::emac::emac<pins::package::lqfp_208::p194, detail::emac::mode::txd1, core::lpc178x::io::detail::alternate::func_1>;
            using rxd0 = detail::emac::emac<pins::package::lqfp_208::p188, detail::emac::mode::rxd0, core::lpc178x::io::detail::alternate::func_1>;
            using rxd1 = detail::emac::emac<pins::package::lqfp_208::p186, detail::emac::mode::rxd1, core::lpc178x::io::detail::alternate::func_1>;

            using tx_en = detail::emac::emac<pins::package::lqfp_208::p192, detail::emac::mode::tx_en, core::lpc178x::io::detail::alternate::func_1>;
            using rx_er = detail::emac::emac<pins::package::lqfp_208::p184, detail::emac::mode::rx_er, core::lpc178x::io::detail::alternate::func_1>;
            using crs = detail::emac::emac<pins::package::lqfp_208::p190, detail::emac::mode::crs, core::lpc178x::io::detail::alternate::func_1>;
            using ref_clk = detail::emac::emac<pins::package::lqfp_208::p182, detail::emac::mode::ref_clk, core::lpc178x::io::detail::alternate::func_1>;
        };
        
        // configuration for the MIIM
        using mdc = detail::emac::emac<pins::package::lqfp_208::p180, detail::emac::mode::mdc, core::lpc178x::io::detail::alternate::func_1>;
        using mdio = detail::emac::emac<pins::package::lqfp_208::p178, detail::emac::mode::mdio, core::lpc178x::io::detail::alternate::func_1>;
    };
}

namespace klib::lpc1788::io {
    template <typename Emac, klib::io::ethernet::mii Mii>
    using emac = klib::core::lpc178x::io::emac<Emac, Mii>;
}

#endif