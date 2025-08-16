#ifndef KLIB_ATSAM4S2B_PINS_HPP
#define KLIB_ATSAM4S2B_PINS_HPP

#include <cstdint>
#include "port.hpp"

namespace klib::atsam4s2b::pins::package::wlcsp_64 {
    struct pa1 {
        // P1.5
        using port = io::periph::gpio1;

        // bit number in port
        constexpr static uint32_t number = 5;
    };

    struct pa2 {
        // P0.31
        using port = io::periph::gpio0;

        // bit number in port
        constexpr static uint32_t number = 31;
    };

    struct pa3 {
        // VDDCORE
    };

    struct pa4 {
        // VDDIO
    };

    struct pa5 {
        // GND
    };

    struct pa6 {
        // P1.8
        using port = io::periph::gpio1;

        // bit number in port
        constexpr static uint32_t number = 8;
    };

    struct pa7 {
        // P1.9
        using port = io::periph::gpio1;

        // bit number in port
        constexpr static uint32_t number = 9;
    };

    struct pa8 {
        // ADVREF
    };

    struct pb1 {
        // P0.1
        using port = io::periph::gpio0;

        // bit number in port
        constexpr static uint32_t number = 1;
    };

    struct pb2 {
        // JTAGSEL
    };

    struct pb3 {
        // P1.10
        using port = io::periph::gpio1;

        // bit number in port
        constexpr static uint32_t number = 10;
    };

    struct pb4 {
        // P1.11
        using port = io::periph::gpio1;

        // bit number in port
        constexpr static uint32_t number = 11;
    };

    struct pb5 {
        // P1.13
        using port = io::periph::gpio1;

        // bit number in port
        constexpr static uint32_t number = 13;
    };

    struct pb6 {
        // VDDPLL
    };

    struct pb7 {
        // P1.14
        using port = io::periph::gpio1;

        // bit number in port
        constexpr static uint32_t number = 14;
    };

    struct pb8 {
        // GNDANA
    };

    struct pc1 {
        // GND
    };

    struct pc2 {
        // P0.0
        using port = io::periph::gpio0;

        // bit number in port
        constexpr static uint32_t number = 0;
    };

    struct pc3 {
        // P1.7
        using port = io::periph::gpio1;

        // bit number in port
        constexpr static uint32_t number = 7;
    };

    struct pc4 {
        // P1.12
        using port = io::periph::gpio1;

        // bit number in port
        constexpr static uint32_t number = 12;
    };

    struct pc5 {
        // P0.10
        using port = io::periph::gpio0;

        // bit number in port
        constexpr static uint32_t number = 10;
    };

    struct pc6 {
        // P1.0
        using port = io::periph::gpio1;

        // bit number in port
        constexpr static uint32_t number = 0;

        // bit number for the adc
        constexpr static uint32_t adc_number = 4;
    };

    struct pc7 {
        // P1.2
        using port = io::periph::gpio1;

        // bit number in port
        constexpr static uint32_t number = 2;

        // bit number for the adc
        constexpr static uint32_t adc_number = 6;
    };

    struct pc8 {
        // P1.1
        using port = io::periph::gpio1;

        // bit number in port
        constexpr static uint32_t number = 1;

        // bit number for the adc
        constexpr static uint32_t adc_number = 5;
    };

    struct pd1 {
        // VDDIO
    };

    struct pd2 {
        // P0.2
        using port = io::periph::gpio0;

        // bit number in port
        constexpr static uint32_t number = 2;
    };

    struct pd3 {
        // P0.28
        using port = io::periph::gpio0;

        // bit number in port
        constexpr static uint32_t number = 28;
    };

    struct pd4 {
        // P1.6
        using port = io::periph::gpio1;

        // bit number in port
        constexpr static uint32_t number = 6;
    };

    struct pd5 {
        // P0.26
        using port = io::periph::gpio0;

        // bit number in port
        constexpr static uint32_t number = 26;
    };

    struct pd6 {
        // P0.23
        using port = io::periph::gpio0;

        // bit number in port
        constexpr static uint32_t number = 23;
    };

    struct pd7 {
        // P0.16
        using port = io::periph::gpio0;

        // bit number in port
        constexpr static uint32_t number = 16;
    };

    struct pd8 {
        // P1.3
        using port = io::periph::gpio1;

        // bit number in port
        constexpr static uint32_t number = 3;

        // bit number for the adc
        constexpr static uint32_t adc_number = 7;
    };

