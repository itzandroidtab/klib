#ifndef KLIB_LPC1788_LCD_HPP
#define KLIB_LPC1788_LCD_HPP

#include <klib/io/port.hpp>
#include <klib/io/peripheral.hpp>

#include <targets/core/nxp/lpc178x/lcd.hpp>

#include "pins.hpp"

namespace klib::lpc1788::io::periph::detail::lcd {
    enum class mode {
        power,
        clock,
        output_enable,
        vsync,
        line_end,
        hsync,
        data,
        clock_in,
    };

    template <typename Pin, mode Type, typename Periph>
    struct lcd {
        // pin of the peripheral
        using pin = Pin;

        // type of the pin
        constexpr static mode type = Type;

        // alternate function
        using periph = Periph;
    };
}

namespace klib::lpc1788::io::periph::lqfp_208 {
    template <
        typename Vd0 = pins::package::lqfp_208::p168,
        typename Vd1 = pins::package::lqfp_208::p166,
        typename Vd2 = pins::package::lqfp_208::p170,
        typename Vd3 = pins::package::lqfp_208::p176,
        typename Vd4 = pins::package::lqfp_208::p138,
        typename Vd5 = pins::package::lqfp_208::p136,
        typename Vd6 = pins::package::lqfp_208::p134,
        typename Vd7 = pins::package::lqfp_208::p132,
        typename Vd8 = pins::package::lqfp_208::p164,
        typename Vd9 = pins::package::lqfp_208::p162,
        typename Vd10 = pins::package::lqfp_208::p70,
        typename Vd11 = pins::package::lqfp_208::p72,
        typename Vd12 = pins::package::lqfp_208::p74,
        typename Vd13 = pins::package::lqfp_208::p76,
        typename Vd14 = pins::package::lqfp_208::p78,
        typename Vd15 = pins::package::lqfp_208::p80        
    >
    struct lcd0 {
        // peripheral id (e.g lcd0, lcd1)
        constexpr static uint32_t id = 0;

        // peripheral clock bit position
        constexpr static uint32_t clock_id = 0;

        // port to the LCD hardware
        static inline LCD_Type *const port = LCD;

        // configurable data signals
        using vd0_pins = std::tuple<
            detail::lcd::lcd<pins::package::lqfp_208::p168, detail::lcd::mode::data, core::lpc178x::io::detail::alternate::func_7>,
            detail::lcd::lcd<pins::package::lqfp_208::p138, detail::lcd::mode::data, core::lpc178x::io::detail::alternate::func_6>
        >;

        using vd1_pins = std::tuple<
            detail::lcd::lcd<pins::package::lqfp_208::p166, detail::lcd::mode::data, core::lpc178x::io::detail::alternate::func_7>,
            detail::lcd::lcd<pins::package::lqfp_208::p136, detail::lcd::mode::data, core::lpc178x::io::detail::alternate::func_6>
        >;

        using vd2_pins = std::tuple<
            detail::lcd::lcd<pins::package::lqfp_208::p134, detail::lcd::mode::data, core::lpc178x::io::detail::alternate::func_6>,
            detail::lcd::lcd<pins::package::lqfp_208::p170, detail::lcd::mode::data, core::lpc178x::io::detail::alternate::func_7>
        >;

        using vd3_pins = std::tuple<
            detail::lcd::lcd<pins::package::lqfp_208::p132, detail::lcd::mode::data, core::lpc178x::io::detail::alternate::func_6>,
            detail::lcd::lcd<pins::package::lqfp_208::p106, detail::lcd::mode::data, core::lpc178x::io::detail::alternate::func_5>,
            detail::lcd::lcd<pins::package::lqfp_208::p176, detail::lcd::mode::data, core::lpc178x::io::detail::alternate::func_7>
        >;

        using vd4_pins = std::tuple<
            detail::lcd::lcd<pins::package::lqfp_208::p138, detail::lcd::mode::data, core::lpc178x::io::detail::alternate::func_7>,
            detail::lcd::lcd<pins::package::lqfp_208::p106, detail::lcd::mode::data, core::lpc178x::io::detail::alternate::func_4>
        >;

        using vd5_pins = std::tuple<
            detail::lcd::lcd<pins::package::lqfp_208::p136, detail::lcd::mode::data, core::lpc178x::io::detail::alternate::func_7>,
            detail::lcd::lcd<pins::package::lqfp_208::p102, detail::lcd::mode::data, core::lpc178x::io::detail::alternate::func_5>
        >;

