#ifndef KLIB_LPC55S66_PINS_HPP
#define KLIB_LPC55S66_PINS_HPP

#include <cstdint>

#include "port.hpp"

namespace klib::lpc55s66::pins::package::hlqfp_100 {
   struct p1 {
        // P1.4
        using port = io::periph::gpio1;

        // bit number in port
        constexpr static uint32_t number = 4;
    };

    struct p2 {
        // P1.13
        using port = io::periph::gpio1;

        // bit number in port
        constexpr static uint32_t number = 13;
    };

    struct p3 {
        // P1.24
        using port = io::periph::gpio1;

        // bit number in port
        constexpr static uint32_t number = 24;
    };

    struct p4 {
        // P1.20
        using port = io::periph::gpio1;

        // bit number in port
        constexpr static uint32_t number = 20;
    };

    struct p5 {
        // P1.6
        using port = io::periph::gpio1;

        // bit number in port
        constexpr static uint32_t number = 6;
    };

    struct p6 {
        // P0.7
        using port = io::periph::gpio0;

        // bit number in port
        constexpr static uint32_t number = 7;
    };

    struct p7 {
        // P0.1
        using port = io::periph::gpio0;

        // bit number in port
        constexpr static uint32_t number = 1;
    };

    struct p8 {
        // P0.17
        using port = io::periph::gpio0;

        // bit number in port
        constexpr static uint32_t number = 17;
    };

    struct p9 {
        // P1.7
        using port = io::periph::gpio1;

        // bit number in port
        constexpr static uint32_t number = 7;
    };

    struct p10 {
        // P1.9
        using port = io::periph::gpio1;

        // bit number in port
        constexpr static uint32_t number = 9;
    };

    struct p11 {
        // P1.0
        using port = io::periph::gpio1;

        // bit number in port
        constexpr static uint32_t number = 0;
    };

    struct p12 {
        // P0.12
        using port = io::periph::gpio0;

        // bit number in port
        constexpr static uint32_t number = 12;
    };

    struct p13 {
        // P0.11
        using port = io::periph::gpio0;

        // bit number in port
        constexpr static uint32_t number = 11;
    };

    struct p14 {
        // P0.16
        using port = io::periph::gpio0;

        // bit number in port
        constexpr static uint32_t number = 16;
    };

    struct p15 {
        // VDD
    };

    struct p16 {
        // VDDA
    };

    struct p17 {
        // VREFP
    };

    struct p18 {
        // VREFN
    };

    struct p19 {
        // VSSA
    };

    struct p20 {
        // P0.23
        using port = io::periph::gpio0;

        // bit number in port
        constexpr static uint32_t number = 23;
    };

    struct p21 {
        // P0.10
        using port = io::periph::gpio0;

        // bit number in port
        constexpr static uint32_t number = 10;
    };

    struct p22 {
        // P0.15
        using port = io::periph::gpio0;

        // bit number in port
        constexpr static uint32_t number = 15;
    };

    struct p23 {
        // P0.31
        using port = io::periph::gpio0;

        // bit number in port
        constexpr static uint32_t number = 31;
    };

    struct p24 {
        // P1.8
        using port = io::periph::gpio1;

        // bit number in port
        constexpr static uint32_t number = 8;
    };

    struct p25 {
        // VDD
    };

    struct p26 {
        // P0.8
        using port = io::periph::gpio0;

        // bit number in port
        constexpr static uint32_t number = 8;
    };

    struct p27 {
        // P0.27
        using port = io::periph::gpio0;

        // bit number in port
        constexpr static uint32_t number = 27;
    };

    struct p28 {
        // XTAL32M_N
    };

    struct p29 {
        // XTAL32M_P
    };

    struct p30 {
        // P1.21
        using port = io::periph::gpio1;

        // bit number in port
        constexpr static uint32_t number = 21;
    };

    struct p31 {
        // P1.5
        using port = io::periph::gpio1;

        // bit number in port
        constexpr static uint32_t number = 5;
    };

    struct p32 {
        // RESETN
    };

    struct p33 {
        // USB1_GND
    };

    struct p34 {
        // USB1_DP
    };

    struct p35 {
        // USB1_DM
    };

    struct p36 {
        // USB1_VBUS
    };

    struct p37 {
        // USB1_GND
    };

    struct p38 {
        // USB1_3V3A
    };

    struct p39 {
        // VDD_PMU
    };

    struct p40 {
        // P1.10
        using port = io::periph::gpio1;

        // bit number in port
        constexpr static uint32_t number = 10;
    };

    struct p41 {
        // P1.22
        using port = io::periph::gpio1;

        // bit number in port
        constexpr static uint32_t number = 22;
    };

    struct p42 {
        // P1.23
        using port = io::periph::gpio1;

        // bit number in port
        constexpr static uint32_t number = 23;
    };

    struct p43 {
        // P1.17
        using port = io::periph::gpio1;

        // bit number in port
        constexpr static uint32_t number = 17;
    };

    struct p44 {
        // VDD
    };

    struct p45 {
        // FB
    };

    struct p46 {
        // VSS_DCDC
    };

    struct p47 {
        // VSS_DCDC
    };

    struct p48 {
        // LX
    };

    struct p49 {
        // VBAT_DCDC
    };

    struct p50 {
        // VBAT_DCDC
    };

    struct p51 {
        // VBAT_PMU
    };

    struct p52 {
        // XTAL32K_P
    };

    struct p53 {
        // XTAL32K_N
    };

    struct p54 {
        // P0.0
        using port = io::periph::gpio0;

        // bit number in port
        constexpr static uint32_t number = 0;
    };

    struct p55 {
        // P0.9
        using port = io::periph::gpio0;

