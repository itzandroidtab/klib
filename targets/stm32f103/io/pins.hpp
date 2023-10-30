#ifndef KLIB_STM32F103_PINS_HPP
#define KLIB_STM32F103_PINS_HPP

#include <cstdint>

#include "port.hpp"

namespace klib::stm32f103::pins::package::lqfp64 {
    struct p1 {
        // VBAT
    };

    struct p2 {
        // P4.13
        using port = io::periph::gpio4;

        // bit number in port
        constexpr static uint32_t number = 13;
    };

    struct p3 {
        // P4.14
        using port = io::periph::gpio4;

        // bit number in port
        constexpr static uint32_t number = 14;
    };

    struct p4 {
        // P4.15
        using port = io::periph::gpio4;

        // bit number in port
        constexpr static uint32_t number = 15;
    };

    struct p5 {
        // OSC_IN
    };

    struct p6 {
        // OSC_OUT
    };

    struct p7 {
        // NRST
    };

    struct p8 {
        // P3.0
        using port = io::periph::gpio3;

        // bit number in port
        constexpr static uint32_t number = 0;
    };

    struct p9 {
        // P3.1
        using port = io::periph::gpio3;

        // bit number in port
        constexpr static uint32_t number = 1;
    };

    struct p10 {
        // P3.2
        using port = io::periph::gpio3;

        // bit number in port
        constexpr static uint32_t number = 2;
    };

    struct p11 {
        // P3.3
        using port = io::periph::gpio3;

        // bit number in port
        constexpr static uint32_t number = 3;
    };

    struct p12 {
        // VSSA
    };

    struct p13 {
        // VDDA
    };

    struct p14 {
        // P0.0
        using port = io::periph::gpio0;

        // bit number in port
        constexpr static uint32_t number = 0;
    };

    struct p15 {
        // P0.1
        using port = io::periph::gpio0;

        // bit number in port
        constexpr static uint32_t number = 1;
    };

    struct p16 {
        // P0.2
        using port = io::periph::gpio0;

        // bit number in port
        constexpr static uint32_t number = 2;
    };

    struct p17 {
        // P0.3
        using port = io::periph::gpio0;

        // bit number in port
        constexpr static uint32_t number = 3;
    };

    struct p18 {
        // VSS
    };

    struct p19 {
        // VDD
    };

    struct p20 {
        // P0.4
        using port = io::periph::gpio0;

        // bit number in port
        constexpr static uint32_t number = 4;
    };

    struct p21 {
        // P0.5
        using port = io::periph::gpio0;

        // bit number in port
        constexpr static uint32_t number = 5;
    };

    struct p22 {
        // P0.6
        using port = io::periph::gpio0;

        // bit number in port
        constexpr static uint32_t number = 6;
    };

    struct p23 {
        // P0.7
        using port = io::periph::gpio0;

        // bit number in port
        constexpr static uint32_t number = 7;
    };

    struct p24 {
        // P2.4
        using port = io::periph::gpio2;

        // bit number in port
        constexpr static uint32_t number = 4;
    };

    struct p25 {
        // P2.5
        using port = io::periph::gpio2;

        // bit number in port
        constexpr static uint32_t number = 5;
    };

    struct p26 {
        // P1.0
        using port = io::periph::gpio1;

        // bit number in port
        constexpr static uint32_t number = 0;
    };

    struct p27 {
        // P1.1
        using port = io::periph::gpio1;

        // bit number in port
        constexpr static uint32_t number = 1;
    };

    struct p28 {
        // P1.2
        using port = io::periph::gpio1;

        // bit number in port
        constexpr static uint32_t number = 2;
    };

    struct p29 {
        // P1.10
        using port = io::periph::gpio1;

        // bit number in port
        constexpr static uint32_t number = 10;
    };

    struct p30 {
        // P1.11
        using port = io::periph::gpio1;

        // bit number in port
        constexpr static uint32_t number = 11;
    };

    struct p31 {
        // VSS
    };

    struct p32 {
        // VDD
    };

    struct p33 {
        // P1.12
        using port = io::periph::gpio1;

