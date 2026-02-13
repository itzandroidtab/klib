#ifndef KLIB_STM32F469_PINS_HPP
#define KLIB_STM32F469_PINS_HPP

#include <cstdint>
#include "port.hpp"

namespace klib::stm32f469::pins::package::tfbga216 {
    struct pa1 {
        // P4.4
        using port = io::periph::gpio4;

        // bit number in port
        constexpr static uint32_t number = 4;
    };

    struct pa2 {
        // P4.3
        using port = io::periph::gpio4;

        // bit number in port
        constexpr static uint32_t number = 3;
    };

    struct pa3 {
        // P4.2
        using port = io::periph::gpio4;

        // bit number in port
        constexpr static uint32_t number = 2;
    };

    struct pa4 {
        // P6.14
        using port = io::periph::gpio6;

        // bit number in port
        constexpr static uint32_t number = 14;
    };

    struct pa5 {
        // P4.1
        using port = io::periph::gpio4;

        // bit number in port
        constexpr static uint32_t number = 1;
    };

    struct pa6 {
        // P4.0
        using port = io::periph::gpio4;

        // bit number in port
        constexpr static uint32_t number = 0;
    };

    struct pa7 {
        // P1.8
        using port = io::periph::gpio1;

        // bit number in port
        constexpr static uint32_t number = 8;
    };

    struct pa8 {
        // P1.5
        using port = io::periph::gpio1;

        // bit number in port
        constexpr static uint32_t number = 5;
    };

    struct pa9 {
        // P1.4
        using port = io::periph::gpio1;

        // bit number in port
        constexpr static uint32_t number = 4;
    };

    struct pa10 {
        // P1.3
        using port = io::periph::gpio1;

        // bit number in port
        constexpr static uint32_t number = 3;
    };

    struct pa11 {
        // P3.7
        using port = io::periph::gpio3;

        // bit number in port
        constexpr static uint32_t number = 7;
    };

    struct pa12 {
        // P2.12
        using port = io::periph::gpio2;

        // bit number in port
        constexpr static uint32_t number = 12;
    };

    struct pa13 {
        // P0.15
        using port = io::periph::gpio0;

        // bit number in port
        constexpr static uint32_t number = 15;
    };

    struct pa14 {
        // P0.14
        using port = io::periph::gpio0;

        // bit number in port
        constexpr static uint32_t number = 14;
    };

    struct pa15 {
        // P0.13
        using port = io::periph::gpio0;

        // bit number in port
        constexpr static uint32_t number = 13;
    };

    struct pb1 {
        // P4.5
        using port = io::periph::gpio4;

        // bit number in port
        constexpr static uint32_t number = 5;
    };

    struct pb2 {
        // P4.6
        using port = io::periph::gpio4;

        // bit number in port
        constexpr static uint32_t number = 6;
    };

    struct pb3 {
        // P6.13
        using port = io::periph::gpio6;

        // bit number in port
        constexpr static uint32_t number = 13;
    };

    struct pb4 {
        // P1.9
        using port = io::periph::gpio1;

        // bit number in port
        constexpr static uint32_t number = 9;
    };

    struct pb5 {
        // P1.7
        using port = io::periph::gpio1;

        // bit number in port
        constexpr static uint32_t number = 7;
    };

    struct pb6 {
        // P1.6
        using port = io::periph::gpio1;

        // bit number in port
        constexpr static uint32_t number = 6;
    };

    struct pb7 {
        // P6.15
        using port = io::periph::gpio6;

        // bit number in port
        constexpr static uint32_t number = 15;
    };

    struct pb8 {
        // P6.11
        using port = io::periph::gpio6;

        // bit number in port
        constexpr static uint32_t number = 11;
    };

    struct pb9 {
        // P9.13
        using port = io::periph::gpio9;

        // bit number in port
        constexpr static uint32_t number = 13;
    };

    struct pb10 {
        // P9.12
        using port = io::periph::gpio9;

        // bit number in port
        constexpr static uint32_t number = 12;
    };

    struct pb11 {
        // P3.6
        using port = io::periph::gpio3;

        // bit number in port
        constexpr static uint32_t number = 6;
    };

    struct pb12 {
        // P3.0
        using port = io::periph::gpio3;

        // bit number in port
        constexpr static uint32_t number = 0;
    };

    struct pb13 {
        // P2.11
        using port = io::periph::gpio2;

        // bit number in port
        constexpr static uint32_t number = 11;
    };

