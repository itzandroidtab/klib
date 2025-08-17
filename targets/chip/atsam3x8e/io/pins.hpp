#ifndef KLIB_ATSAM3X8E_PINS_HPP
#define KLIB_ATSAM3X8E_PINS_HPP

#include <cstdint>
#include "port.hpp"

namespace klib::atsam3x8e::pins::package::lqfp_144 {
    struct p1 {
        // P1.26
        using port = io::periph::gpio1;

        // bit number in port
        constexpr static uint32_t number = 26;
    };

    struct p2 {
        // P0.9
        using port = io::periph::gpio0;

        // bit number in port
        constexpr static uint32_t number = 9;
    };

    struct p3 {
        // P0.10
        using port = io::periph::gpio0;

        // bit number in port
        constexpr static uint32_t number = 10;
    };

    struct p4 {
        // P0.11
        using port = io::periph::gpio0;

        // bit number in port
        constexpr static uint32_t number = 11;
    };

    struct p5 {
        // P0.12
        using port = io::periph::gpio0;

        // bit number in port
        constexpr static uint32_t number = 12;
    };

    struct p6 {
        // P0.13
        using port = io::periph::gpio0;

        // bit number in port
        constexpr static uint32_t number = 13;
    };

    struct p7 {
        // P0.14
        using port = io::periph::gpio0;

        // bit number in port
        constexpr static uint32_t number = 14;
    };

    struct p8 {
        // P0.15
        using port = io::periph::gpio0;

        // bit number in port
        constexpr static uint32_t number = 15;
    };

    struct p9 {
        // P0.17
        using port = io::periph::gpio0;

        // bit number in port
        constexpr static uint32_t number = 17;
    };

    struct p10 {
        // VDDCORE
    };

    struct p11 {
        // VDDIO
    };

    struct p12 {
        // GND
    };

    struct p13 {
        // P3.0
        using port = io::periph::gpio3;

        // bit number in port
        constexpr static uint32_t number = 0;
    };

    struct p14 {
        // P3.1
        using port = io::periph::gpio3;

        // bit number in port
        constexpr static uint32_t number = 1;
    };

    struct p15 {
        // P3.2
        using port = io::periph::gpio3;

        // bit number in port
        constexpr static uint32_t number = 2;
    };

    struct p16 {
        // P3.3
        using port = io::periph::gpio3;

        // bit number in port
        constexpr static uint32_t number = 3;
    };

    struct p17 {
        // P3.4
        using port = io::periph::gpio3;

        // bit number in port
        constexpr static uint32_t number = 4;
    };

    struct p18 {
        // P3.5
        using port = io::periph::gpio3;

        // bit number in port
        constexpr static uint32_t number = 5;
    };

    struct p19 {
        // P3.6
        using port = io::periph::gpio3;

        // bit number in port
        constexpr static uint32_t number = 6;
    };

    struct p20 {
        // P3.7
        using port = io::periph::gpio3;

        // bit number in port
        constexpr static uint32_t number = 7;
    };

    struct p21 {
        // P3.8
        using port = io::periph::gpio3;

        // bit number in port
        constexpr static uint32_t number = 8;
    };

    struct p22 {
        // P3.9
        using port = io::periph::gpio3;

        // bit number in port
        constexpr static uint32_t number = 9;
    };

    struct p23 {
        // P0.0
        using port = io::periph::gpio0;

        // bit number in port
        constexpr static uint32_t number = 0;
    };

    struct p24 {
        // P0.1
        using port = io::periph::gpio0;

        // bit number in port
        constexpr static uint32_t number = 1;
    };

    struct p25 {
        // P0.5
        using port = io::periph::gpio0;

        // bit number in port
        constexpr static uint32_t number = 5;
    };

    struct p26 {
        // P0.7
        using port = io::periph::gpio0;

        // bit number in port
        constexpr static uint32_t number = 7;
    };

    struct p27 {
        // P0.8
        using port = io::periph::gpio0;

        // bit number in port
        constexpr static uint32_t number = 8;
    };

    struct p28 {
        // P1.28
        using port = io::periph::gpio1;

        // bit number in port
        constexpr static uint32_t number = 28;
    };

    struct p29 {
        // P1.29
        using port = io::periph::gpio1;

        // bit number in port
        constexpr static uint32_t number = 29;
    };

    struct p30 {
        // P1.30
        using port = io::periph::gpio1;

