#ifndef KLIB_LPC1756_SSP_HPP
#define KLIB_LPC1756_SSP_HPP

#include <cstdint>

#include <lpc1756.hpp>
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
    struct ssp0 {
        // peripheral id (e.g ssp0, ssp1)
        constexpr static uint32_t id = 0;

        // interrupt id (including the arm vector table)
        constexpr static uint32_t interrupt_id = 30;

        // peripheral clock bit position
        constexpr static uint32_t clock_id = 21;

        // port to the SSP hardware
        static inline SSP0_Type *const port = SSP0;

        // configuration of the pins 
        using mosi = detail::ssp::ssp<pins::package::lqfp_80::p45, detail::ssp::mode::mosi, io::detail::alternate::func_2>;
        using miso = detail::ssp::ssp<pins::package::lqfp_80::p46, detail::ssp::mode::miso, io::detail::alternate::func_2>;
        using sck = detail::ssp::ssp<pins::package::lqfp_80::p47, detail::ssp::mode::sck, io::detail::alternate::func_2>;
        using cs0 = detail::ssp::ssp<pins::package::lqfp_80::p48, detail::ssp::mode::cs0, io::detail::alternate::func_2>;

        // using mosi = detail::ssp::ssp<pins::package::lqfp_80::p30, detail::ssp::mode::mosi, io::detail::alternate::func_3>;
        // using miso = detail::ssp::ssp<pins::package::lqfp_80::p29, detail::ssp::mode::miso, io::detail::alternate::func_3>;
        // using sck = detail::ssp::ssp<pins::package::lqfp_80::p27, detail::ssp::mode::sck, io::detail::alternate::func_3>;

    };

    struct ssp1 {
        // peripheral id (e.g ssp0, ssp1)
        constexpr static uint32_t id = 1;

        // interrupt id (including the arm vector table)
        constexpr static uint32_t interrupt_id = 31;

        // peripheral clock bit position
        constexpr static uint32_t clock_id = 10;

        // port to the SSP hardware
        static inline SSP0_Type *const port = SSP1;

        // configuration of the pins 
        using mosi = detail::ssp::ssp<pins::package::lqfp_80::p61, detail::ssp::mode::mosi, io::detail::alternate::func_2>;
        using miso = detail::ssp::ssp<pins::package::lqfp_80::p62, detail::ssp::mode::miso, io::detail::alternate::func_2>;
        using sck = detail::ssp::ssp<pins::package::lqfp_80::p63, detail::ssp::mode::sck, io::detail::alternate::func_2>;
        using cs0 = detail::ssp::ssp<pins::package::lqfp_80::p64, detail::ssp::mode::cs0, io::detail::alternate::func_2>;

        // using sck = detail::ssp::ssp<pins::package::lqfp_80::p17, detail::ssp::mode::sck, io::detail::alternate::func_2>;
    };
}

#endif