    struct pb14 {
        // P2.10
        using port = io::periph::gpio2;

        // bit number in port
        constexpr static uint32_t number = 10;
    };

    struct pb15 {
        // P0.12
        using port = io::periph::gpio0;

        // bit number in port
        constexpr static uint32_t number = 12;
    };

    struct pc1 {
        // VBAT
    };

    struct pc2 {
        // P8.8
        using port = io::periph::gpio8;

        // bit number in port
        constexpr static uint32_t number = 8;
    };

    struct pc3 {
        // P8.4
        using port = io::periph::gpio8;

        // bit number in port
        constexpr static uint32_t number = 4;
    };

    struct pc4 {
        // P10.7
        using port = io::periph::gpio10;

        // bit number in port
        constexpr static uint32_t number = 7;
    };

    struct pc5 {
        // P10.6
        using port = io::periph::gpio10;

        // bit number in port
        constexpr static uint32_t number = 6;
    };

    struct pc6 {
        // P10.5
        using port = io::periph::gpio10;

        // bit number in port
        constexpr static uint32_t number = 5;
    };

    struct pc7 {
        // P6.12
        using port = io::periph::gpio6;

        // bit number in port
        constexpr static uint32_t number = 12;
    };

    struct pc8 {
        // P6.10
        using port = io::periph::gpio6;

        // bit number in port
        constexpr static uint32_t number = 10;
    };

    struct pc9 {
        // P9.14
        using port = io::periph::gpio9;

        // bit number in port
        constexpr static uint32_t number = 14;
    };

    struct pc10 {
        // P3.5
        using port = io::periph::gpio3;

        // bit number in port
        constexpr static uint32_t number = 5;
    };

    struct pc11 {
        // P3.3
        using port = io::periph::gpio3;

        // bit number in port
        constexpr static uint32_t number = 3;
    };

    struct pc12 {
        // P3.1
        using port = io::periph::gpio3;

        // bit number in port
        constexpr static uint32_t number = 1;
    };

    struct pc13 {
        // P8.3
        using port = io::periph::gpio8;

        // bit number in port
        constexpr static uint32_t number = 3;
    };

    struct pc14 {
        // P8.2
        using port = io::periph::gpio8;

        // bit number in port
        constexpr static uint32_t number = 2;
    };

    struct pc15 {
        // P0.11
        using port = io::periph::gpio0;

        // bit number in port
        constexpr static uint32_t number = 11;
    };

    struct pd1 {
        // P2.13
        using port = io::periph::gpio2;

        // bit number in port
        constexpr static uint32_t number = 13;
    };

    struct pd2 {
        // P5.0
        using port = io::periph::gpio5;

        // bit number in port
        constexpr static uint32_t number = 0;
    };

    struct pd3 {
        // P8.5
        using port = io::periph::gpio8;

        // bit number in port
        constexpr static uint32_t number = 5;
    };

    struct pd4 {
        // P8.7
        using port = io::periph::gpio8;

        // bit number in port
        constexpr static uint32_t number = 7;
    };

    struct pd5 {
        // P8.10
        using port = io::periph::gpio8;

        // bit number in port
        constexpr static uint32_t number = 10;
    };

    struct pd6 {
        // P8.6
        using port = io::periph::gpio8;

        // bit number in port
        constexpr static uint32_t number = 6;
    };

    struct pd7 {
        // P10.4
        using port = io::periph::gpio10;

        // bit number in port
        constexpr static uint32_t number = 4;
    };

    struct pd8 {
        // P10.3
        using port = io::periph::gpio10;

        // bit number in port
        constexpr static uint32_t number = 3;
    };

    struct pd9 {
        // P6.9
        using port = io::periph::gpio6;

        // bit number in port
        constexpr static uint32_t number = 9;
    };

    struct pd10 {
        // P9.15
        using port = io::periph::gpio9;

        // bit number in port
        constexpr static uint32_t number = 15;
    };

    struct pd11 {
        // P3.4
        using port = io::periph::gpio3;

        // bit number in port
        constexpr static uint32_t number = 4;
    };

    struct pd12 {
        // P3.2
        using port = io::periph::gpio3;

        // bit number in port
        constexpr static uint32_t number = 2;
    };

    struct pd13 {
        // P7.15
        using port = io::periph::gpio7;

        // bit number in port
        constexpr static uint32_t number = 15;
    };

    struct pd14 {
        // P8.1
        using port = io::periph::gpio8;

