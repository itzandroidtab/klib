#ifndef KLIB_LPC1756_SSP_HPP
#define KLIB_LPC1756_SSP_HPP

#include <klib/io/peripheral.hpp>
#include <targets/core/nxp/lpc17xx/ssp.hpp>
#include "pins.hpp"

namespace klib::lpc1756::io::periph::detail::ssp {
    enum class mode {
        miso,
        mosi,
        sck,
        cs0
    };

    template <typename Pin, mode Type, typename Periph>
    struct ssp {
        // pin of the peripheral
        using pin = Pin;

        // type of the pin
        constexpr static mode type = Type;

        // alternate function
        using periph = Periph;
    };
}

namespace klib::lpc1756::io::periph::lqfp_80 {
    template <
        typename Mosi = pins::package::lqfp_80::p45, 
        typename Miso = pins::package::lqfp_80::p46,
        typename Sck = pins::package::lqfp_80::p47
    >
    struct ssp0 {
        // peripheral id (e.g ssp0, ssp1)
        constexpr static uint32_t id = 0;

        // interrupt id (including the arm vector table)
        constexpr static uint32_t interrupt_id = 30;

        // peripheral clock bit position
        constexpr static uint32_t clock_id = 21;

        // base dma id
        constexpr static uint32_t dma_id = 0;

        // port to the SSP hardware
        static inline SSP0_Type *const port = SSP0;

        // pins allowed per output pin. Used for determining if a pin is valid on compile time
        using mosi_pins = std::tuple<
            detail::ssp::ssp<pins::package::lqfp_80::p45, detail::ssp::mode::mosi, core::lpc175x::io::detail::alternate::func_2>,
            detail::ssp::ssp<pins::package::lqfp_80::p30, detail::ssp::mode::mosi, core::lpc175x::io::detail::alternate::func_3>
        >;

        using miso_pins = std::tuple<
            detail::ssp::ssp<pins::package::lqfp_80::p46, detail::ssp::mode::miso, core::lpc175x::io::detail::alternate::func_2>,
            detail::ssp::ssp<pins::package::lqfp_80::p29, detail::ssp::mode::miso, core::lpc175x::io::detail::alternate::func_3>
        >;

        using sck_pins = std::tuple<
            detail::ssp::ssp<pins::package::lqfp_80::p47, detail::ssp::mode::sck, core::lpc175x::io::detail::alternate::func_2>,
            detail::ssp::ssp<pins::package::lqfp_80::p27, detail::ssp::mode::sck, core::lpc175x::io::detail::alternate::func_3>
        >;

        // pin configuration for the ssp. Uses above mapping
        using mosi = std::tuple_element<klib::io::peripheral::get_index<Mosi, mosi_pins>(), mosi_pins>::type;
        using miso = std::tuple_element<klib::io::peripheral::get_index<Miso, miso_pins>(), miso_pins>::type;
        using sck = std::tuple_element<klib::io::peripheral::get_index<Sck, sck_pins>(), sck_pins>::type;
        using cs0 = detail::ssp::ssp<pins::package::lqfp_80::p48, detail::ssp::mode::cs0, core::lpc175x::io::detail::alternate::func_2>;
    };

    template <
        typename Sck = pins::package::lqfp_80::p63
    >
    struct ssp1 {
        // peripheral id (e.g ssp0, ssp1)
        constexpr static uint32_t id = 1;

        // interrupt id (including the arm vector table)
        constexpr static uint32_t interrupt_id = 31;

        // peripheral clock bit position
        constexpr static uint32_t clock_id = 10;

        // base dma id
        constexpr static uint32_t dma_id = 2;

        // port to the SSP hardware
        static inline SSP0_Type *const port = SSP1;

        // pins allowed per output pin. Used for determining if a pin is valid on compile time
        using sck_pins = std::tuple<
            detail::ssp::ssp<pins::package::lqfp_80::p63, detail::ssp::mode::sck, core::lpc175x::io::detail::alternate::func_2>,
            detail::ssp::ssp<pins::package::lqfp_80::p17, detail::ssp::mode::sck, core::lpc175x::io::detail::alternate::func_2>
        >;

        // pin configuration for the ssp. Uses above mapping
        using mosi = detail::ssp::ssp<pins::package::lqfp_80::p61, detail::ssp::mode::mosi, core::lpc175x::io::detail::alternate::func_2>;
        using miso = detail::ssp::ssp<pins::package::lqfp_80::p62, detail::ssp::mode::miso, core::lpc175x::io::detail::alternate::func_2>;
        using sck = std::tuple_element<klib::io::peripheral::get_index<Sck, sck_pins>(), sck_pins>::type;
        using cs0 = detail::ssp::ssp<pins::package::lqfp_80::p64, detail::ssp::mode::cs0, core::lpc175x::io::detail::alternate::func_2>;
    };
}

namespace klib::lpc1756::io {
    template <typename Ssp>
    using ssp = klib::core::lpc17xx::io::ssp<Ssp>;
}

#endif