        // bit number in port
        constexpr static uint32_t number = 9;
    };

    struct p56 {
        // P0.18
        using port = io::periph::gpio0;

        // bit number in port
        constexpr static uint32_t number = 18;
    };

    struct p57 {
        // P1.14
        using port = io::periph::gpio1;

        // bit number in port
        constexpr static uint32_t number = 14;
    };

    struct p58 {
        // P1.19
        using port = io::periph::gpio1;

        // bit number in port
        constexpr static uint32_t number = 19;
    };

    struct p59 {
        // P1.1
        using port = io::periph::gpio1;

        // bit number in port
        constexpr static uint32_t number = 1;
    };

    struct p60 {
        // P0.26
        using port = io::periph::gpio0;

        // bit number in port
        constexpr static uint32_t number = 26;
    };

    struct p61 {
        // P1.2
        using port = io::periph::gpio1;

        // bit number in port
        constexpr static uint32_t number = 2;
    };

    struct p62 {
        // P1.3
        using port = io::periph::gpio1;

        // bit number in port
        constexpr static uint32_t number = 3;
    };

    struct p63 {
        // VDD
    };

    struct p64 {
        // P1.18
        using port = io::periph::gpio1;

        // bit number in port
        constexpr static uint32_t number = 18;
    };

    struct p65 {
        // P1.30
        using port = io::periph::gpio1;

        // bit number in port
        constexpr static uint32_t number = 30;
    };

    struct p66 {
        // P0.28
        using port = io::periph::gpio0;

        // bit number in port
        constexpr static uint32_t number = 28;
    };

    struct p67 {
        // P1.12
        using port = io::periph::gpio1;

        // bit number in port
        constexpr static uint32_t number = 12;
    };

    struct p68 {
        // P1.26
        using port = io::periph::gpio1;

        // bit number in port
        constexpr static uint32_t number = 26;
    };

    struct p69 {
        // VDD
    };

    struct p70 {
        // P0.24
        using port = io::periph::gpio0;

        // bit number in port
        constexpr static uint32_t number = 24;
    };

    struct p71 {
        // P0.13
        using port = io::periph::gpio0;

        // bit number in port
        constexpr static uint32_t number = 13;
    };

    struct p72 {
        // P0.14
        using port = io::periph::gpio0;

        // bit number in port
        constexpr static uint32_t number = 14;
    };

    struct p73 {
        // P1.28
        using port = io::periph::gpio1;

        // bit number in port
        constexpr static uint32_t number = 28;
    };

    struct p74 {
        // P0.20
        using port = io::periph::gpio0;

        // bit number in port
        constexpr static uint32_t number = 20;
    };

    struct p75 {
        // VDD
    };

    struct p76 {
        // P0.21
        using port = io::periph::gpio0;

        // bit number in port
        constexpr static uint32_t number = 21;
    };

    struct p77 {
        // P1.25
        using port = io::periph::gpio1;

        // bit number in port
        constexpr static uint32_t number = 25;
    };

    struct p78 {
        // P0.22
        using port = io::periph::gpio0;

        // bit number in port
        constexpr static uint32_t number = 22;
    };

    struct p79 {
        // P0.25
        using port = io::periph::gpio0;

        // bit number in port
        constexpr static uint32_t number = 25;
    };

    struct p80 {
        // P1.29
        using port = io::periph::gpio1;

        // bit number in port
        constexpr static uint32_t number = 29;
    };

    struct p81 {
        // P0.2
        using port = io::periph::gpio0;

        // bit number in port
        constexpr static uint32_t number = 2;
    };

    struct p82 {
        // P1.15
        using port = io::periph::gpio1;

        // bit number in port
        constexpr static uint32_t number = 15;
    };

    struct p83 {
        // P0.3
        using port = io::periph::gpio0;

        // bit number in port
        constexpr static uint32_t number = 3;
    };

    struct p84 {
        // VDD
    };

    struct p85 {
        // P1.27
        using port = io::periph::gpio1;

        // bit number in port
        constexpr static uint32_t number = 27;
    };

    struct p86 {
        // P0.4
        using port = io::periph::gpio0;

        // bit number in port
        constexpr static uint32_t number = 4;
    };

    struct p87 {
        // P1.16
        using port = io::periph::gpio1;

        // bit number in port
        constexpr static uint32_t number = 16;
    };

    struct p88 {
        // P0.5
        using port = io::periph::gpio0;

        // bit number in port
        constexpr static uint32_t number = 5;
    };

    struct p89 {
        // P0.6
        using port = io::periph::gpio0;

        // bit number in port
        constexpr static uint32_t number = 6;
    };

    struct p90 {
        // P0.19
        using port = io::periph::gpio0;

        // bit number in port
        constexpr static uint32_t number = 19;
    };

    struct p91 {
        // P1.31
        using port = io::periph::gpio1;

        // bit number in port
        constexpr static uint32_t number = 31;
    };

    struct p92 {
        // P0.29
        using port = io::periph::gpio0;

        // bit number in port
        constexpr static uint32_t number = 29;
    };

    struct p93 {
        // P1.11
        using port = io::periph::gpio1;

        // bit number in port
        constexpr static uint32_t number = 11;
    };

    struct p94 {
        // P0.30
        using port = io::periph::gpio0;

        // bit number in port
        constexpr static uint32_t number = 30;
    };

    struct p95 {
        // VDD
    };

    struct p96 {
        // USB0_3V3A
    };

    struct p97 {
        // USB0_DP
    };

    struct p98 {
        // USB0_DM
    };

    struct p99 {
        // USB0_GND
    };

    struct p100 {
        // VDD
    };
}

#endif