        // bit number in port
        constexpr static uint32_t number = 1;
    };

    struct pd15 {
        // P0.10
        using port = io::periph::gpio0;

        // bit number in port
        constexpr static uint32_t number = 10;
    };

    struct pe1 {
        // P2.14
        using port = io::periph::gpio2;

        // bit number in port
        constexpr static uint32_t number = 14;
    };

    struct pe2 {
        // P5.1
        using port = io::periph::gpio5;

        // bit number in port
        constexpr static uint32_t number = 1;
    };

    struct pe3 {
        // P8.12
        using port = io::periph::gpio8;

        // bit number in port
        constexpr static uint32_t number = 12;
    };

    struct pe4 {
        // P8.9
        using port = io::periph::gpio8;

        // bit number in port
        constexpr static uint32_t number = 9;
    };

    struct pe5 {
        // PDRON
    };

    struct pe6 {
        // BOOT0
    };

    struct pe7 {
        // VDD
    };

    struct pe8 {
        // VDD
    };

    struct pe9 {
        // VDD
    };

    struct pe10 {
        // VDD
    };

    struct pe11 {
        // VCAP2
    };

    struct pe12 {
        // P7.13
        using port = io::periph::gpio7;

        // bit number in port
        constexpr static uint32_t number = 13;
    };

    struct pe13 {
        // P7.14
        using port = io::periph::gpio7;

        // bit number in port
        constexpr static uint32_t number = 14;
    };

    struct pe14 {
        // P8.0
        using port = io::periph::gpio8;

        // bit number in port
        constexpr static uint32_t number = 0;
    };

    struct pe15 {
        // P0.9
        using port = io::periph::gpio0;

        // bit number in port
        constexpr static uint32_t number = 9;
    };

    struct pf1 {
        // P2.15
        using port = io::periph::gpio2;

        // bit number in port
        constexpr static uint32_t number = 15;
    };

    struct pf2 {
        // VSS
    };

    struct pf3 {
        // P8.11
        using port = io::periph::gpio8;

        // bit number in port
        constexpr static uint32_t number = 11;
    };

    struct pf4 {
        // VDD
    };

    struct pf5 {
        // VDD
    };

    struct pf6 {
        // VSS
    };

    struct pf7 {
        // VSS
    };

    struct pf8 {
        // VSS
    };

    struct pf9 {
        // VSS
    };

    struct pf10 {
        // VSS
    };

    struct pf11 {
        // VDD
    };

    struct pf12 {
        // DSIHOST-D1P
    };

    struct pf13 {
        // DSIHOST-D1N
    };

    struct pf14 {
        // P2.9
        using port = io::periph::gpio2;

        // bit number in port
        constexpr static uint32_t number = 9;
    };

    struct pf15 {
        // P0.8
        using port = io::periph::gpio0;

        // bit number in port
        constexpr static uint32_t number = 8;
    };

    struct pg1 {
        // P7.0
        using port = io::periph::gpio7;

        // bit number in port
        constexpr static uint32_t number = 0;
    };

    struct pg2 {
        // P5.2
        using port = io::periph::gpio5;

        // bit number in port
        constexpr static uint32_t number = 2;
    };

    struct pg3 {
        // P8.13
        using port = io::periph::gpio8;

        // bit number in port
        constexpr static uint32_t number = 13;
    };

    struct pg4 {
        // P8.15
        using port = io::periph::gpio8;

        // bit number in port
        constexpr static uint32_t number = 15;
    };

    struct pg5 {
        // VDD
    };

    struct pg6 {
        // VSS
    };

    struct pg10 {
        // VSS
    };

    struct pg11 {
        // VDDUSB
    };

    struct pg12 {
        // VSSDSI
    };

    struct pg13 {
        // VDD12DSI
    };

    struct pg14 {
        // P2.8
        using port = io::periph::gpio2;

        // bit number in port
        constexpr static uint32_t number = 8;
    };

    struct pg15 {
        // P2.7
        using port = io::periph::gpio2;

        // bit number in port
        constexpr static uint32_t number = 7;
    };

    struct ph1 {
        // P7.1
        using port = io::periph::gpio7;

        // bit number in port
        constexpr static uint32_t number = 1;
    };

    struct ph2 {
        // P5.3
        using port = io::periph::gpio5;

        // bit number in port
        constexpr static uint32_t number = 3;
    };

    struct ph3 {
        // P8.14
        using port = io::periph::gpio8;

        // bit number in port
        constexpr static uint32_t number = 14;
    };