        // bit number in port
        constexpr static uint32_t number = 30;
    };

    struct p31 {
        // P1.31
        using port = io::periph::gpio1;

        // bit number in port
        constexpr static uint32_t number = 31;
    };

    struct p32 {
        // P3.10
        using port = io::periph::gpio3;

        // bit number in port
        constexpr static uint32_t number = 10;
    };

    struct p33 {
        // GNDPLL
    };

    struct p34 {
        // VDDPLL
    };

    struct p35 {
        // XOUT
    };

    struct p36 {
        // XIN
    };

    struct p37 {
        // DHSDP
    };

    struct p38 {
        // DHSDM
    };

    struct p39 {
        // VBUS
    };

    struct p40 {
        // VBG
    };

    struct p41 {
        // VDDUTMI
    };

    struct p42 {
        // DFSDP
    };

    struct p43 {
        // DFSDM
    };

    struct p44 {
        // GNDUTMI
    };

    struct p45 {
        // VDDCORE
    };

    struct p46 {
        // JTAGSEL
    };

    struct p47 {
        // NRSTB
    };

    struct p48 {
        // XIN32
    };

    struct p49 {
        // XOUT32
    };

    struct p50 {
        // SHDN
    };

    struct p51 {
        // TST
    };

    struct p52 {
        // VDDBU
    };

    struct p53 {
        // FWUP
    };

    struct p54 {
        // GNDBU
    };

    struct p55 {
        // P2.1
        using port = io::periph::gpio2;

        // bit number in port
        constexpr static uint32_t number = 1;
    };

    struct p56 {
        // VDDOUT
    };

    struct p57 {
        // VDDIN
    };

    struct p58 {
        // GND
    };

    struct p59 {
        // P2.2
        using port = io::periph::gpio2;

        // bit number in port
        constexpr static uint32_t number = 2;
    };

    struct p60 {
        // P2.3
        using port = io::periph::gpio2;

        // bit number in port
        constexpr static uint32_t number = 3;
    };

    struct p61 {
        // VDDCORE
    };

    struct p62 {
        // VDDIO
    };

    struct p63 {
        // P2.5
        using port = io::periph::gpio2;

        // bit number in port
        constexpr static uint32_t number = 5;
    };

    struct p64 {
        // P2.6
        using port = io::periph::gpio2;

        // bit number in port
        constexpr static uint32_t number = 6;
    };

    struct p65 {
        // P2.7
        using port = io::periph::gpio2;

        // bit number in port
        constexpr static uint32_t number = 7;
    };

    struct p66 {
        // P2.8
        using port = io::periph::gpio2;

        // bit number in port
        constexpr static uint32_t number = 8;
    };

    struct p67 {
        // P2.9
        using port = io::periph::gpio2;

        // bit number in port
        constexpr static uint32_t number = 9;
    };

    struct p68 {
        // P1.27
        using port = io::periph::gpio1;

        // bit number in port
        constexpr static uint32_t number = 27;
    };

    struct p69 {
        // NRST
    };

    struct p70 {
        // P0.18
        using port = io::periph::gpio0;

        // bit number in port
        constexpr static uint32_t number = 18;
    };

    struct p71 {
        // P0.19
        using port = io::periph::gpio0;

        // bit number in port
        constexpr static uint32_t number = 19;
    };

    struct p72 {
        // P0.20
        using port = io::periph::gpio0;

        // bit number in port
        constexpr static uint32_t number = 20;
    };

    struct p73 {
        // VDDANA
    };

    struct p74 {
        // GNDANA
    };

    struct p75 {
        // ADVREF
    };

    struct p76 {
        // P1.15
        using port = io::periph::gpio1;

        // bit number in port
        constexpr static uint32_t number = 15;
    };

    struct p77 {
        // P1.16
        using port = io::periph::gpio1;

        // bit number in port
        constexpr static uint32_t number = 16;
    };

    struct p78 {
        // P0.16
        using port = io::periph::gpio0;

        // bit number in port
        constexpr static uint32_t number = 16;
    };

    struct p79 {
        // P0.24
        using port = io::periph::gpio0;

        // bit number in port
        constexpr static uint32_t number = 24;
    };

    struct p80 {
        // P0.23
        using port = io::periph::gpio0;

        // bit number in port
        constexpr static uint32_t number = 23;
    };

