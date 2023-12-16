#ifndef KLIB_LPC1788_PINS_HPP
#define KLIB_LPC1788_PINS_HPP

#include <cstdint>

#include "port.hpp"

namespace klib::lpc1788::pins::package::lqfp_208 {
    struct p1 {
        // P3.12
        using port = io::periph::gpio3;

        // bit number in port
        constexpr static uint32_t number = 12;
    };

    struct p2 {
        // JTAG_TDO
    };

    struct p3 {
        // P3.3
        using port = io::periph::gpio3;

        // bit number in port
        constexpr static uint32_t number = 3;
    };

    struct p4 {
        // JTAG_TDI
    };

    struct p5 {
        // P3.28
        using port = io::periph::gpio3;

        // bit number in port
        constexpr static uint32_t number = 28;
    };

    struct p6 {
        // JTAG_TMS
    };

    struct p7 {
        // P3.13
        using port = io::periph::gpio3;

        // bit number in port
        constexpr static uint32_t number = 13;
    };

    struct p8 {
        // JTAG_TRSTN
    };

    struct p9 {
        // P5.0
        using port = io::periph::gpio5;

        // bit number in port
        constexpr static uint32_t number = 0;
    };

    struct p10 {
        // JTAG_TCK
    };

    struct p11 {
        // P3.29
        using port = io::periph::gpio3;

        // bit number in port
        constexpr static uint32_t number = 29;
    };

    struct p12 {
        // P0.26
        using port = io::periph::gpio0;

        // bit number in port
        constexpr static uint32_t number = 26;
    };

    struct p13 {
        // P3.4
        using port = io::periph::gpio3;

        // bit number in port
        constexpr static uint32_t number = 4;
    };

    struct p14 {
        // P0.25
        using port = io::periph::gpio0;

        // bit number in port
        constexpr static uint32_t number = 25;
    };

    struct p15 {
        // VDD
    };

    struct p16 {
        // P0.24
        using port = io::periph::gpio0;

        // bit number in port
        constexpr static uint32_t number = 24;
    };

    struct p17 {
        // P3.5
        using port = io::periph::gpio3;

        // bit number in port
        constexpr static uint32_t number = 5;
    };

    struct p18 {
        // P0.23
        using port = io::periph::gpio0;

        // bit number in port
        constexpr static uint32_t number = 23;
    };

    struct p19 {
        // P3.30
        using port = io::periph::gpio3;

        // bit number in port
        constexpr static uint32_t number = 30;
    };

    struct p20 {
        // VDDA
    };

    struct p21 {
        // P3.14
        using port = io::periph::gpio3;

        // bit number in port
        constexpr static uint32_t number = 14;
    };

    struct p22 {
        // VSSA
    };

    struct p23 {
        // P3.6
        using port = io::periph::gpio3;

        // bit number in port
        constexpr static uint32_t number = 6;
    };

    struct p24 {
        // VREFP
    };

    struct p25 {
        // P3.31
        using port = io::periph::gpio3;

        // bit number in port
        constexpr static uint32_t number = 31;
    };

    struct p26 {
        // VDD_REG
    };

    struct p27 {
        // P3.7
        using port = io::periph::gpio3;

        // bit number in port
        constexpr static uint32_t number = 7;
    };

    struct p28 {
        // P3.15
        using port = io::periph::gpio3;

        // bit number in port
        constexpr static uint32_t number = 15;
    };

    struct p29 {
        // RSTOUTN
    };

    struct p30 {
        // P5.1
        using port = io::periph::gpio5;

        // bit number in port
        constexpr static uint32_t number = 1;
    };

    struct p31 {
        // P2.30
        using port = io::periph::gpio2;

        // bit number in port
        constexpr static uint32_t number = 30;
    };

    struct p32 {
        // VSS_REG
    };

    struct p33 {
        // VSS
    };

    struct p34 {
        // RTCX1
    };

    struct p35 {
        // RESETN
    };

    struct p36 {
        // RTCX2
    };

    struct p37 {
        // RTC_ALARM
    };

    struct p38 {
        // VBAT
    };

    struct p39 {
        // P2.31
        using port = io::periph::gpio2;

        // bit number in port
        constexpr static uint32_t number = 31;
    };

    struct p40 {
        // P1.31
        using port = io::periph::gpio1;

        // bit number in port
        constexpr static uint32_t number = 31;
    };