    struct ph4 {
        // P7.4
        using port = io::periph::gpio7;

        // bit number in port
        constexpr static uint32_t number = 4;
    };

    struct ph5 {
        // VDD
    };

    struct ph6 {
        // VSS
    };

    struct ph10 {
        // VSS
    };

    struct ph11 {
        // VDDDSI
    };

    struct ph12 {
        // DSIHOST-CKP
    };

    struct ph13 {
        // DSIHOST-CKN
    };

    struct ph14 {
        // P6.8
        using port = io::periph::gpio6;

        // bit number in port
        constexpr static uint32_t number = 8;
    };

    struct ph15 {
        // P2.6
        using port = io::periph::gpio2;

        // bit number in port
        constexpr static uint32_t number = 6;
    };

    struct pj1 {
        // NRST
    };

    struct pj2 {
        // P5.4
        using port = io::periph::gpio5;

        // bit number in port
        constexpr static uint32_t number = 4;
    };

    struct pj3 {
        // P7.5
        using port = io::periph::gpio7;

        // bit number in port
        constexpr static uint32_t number = 5;
    };

    struct pj4 {
        // P7.3
        using port = io::periph::gpio7;

        // bit number in port
        constexpr static uint32_t number = 3;
    };

    struct pj5 {
        // VDD
    };

    struct pj6 {
        // VSS
    };

    struct pj10 {
        // VSS
    };

    struct pj11 {
        // VDD
    };

    struct pj12 {
        // DSIHOST-D0P
    };

    struct pj13 {
        // DSIHOST-D0N
    };

    struct pj14 {
        // P6.7
        using port = io::periph::gpio6;

        // bit number in port
        constexpr static uint32_t number = 7;
    };

    struct pj15 {
        // P6.6
        using port = io::periph::gpio6;

        // bit number in port
        constexpr static uint32_t number = 6;
    };

    struct pk1 {
        // P5.7
        using port = io::periph::gpio5;

        // bit number in port
        constexpr static uint32_t number = 7;
    };

    struct pk2 {
        // P5.6
        using port = io::periph::gpio5;

        // bit number in port
        constexpr static uint32_t number = 6;
    };

    struct pk3 {
        // P5.5
        using port = io::periph::gpio5;

        // bit number in port
        constexpr static uint32_t number = 5;
    };

    struct pk4 {
        // P7.2
        using port = io::periph::gpio7;

        // bit number in port
        constexpr static uint32_t number = 2;
    };

    struct pk5 {
        // VDD
    };

    struct pk6 {
        // VSS
    };

    struct pk7 {
        // VSS
    };

    struct pk8 {
        // VSS
    };

    struct pk9 {
        // VSS
    };

    struct pk10 {
        // VSS
    };

    struct pk11 {
        // VDD
    };

    struct pk12 {
        // VCAPDSI
    };

    struct pk13 {
        // P3.15
        using port = io::periph::gpio3;

        // bit number in port
        constexpr static uint32_t number = 15;
    };

    struct pk14 {
        // P1.13
        using port = io::periph::gpio1;

        // bit number in port
        constexpr static uint32_t number = 13;
    };

    struct pk15 {
        // P3.10
        using port = io::periph::gpio3;

        // bit number in port
        constexpr static uint32_t number = 10;
    };

    struct pl1 {
        // P5.10
        using port = io::periph::gpio5;

        // bit number in port
        constexpr static uint32_t number = 10;
    };

    struct pl2 {
        // P5.9
        using port = io::periph::gpio5;

        // bit number in port
        constexpr static uint32_t number = 9;
    };

    struct pl3 {
        // P5.8
        using port = io::periph::gpio5;

        // bit number in port
        constexpr static uint32_t number = 8;
    };

    struct pl4 {
        // P2.3
        using port = io::periph::gpio2;

        // bit number in port
        constexpr static uint32_t number = 3;
    };

    struct pl5 {
        // BYPASS-REG
    };

    struct pl6 {
        // VSS
    };

    struct pl7 {
        // VDD
    };

    struct pl8 {
        // VDD
    };

    struct pl9 {
        // VDD
    };

    struct pl10 {
        // VDD
    };

    struct pl11 {
        // VCAP1
    };

    struct pl12 {
        // P3.14
        using port = io::periph::gpio3;

        // bit number in port
        constexpr static uint32_t number = 14;
    };

    struct pl13 {
        // P1.12
        using port = io::periph::gpio1;

