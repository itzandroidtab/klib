#ifndef KLIB_TMPM373_PINS_HPP
#define KLIB_TMPM373_PINS_HPP

#include <cstdint>

#include "port.hpp"

namespace klib::tmpm373::pins::package::lqfp_48 {
    struct p1 {
        // P3.6
        using port = io::periph::gpio3;

        // bit number in port
        constexpr static uint32_t number = 6;
    };

    struct p2 {
        // P6.0
        using port = io::periph::gpio6;

        // bit number in port
        constexpr static uint32_t number = 0;
    };

    struct p3 {
        // P6.1
        using port = io::periph::gpio6;

        // bit number in port
        constexpr static uint32_t number = 1;
    };

    struct p4 {
        // P6.2
        using port = io::periph::gpio6;

        // bit number in port
        constexpr static uint32_t number = 2;
    };

    struct p5 {
        // P6.3
        using port = io::periph::gpio6;

        // bit number in port
        constexpr static uint32_t number = 3;
    };

    struct p6 {
        // P6.4
        using port = io::periph::gpio6;

        // bit number in port
        constexpr static uint32_t number = 4;
    };

    struct p7 {
        // P6.5
        using port = io::periph::gpio6;

        // bit number in port
        constexpr static uint32_t number = 5;
    };

    struct p8 {
        // P6.6
        using port = io::periph::gpio6;

        // bit number in port
        constexpr static uint32_t number = 6;
    };

    struct p9 {
        // P6.7
        using port = io::periph::gpio6;

        // bit number in port
        constexpr static uint32_t number = 7;
    };

    struct p10 {
        // DVSS
    };

    struct p11 {
        // DVDD5
    };

    struct p12 {
        // P5.0
        using port = io::periph::gpio5;

        // bit number in port
        constexpr static uint32_t number = 0;
    };

    struct p13 {
        // P1.6
        using port = io::periph::gpio1;

        // bit number in port
        constexpr static uint32_t number = 6;
    };

    struct p14 {
        // P1.5
        using port = io::periph::gpio1;

        // bit number in port
        constexpr static uint32_t number = 5;
    };

    struct p15 {
        // P1.4
        using port = io::periph::gpio1;

        // bit number in port
        constexpr static uint32_t number = 4;
    };

    struct p16 {
        // P1.3
        using port = io::periph::gpio1;

        // bit number in port
        constexpr static uint32_t number = 3;
    };

    struct p17 {
        // P4.0
        using port = io::periph::gpio4;

        // bit number in port
        constexpr static uint32_t number = 0;
    };

    struct p18 {
        // P4.1
        using port = io::periph::gpio4;

        // bit number in port
        constexpr static uint32_t number = 1;
    };

    struct p19 {
        // P4.2
        using port = io::periph::gpio4;

        // bit number in port
        constexpr static uint32_t number = 2;
    };

    struct p20 {
        // P4.4
        using port = io::periph::gpio4;

        // bit number in port
        constexpr static uint32_t number = 4;
    };

    struct p21 {
        // P5.2
        using port = io::periph::gpio5;

        // bit number in port
        constexpr static uint32_t number = 2;
    };

    struct p22 {
        // P5.3
        using port = io::periph::gpio5;

        // bit number in port
        constexpr static uint32_t number = 3;
    };

    struct p23 {
        // P5.4
        using port = io::periph::gpio5;

        // bit number in port
        constexpr static uint32_t number = 4;
    };

    struct p24 {
        // P5.1
        using port = io::periph::gpio5;

        // bit number in port
        constexpr static uint32_t number = 1;
    };

    struct p25 {
        // P0.2
        using port = io::periph::gpio0;

        // bit number in port
        constexpr static uint32_t number = 2;
    };

    struct p26 {
        // DVDD5
    };

    struct p27 {
        // P12.0
        using port = io::periph::gpio12;

        // bit number in port
        constexpr static uint32_t number = 0;
    };

    struct p28 {
        // DVSS
    };

    struct p29 {
        // P12.1
        using port = io::periph::gpio12;

        // bit number in port
        constexpr static uint32_t number = 1;
    };

    struct p30 {
        // VOUT15
    };

    struct p31 {
        // MODE
    };

    struct p32 {
        // RVDD5
    };

    struct p33 {
        // VOUT3
    };

    struct p34 {
        // NRESET
    };

    struct p35 {
        // P8.3
        using port = io::periph::gpio8;

        // bit number in port
        constexpr static uint32_t number = 3;
    };

    struct p36 {
        // P9.0
        using port = io::periph::gpio9;

        // bit number in port
        constexpr static uint32_t number = 0;
    };

    struct p37 {
        // P9.5
        using port = io::periph::gpio9;

        // bit number in port
        constexpr static uint32_t number = 5;
    };

    struct p38 {
        // P9.6
        using port = io::periph::gpio9;

        // bit number in port
        constexpr static uint32_t number = 6;
    };

    struct p39 {
        // P9.7
        using port = io::periph::gpio9;

        // bit number in port
        constexpr static uint32_t number = 7;
    };

    struct p40 {
        // P10.0
        using port = io::periph::gpio10;

        // bit number in port
        constexpr static uint32_t number = 0;
    };

    struct p41 {
        // P10.1
        using port = io::periph::gpio10;

        // bit number in port
        constexpr static uint32_t number = 1;
    };

    struct p42 {
        // VREFLB
    };

    struct p43 {
        // VREFHB
    };

    struct p44 {
        // P4.7
        using port = io::periph::gpio4;

        // bit number in port
        constexpr static uint32_t number = 7;
    };

    struct p45 {
        // P4.6
        using port = io::periph::gpio4;

        // bit number in port
        constexpr static uint32_t number = 6;
    };

    struct p46 {
        // P0.6
        using port = io::periph::gpio0;

        // bit number in port
        constexpr static uint32_t number = 6;
    };

    struct p47 {
        // P0.5
        using port = io::periph::gpio0;

        // bit number in port
        constexpr static uint32_t number = 5;
    };

    struct p48 {
        // P0.4
        using port = io::periph::gpio0;

        // bit number in port
        constexpr static uint32_t number = 4;
    };
}

#endif