        using vd6_pins = std::tuple<
            detail::lcd::lcd<pins::package::lqfp_208::p70, detail::lcd::mode::data, core::lpc178x::io::detail::alternate::func_6>,
            detail::lcd::lcd<pins::package::lqfp_208::p134, detail::lcd::mode::data, core::lpc178x::io::detail::alternate::func_7>,
            detail::lcd::lcd<pins::package::lqfp_208::p170, detail::lcd::mode::data, core::lpc178x::io::detail::alternate::func_5>
        >;

        using vd7_pins = std::tuple<
            detail::lcd::lcd<pins::package::lqfp_208::p72, detail::lcd::mode::data, core::lpc178x::io::detail::alternate::func_6>,
            detail::lcd::lcd<pins::package::lqfp_208::p132, detail::lcd::mode::data, core::lpc178x::io::detail::alternate::func_7>,
            detail::lcd::lcd<pins::package::lqfp_208::p176, detail::lcd::mode::data, core::lpc178x::io::detail::alternate::func_5>
        >;

        using vd8_pins = std::tuple<
            detail::lcd::lcd<pins::package::lqfp_208::p164, detail::lcd::mode::data, core::lpc178x::io::detail::alternate::func_7>,
            detail::lcd::lcd<pins::package::lqfp_208::p74, detail::lcd::mode::data, core::lpc178x::io::detail::alternate::func_6>,
            detail::lcd::lcd<pins::package::lqfp_208::p106, detail::lcd::mode::data, core::lpc178x::io::detail::alternate::func_6>
        >;

        using vd9_pins = std::tuple<
            detail::lcd::lcd<pins::package::lqfp_208::p162, detail::lcd::mode::data, core::lpc178x::io::detail::alternate::func_7>,
            detail::lcd::lcd<pins::package::lqfp_208::p76, detail::lcd::mode::data, core::lpc178x::io::detail::alternate::func_6>,
            detail::lcd::lcd<pins::package::lqfp_208::p102, detail::lcd::mode::data, core::lpc178x::io::detail::alternate::func_6>
        >;

        using vd10_pins = std::tuple<
            detail::lcd::lcd<pins::package::lqfp_208::p70, detail::lcd::mode::data, core::lpc178x::io::detail::alternate::func_7>,
            detail::lcd::lcd<pins::package::lqfp_208::p78, detail::lcd::mode::data, core::lpc178x::io::detail::alternate::func_6>,
            detail::lcd::lcd<pins::package::lqfp_208::p170, detail::lcd::mode::data, core::lpc178x::io::detail::alternate::func_6>
        >;

        using vd11_pins = std::tuple<
            detail::lcd::lcd<pins::package::lqfp_208::p72, detail::lcd::mode::data, core::lpc178x::io::detail::alternate::func_7>,
            detail::lcd::lcd<pins::package::lqfp_208::p80, detail::lcd::mode::data, core::lpc178x::io::detail::alternate::func_6>,
            detail::lcd::lcd<pins::package::lqfp_208::p176, detail::lcd::mode::data, core::lpc178x::io::detail::alternate::func_6>
        >;

        using vd12_pins = std::tuple<
            detail::lcd::lcd<pins::package::lqfp_208::p74, detail::lcd::mode::data, core::lpc178x::io::detail::alternate::func_7>,
            detail::lcd::lcd<pins::package::lqfp_208::p82, detail::lcd::mode::data, core::lpc178x::io::detail::alternate::func_6>
        >;

        using vd13_pins = std::tuple<
            detail::lcd::lcd<pins::package::lqfp_208::p76, detail::lcd::mode::data, core::lpc178x::io::detail::alternate::func_7>,
            detail::lcd::lcd<pins::package::lqfp_208::p88, detail::lcd::mode::data, core::lpc178x::io::detail::alternate::func_6>
        >;

        using vd14_pins = std::tuple<
            detail::lcd::lcd<pins::package::lqfp_208::p78, detail::lcd::mode::data, core::lpc178x::io::detail::alternate::func_7>,
            detail::lcd::lcd<pins::package::lqfp_208::p90, detail::lcd::mode::data, core::lpc178x::io::detail::alternate::func_6>
        >;

        using vd15_pins = std::tuple<
            detail::lcd::lcd<pins::package::lqfp_208::p80, detail::lcd::mode::data, core::lpc178x::io::detail::alternate::func_7>,
            detail::lcd::lcd<pins::package::lqfp_208::p92, detail::lcd::mode::data, core::lpc178x::io::detail::alternate::func_6>
        >;

