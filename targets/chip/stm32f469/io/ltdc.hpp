#ifndef KLIB_STM32F469_LTDC_HPP
#define KLIB_STM32F469_LTDC_HPP

#include <cstdint>

#include <klib/io/peripheral.hpp>

#include <io/pins.hpp>

namespace klib::stm32f469::io::periph::detail::ltdc {
    enum class mode {
        r0, r1, r2, r3, r4, r5, r6, r7,
        g0, g1, g2, g3, g4, g5, g6, g7,
        b0, b1, b2, b3, b4, b5, b6, b7,
        clk, hsync, vsync, de
    };

    template <typename Pin, mode Type, typename Periph>
    struct ltdc {
        // pin of the peripheral
        using pin = Pin;

        // type of the pin
        constexpr static mode type = Type;

        // alternate function
        using periph = Periph;
    };
}

// global peripherals, not affected by chip package
namespace klib::stm32f469::io::periph {
    template <
        typename Clk = pins::package::tfbga216::ph3,
        typename Hsync = pins::package::tfbga216::pd5,
        typename Vsync = pins::package::tfbga216::pe4,
        typename De = pins::package::tfbga216::pc4,
        typename R0 = pins::package::tfbga216::pb3,
        typename R1 = pins::package::tfbga216::pj4,
        typename R2 = pins::package::tfbga216::pb14,
        typename R3 = pins::package::tfbga216::pn14,
        typename R4 = pins::package::tfbga216::pc15,
        typename R5 = pins::package::tfbga216::pb15,
        typename R6 = pins::package::tfbga216::pf15,
        typename R7 = pins::package::tfbga216::pj15,
        typename G0 = pins::package::tfbga216::pb1,
        typename G1 = pins::package::tfbga216::pb2,
        typename G2 = pins::package::tfbga216::pe12,
        typename G3 = pins::package::tfbga216::pb10,
        typename G4 = pins::package::tfbga216::pb9,
        typename G5 = pins::package::tfbga216::pe14,
        typename G6 = pins::package::tfbga216::pd14,
        typename G7 = pins::package::tfbga216::pa8,
        typename B0 = pins::package::tfbga216::pa1,
        typename B1 = pins::package::tfbga216::pb9,
        typename B2 = pins::package::tfbga216::pb11,
        typename B3 = pins::package::tfbga216::pb8,
        typename B4 = pins::package::tfbga216::pc3,
        typename B5 = pins::package::tfbga216::pd3,
        typename B6 = pins::package::tfbga216::pa7,
        typename B = pins::package::tfbga216::pb47
    >
    struct ltdc0 {
        // peripheral id (e.g ltdc0, ltdc1)
        constexpr static uint32_t id = 0;

        // interrupt id (including the arm vector table)
        constexpr static uint32_t interrupt_id = 16 + 88;

        // error interrupt id (including the arm vector table)
        constexpr static uint32_t error_interrupt_id = 16 + 89;

        // port to the LTDC hardware
        static inline LTDC_Type *const port = LTDC;

        using clk_pins = std::tuple<
            detail::ltdc::ltdc<pins::package::tfbga216::ph3, detail::ltdc::mode::clk, io::detail::alternate::>,
            detail::ltdc::ltdc<pins::package::tfbga216::pj14, detail::ltdc::mode::clk, io::detail::alternate::>,
            detail::ltdc::ltdc<pins::package::tfbga216::pp11, detail::ltdc::mode::clk, io::detail::alternate::>
        >;
        using hsync_pins = std::tuple<
            detail::ltdc::ltdc<pins::package::tfbga216::pd5, detail::ltdc::mode::hsync, io::detail::alternate::>,
            detail::ltdc::ltdc<pins::package::tfbga216::pe3, detail::ltdc::mode::hsync, io::detail::alternate::>,
            detail::ltdc::ltdc<pins::package::tfbga216::ph15, detail::ltdc::mode::hsync, io::detail::alternate::>
        >;
        using vsync_pins = std::tuple<
            detail::ltdc::ltdc<pins::package::tfbga216::pe4, detail::ltdc::mode::vsync, io::detail::alternate::>,
            detail::ltdc::ltdc<pins::package::tfbga216::pg3, detail::ltdc::mode::vsync, io::detail::alternate::>,
            detail::ltdc::ltdc<pins::package::tfbga216::pn4, detail::ltdc::mode::vsync, io::detail::alternate::>
        >;
        using de_pins = std::tuple<
            detail::ltdc::ltdc<pins::package::tfbga216::pc4, detail::ltdc::mode::de, io::detail::alternate::>,
            detail::ltdc::ltdc<pins::package::tfbga216::pl1, detail::ltdc::mode::de, io::detail::alternate::>,
            detail::ltdc::ltdc<pins::package::tfbga216::pr12, detail::ltdc::mode::de, io::detail::alternate::>
        >;