    struct p41 {
        // P0.12
        using port = io::periph::gpio0;

        // bit number in port
        constexpr static uint32_t number = 12;
    };

    struct p42 {
        // P1.30
        using port = io::periph::gpio1;

        // bit number in port
        constexpr static uint32_t number = 30;
    };

    struct p43 {
        // P2.29
        using port = io::periph::gpio2;

        // bit number in port
        constexpr static uint32_t number = 29;
    };

    struct p44 {
        // XTAL1
    };

    struct p45 {
        // P0.13
        using port = io::periph::gpio0;

        // bit number in port
        constexpr static uint32_t number = 13;
    };

    struct p46 {
        // XTAL2
    };

    struct p47 {
        // P2.27
        using port = io::periph::gpio2;

        // bit number in port
        constexpr static uint32_t number = 27;
    };

    struct p48 {
        // P0.28
        using port = io::periph::gpio0;

        // bit number in port
        constexpr static uint32_t number = 28;
    };

    struct p49 {
        // P2.28
        using port = io::periph::gpio2;

        // bit number in port
        constexpr static uint32_t number = 28;
    };

    struct p50 {
        // P0.27
        using port = io::periph::gpio0;

        // bit number in port
        constexpr static uint32_t number = 27;
    };

    struct p51 {
        // P0.31
        using port = io::periph::gpio0;

        // bit number in port
        constexpr static uint32_t number = 31;
    };

    struct p52 {
        // USB_D_MINUS_2
    };

    struct p53 {
        // P2.24
        using port = io::periph::gpio2;

        // bit number in port
        constexpr static uint32_t number = 24;
    };

    struct p54 {
        // P2.25
        using port = io::periph::gpio2;

        // bit number in port
        constexpr static uint32_t number = 25;
    };

    struct p55 {
        // P3.26
        using port = io::periph::gpio3;

        // bit number in port
        constexpr static uint32_t number = 26;
    };

    struct p56 {
        // P3.25
        using port = io::periph::gpio3;

        // bit number in port
        constexpr static uint32_t number = 25;
    };

    struct p57 {
        // P2.26
        using port = io::periph::gpio2;

        // bit number in port
        constexpr static uint32_t number = 26;
    };

    struct p58 {
        // P3.24
        using port = io::periph::gpio3;

        // bit number in port
        constexpr static uint32_t number = 24;
    };

    struct p59 {
        // P2.18
        using port = io::periph::gpio2;

        // bit number in port
        constexpr static uint32_t number = 18;
    };

    struct p60 {
        // VDD
    };

    struct p61 {
        // P0.29
        using port = io::periph::gpio0;

        // bit number in port
        constexpr static uint32_t number = 29;
    };

    struct p62 {
        // P0.30
        using port = io::periph::gpio0;

        // bit number in port
        constexpr static uint32_t number = 30;
    };

    struct p63 {
        // VSS
    };

    struct p64 {
        // P2.23
        using port = io::periph::gpio2;

        // bit number in port
        constexpr static uint32_t number = 23;
    };

    struct p65 {
        // P3.23
        using port = io::periph::gpio3;

        // bit number in port
        constexpr static uint32_t number = 23;
    };

    struct p66 {
        // P1.18
        using port = io::periph::gpio1;

        // bit number in port
        constexpr static uint32_t number = 18;
    };

    struct p67 {
        // P2.19
        using port = io::periph::gpio2;

        // bit number in port
        constexpr static uint32_t number = 19;
    };

    struct p68 {
        // P1.19
        using port = io::periph::gpio1;

        // bit number in port
        constexpr static uint32_t number = 19;
    };

    struct p69 {
        // P0.14
        using port = io::periph::gpio0;

        // bit number in port
        constexpr static uint32_t number = 14;
    };

    struct p70 {
        // P1.20
        using port = io::periph::gpio1;

        // bit number in port
        constexpr static uint32_t number = 20;
    };

    struct p71 {
        // VDD
    };

    struct p72 {
        // P1.21
        using port = io::periph::gpio1;

        // bit number in port
        constexpr static uint32_t number = 21;
    };

    struct p73 {
        // P2.20
        using port = io::periph::gpio2;

        // bit number in port
        constexpr static uint32_t number = 20;
    };

    struct p74 {
        // P1.22
        using port = io::periph::gpio1;

        // bit number in port
        constexpr static uint32_t number = 22;
    };

    struct p75 {
        // P4.0
        using port = io::periph::gpio4;

