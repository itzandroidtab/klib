#ifndef KLIB_LPC1754_PINS_HPP
#define KLIB_LPC1754_PINS_HPP

#include <cstdint>

#include "port.hpp"

namespace klib::lpc1754::pins::package::lqfp_80 {
    struct p1 {
        // TDO
    };

    struct p2 {
        // TDI
    };

    struct p3 {
        // TMS
    };

    struct p4 {
        // TRST
    };

    struct p5 {
        // TCK
    };

    struct p6 {
        // P0.26
        using port = io::periph::gpio0;

        // bit number in port
        constexpr static uint32_t number = 26;
    };

    struct p7 {
        // P0.25
        using port = io::periph::gpio0;

        // bit number in port
        constexpr static uint32_t number = 25;
    };

    struct p8 {
        // VDDA
    };

    struct p9 {
        // VSSA
    };

    struct p10 {
        // VREFP
    };

    struct p11 {
        // RSTOUT
    };

    struct p12 {
        // VREFN
    };

    struct p13 {
        // RTCX1
    };

    struct p14 {
        // RSTN
    };

    struct p15 {
        // RTCX2
    };

    struct p16 {
        // VBAT
    };

    struct p17 {
        // P1.31
        using port = io::periph::gpio1;

        // bit number in port
        constexpr static uint32_t number = 31;
    };

    struct p18 {
        // P1.30
        using port = io::periph::gpio1;

        // bit number in port
        constexpr static uint32_t number = 30;
    };

    struct p19 {
        // XTAL1
    };

    struct p20 {
        // XTAL2
    };

    struct p21 {
        // VDD
    };

    struct p22 {
        // P0.29
        using port = io::periph::gpio0;

        // bit number in port
        constexpr static uint32_t number = 29;
    };

    struct p23 {
        // P0.30
        using port = io::periph::gpio0;

        // bit number in port
        constexpr static uint32_t number = 30;
    };

    struct p24 {
        // VSS
    };

    struct p25 {
        // P1.18
        using port = io::periph::gpio1;

        // bit number in port
        constexpr static uint32_t number = 18;
    };

    struct p26 {
        // P1.19
        using port = io::periph::gpio1;

        // bit number in port
        constexpr static uint32_t number = 19;
    };

    struct p27 {
        // P1.20
        using port = io::periph::gpio1;

        // bit number in port
        constexpr static uint32_t number = 20;
    };

    struct p28 {
        // P1.22
        using port = io::periph::gpio1;

        // bit number in port
        constexpr static uint32_t number = 22;
    };

    struct p29 {
        // P1.23
        using port = io::periph::gpio1;

        // bit number in port
        constexpr static uint32_t number = 23;
    };

    struct p30 {
        // P1.24
        using port = io::periph::gpio1;

        // bit number in port
        constexpr static uint32_t number = 24;
    };

    struct p31 {
        // P1.25
        using port = io::periph::gpio1;

        // bit number in port
        constexpr static uint32_t number = 25;
    };

    struct p32 {
        // P1.26
        using port = io::periph::gpio1;

        // bit number in port
        constexpr static uint32_t number = 26;
    };

    struct p33 {
        // VSS
    };

    struct p34 {
        // VDDR
    };

    struct p35 {
        // P1.28
        using port = io::periph::gpio1;

        // bit number in port
        constexpr static uint32_t number = 28;
    };

    struct p36 {
        // P1.29
        using port = io::periph::gpio1;

        // bit number in port
        constexpr static uint32_t number = 29;
    };

    struct p37 {
        // P0.0
        using port = io::periph::gpio0;

        // bit number in port
        constexpr static uint32_t number = 0;
    };

    struct p38 {
        // P0.1
        using port = io::periph::gpio0;

        // bit number in port
        constexpr static uint32_t number = 1;
    };

    struct p39 {
        // P0.10
        using port = io::periph::gpio0;

        // bit number in port
        constexpr static uint32_t number = 10;
    };

    struct p40 {
        // P0.11
        using port = io::periph::gpio0;

        // bit number in port
        constexpr static uint32_t number = 11;
    };

    struct p41 {
        // P2.10
        using port = io::periph::gpio2;

        // bit number in port
        constexpr static uint32_t number = 10;
    };

    struct p42 {
        // VDD
    };

    struct p43 {
        // VSS
    };

    struct p44 {
        // P0.22
        using port = io::periph::gpio0;

        // bit number in port
        constexpr static uint32_t number = 22;
    };

    struct p45 {
        // P0.18
        using port = io::periph::gpio0;