        // red data lines
        using r0_pins = std::tuple<
            detail::ltdc::ltdc<pins::package::tfbga216::pb3, detail::ltdc::mode::r0, io::detail::alternate::>,
            detail::ltdc::ltdc<pins::package::tfbga216::pg4, detail::ltdc::mode::r0, io::detail::alternate::>,
            detail::ltdc::ltdc<pins::package::tfbga216::pk4, detail::ltdc::mode::r0, io::detail::alternate::>
        >;
        using r1_pins = std::tuple<
            detail::ltdc::ltdc<pins::package::tfbga216::pj4, detail::ltdc::mode::r1, io::detail::alternate::>,
            detail::ltdc::ltdc<pins::package::tfbga216::pp2, detail::ltdc::mode::r1, io::detail::alternate::>,
            detail::ltdc::ltdc<pins::package::tfbga216::pr6, detail::ltdc::mode::r1, io::detail::alternate::>
        >;
        using r2_pins = std::tuple<
            detail::ltdc::ltdc<pins::package::tfbga216::pb14, detail::ltdc::mode::r2, io::detail::alternate::>,
            detail::ltdc::ltdc<pins::package::tfbga216::pn2, detail::ltdc::mode::r2, io::detail::alternate::>,
            detail::ltdc::ltdc<pins::package::tfbga216::pp14, detail::ltdc::mode::r2, io::detail::alternate::>,
            detail::ltdc::ltdc<pins::package::tfbga216::pr7, detail::ltdc::mode::r2, io::detail::alternate::>
        >;
        using r3_pins = std::tuple<
            detail::ltdc::ltdc<pins::package::tfbga216::pn14, detail::ltdc::mode::r3, io::detail::alternate::>,
            detail::ltdc::ltdc<pins::package::tfbga216::pp7, detail::ltdc::mode::r3, io::detail::alternate::>,
            detail::ltdc::ltdc<pins::package::tfbga216::pr5, detail::ltdc::mode::r3, io::detail::alternate::>
        >;
        using r4_pins = std::tuple<
            detail::ltdc::ltdc<pins::package::tfbga216::pc15, detail::ltdc::mode::r4, io::detail::alternate::>,
            detail::ltdc::ltdc<pins::package::tfbga216::pn8, detail::ltdc::mode::r4, io::detail::alternate::>,
            detail::ltdc::ltdc<pins::package::tfbga216::pp4, detail::ltdc::mode::r4, io::detail::alternate::>,
            detail::ltdc::ltdc<pins::package::tfbga216::pp15, detail::ltdc::mode::r4, io::detail::alternate::>
        >;
        using r5_pins = std::tuple<
            detail::ltdc::ltdc<pins::package::tfbga216::pb15, detail::ltdc::mode::r5, io::detail::alternate::>,
            detail::ltdc::ltdc<pins::package::tfbga216::pm2, detail::ltdc::mode::r5, io::detail::alternate::>,
            detail::ltdc::ltdc<pins::package::tfbga216::pm9, detail::ltdc::mode::r5, io::detail::alternate::>,
            detail::ltdc::ltdc<pins::package::tfbga216::pn15, detail::ltdc::mode::r5, io::detail::alternate::>
        >;
        using r6_pins = std::tuple<
            detail::ltdc::ltdc<pins::package::tfbga216::pf15, detail::ltdc::mode::r6, io::detail::alternate::>,
            detail::ltdc::ltdc<pins::package::tfbga216::pm14, detail::ltdc::mode::r6, io::detail::alternate::>,
            detail::ltdc::ltdc<pins::package::tfbga216::pm15, detail::ltdc::mode::r6, io::detail::alternate::>,
            detail::ltdc::ltdc<pins::package::tfbga216::pr4, detail::ltdc::mode::r6, io::detail::alternate::>
        >;
        using r7_pins = std::tuple<
            detail::ltdc::ltdc<pins::package::tfbga216::pj15, detail::ltdc::mode::r7, io::detail::alternate::>,
            detail::ltdc::ltdc<pins::package::tfbga216::pr6, detail::ltdc::mode::r7, io::detail::alternate::>,
            detail::ltdc::ltdc<pins::package::tfbga216::pr11, detail::ltdc::mode::r7, io::detail::alternate::>
        >;

