#ifndef KLIB_ATSAM4S2B_SPI_HPP
#define KLIB_ATSAM4S2B_SPI_HPP

#include <cstdint>

#include <atsam4s2b.hpp>

#include <klib/io/peripheral.hpp>

#include "pins.hpp"

namespace klib::atsam4s2b::io::periph::detail::spi {
    enum class mode {
        miso,
        mosi,
        sck,
        cs0,
        cs1,
        cs2,
        cs3,
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

namespace klib::atsam4s2b::io::periph::wlcsp_64 {
    template <
        typename Cs1 = pins::package::wlcsp_64::pg3,
        typename Cs2 = pins::package::wlcsp_64::pc5,
        typename Cs3 = pins::package::wlcsp_64::pe1
    >
    struct spi0 {
        // peripheral id (e.g spi0, spi1)
        constexpr static uint32_t id = 0;

        // peripheral clock bit position
        constexpr static uint32_t clock_id = 21;

        // interrupt id (including the arm vector table)
        constexpr static uint32_t interrupt_id = 37;

        // port to the SPI hardware
        static inline SPI_Type *const port = SPI;

        // pins allowed per output pin. Used for determining if a pin is valid on compile time
        using cs1_pins = std::tuple<
            detail::spi::spi<pins::package::wlcsp_64::pg3, detail::spi::mode::cs1, core::atsam4s::io::detail::alternate::func_2>,
            detail::spi::spi<pins::package::wlcsp_64::pa2, detail::spi::mode::cs1, core::atsam4s::io::detail::alternate::func_1>,
            detail::spi::spi<pins::package::wlcsp_64::pb7, detail::spi::mode::cs1, core::atsam4s::io::detail::alternate::func_1>
        >;
        using cs2_pins = std::tuple<
            detail::spi::spi<pins::package::wlcsp_64::pc5, detail::spi::mode::cs2, core::atsam4s::io::detail::alternate::func_2>,
            detail::spi::spi<pins::package::wlcsp_64::pe2, detail::spi::mode::cs2, core::atsam4s::io::detail::alternate::func_2>,
            detail::spi::spi<pins::package::wlcsp_64::pc7, detail::spi::mode::cs2, core::atsam4s::io::detail::alternate::func_2>
        >;
        using cs3_pins = std::tuple<
            detail::spi::spi<pins::package::wlcsp_64::pe1, detail::spi::mode::cs3, core::atsam4s::io::detail::alternate::func_2>,
            detail::spi::spi<pins::package::wlcsp_64::pf3, detail::spi::mode::cs3, core::atsam4s::io::detail::alternate::func_2>,
            detail::spi::spi<pins::package::wlcsp_64::pf6, detail::spi::mode::cs3, core::atsam4s::io::detail::alternate::func_2>
        >;

        // configuration of the pins 
        using mosi = detail::spi::spi<pins::package::wlcsp_64::pf5, detail::spi::mode::mosi, core::atsam4s::io::detail::alternate::func_1>;
        using miso = detail::spi::spi<pins::package::wlcsp_64::ph4, detail::spi::mode::miso, core::atsam4s::io::detail::alternate::func_1>;
        using sck = detail::spi::spi<pins::package::wlcsp_64::pg6, detail::spi::mode::sck, core::atsam4s::io::detail::alternate::func_1>;
        using cs0 = detail::spi::spi<pins::package::wlcsp_64::pg4, detail::spi::mode::cs0, core::atsam4s::io::detail::alternate::func_1>;

        // pin mapping that uses above mapping
        using cs1 = std::tuple_element<klib::io::peripheral::get_index<Cs1, cs1_pins>(), cs1_pins>::type;
        using cs2 = std::tuple_element<klib::io::peripheral::get_index<Cs2, cs2_pins>(), cs2_pins>::type;
        using cs3 = std::tuple_element<klib::io::peripheral::get_index<Cs3, cs3_pins>(), cs3_pins>::type;
    };
}

#endif