        // bit number in port
        constexpr static uint32_t number = 12;
    };

    struct pl14 {
        // P3.9
        using port = io::periph::gpio3;

        // bit number in port
        constexpr static uint32_t number = 9;
    };

    struct pl15 {
        // P3.8
        using port = io::periph::gpio3;

        // bit number in port
        constexpr static uint32_t number = 8;
    };

    struct pm1 {
        // VSSA
    };

    struct pm2 {
        // P2.0
        using port = io::periph::gpio2;

        // bit number in port
        constexpr static uint32_t number = 0;
    };

    struct pm3 {
        // P2.1
        using port = io::periph::gpio2;

        // bit number in port
        constexpr static uint32_t number = 1;
    };

    struct pm4 {
        // P2.2
        using port = io::periph::gpio2;

        // bit number in port
        constexpr static uint32_t number = 2;
    };

    struct pm5 {
        // P1.2
        using port = io::periph::gpio1;

        // bit number in port
        constexpr static uint32_t number = 2;
    };

    struct pm6 {
        // P5.12
        using port = io::periph::gpio5;

        // bit number in port
        constexpr static uint32_t number = 12;
    };

    struct pm7 {
        // P6.1
        using port = io::periph::gpio6;

        // bit number in port
        constexpr static uint32_t number = 1;
    };

    struct pm8 {
        // P5.15
        using port = io::periph::gpio5;

        // bit number in port
        constexpr static uint32_t number = 15;
    };

    struct pm9 {
        // P9.4
        using port = io::periph::gpio9;

        // bit number in port
        constexpr static uint32_t number = 4;
    };

    struct pm10 {
        // P3.12
        using port = io::periph::gpio3;

        // bit number in port
        constexpr static uint32_t number = 12;
    };

    struct pm11 {
        // P3.13
        using port = io::periph::gpio3;

        // bit number in port
        constexpr static uint32_t number = 13;
    };

    struct pm12 {
        // P6.3
        using port = io::periph::gpio6;

        // bit number in port
        constexpr static uint32_t number = 3;
    };

    struct pm13 {
        // P6.2
        using port = io::periph::gpio6;

        // bit number in port
        constexpr static uint32_t number = 2;
    };

    struct pm14 {
        // P9.5
        using port = io::periph::gpio9;

        // bit number in port
        constexpr static uint32_t number = 5;
    };

    struct pm15 {
        // P7.12
        using port = io::periph::gpio7;

        // bit number in port
        constexpr static uint32_t number = 12;
    };

    struct pn1 {
        // VREF-
    };

    struct pn2 {
        // P0.1
        using port = io::periph::gpio0;

        // bit number in port
        constexpr static uint32_t number = 1;
    };

    struct pn3 {
        // P0.0
        using port = io::periph::gpio0;

        // bit number in port
        constexpr static uint32_t number = 0;
    };

    struct pn4 {
        // P0.4
        using port = io::periph::gpio0;

        // bit number in port
        constexpr static uint32_t number = 4;
    };

    struct pn5 {
        // P2.4
        using port = io::periph::gpio2;

        // bit number in port
        constexpr static uint32_t number = 4;
    };

    struct pn6 {
        // P5.13
        using port = io::periph::gpio5;

        // bit number in port
        constexpr static uint32_t number = 13;
    };

    struct pn7 {
        // P6.0
        using port = io::periph::gpio6;

        // bit number in port
        constexpr static uint32_t number = 0;
    };

    struct pn8 {
        // P9.3
        using port = io::periph::gpio9;

        // bit number in port
        constexpr static uint32_t number = 3;
    };

    struct pn9 {
        // P4.8
        using port = io::periph::gpio4;

        // bit number in port
        constexpr static uint32_t number = 8;
    };

    struct pn10 {
        // P3.11
        using port = io::periph::gpio3;

        // bit number in port
        constexpr static uint32_t number = 11;
    };

    struct pn11 {
        // P6.5
        using port = io::periph::gpio6;

        // bit number in port
        constexpr static uint32_t number = 5;
    };

    struct pn12 {
        // P6.4
        using port = io::periph::gpio6;

        // bit number in port
        constexpr static uint32_t number = 4;
    };

    struct pn13 {
        // P7.7
        using port = io::periph::gpio7;

        // bit number in port
        constexpr static uint32_t number = 7;
    };

    struct pn14 {
        // P7.9
        using port = io::periph::gpio7;

        // bit number in port
        constexpr static uint32_t number = 9;
    };