        // green data lines
        using g0_pins = std::tuple<
            detail::ltdc::ltdc<pins::package::tfbga216::pb1, detail::ltdc::mode::g0, io::detail::alternate::>,
            detail::ltdc::ltdc<pins::package::tfbga216::pr4, detail::ltdc::mode::g0, io::detail::alternate::>
        >;
        using g1_pins = std::tuple<
            detail::ltdc::ltdc<pins::package::tfbga216::pb2, detail::ltdc::mode::g1, io::detail::alternate::>,
            detail::ltdc::ltdc<pins::package::tfbga216::pr5, detail::ltdc::mode::g1, io::detail::alternate::>
        >;
        using g2_pins = std::tuple<
            detail::ltdc::ltdc<pins::package::tfbga216::pe12, detail::ltdc::mode::g2, io::detail::alternate::>,
            detail::ltdc::ltdc<pins::package::tfbga216::pg4, detail::ltdc::mode::g2, io::detail::alternate::>,
            detail::ltdc::ltdc<pins::package::tfbga216::pp3, detail::ltdc::mode::g2, io::detail::alternate::>
        >;
        using g3_pins = std::tuple<
            detail::ltdc::ltdc<pins::package::tfbga216::pb10, detail::ltdc::mode::g3, io::detail::alternate::>,
            detail::ltdc::ltdc<pins::package::tfbga216::pc8, detail::ltdc::mode::g3, io::detail::alternate::>,
            detail::ltdc::ltdc<pins::package::tfbga216::pe13, detail::ltdc::mode::g3, io::detail::alternate::>,
            detail::ltdc::ltdc<pins::package::tfbga216::pp10, detail::ltdc::mode::g3, io::detail::alternate::>
        >;
        using g4_pins = std::tuple<
            detail::ltdc::ltdc<pins::package::tfbga216::pb9, detail::ltdc::mode::g4, io::detail::alternate::>,
            detail::ltdc::ltdc<pins::package::tfbga216::pd13, detail::ltdc::mode::g4, io::detail::alternate::>,
            detail::ltdc::ltdc<pins::package::tfbga216::ph4, detail::ltdc::mode::g4, io::detail::alternate::>,
            detail::ltdc::ltdc<pins::package::tfbga216::pp12, detail::ltdc::mode::g4, io::detail::alternate::>
        >;
        using g5_pins = std::tuple<
            detail::ltdc::ltdc<pins::package::tfbga216::pe14, detail::ltdc::mode::g5, io::detail::alternate::>,
            detail::ltdc::ltdc<pins::package::tfbga216::ph4, detail::ltdc::mode::g5, io::detail::alternate::>,
            detail::ltdc::ltdc<pins::package::tfbga216::pr13, detail::ltdc::mode::g5, io::detail::alternate::>
        >;
        using g6_pins = std::tuple<
            detail::ltdc::ltdc<pins::package::tfbga216::pd14, detail::ltdc::mode::g6, io::detail::alternate::>,
            detail::ltdc::ltdc<pins::package::tfbga216::pf3, detail::ltdc::mode::g6, io::detail::alternate::>,
            detail::ltdc::ltdc<pins::package::tfbga216::pg15, detail::ltdc::mode::g6, io::detail::alternate::>
        >;
        using g7_pins = std::tuple<
            detail::ltdc::ltdc<pins::package::tfbga216::pa8, detail::ltdc::mode::g7, io::detail::alternate::>,
            detail::ltdc::ltdc<pins::package::tfbga216::pc11, detail::ltdc::mode::g7, io::detail::alternate::>,
            detail::ltdc::ltdc<pins::package::tfbga216::pc14, detail::ltdc::mode::g7, io::detail::alternate::>,
            detail::ltdc::ltdc<pins::package::tfbga216::ph14, detail::ltdc::mode::g7, io::detail::alternate::>
        >;