        // bit number in port
        constexpr static uint32_t number = 0;
    };

    struct p76 {
        // P1.23
        using port = io::periph::gpio1;

        // bit number in port
        constexpr static uint32_t number = 23;
    };

    struct p77 {
        // VSS
    };

    struct p78 {
        // P1.24
        using port = io::periph::gpio1;

        // bit number in port
        constexpr static uint32_t number = 24;
    };

    struct p79 {
        // P4.1
        using port = io::periph::gpio4;

        // bit number in port
        constexpr static uint32_t number = 1;
    };

    struct p80 {
        // P1.25
        using port = io::periph::gpio1;

        // bit number in port
        constexpr static uint32_t number = 25;
    };

    struct p81 {
        // P2.21
        using port = io::periph::gpio2;

        // bit number in port
        constexpr static uint32_t number = 21;
    };

    struct p82 {
        // P1.26
        using port = io::periph::gpio1;

        // bit number in port
        constexpr static uint32_t number = 26;
    };

    struct p83 {
        // P4.2
        using port = io::periph::gpio4;

        // bit number in port
        constexpr static uint32_t number = 2;
    };

    struct p84 {
        // VSS_REG
    };

    struct p85 {
        // P2.22
        using port = io::periph::gpio2;

        // bit number in port
        constexpr static uint32_t number = 22;
    };

    struct p86 {
        // VDD_REG
    };

    struct p87 {
        // P2.16
        using port = io::periph::gpio2;

        // bit number in port
        constexpr static uint32_t number = 16;
    };

    struct p88 {
        // P1.27
        using port = io::periph::gpio1;

        // bit number in port
        constexpr static uint32_t number = 27;
    };

    struct p89 {
        // VDD
    };

    struct p90 {
        // P1.28
        using port = io::periph::gpio1;

        // bit number in port
        constexpr static uint32_t number = 28;
    };

    struct p91 {
        // P2.14
        using port = io::periph::gpio2;

        // bit number in port
        constexpr static uint32_t number = 14;
    };

    struct p92 {
        // P1.29
        using port = io::periph::gpio1;

        // bit number in port
        constexpr static uint32_t number = 29;
    };

    struct p93 {
        // VSS
    };

    struct p94 {
        // P0.0
        using port = io::periph::gpio0;

        // bit number in port
        constexpr static uint32_t number = 0;
    };

    struct p95 {
        // P2.17
        using port = io::periph::gpio2;

        // bit number in port
        constexpr static uint32_t number = 17;
    };

    struct p96 {
        // P0.1
        using port = io::periph::gpio0;

        // bit number in port
        constexpr static uint32_t number = 1;
    };

    struct p97 {
        // P4.3
        using port = io::periph::gpio4;

        // bit number in port
        constexpr static uint32_t number = 3;
    };

    struct p98 {
        // P0.10
        using port = io::periph::gpio0;

        // bit number in port
        constexpr static uint32_t number = 10;
    };

    struct p99 {
        // P2.15
        using port = io::periph::gpio2;

        // bit number in port
        constexpr static uint32_t number = 15;
    };

    struct p100 {
        // P0.11
        using port = io::periph::gpio0;

        // bit number in port
        constexpr static uint32_t number = 11;
    };

    struct p101 {
        // P4.16
        using port = io::periph::gpio4;

        // bit number in port
        constexpr static uint32_t number = 16;
    };

    struct p102 {
        // P2.13
        using port = io::periph::gpio2;

        // bit number in port
        constexpr static uint32_t number = 13;
    };

    struct p103 {
        // P4.4
        using port = io::periph::gpio4;

        // bit number in port
        constexpr static uint32_t number = 4;
    };

    struct p104 {
        // P4.17
        using port = io::periph::gpio4;

        // bit number in port
        constexpr static uint32_t number = 17;
    };

    struct p105 {
        // P4.18
        using port = io::periph::gpio4;

        // bit number in port
        constexpr static uint32_t number = 18;
    };

    struct p106 {
        // P2.12
        using port = io::periph::gpio2;

        // bit number in port
        constexpr static uint32_t number = 12;
    };

    struct p107 {
        // P4.5
        using port = io::periph::gpio4;

        // bit number in port
        constexpr static uint32_t number = 5;
    };

    struct p108 {
        // P2.11
        using port = io::periph::gpio2;

        // bit number in port
        constexpr static uint32_t number = 11;
    };