    struct p81 {
        // P0.22
        using port = io::periph::gpio0;

        // bit number in port
        constexpr static uint32_t number = 22;
    };

    struct p82 {
        // P0.6
        using port = io::periph::gpio0;

        // bit number in port
        constexpr static uint32_t number = 6;
    };

    struct p83 {
        // P0.4
        using port = io::periph::gpio0;

        // bit number in port
        constexpr static uint32_t number = 4;
    };

    struct p84 {
        // P0.3
        using port = io::periph::gpio0;

        // bit number in port
        constexpr static uint32_t number = 3;
    };

    struct p85 {
        // P0.2
        using port = io::periph::gpio0;

        // bit number in port
        constexpr static uint32_t number = 2;
    };

    struct p86 {
        // P1.12
        using port = io::periph::gpio1;

        // bit number in port
        constexpr static uint32_t number = 12;
    };

    struct p87 {
        // P1.13
        using port = io::periph::gpio1;

        // bit number in port
        constexpr static uint32_t number = 13;
    };

    struct p88 {
        // P1.17
        using port = io::periph::gpio1;

        // bit number in port
        constexpr static uint32_t number = 17;
    };

    struct p89 {
        // P1.18
        using port = io::periph::gpio1;

        // bit number in port
        constexpr static uint32_t number = 18;
    };

    struct p90 {
        // P1.19
        using port = io::periph::gpio1;

        // bit number in port
        constexpr static uint32_t number = 19;
    };

    struct p91 {
        // P1.20
        using port = io::periph::gpio1;

        // bit number in port
        constexpr static uint32_t number = 20;
    };

    struct p92 {
        // P1.21
        using port = io::periph::gpio1;

        // bit number in port
        constexpr static uint32_t number = 21;
    };

    struct p93 {
        // P2.11
        using port = io::periph::gpio2;

        // bit number in port
        constexpr static uint32_t number = 11;
    };

    struct p94 {
        // P2.12
        using port = io::periph::gpio2;

        // bit number in port
        constexpr static uint32_t number = 12;
    };

    struct p95 {
        // P2.13
        using port = io::periph::gpio2;

        // bit number in port
        constexpr static uint32_t number = 13;
    };

    struct p96 {
        // P2.14
        using port = io::periph::gpio2;

        // bit number in port
        constexpr static uint32_t number = 14;
    };

    struct p97 {
        // P2.15
        using port = io::periph::gpio2;

        // bit number in port
        constexpr static uint32_t number = 15;
    };

    struct p98 {
        // P2.16
        using port = io::periph::gpio2;

        // bit number in port
        constexpr static uint32_t number = 16;
    };

    struct p99 {
        // P2.17
        using port = io::periph::gpio2;

        // bit number in port
        constexpr static uint32_t number = 17;
    };

    struct p100 {
        // P2.18
        using port = io::periph::gpio2;

        // bit number in port
        constexpr static uint32_t number = 18;
    };

    struct p101 {
        // P2.19
        using port = io::periph::gpio2;

        // bit number in port
        constexpr static uint32_t number = 19;
    };

    struct p102 {
        // P2.29
        using port = io::periph::gpio2;

        // bit number in port
        constexpr static uint32_t number = 29;
    };

    struct p103 {
        // P2.30
        using port = io::periph::gpio2;

        // bit number in port
        constexpr static uint32_t number = 30;
    };

    struct p104 {
        // VDDCORE
    };

    struct p105 {
        // VDDIO
    };

    struct p106 {
        // GND
    };

    struct p107 {
        // P0.21
        using port = io::periph::gpio0;

        // bit number in port
        constexpr static uint32_t number = 21;
    };

    struct p108 {
        // P0.25
        using port = io::periph::gpio0;

        // bit number in port
        constexpr static uint32_t number = 25;
    };

    struct p109 {
        // P0.26
        using port = io::periph::gpio0;

        // bit number in port
        constexpr static uint32_t number = 26;
    };

    struct p110 {
        // P0.27
        using port = io::periph::gpio0;

        // bit number in port
        constexpr static uint32_t number = 27;
    };

    struct p111 {
        // P0.28
        using port = io::periph::gpio0;

        // bit number in port
        constexpr static uint32_t number = 28;
    };

    struct p112 {
        // P0.29
        using port = io::periph::gpio0;

        // bit number in port
        constexpr static uint32_t number = 29;
    };