        // blue data lines
        using b0_pins = std::tuple<
            detail::ltdc::ltdc<pins::package::tfbga216::pa1, detail::ltdc::mode::b0, io::detail::alternate::>,
            detail::ltdc::ltdc<pins::package::tfbga216::pa4, detail::ltdc::mode::b0, io::detail::alternate::>,
            detail::ltdc::ltdc<pins::package::tfbga216::pb10, detail::ltdc::mode::b0, io::detail::alternate::>
        >;
        using b1_pins = std::tuple<
            detail::ltdc::ltdc<pins::package::tfbga216::pb9, detail::ltdc::mode::b1, io::detail::alternate::>,
            detail::ltdc::ltdc<pins::package::tfbga216::pc7, detail::ltdc::mode::b1, io::detail::alternate::>
        >;
        using b2_pins = std::tuple<
            detail::ltdc::ltdc<pins::package::tfbga216::pb11, detail::ltdc::mode::b2, io::detail::alternate::>,
            detail::ltdc::ltdc<pins::package::tfbga216::pc8, detail::ltdc::mode::b2, io::detail::alternate::>,
            detail::ltdc::ltdc<pins::package::tfbga216::pc9, detail::ltdc::mode::b2, io::detail::alternate::>,
            detail::ltdc::ltdc<pins::package::tfbga216::pr2, detail::ltdc::mode::b2, io::detail::alternate::>
        >;
        using b3_pins = std::tuple<
            detail::ltdc::ltdc<pins::package::tfbga216::pb8, detail::ltdc::mode::b3, io::detail::alternate::>,
            detail::ltdc::ltdc<pins::package::tfbga216::pd10, detail::ltdc::mode::b3, io::detail::alternate::>,
            detail::ltdc::ltdc<pins::package::tfbga216::pk15, detail::ltdc::mode::b3, io::detail::alternate::>
        >;
        using b4_pins = std::tuple<
            detail::ltdc::ltdc<pins::package::tfbga216::pc3, detail::ltdc::mode::b4, io::detail::alternate::>,
            detail::ltdc::ltdc<pins::package::tfbga216::pc7, detail::ltdc::mode::b4, io::detail::alternate::>,
            detail::ltdc::ltdc<pins::package::tfbga216::pd8, detail::ltdc::mode::b4, io::detail::alternate::>,
            detail::ltdc::ltdc<pins::package::tfbga216::pr10, detail::ltdc::mode::b4, io::detail::alternate::>
        >;
        using b5_pins = std::tuple<
            detail::ltdc::ltdc<pins::package::tfbga216::pd3, detail::ltdc::mode::b5, io::detail::alternate::>,
            detail::ltdc::ltdc<pins::package::tfbga216::pd7, detail::ltdc::mode::b5, io::detail::alternate::>,
            detail::ltdc::ltdc<pins::package::tfbga216::pr2, detail::ltdc::mode::b5, io::detail::alternate::>
        >;
        using b6_pins = std::tuple<
            detail::ltdc::ltdc<pins::package::tfbga216::pa7, detail::ltdc::mode::b6, io::detail::alternate::>,
            detail::ltdc::ltdc<pins::package::tfbga216::pc6, detail::ltdc::mode::b6, io::detail::alternate::>,
            detail::ltdc::ltdc<pins::package::tfbga216::pd6, detail::ltdc::mode::b6, io::detail::alternate::>
        >;
        using b7_pins = std::tuple<
            detail::ltdc::ltdc<pins::package::tfbga216::pb4, detail::ltdc::mode::b7, io::detail::alternate::>,
            detail::ltdc::ltdc<pins::package::tfbga216::pc5, detail::ltdc::mode::b7, io::detail::alternate::>,
            detail::ltdc::ltdc<pins::package::tfbga216::pd4, detail::ltdc::mode::b7, io::detail::alternate::>
        >;