    struct p109 {
        // P4.20
        using port = io::periph::gpio4;

        // bit number in port
        constexpr static uint32_t number = 20;
    };

    struct p110 {
        // P2.10
        using port = io::periph::gpio2;

        // bit number in port
        constexpr static uint32_t number = 10;
    };

    struct p111 {
        // P4.19
        using port = io::periph::gpio4;

        // bit number in port
        constexpr static uint32_t number = 19;
    };

    struct p112 {
        // VDD
    };

    struct p113 {
        // P4.6
        using port = io::periph::gpio4;

        // bit number in port
        constexpr static uint32_t number = 6;
    };

    struct p114 {
        // VSS
    };

    struct p115 {
        // P4.21
        using port = io::periph::gpio4;

        // bit number in port
        constexpr static uint32_t number = 21;
    };

    struct p116 {
        // P0.22
        using port = io::periph::gpio0;

        // bit number in port
        constexpr static uint32_t number = 22;
    };

    struct p117 {
        // P5.2
        using port = io::periph::gpio5;

        // bit number in port
        constexpr static uint32_t number = 2;
    };

    struct p118 {
        // P0.21
        using port = io::periph::gpio0;

        // bit number in port
        constexpr static uint32_t number = 21;
    };

    struct p119 {
        // P4.26
        using port = io::periph::gpio4;

        // bit number in port
        constexpr static uint32_t number = 26;
    };

    struct p120 {
        // P0.20
        using port = io::periph::gpio0;

        // bit number in port
        constexpr static uint32_t number = 20;
    };

    struct p121 {
        // P4.7
        using port = io::periph::gpio4;

        // bit number in port
        constexpr static uint32_t number = 7;
    };

    struct p122 {
        // P0.19
        using port = io::periph::gpio0;

        // bit number in port
        constexpr static uint32_t number = 19;
    };

    struct p123 {
        // P4.22
        using port = io::periph::gpio4;

        // bit number in port
        constexpr static uint32_t number = 22;
    };

    struct p124 {
        // P0.18
        using port = io::periph::gpio0;

        // bit number in port
        constexpr static uint32_t number = 18;
    };

    struct p125 {
        // VDD
    };

    struct p126 {
        // P0.17
        using port = io::periph::gpio0;

        // bit number in port
        constexpr static uint32_t number = 17;
    };

    struct p127 {
        // P4.8
        using port = io::periph::gpio4;

        // bit number in port
        constexpr static uint32_t number = 8;
    };

    struct p128 {
        // P0.15
        using port = io::periph::gpio0;

        // bit number in port
        constexpr static uint32_t number = 15;
    };

    struct p129 {
        // P4.23
        using port = io::periph::gpio4;

        // bit number in port
        constexpr static uint32_t number = 23;
    };

    struct p130 {
        // P0.16
        using port = io::periph::gpio0;

        // bit number in port
        constexpr static uint32_t number = 16;
    };

    struct p131 {
        // P4.9
        using port = io::periph::gpio4;

        // bit number in port
        constexpr static uint32_t number = 9;
    };

    struct p132 {
        // P2.9
        using port = io::periph::gpio2;

        // bit number in port
        constexpr static uint32_t number = 9;
    };

    struct p133 {
        // VSS
    };

    struct p134 {
        // P2.8
        using port = io::periph::gpio2;

        // bit number in port
        constexpr static uint32_t number = 8;
    };

    struct p135 {
        // P4.10
        using port = io::periph::gpio4;

        // bit number in port
        constexpr static uint32_t number = 10;
    };

    struct p136 {
        // P2.7
        using port = io::periph::gpio2;

        // bit number in port
        constexpr static uint32_t number = 7;
    };

    struct p137 {
        // P3.16
        using port = io::periph::gpio3;

        // bit number in port
        constexpr static uint32_t number = 16;
    };

    struct p138 {
        // P2.6
        using port = io::periph::gpio2;

        // bit number in port
        constexpr static uint32_t number = 6;
    };

    struct p139 {
        // P4.27
        using port = io::periph::gpio4;

        // bit number in port
        constexpr static uint32_t number = 27;
    };

    struct p140 {
        // P2.5
        using port = io::periph::gpio2;

        // bit number in port
        constexpr static uint32_t number = 5;
    };

    struct p141 {
        // P5.3
        using port = io::periph::gpio5;

        // bit number in port
        constexpr static uint32_t number = 3;
    };