    struct pe1 {
        // P0.3
        using port = io::periph::gpio0;

        // bit number in port
        constexpr static uint32_t number = 3;
    };

    struct pe2 {
        // P0.30
        using port = io::periph::gpio0;

        // bit number in port
        constexpr static uint32_t number = 30;
    };

    struct pe3 {
        // P0.29
        using port = io::periph::gpio0;

        // bit number in port
        constexpr static uint32_t number = 29;
    };

    struct pe4 {
        // P0.27
        using port = io::periph::gpio0;

        // bit number in port
        constexpr static uint32_t number = 27;
    };

    struct pe5 {
        // P0.24
        using port = io::periph::gpio0;

        // bit number in port
        constexpr static uint32_t number = 24;
    };

    struct pe6 {
        // P0.18
        using port = io::periph::gpio0;

        // bit number in port
        constexpr static uint32_t number = 18;

        // bit number for the adc
        constexpr static uint32_t adc_number = 1;
    };

    struct pe7 {
        // P0.17
        using port = io::periph::gpio0;

        // bit number in port
        constexpr static uint32_t number = 17;

        // bit number for the adc
        constexpr static uint32_t adc_number = 0;
    };

    struct pe8 {
        // VDDIN
    };

    struct pf1 {
        // TST
    };

    struct pf2 {
        // NRST
    };

    struct pf3 {
        // P0.5
        using port = io::periph::gpio0;

        // bit number in port
        constexpr static uint32_t number = 5;
    };

    struct pf4 {
        // P0.6
        using port = io::periph::gpio0;

        // bit number in port
        constexpr static uint32_t number = 6;
    };

    struct pf5 {
        // P0.13
        using port = io::periph::gpio0;

        // bit number in port
        constexpr static uint32_t number = 13;
    };

    struct pf6 {
        // P0.22
        using port = io::periph::gpio0;

        // bit number in port
        constexpr static uint32_t number = 22;

        // bit number for the adc
        constexpr static uint32_t adc_number = 9;
    };

    struct pf7 {
        // P0.21
        using port = io::periph::gpio0;

        // bit number in port
        constexpr static uint32_t number = 21;

        // bit number for the adc
        constexpr static uint32_t adc_number = 8;
    };

    struct pf8 {
        // VDDOUT
    };

    struct pg1 {
        // VDDCORE
    };

    struct pg2 {
        // P0.4
        using port = io::periph::gpio0;

        // bit number in port
        constexpr static uint32_t number = 4;
    };

    struct pg3 {
        // P0.9
        using port = io::periph::gpio0;

        // bit number in port
        constexpr static uint32_t number = 9;
    };

    struct pg4 {
        // P0.11
        using port = io::periph::gpio0;

        // bit number in port
        constexpr static uint32_t number = 11;
    };

    struct pg5 {
        // P0.25
        using port = io::periph::gpio0;

        // bit number in port
        constexpr static uint32_t number = 25;
    };

    struct pg6 {
        // P0.14
        using port = io::periph::gpio0;

        // bit number in port
        constexpr static uint32_t number = 14;
    };

    struct pg7 {
        // VDDIO
    };

    struct pg8 {
        // P0.19
        using port = io::periph::gpio0;

        // bit number in port
        constexpr static uint32_t number = 19;

        // bit number for the adc
        constexpr static uint32_t adc_number = 2;
    };

    struct ph1 {
        // P1.4
        using port = io::periph::gpio1;

        // bit number in port
        constexpr static uint32_t number = 4;
    };

    struct ph2 {
        // P0.7
        using port = io::periph::gpio0;

        // bit number in port
        constexpr static uint32_t number = 7;
    };

    struct ph3 {
        // P0.8
        using port = io::periph::gpio0;

        // bit number in port
        constexpr static uint32_t number = 8;
    };

    struct ph4 {
        // P0.12
        using port = io::periph::gpio0;

        // bit number in port
        constexpr static uint32_t number = 12;
    };

    struct ph5 {
        // VDDCORE
    };

    struct ph6 {
        // P0.15
        using port = io::periph::gpio0;

        // bit number in port
        constexpr static uint32_t number = 15;
    };

    struct ph7 {
        // GND
    };

    struct ph8 {
        // P0.20
        using port = io::periph::gpio0;

        // bit number in port
        constexpr static uint32_t number = 20;

        // bit number for the adc
        constexpr static uint32_t adc_number = 3;
    };
}

#endif