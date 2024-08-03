#ifndef KLIB_MB9BF566K_PINS_HPP
#define KLIB_MB9BF566K_PINS_HPP

#include <cstdint>

#include "port.hpp"

namespace klib::mb9bf566k::pins::package::lqfp_48 {
    struct p1 {
        // VCC
    };

    struct p2 {
        // P5.4
        using port = io::periph::gpio5;

        // bit number in port
        constexpr static uint32_t number = 4;
    };

    struct p3 {
        // P5.5
        using port = io::periph::gpio5;

        // bit number in port
        constexpr static uint32_t number = 5;
    };

    struct p4 {
        // P5.6
        using port = io::periph::gpio5;

        // bit number in port
        constexpr static uint32_t number = 6;
    };

    struct p5 {
        // P5.7
        using port = io::periph::gpio5;

        // bit number in port
        constexpr static uint32_t number = 7;
    };

    struct p6 {
        // P3.0
        using port = io::periph::gpio3;

        // bit number in port
        constexpr static uint32_t number = 0;
    };

    struct p7 {
        // P3.1
        using port = io::periph::gpio3;

        // bit number in port
        constexpr static uint32_t number = 1;
    };

    struct p8 {
        // P3.2
        using port = io::periph::gpio3;

        // bit number in port
        constexpr static uint32_t number = 2;
    };

    struct p9 {
        // P3.3
        using port = io::periph::gpio3;

        // bit number in port
        constexpr static uint32_t number = 3;
    };

    struct p10 {
        // P3.4
        using port = io::periph::gpio3;

        // bit number in port
        constexpr static uint32_t number = 4;
    };

    struct p11 {
        // P3.5
        using port = io::periph::gpio3;

        // bit number in port
        constexpr static uint32_t number = 5;
    };

    struct p12 {
        // VSS
    };

    struct p13 {
        // P4.6
        using port = io::periph::gpio4;

        // bit number in port
        constexpr static uint32_t number = 6;
    };

    struct p14 {
        // P4.7
        using port = io::periph::gpio4;

        // bit number in port
        constexpr static uint32_t number = 7;
    };

    struct p15 {
        // VCC
    };

    struct p16 {
        // INTX
    };

    struct p17 {
        // C
    };

    struct p18 {
        // VSS
    };

    struct p19 {
        // VCC
    };

    struct p20 {
        // P14.0
        using port = io::periph::gpio14;

        // bit number in port
        constexpr static uint32_t number = 0;
    };

    struct p21 {
        // MD0
    };

    struct p22 {
        // P14.2
        using port = io::periph::gpio14;

        // bit number in port
        constexpr static uint32_t number = 2;
    };

    struct p23 {
        // P14.3
        using port = io::periph::gpio14;

        // bit number in port
        constexpr static uint32_t number = 3;
    };

    struct p24 {
        // VSS
    };

    struct p25 {
        // P1.0
        using port = io::periph::gpio1;

        // bit number in port
        constexpr static uint32_t number = 0;
    };

    struct p26 {
        // P1.1
        using port = io::periph::gpio1;

        // bit number in port
        constexpr static uint32_t number = 1;
    };

    struct p27 {
        // P1.2
        using port = io::periph::gpio1;

        // bit number in port
        constexpr static uint32_t number = 2;
    };

    struct p28 {
        // P1.3
        using port = io::periph::gpio1;

        // bit number in port
        constexpr static uint32_t number = 3;
    };

    struct p29 {
        // P2.0
        using port = io::periph::gpio2;

        // bit number in port
        constexpr static uint32_t number = 0;
    };

    struct p30 {
        // AVCC
    };

    struct p31 {
        // AVSS
    };

    struct p32 {
        // AVRL
    };

    struct p33 {
        // AVRH
    };

    struct p34 {
        // P2.1
        using port = io::periph::gpio2;

        // bit number in port
        constexpr static uint32_t number = 1;
    };

    struct p35 {
        // P2.2
        using port = io::periph::gpio2;

        // bit number in port
        constexpr static uint32_t number = 2;
    };

    struct p36 {
        // P2.3
        using port = io::periph::gpio2;

        // bit number in port
        constexpr static uint32_t number = 3;
    };

    struct p37 {
        // P0.4
        using port = io::periph::gpio0;

        // bit number in port
        constexpr static uint32_t number = 4;
    };

    struct p38 {
        // P0.3
        using port = io::periph::gpio0;

        // bit number in port
        constexpr static uint32_t number = 3;
    };

    struct p39 {
        // P0.2
        using port = io::periph::gpio0;

        // bit number in port
        constexpr static uint32_t number = 2;
    };

    struct p40 {
        // P0.1
        using port = io::periph::gpio0;

        // bit number in port
        constexpr static uint32_t number = 1;
    };

    struct p41 {
        // P0.0
        using port = io::periph::gpio0;

        // bit number in port
        constexpr static uint32_t number = 0;
    };

    struct p42 {
        // P6.6
        using port = io::periph::gpio6;

        // bit number in port
        constexpr static uint32_t number = 6;
    };

    struct p43 {
        // P6.1
        using port = io::periph::gpio6;

        // bit number in port
        constexpr static uint32_t number = 1;
    };

    struct p44 {
        // P6.0
        using port = io::periph::gpio6;

        // bit number in port
        constexpr static uint32_t number = 0;
    };

    struct p45 {
        // USBVCC
    };

    struct p46 {
        // P8.0
        using port = io::periph::gpio8;

        // bit number in port
        constexpr static uint32_t number = 0;
    };

    struct p47 {
        // P8.1
        using port = io::periph::gpio8;

        // bit number in port
        constexpr static uint32_t number = 1;
    };

    struct p48 {
        // VSS
    };
}

#endif