    struct p142 {
        // P2.4
        using port = io::periph::gpio2;

        // bit number in port
        constexpr static uint32_t number = 4;
    };

    struct p143 {
        // P3.17
        using port = io::periph::gpio3;

        // bit number in port
        constexpr static uint32_t number = 17;
    };

    struct p144 {
        // P2.3
        using port = io::periph::gpio2;

        // bit number in port
        constexpr static uint32_t number = 3;
    };

    struct p145 {
        // P4.11
        using port = io::periph::gpio4;

        // bit number in port
        constexpr static uint32_t number = 11;
    };

    struct p146 {
        // VDD
    };

    struct p147 {
        // P1.13
        using port = io::periph::gpio1;

        // bit number in port
        constexpr static uint32_t number = 13;
    };

    struct p148 {
        // VSS
    };

    struct p149 {
        // P4.12
        using port = io::periph::gpio4;

        // bit number in port
        constexpr static uint32_t number = 12;
    };

    struct p150 {
        // P2.2
        using port = io::periph::gpio2;

        // bit number in port
        constexpr static uint32_t number = 2;
    };

    struct p151 {
        // P3.18
        using port = io::periph::gpio3;

        // bit number in port
        constexpr static uint32_t number = 18;
    };

    struct p152 {
        // P2.1
        using port = io::periph::gpio2;

        // bit number in port
        constexpr static uint32_t number = 1;
    };

    struct p153 {
        // P1.7
        using port = io::periph::gpio1;

        // bit number in port
        constexpr static uint32_t number = 7;
    };

    struct p154 {
        // P2.0
        using port = io::periph::gpio2;

        // bit number in port
        constexpr static uint32_t number = 0;
    };

    struct p155 {
        // P4.13
        using port = io::periph::gpio4;

        // bit number in port
        constexpr static uint32_t number = 13;
    };

    struct p156 {
        // P1.5
        using port = io::periph::gpio1;

        // bit number in port
        constexpr static uint32_t number = 5;
    };

    struct p157 {
        // P1.12
        using port = io::periph::gpio1;

        // bit number in port
        constexpr static uint32_t number = 12;
    };

    struct p158 {
        // P0.9
        using port = io::periph::gpio0;

        // bit number in port
        constexpr static uint32_t number = 9;
    };

    struct p159 {
        // P4.14
        using port = io::periph::gpio4;

        // bit number in port
        constexpr static uint32_t number = 14;
    };

    struct p160 {
        // P0.8
        using port = io::periph::gpio0;

        // bit number in port
        constexpr static uint32_t number = 8;
    };

    struct p161 {
        // P3.19
        using port = io::periph::gpio3;

        // bit number in port
        constexpr static uint32_t number = 19;
    };

    struct p162 {
        // P0.7
        using port = io::periph::gpio0;

        // bit number in port
        constexpr static uint32_t number = 7;
    };

    struct p163 {
        // P1.11
        using port = io::periph::gpio1;

        // bit number in port
        constexpr static uint32_t number = 11;
    };

    struct p164 {
        // P0.6
        using port = io::periph::gpio0;

        // bit number in port
        constexpr static uint32_t number = 6;
    };

    struct p165 {
        // VDD
    };

    struct p166 {
        // P0.5
        using port = io::periph::gpio0;

        // bit number in port
        constexpr static uint32_t number = 5;
    };

    struct p167 {
        // P3.20
        using port = io::periph::gpio3;

        // bit number in port
        constexpr static uint32_t number = 20;
    };

    struct p168 {
        // P0.4
        using port = io::periph::gpio0;

        // bit number in port
        constexpr static uint32_t number = 4;
    };

    struct p169 {
        // VSS
    };

    struct p170 {
        // P4.28
        using port = io::periph::gpio4;

        // bit number in port
        constexpr static uint32_t number = 28;
    };

    struct p171 {
        // P1.6
        using port = io::periph::gpio1;

        // bit number in port
        constexpr static uint32_t number = 6;
    };

    struct p172 {
        // VSS_REG
    };

    struct p173 {
        // P4.15
        using port = io::periph::gpio4;

        // bit number in port
        constexpr static uint32_t number = 15;
    };

    struct p174 {
        // VDD_REG
    };

    struct p175 {
        // P3.21
        using port = io::periph::gpio3;

        // bit number in port
        constexpr static uint32_t number = 21;
    };

