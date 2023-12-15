#ifndef KLIB_LPC1788_SSP_HPP
#define KLIB_LPC1788_SSP_HPP

#include <klib/io/peripheral.hpp>
#include <targets/core/nxp/lpc17xx/ssp.hpp>
#include "pins.hpp"

namespace klib::lpc1788::io::periph::detail::ssp {
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

namespace klib::lpc1788::io::periph::lqfp208 {
    template <
        typename Mosi = pins::package::lqfp208::p124, 
        typename Miso = pins::package::lqfp208::p126,
        typename Sck = pins::package::lqfp208::p128,
        typename Cs = pins::package::lqfp208::p130
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
            detail::ssp::ssp<pins::package::lqfp208::p124, detail::ssp::mode::mosi, core::lpc178x::io::detail::alternate::func_2>,
            detail::ssp::ssp<pins::package::lqfp208::p78, detail::ssp::mode::mosi, core::lpc178x::io::detail::alternate::func_5>,
            detail::ssp::ssp<pins::package::lqfp208::p47, detail::ssp::mode::mosi, core::lpc178x::io::detail::alternate::func_2>
        >;

        using miso_pins = std::tuple<
            detail::ssp::ssp<pins::package::lqfp208::p126, detail::ssp::mode::miso, core::lpc178x::io::detail::alternate::func_2>,
            detail::ssp::ssp<pins::package::lqfp208::p76, detail::ssp::mode::miso, core::lpc178x::io::detail::alternate::func_5>,
            detail::ssp::ssp<pins::package::lqfp208::p57, detail::ssp::mode::miso, core::lpc178x::io::detail::alternate::func_2>
        >;

        using sck_pins = std::tuple<
            detail::ssp::ssp<pins::package::lqfp208::p128, detail::ssp::mode::sck, core::lpc178x::io::detail::alternate::func_2>,
            detail::ssp::ssp<pins::package::lqfp208::p70, detail::ssp::mode::sck, core::lpc178x::io::detail::alternate::func_5>,
            detail::ssp::ssp<pins::package::lqfp208::p85, detail::ssp::mode::sck, core::lpc178x::io::detail::alternate::func_2>
        >;

        using cs_pins = std::tuple<
            detail::ssp::ssp<pins::package::lqfp208::p130, detail::ssp::mode::sck, core::lpc178x::io::detail::alternate::func_2>,
            detail::ssp::ssp<pins::package::lqfp208::p90, detail::ssp::mode::sck, core::lpc178x::io::detail::alternate::func_5>,
            detail::ssp::ssp<pins::package::lqfp208::p64, detail::ssp::mode::sck, core::lpc178x::io::detail::alternate::func_2>
        >;

        // pin configuration for the ssp. Uses above mapping
        using mosi = std::tuple_element<klib::io::peripheral::get_index<Mosi, mosi_pins>(), mosi_pins>::type;
        using miso = std::tuple_element<klib::io::peripheral::get_index<Miso, miso_pins>(), miso_pins>::type;
        using sck = std::tuple_element<klib::io::peripheral::get_index<Sck, sck_pins>(), sck_pins>::type;
        using cs0 = std::tuple_element<klib::io::peripheral::get_index<Cs, cs_pins>(), cs_pins>::type;
    };
}

namespace klib::lpc1788::io {
    template <typename Ssp>
    using ssp = klib::core::lpc17xx::io::ssp<Ssp>;
}

#endif