    struct p113 {
        // P1.0
        using port = io::periph::gpio1;

        // bit number in port
        constexpr static uint32_t number = 0;
    };

    struct p114 {
        // P1.1
        using port = io::periph::gpio1;

        // bit number in port
        constexpr static uint32_t number = 1;
    };

    struct p115 {
        // P1.2
        using port = io::periph::gpio1;

        // bit number in port
        constexpr static uint32_t number = 2;
    };

    struct p116 {
        // P2.4
        using port = io::periph::gpio2;

        // bit number in port
        constexpr static uint32_t number = 4;
    };

    struct p117 {
        // P2.10
        using port = io::periph::gpio2;

        // bit number in port
        constexpr static uint32_t number = 10;
    };

    struct p118 {
        // P1.3
        using port = io::periph::gpio1;

        // bit number in port
        constexpr static uint32_t number = 3;
    };

    struct p119 {
        // P1.4
        using port = io::periph::gpio1;

        // bit number in port
        constexpr static uint32_t number = 4;
    };

    struct p120 {
        // P1.5
        using port = io::periph::gpio1;

        // bit number in port
        constexpr static uint32_t number = 5;
    };

    struct p121 {
        // P1.6
        using port = io::periph::gpio1;

        // bit number in port
        constexpr static uint32_t number = 6;
    };

    struct p122 {
        // P1.7
        using port = io::periph::gpio1;

        // bit number in port
        constexpr static uint32_t number = 7;
    };

    struct p123 {
        // P1.8
        using port = io::periph::gpio1;

        // bit number in port
        constexpr static uint32_t number = 8;
    };

    struct p124 {
        // VDDCORE
    };

    struct p125 {
        // VDDIO
    };

    struct p126 {
        // GND
    };

    struct p127 {
        // P1.9
        using port = io::periph::gpio1;

        // bit number in port
        constexpr static uint32_t number = 9;
    };

    struct p128 {
        // P1.10
        using port = io::periph::gpio1;

        // bit number in port
        constexpr static uint32_t number = 10;
    };

    struct p129 {
        // P1.11
        using port = io::periph::gpio1;

        // bit number in port
        constexpr static uint32_t number = 11;
    };

    struct p130 {
        // P2.0
        using port = io::periph::gpio2;

        // bit number in port
        constexpr static uint32_t number = 0;
    };

    struct p131 {
        // P2.20
        using port = io::periph::gpio2;

        // bit number in port
        constexpr static uint32_t number = 20;
    };

    struct p132 {
        // P2.21
        using port = io::periph::gpio2;

        // bit number in port
        constexpr static uint32_t number = 21;
    };

    struct p133 {
        // P2.22
        using port = io::periph::gpio2;

        // bit number in port
        constexpr static uint32_t number = 22;
    };

    struct p134 {
        // P2.23
        using port = io::periph::gpio2;

        // bit number in port
        constexpr static uint32_t number = 23;
    };

    struct p135 {
        // P2.24
        using port = io::periph::gpio2;

        // bit number in port
        constexpr static uint32_t number = 24;
    };

    struct p136 {
        // P2.25
        using port = io::periph::gpio2;

        // bit number in port
        constexpr static uint32_t number = 25;
    };

    struct p137 {
        // P2.26
        using port = io::periph::gpio2;

        // bit number in port
        constexpr static uint32_t number = 26;
    };

    struct p138 {
        // P2.27
        using port = io::periph::gpio2;

        // bit number in port
        constexpr static uint32_t number = 27;
    };

    struct p139 {
        // P2.28
        using port = io::periph::gpio2;

        // bit number in port
        constexpr static uint32_t number = 28;
    };

    struct p140 {
        // P1.14
        using port = io::periph::gpio1;

        // bit number in port
        constexpr static uint32_t number = 14;
    };

    struct p141 {
        // P1.22
        using port = io::periph::gpio1;

        // bit number in port
        constexpr static uint32_t number = 22;
    };

    struct p142 {
        // P1.23
        using port = io::periph::gpio1;

        // bit number in port
        constexpr static uint32_t number = 23;
    };

    struct p143 {
        // P1.24
        using port = io::periph::gpio1;

        // bit number in port
        constexpr static uint32_t number = 24;
    };

    struct p144 {
        // P1.2
        using port = io::periph::gpio1;

        // bit number in port
        constexpr static uint32_t number = 2;
    };
}

#endif