    struct p176 {
        // P4.29
        using port = io::periph::gpio4;

        // bit number in port
        constexpr static uint32_t number = 29;
    };

    struct p177 {
        // P1.3
        using port = io::periph::gpio1;

        // bit number in port
        constexpr static uint32_t number = 3;
    };

    struct p178 {
        // P1.17
        using port = io::periph::gpio1;

        // bit number in port
        constexpr static uint32_t number = 17;
    };

    struct p179 {
        // P4.25
        using port = io::periph::gpio4;

        // bit number in port
        constexpr static uint32_t number = 25;
    };

    struct p180 {
        // P1.16
        using port = io::periph::gpio1;

        // bit number in port
        constexpr static uint32_t number = 16;
    };

    struct p181 {
        // VDD
    };

    struct p182 {
        // P1.15
        using port = io::periph::gpio1;

        // bit number in port
        constexpr static uint32_t number = 15;
    };

    struct p183 {
        // P4.24
        using port = io::periph::gpio4;

        // bit number in port
        constexpr static uint32_t number = 24;
    };

    struct p184 {
        // P1.14
        using port = io::periph::gpio1;

        // bit number in port
        constexpr static uint32_t number = 14;
    };

    struct p185 {
        // P1.2
        using port = io::periph::gpio1;

        // bit number in port
        constexpr static uint32_t number = 2;
    };

    struct p186 {
        // P1.10
        using port = io::periph::gpio1;

        // bit number in port
        constexpr static uint32_t number = 10;
    };

    struct p187 {
        // P4.30
        using port = io::periph::gpio4;

        // bit number in port
        constexpr static uint32_t number = 30;
    };

    struct p188 {
        // P1.9
        using port = io::periph::gpio1;

        // bit number in port
        constexpr static uint32_t number = 9;
    };

    struct p189 {
        // VSS
    };

    struct p190 {
        // P1.8
        using port = io::periph::gpio1;

        // bit number in port
        constexpr static uint32_t number = 8;
    };

    struct p191 {
        // P3.8
        using port = io::periph::gpio3;

        // bit number in port
        constexpr static uint32_t number = 8;
    };

    struct p192 {
        // P1.4
        using port = io::periph::gpio1;

        // bit number in port
        constexpr static uint32_t number = 4;
    };

    struct p193 {
        // P4.31
        using port = io::periph::gpio4;

        // bit number in port
        constexpr static uint32_t number = 31;
    };

    struct p194 {
        // P1.1
        using port = io::periph::gpio1;

        // bit number in port
        constexpr static uint32_t number = 1;
    };

    struct p195 {
        // P3.22
        using port = io::periph::gpio3;

        // bit number in port
        constexpr static uint32_t number = 22;
    };

    struct p196 {
        // P1.0
        using port = io::periph::gpio1;

        // bit number in port
        constexpr static uint32_t number = 0;
    };

    struct p197 {
        // P3.0
        using port = io::periph::gpio3;

        // bit number in port
        constexpr static uint32_t number = 0;
    };

    struct p198 {
        // VDD
    };

    struct p199 {
        // P3.9
        using port = io::periph::gpio3;

        // bit number in port
        constexpr static uint32_t number = 9;
    };

    struct p200 {
        // VSS
    };

    struct p201 {
        // P3.1
        using port = io::periph::gpio3;

        // bit number in port
        constexpr static uint32_t number = 1;
    };

    struct p202 {
        // P0.2
        using port = io::periph::gpio0;

        // bit number in port
        constexpr static uint32_t number = 2;
    };

    struct p203 {
        // P3.27
        using port = io::periph::gpio3;

        // bit number in port
        constexpr static uint32_t number = 27;
    };

    struct p204 {
        // P0.3
        using port = io::periph::gpio0;

        // bit number in port
        constexpr static uint32_t number = 3;
    };

    struct p205 {
        // P3.10
        using port = io::periph::gpio3;

        // bit number in port
        constexpr static uint32_t number = 10;
    };

    struct p206 {
        // P5.4
        using port = io::periph::gpio5;

        // bit number in port
        constexpr static uint32_t number = 4;
    };

    struct p207 {
        // P3.2
        using port = io::periph::gpio3;

        // bit number in port
        constexpr static uint32_t number = 2;
    };

    struct p208 {
        // P3.11
        using port = io::periph::gpio3;

        // bit number in port
        constexpr static uint32_t number = 11;
    };
}

#endif