        // bit number in port
        constexpr static uint32_t number = 18;
    };

    struct p46 {
        // P0.17
        using port = io::periph::gpio0;

        // bit number in port
        constexpr static uint32_t number = 17;
    };

    struct p47 {
        // P0.15
        using port = io::periph::gpio0;

        // bit number in port
        constexpr static uint32_t number = 15;
    };

    struct p48 {
        // P0.16
        using port = io::periph::gpio0;

        // bit number in port
        constexpr static uint32_t number = 16;
    };

    struct p49 {
        // P2.9
        using port = io::periph::gpio2;

        // bit number in port
        constexpr static uint32_t number = 9;
    };

    struct p50 {
        // P2.8
        using port = io::periph::gpio2;

        // bit number in port
        constexpr static uint32_t number = 8;
    };

    struct p51 {
        // P2.7
        using port = io::periph::gpio2;

        // bit number in port
        constexpr static uint32_t number = 7;
    };

    struct p52 {
        // P2.6
        using port = io::periph::gpio2;

        // bit number in port
        constexpr static uint32_t number = 6;
    };

    struct p53 {
        // P2.5
        using port = io::periph::gpio2;

        // bit number in port
        constexpr static uint32_t number = 5;
    };

    struct p54 {
        // P2.4
        using port = io::periph::gpio2;

        // bit number in port
        constexpr static uint32_t number = 4;
    };

    struct p55 {
        // P2.3
        using port = io::periph::gpio2;

        // bit number in port
        constexpr static uint32_t number = 3;
    };

    struct p56 {
        // VDD
    };

    struct p57 {
        // VSS
    };

    struct p58 {
        // P2.2
        using port = io::periph::gpio2;

        // bit number in port
        constexpr static uint32_t number = 2;
    };

    struct p59 {
        // P2.1
        using port = io::periph::gpio2;

        // bit number in port
        constexpr static uint32_t number = 1;
    };

    struct p60 {
        // P2.0
        using port = io::periph::gpio2;

        // bit number in port
        constexpr static uint32_t number = 0;
    };

    struct p61 {
        // P0.9
        using port = io::periph::gpio0;

        // bit number in port
        constexpr static uint32_t number = 9;
    };

    struct p62 {
        // P0.8
        using port = io::periph::gpio0;

        // bit number in port
        constexpr static uint32_t number = 8;
    };

    struct p63 {
        // P0.7
        using port = io::periph::gpio0;

        // bit number in port
        constexpr static uint32_t number = 7;
    };

    struct p64 {
        // P0.6
        using port = io::periph::gpio0;

        // bit number in port
        constexpr static uint32_t number = 6;
    };

    struct p65 {
        // P4.28
        using port = io::periph::gpio4;

        // bit number in port
        constexpr static uint32_t number = 28;
    };

    struct p66 {
        // VSS
    };

    struct p67 {
        // VDDR
    };

    struct p68 {
        // P4.29
        using port = io::periph::gpio4;

        // bit number in port
        constexpr static uint32_t number = 29;
    };

    struct p69 {
        // P1.15
        using port = io::periph::gpio1;

        // bit number in port
        constexpr static uint32_t number = 15;
    };

    struct p70 {
        // P1.14
        using port = io::periph::gpio1;

        // bit number in port
        constexpr static uint32_t number = 14;
    };

    struct p71 {
        // P1.10
        using port = io::periph::gpio1;

        // bit number in port
        constexpr static uint32_t number = 10;
    };

    struct p72 {
        // P1.9
        using port = io::periph::gpio1;

        // bit number in port
        constexpr static uint32_t number = 9;
    };

    struct p73 {
        // P1.8
        using port = io::periph::gpio1;

        // bit number in port
        constexpr static uint32_t number = 8;
    };

    struct p74 {
        // P1.4
        using port = io::periph::gpio1;

        // bit number in port
        constexpr static uint32_t number = 4;
    };

    struct p75 {
        // P1.1
        using port = io::periph::gpio1;

        // bit number in port
        constexpr static uint32_t number = 1;
    };

    struct p76 {
        // P1.0
        using port = io::periph::gpio1;

        // bit number in port
        constexpr static uint32_t number = 0;
    };

    struct p77 {
        // VDD
    };

    struct p78 {
        // VSS
    };

    struct p79 {
        // P0.2
        using port = io::periph::gpio0;

        // bit number in port
        constexpr static uint32_t number = 2;
    };

    struct p80 {
        // P0.3
        using port = io::periph::gpio0;

        // bit number in port
        constexpr static uint32_t number = 3;
    };
}

#endif