        // control signals
        using clk = std::tuple_element<klib::io::peripheral::get_index<Clk, clk_pins>(), clk_pins>::type;
        using hsync = std::tuple_element<klib::io::peripheral::get_index<Hsync, hsync_pins>(), hsync_pins>::type;
        using vsync = std::tuple_element<klib::io::peripheral::get_index<Vsync, vsync_pins>(), vsync_pins>::type;
        using de = std::tuple_element<klib::io::peripheral::get_index<De, de_pins>(), de_pins>::type;

        // red data lines
        using r0 = std::tuple_element<klib::io::peripheral::get_index<R0, r0_pins>(), r0_pins>::type;
        using r1 = std::tuple_element<klib::io::peripheral::get_index<R1, r1_pins>(), r1_pins>::type;
        using r2 = std::tuple_element<klib::io::peripheral::get_index<R2, r2_pins>(), r2_pins>::type;
        using r3 = std::tuple_element<klib::io::peripheral::get_index<R3, r3_pins>(), r3_pins>::type;
        using r4 = std::tuple_element<klib::io::peripheral::get_index<R4, r4_pins>(), r4_pins>::type;
        using r5 = std::tuple_element<klib::io::peripheral::get_index<R5, r5_pins>(), r5_pins>::type;
        using r6 = std::tuple_element<klib::io::peripheral::get_index<R6, r6_pins>(), r6_pins>::type;
        using r7 = std::tuple_element<klib::io::peripheral::get_index<R7, r7_pins>(), r7_pins>::type;

        // green data lines
        using g0 = std::tuple_element<klib::io::peripheral::get_index<G0, g0_pins>(), g0_pins>::type;
        using g1 = std::tuple_element<klib::io::peripheral::get_index<G1, g1_pins>(), g1_pins>::type;
        using g2 = std::tuple_element<klib::io::peripheral::get_index<G2, g2_pins>(), g2_pins>::type;
        using g3 = std::tuple_element<klib::io::peripheral::get_index<G3, g3_pins>(), g3_pins>::type;
        using g4 = std::tuple_element<klib::io::peripheral::get_index<G4, g4_pins>(), g4_pins>::type;
        using g5 = std::tuple_element<klib::io::peripheral::get_index<G5, g5_pins>(), g5_pins>::type;
        using g6 = std::tuple_element<klib::io::peripheral::get_index<G6, g6_pins>(), g6_pins>::type;
        using g7 = std::tuple_element<klib::io::peripheral::get_index<G7, g7_pins>(), g7_pins>::type;

        // blue data lines
        using b0 = std::tuple_element<klib::io::peripheral::get_index<B0, b0_pins>(), b0_pins>::type;
        using b1 = std::tuple_element<klib::io::peripheral::get_index<B1, b1_pins>(), b1_pins>::type;
        using b2 = std::tuple_element<klib::io::peripheral::get_index<B2, b2_pins>(), b2_pins>::type;
        using b3 = std::tuple_element<klib::io::peripheral::get_index<B3, b3_pins>(), b3_pins>::type;
        using b4 = std::tuple_element<klib::io::peripheral::get_index<B4, b4_pins>(), b4_pins>::type;
        using b5 = std::tuple_element<klib::io::peripheral::get_index<B5, b5_pins>(), b5_pins>::type;
        using b6 = std::tuple_element<klib::io::peripheral::get_index<B6, b6_pins>(), b6_pins>::type;
        using b7 = std::tuple_element<klib::io::peripheral::get_index<B7, b7_pins>(), b7_pins>::type;
    };
}

#endif