        using vd0 = std::tuple_element<klib::io::peripheral::get_index<Vd0, vd0_pins>(), vd0_pins>::type;
        using vd1 = std::tuple_element<klib::io::peripheral::get_index<Vd1, vd1_pins>(), vd1_pins>::type;
        using vd2 = std::tuple_element<klib::io::peripheral::get_index<Vd2, vd2_pins>(), vd2_pins>::type;
        using vd3 = std::tuple_element<klib::io::peripheral::get_index<Vd3, vd3_pins>(), vd3_pins>::type;
        using vd4 = std::tuple_element<klib::io::peripheral::get_index<Vd4, vd4_pins>(), vd4_pins>::type;
        using vd5 = std::tuple_element<klib::io::peripheral::get_index<Vd5, vd5_pins>(), vd5_pins>::type;
        using vd6 = std::tuple_element<klib::io::peripheral::get_index<Vd6, vd6_pins>(), vd6_pins>::type;
        using vd7 = std::tuple_element<klib::io::peripheral::get_index<Vd7, vd7_pins>(), vd7_pins>::type;
        using vd8 = std::tuple_element<klib::io::peripheral::get_index<Vd8, vd8_pins>(), vd8_pins>::type;
        using vd9 = std::tuple_element<klib::io::peripheral::get_index<Vd9, vd9_pins>(), vd9_pins>::type;
        using vd10 = std::tuple_element<klib::io::peripheral::get_index<Vd10, vd10_pins>(), vd10_pins>::type;
        using vd11 = std::tuple_element<klib::io::peripheral::get_index<Vd11, vd11_pins>(), vd11_pins>::type;
        using vd12 = std::tuple_element<klib::io::peripheral::get_index<Vd12, vd12_pins>(), vd12_pins>::type;
        using vd13 = std::tuple_element<klib::io::peripheral::get_index<Vd13, vd13_pins>(), vd13_pins>::type;
        using vd14 = std::tuple_element<klib::io::peripheral::get_index<Vd14, vd14_pins>(), vd14_pins>::type;
        using vd15 = std::tuple_element<klib::io::peripheral::get_index<Vd15, vd15_pins>(), vd15_pins>::type;
        
        // fixed data signals
        using vd16 = detail::lcd::lcd<pins::package::lqfp_208::p160, detail::lcd::mode::data, core::lpc178x::io::detail::alternate::func_7>;
        using vd17 = detail::lcd::lcd<pins::package::lqfp_208::p158, detail::lcd::mode::data, core::lpc178x::io::detail::alternate::func_7>;
        using vd18 = detail::lcd::lcd<pins::package::lqfp_208::p106, detail::lcd::mode::data, core::lpc178x::io::detail::alternate::func_7>;
        using vd19 = detail::lcd::lcd<pins::package::lqfp_208::p102, detail::lcd::mode::data, core::lpc178x::io::detail::alternate::func_7>;
        using vd20 = detail::lcd::lcd<pins::package::lqfp_208::p82, detail::lcd::mode::data, core::lpc178x::io::detail::alternate::func_7>;
        using vd21 = detail::lcd::lcd<pins::package::lqfp_208::p88, detail::lcd::mode::data, core::lpc178x::io::detail::alternate::func_7>;
        using vd22 = detail::lcd::lcd<pins::package::lqfp_208::p90, detail::lcd::mode::data, core::lpc178x::io::detail::alternate::func_7>;
        using vd23 = detail::lcd::lcd<pins::package::lqfp_208::p92, detail::lcd::mode::data, core::lpc178x::io::detail::alternate::func_7>;
        
        // other signals (also fixed)
        using power = detail::lcd::lcd<pins::package::lqfp_208::p154, detail::lcd::mode::power, core::lpc178x::io::detail::alternate::func_7>;
        using line_end = detail::lcd::lcd<pins::package::lqfp_208::p152, detail::lcd::mode::line_end, core::lpc178x::io::detail::alternate::func_7>;
        using clock = detail::lcd::lcd<pins::package::lqfp_208::p150, detail::lcd::mode::clock, core::lpc178x::io::detail::alternate::func_7>;
        using clock_in = detail::lcd::lcd<pins::package::lqfp_208::p108, detail::lcd::mode::clock_in, core::lpc178x::io::detail::alternate::func_7>;
        using vsync = detail::lcd::lcd<pins::package::lqfp_208::p144, detail::lcd::mode::vsync, core::lpc178x::io::detail::alternate::func_7>;
        using hsync = detail::lcd::lcd<pins::package::lqfp_208::p140, detail::lcd::mode::hsync, core::lpc178x::io::detail::alternate::func_7>;
        using output_enable = detail::lcd::lcd<pins::package::lqfp_208::p142, detail::lcd::mode::output_enable, core::lpc178x::io::detail::alternate::func_7>;
    };
}

namespace klib::lpc1788::io {
    template <typename Lcd, graphics::mode Mode = graphics::mode::rgb565>
    using lcd = klib::core::lpc178x::io::lcd<Lcd, Mode>;
}

#endif