        // bit number in port
        constexpr static uint32_t number = 12;
    };

    struct p34 {
        // P1.13
        using port = io::periph::gpio1;

        // bit number in port
        constexpr static uint32_t number = 13;
    };

    struct p35 {
        // P1.14
        using port = io::periph::gpio1;

        // bit number in port
        constexpr static uint32_t number = 14;
    };

    struct p36 {
        // P1.15
        using port = io::periph::gpio1;

        // bit number in port
        constexpr static uint32_t number = 15;
    };

    struct p37 {
        // P2.6
        using port = io::periph::gpio2;

        // bit number in port
        constexpr static uint32_t number = 6;
    };

    struct p38 {
        // P2.7
        using port = io::periph::gpio2;

        // bit number in port
        constexpr static uint32_t number = 7;
    };

    struct p39 {
        // P2.8
        using port = io::periph::gpio2;

        // bit number in port
        constexpr static uint32_t number = 8;
    };

    struct p40 {
        // P2.9
        using port = io::periph::gpio2;

        // bit number in port
        constexpr static uint32_t number = 9;
    };

    struct p41 {
        // P0.8
        using port = io::periph::gpio0;

        // bit number in port
        constexpr static uint32_t number = 8;
    };

    struct p42 {
        // P0.9
        using port = io::periph::gpio0;

        // bit number in port
        constexpr static uint32_t number = 9;
    };

    struct p43 {
        // P0.10
        using port = io::periph::gpio0;

        // bit number in port
        constexpr static uint32_t number = 10;
    };

    struct p44 {
        // P0.11
        using port = io::periph::gpio0;

        // bit number in port
        constexpr static uint32_t number = 11;
    };

    struct p45 {
        // P0.12
        using port = io::periph::gpio0;

        // bit number in port
        constexpr static uint32_t number = 12;
    };

    struct p46 {
        // P0.13
        using port = io::periph::gpio0;

        // bit number in port
        constexpr static uint32_t number = 13;
    };

    struct p47 {
        // VSS
    };

    struct p48 {
        // VDD
    };

    struct p49 {
        // P0.14
        using port = io::periph::gpio0;

        // bit number in port
        constexpr static uint32_t number = 14;
    };

    struct p50 {
        // P0.15
        using port = io::periph::gpio0;

        // bit number in port
        constexpr static uint32_t number = 15;
    };

    struct p51 {
        // P2.10
        using port = io::periph::gpio2;

        // bit number in port
        constexpr static uint32_t number = 10;
    };

    struct p52 {
        // P2.11
        using port = io::periph::gpio2;

        // bit number in port
        constexpr static uint32_t number = 11;
    };

    struct p53 {
        // P2.12
        using port = io::periph::gpio2;

        // bit number in port
        constexpr static uint32_t number = 12;
    };

    struct p54 {
        // P3.2
        using port = io::periph::gpio3;

        // bit number in port
        constexpr static uint32_t number = 2;
    };

    struct p55 {
        // P1.3
        using port = io::periph::gpio1;

        // bit number in port
        constexpr static uint32_t number = 3;
    };

    struct p56 {
        // P1.4
        using port = io::periph::gpio1;

        // bit number in port
        constexpr static uint32_t number = 4;
    };

    struct p57 {
        // P1.5
        using port = io::periph::gpio1;

        // bit number in port
        constexpr static uint32_t number = 5;
    };

    struct p58 {
        // P1.6
        using port = io::periph::gpio1;

        // bit number in port
        constexpr static uint32_t number = 6;
    };

    struct p59 {
        // P1.7
        using port = io::periph::gpio1;

        // bit number in port
        constexpr static uint32_t number = 7;
    };

    struct p60 {
        // BOOT0
    };

    struct p61 {
        // P1.8
        using port = io::periph::gpio1;

        // bit number in port
        constexpr static uint32_t number = 8;
    };

    struct p62 {
        // P1.9
        using port = io::periph::gpio1;

        // bit number in port
        constexpr static uint32_t number = 9;
    };

    struct p63 {
        // VSS
    };

    struct p64 {
        // VDD
    };
}

#endif