    struct pn15 {
        // P7.11
        using port = io::periph::gpio7;

        // bit number in port
        constexpr static uint32_t number = 11;
    };

    struct pp1 {
        // VREF+
    };

    struct pp2 {
        // P0.2
        using port = io::periph::gpio0;

        // bit number in port
        constexpr static uint32_t number = 2;
    };

    struct pp3 {
        // P0.6
        using port = io::periph::gpio0;

        // bit number in port
        constexpr static uint32_t number = 6;
    };

    struct pp4 {
        // P0.5
        using port = io::periph::gpio0;

        // bit number in port
        constexpr static uint32_t number = 5;
    };

    struct pp5 {
        // P2.5
        using port = io::periph::gpio2;

        // bit number in port
        constexpr static uint32_t number = 5;
    };

    struct pp6 {
        // P5.14
        using port = io::periph::gpio5;

        // bit number in port
        constexpr static uint32_t number = 14;
    };

    struct pp7 {
        // P9.2
        using port = io::periph::gpio9;

        // bit number in port
        constexpr static uint32_t number = 2;
    };

    struct pp8 {
        // P5.11
        using port = io::periph::gpio5;

        // bit number in port
        constexpr static uint32_t number = 11;
    };

    struct pp9 {
        // P4.9
        using port = io::periph::gpio4;

        // bit number in port
        constexpr static uint32_t number = 9;
    };

    struct pp10 {
        // P4.11
        using port = io::periph::gpio4;

        // bit number in port
        constexpr static uint32_t number = 11;
    };

    struct pp11 {
        // P4.14
        using port = io::periph::gpio4;

        // bit number in port
        constexpr static uint32_t number = 14;
    };

    struct pp12 {
        // P1.10
        using port = io::periph::gpio1;

        // bit number in port
        constexpr static uint32_t number = 10;
    };

    struct pp13 {
        // P7.6
        using port = io::periph::gpio7;

        // bit number in port
        constexpr static uint32_t number = 6;
    };

    struct pp14 {
        // P7.8
        using port = io::periph::gpio7;

        // bit number in port
        constexpr static uint32_t number = 8;
    };

    struct pp15 {
        // P7.10
        using port = io::periph::gpio7;

        // bit number in port
        constexpr static uint32_t number = 10;
    };

    struct pr1 {
        // VDDA
    };

    struct pr2 {
        // P0.3
        using port = io::periph::gpio0;

        // bit number in port
        constexpr static uint32_t number = 3;
    };

    struct pr3 {
        // P0.7
        using port = io::periph::gpio0;

        // bit number in port
        constexpr static uint32_t number = 7;
    };

    struct pr4 {
        // P1.1
        using port = io::periph::gpio1;

        // bit number in port
        constexpr static uint32_t number = 1;
    };

    struct pr5 {
        // P1.0
        using port = io::periph::gpio1;

        // bit number in port
        constexpr static uint32_t number = 0;
    };

    struct pr6 {
        // P9.0
        using port = io::periph::gpio9;

        // bit number in port
        constexpr static uint32_t number = 0;
    };

    struct pr7 {
        // P9.1
        using port = io::periph::gpio9;

        // bit number in port
        constexpr static uint32_t number = 1;
    };

    struct pr8 {
        // P4.7
        using port = io::periph::gpio4;

        // bit number in port
        constexpr static uint32_t number = 7;
    };

    struct pr9 {
        // P4.10
        using port = io::periph::gpio4;

        // bit number in port
        constexpr static uint32_t number = 10;
    };

    struct pr10 {
        // P4.12
        using port = io::periph::gpio4;

        // bit number in port
        constexpr static uint32_t number = 12;
    };

    struct pr11 {
        // P4.15
        using port = io::periph::gpio4;

        // bit number in port
        constexpr static uint32_t number = 15;
    };

    struct pr12 {
        // P4.13
        using port = io::periph::gpio4;

        // bit number in port
        constexpr static uint32_t number = 13;
    };

    struct pr13 {
        // P1.11
        using port = io::periph::gpio1;

        // bit number in port
        constexpr static uint32_t number = 11;
    };

    struct pr14 {
        // P1.14
        using port = io::periph::gpio1;

        // bit number in port
        constexpr static uint32_t number = 14;
    };

    struct pr15 {
        // P1.15
        using port = io::periph::gpio1;

        // bit number in port
        constexpr static uint32_t number = 15;
    };
}

#endif
