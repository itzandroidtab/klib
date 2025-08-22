#ifndef KLIB_MB9BF566K_PINS_HPP
#define KLIB_MB9BF566K_PINS_HPP

#include <cstdint>
#include <tuple>

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

        // all the alternate functions of this pin
        using alternate = std::tuple<
            klib::core::mb9bf560l::io::detail::alternate::multi_function_timer::ic02s<0b001>,
            klib::core::mb9bf560l::io::detail::alternate::external_interrupt::eint04s<0b01>,
            klib::core::mb9bf560l::io::detail::alternate::base_timer::tioa2e<0b01>
        >;
    };

    struct p3 {
        // P5.5
        using port = io::periph::gpio5;

        // bit number in port
        constexpr static uint32_t number = 5;

        // all the alternate functions of this pin
        using alternate = std::tuple<
            klib::core::mb9bf560l::io::detail::alternate::multi_function_timer::ic03s<0b001>,
            klib::core::mb9bf560l::io::detail::alternate::external_interrupt::eint05s<0b01>,
            klib::core::mb9bf560l::io::detail::alternate::base_timer::tiob2s<0b001>,
            klib::core::mb9bf560l::io::detail::alternate::multi_functional_serial::sin3s<0b01>
        >;
    };

    struct p4 {
        // P5.6
        using port = io::periph::gpio5;

        // bit number in port
        constexpr static uint32_t number = 6;

        // all the alternate functions of this pin
        using alternate = std::tuple<
            klib::core::mb9bf560l::io::detail::alternate::multi_function_timer::frck0s<0b01>,
            klib::core::mb9bf560l::io::detail::alternate::external_interrupt::eint06s<0b01>,
            klib::core::mb9bf560l::io::detail::alternate::base_timer::tioa3e<0b01>,
            klib::core::mb9bf560l::io::detail::alternate::base_timer::tioa3s<0b01>,
            klib::core::mb9bf560l::io::detail::alternate::multi_functional_serial::sot3b<0b01>
        >;
    };

    struct p5 {
        // P5.7
        using port = io::periph::gpio5;

        // bit number in port
        constexpr static uint32_t number = 7;

        // all the alternate functions of this pin
        using alternate = std::tuple<
            klib::core::mb9bf560l::io::detail::alternate::multi_function_timer::dtti0s<0b01>,
            klib::core::mb9bf560l::io::detail::alternate::external_interrupt::eint07s<0b01>,
            klib::core::mb9bf560l::io::detail::alternate::base_timer::tiob3s<0b01>,
            klib::core::mb9bf560l::io::detail::alternate::multi_functional_serial::sck3b<0b01>,
            klib::core::mb9bf560l::io::detail::alternate::can_adc_trigger_qprc::adtrg0s<0b0001>,
            klib::core::mb9bf560l::io::detail::alternate::can_adc_trigger_qprc::adtrg1s<0b0001>,
            klib::core::mb9bf560l::io::detail::alternate::can_adc_trigger_qprc::adtrg2s<0b0001>
        >;
    };

    struct p6 {
        // P3.0
        using port = io::periph::gpio3;

        // bit number in port
        constexpr static uint32_t number = 0;

        // all the alternate functions of this pin
        using alternate = std::tuple<
            klib::core::mb9bf560l::io::detail::alternate::multi_function_timer::rto00e<0b01>,
            klib::core::mb9bf560l::io::detail::alternate::can_adc_trigger_qprc::qain0s<0b10>,
            klib::core::mb9bf560l::io::detail::alternate::external_interrupt::eint08s<0b01>,
            klib::core::mb9bf560l::io::detail::alternate::base_timer::tioa4e<0b01>,
            klib::core::mb9bf560l::io::detail::alternate::multi_functional_serial::sin2s<0b01>
        >;
    };

    struct p7 {
        // P3.1
        using port = io::periph::gpio3;

        // bit number in port
        constexpr static uint32_t number = 1;

        // all the alternate functions of this pin
        using alternate = std::tuple<
            klib::core::mb9bf560l::io::detail::alternate::multi_function_timer::rto01e<0b01>,
            klib::core::mb9bf560l::io::detail::alternate::can_adc_trigger_qprc::qbin0s<0b10>,
            klib::core::mb9bf560l::io::detail::alternate::external_interrupt::eint09s<0b01>,
            klib::core::mb9bf560l::io::detail::alternate::base_timer::tiob4s<0b01>,
            klib::core::mb9bf560l::io::detail::alternate::multi_functional_serial::sot2b<0b01>
        >;
    };

    struct p8 {
        // P3.2
        using port = io::periph::gpio3;

        // bit number in port
        constexpr static uint32_t number = 2;

        // all the alternate functions of this pin
        using alternate = std::tuple<
            klib::core::mb9bf560l::io::detail::alternate::multi_function_timer::rto02e<0b01>,
            klib::core::mb9bf560l::io::detail::alternate::can_adc_trigger_qprc::qzin0s<0b10>,
            klib::core::mb9bf560l::io::detail::alternate::external_interrupt::eint10s<0b01>,
            klib::core::mb9bf560l::io::detail::alternate::base_timer::tioa5e<0b01>,
            klib::core::mb9bf560l::io::detail::alternate::base_timer::tioa5s<0b01>,
            klib::core::mb9bf560l::io::detail::alternate::multi_functional_serial::sck2b<0b01>
        >;
    };

    struct p9 {
        // P3.3
        using port = io::periph::gpio3;

        // bit number in port
        constexpr static uint32_t number = 3;

        // all the alternate functions of this pin
        using alternate = std::tuple<
            klib::core::mb9bf560l::io::detail::alternate::multi_function_timer::rto03e<0b01>,
            klib::core::mb9bf560l::io::detail::alternate::external_interrupt::eint11s<0b01>,
            klib::core::mb9bf560l::io::detail::alternate::base_timer::tiob5s<0b01>,
            klib::core::mb9bf560l::io::detail::alternate::multi_functional_serial::sin4s<0b10>
        >;
    };

    struct p10 {
        // P3.4
        using port = io::periph::gpio3;

        // bit number in port
        constexpr static uint32_t number = 4;

        // all the alternate functions of this pin
        using alternate = std::tuple<
            klib::core::mb9bf560l::io::detail::alternate::multi_function_timer::rto04e<0b01>,
            klib::core::mb9bf560l::io::detail::alternate::external_interrupt::eint12s<0b01>,
            klib::core::mb9bf560l::io::detail::alternate::base_timer::tioa6e<0b01>,
            klib::core::mb9bf560l::io::detail::alternate::multi_functional_serial::sot4b<0b10>
        >;
    };

    struct p11 {
        // P3.5
        using port = io::periph::gpio3;

        // bit number in port
        constexpr static uint32_t number = 5;

        // all the alternate functions of this pin
        using alternate = std::tuple<
            klib::core::mb9bf560l::io::detail::alternate::multi_function_timer::rto05e<0b01>,
            klib::core::mb9bf560l::io::detail::alternate::external_interrupt::eint13s<0b01>,
            klib::core::mb9bf560l::io::detail::alternate::base_timer::tiob6s<0b01>,
            klib::core::mb9bf560l::io::detail::alternate::multi_functional_serial::sck4b<0b10>
        >;
    };

    struct p12 {
        // VSS
    };

    struct p13 {
        // P4.6
        using port = io::periph::gpio4;

        // bit number in port
        constexpr static uint32_t number = 6;

        // all the alternate functions of this pin
        using alternate = std::tuple<
            klib::core::mb9bf560l::io::detail::alternate::special::subxc<0b01>,
            klib::core::mb9bf560l::io::detail::alternate::special::subxc<0b11>
        >;
    };

    struct p14 {
        // P4.7
        using port = io::periph::gpio4;

        // bit number in port
        constexpr static uint32_t number = 7;

        // all the alternate functions of this pin
        using alternate = std::tuple<
            klib::core::mb9bf560l::io::detail::alternate::special::subxc<0b01>
        >;
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

        // all the alternate functions of this pin
        using alternate = std::tuple<
            klib::core::mb9bf560l::io::detail::alternate::special::mainxc<0b01>,
            klib::core::mb9bf560l::io::detail::alternate::special::mainxc<0b11>
        >;
    };

    struct p23 {
        // P14.3
        using port = io::periph::gpio14;

        // bit number in port
        constexpr static uint32_t number = 3;

        // all the alternate functions of this pin
        using alternate = std::tuple<
            klib::core::mb9bf560l::io::detail::alternate::special::mainxc<0b01>
        >;
    };

    struct p24 {
        // VSS
    };

    struct p25 {
        // P1.0
        using port = io::periph::gpio1;

        // bit number in port
        constexpr static uint32_t number = 0;

        // all the alternate functions of this pin
        using alternate = std::tuple<
            klib::core::mb9bf560l::io::detail::alternate::base_timer::tioa4e<0b10>,
            klib::core::mb9bf560l::io::detail::alternate::external_interrupt::eint00s<0b10>,
            klib::core::mb9bf560l::io::detail::alternate::can_adc_trigger_qprc::qain0s<0b11>,
            klib::core::mb9bf560l::io::detail::alternate::multi_function_timer::ic03s<0b010>
        >;
    };

    struct p26 {
        // P1.1
        using port = io::periph::gpio1;

        // bit number in port
        constexpr static uint32_t number = 1;

        // all the alternate functions of this pin
        using alternate = std::tuple<
            klib::core::mb9bf560l::io::detail::alternate::can_adc_trigger_qprc::adtrg0s<0b0011>,
            klib::core::mb9bf560l::io::detail::alternate::can_adc_trigger_qprc::adtrg1s<0b0011>,
            klib::core::mb9bf560l::io::detail::alternate::can_adc_trigger_qprc::adtrg2s<0b0011>,
            klib::core::mb9bf560l::io::detail::alternate::multi_functional_serial::scs6b<0b01>,
            klib::core::mb9bf560l::io::detail::alternate::base_timer::tiob4s<0b10>,
            klib::core::mb9bf560l::io::detail::alternate::external_interrupt::eint01s<0b10>,
            klib::core::mb9bf560l::io::detail::alternate::can_adc_trigger_qprc::qbin0s<0b11>,
            klib::core::mb9bf560l::io::detail::alternate::multi_function_timer::ic02s<0b010>
        >;
    };

    struct p27 {
        // P1.2
        using port = io::periph::gpio1;

        // bit number in port
        constexpr static uint32_t number = 2;

        // bit number in port for the analog selection
        constexpr static uint32_t analog_number = 0;

        // all the alternate functions of this pin
        using alternate = std::tuple<
            klib::core::mb9bf560l::io::detail::alternate::multi_functional_serial::sck6b<0b01>,
            klib::core::mb9bf560l::io::detail::alternate::base_timer::tioa7e<0b10>,
            klib::core::mb9bf560l::io::detail::alternate::base_timer::tioa7s<0b10>,
            klib::core::mb9bf560l::io::detail::alternate::external_interrupt::eint02s<0b10>,
            klib::core::mb9bf560l::io::detail::alternate::can_adc_trigger_qprc::qzin0s<0b11>,
            klib::core::mb9bf560l::io::detail::alternate::multi_function_timer::ic01s<0b010>,
            klib::core::mb9bf560l::io::detail::alternate::can_adc_trigger_qprc::crx0s<0b10>
        >;
    };

    struct p28 {
        // P1.3
        using port = io::periph::gpio1;

        // bit number in port
        constexpr static uint32_t number = 3;

        // bit number in port for the analog selection
        constexpr static uint32_t analog_number = 1;

        // all the alternate functions of this pin
        using alternate = std::tuple<
            klib::core::mb9bf560l::io::detail::alternate::multi_functional_serial::sot6b<0b01>,
            klib::core::mb9bf560l::io::detail::alternate::base_timer::tiob7s<0b10>,
            klib::core::mb9bf560l::io::detail::alternate::multi_function_timer::ic00s<0b010>,
            klib::core::mb9bf560l::io::detail::alternate::can_adc_trigger_qprc::ctx0e<0b10>
        >;
    };

    struct p29 {
        // P2.0
        using port = io::periph::gpio2;

        // bit number in port
        constexpr static uint32_t number = 0;

        // bit number in port for the analog selection
        constexpr static uint32_t analog_number = 2;

        // all the alternate functions of this pin
        using alternate = std::tuple<
            klib::core::mb9bf560l::io::detail::alternate::multi_functional_serial::sin6s<0b01>,
            klib::core::mb9bf560l::io::detail::alternate::base_timer::tioa0e<0b10>,
            klib::core::mb9bf560l::io::detail::alternate::external_interrupt::eint03s<0b10>,
            klib::core::mb9bf560l::io::detail::alternate::system_function::rtccoe<0b10>,
            klib::core::mb9bf560l::io::detail::alternate::system_function::suboute<0b10>
        >;
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

        // bit number in port for the analog selection
        constexpr static uint32_t analog_number = 3;

        // all the alternate functions of this pin
        using alternate = std::tuple<
            klib::core::mb9bf560l::io::detail::alternate::can_adc_trigger_qprc::adtrg0s<0b0100>,
            klib::core::mb9bf560l::io::detail::alternate::can_adc_trigger_qprc::adtrg1s<0b0100>,
            klib::core::mb9bf560l::io::detail::alternate::can_adc_trigger_qprc::adtrg2s<0b0100>,
            klib::core::mb9bf560l::io::detail::alternate::multi_functional_serial::sin0s<0b01>,
            klib::core::mb9bf560l::io::detail::alternate::base_timer::tiob0s<0b10>,
            klib::core::mb9bf560l::io::detail::alternate::external_interrupt::eint04s<0b10>
        >;
    };

    struct p35 {
        // P2.2
        using port = io::periph::gpio2;

        // bit number in port
        constexpr static uint32_t number = 2;

        // bit number in port for the analog selection
        constexpr static uint32_t analog_number = 4;

        // all the alternate functions of this pin
        using alternate = std::tuple<
            klib::core::mb9bf560l::io::detail::alternate::multi_functional_serial::sot0b<0b01>,
            klib::core::mb9bf560l::io::detail::alternate::base_timer::tioa1e<0b10>,
            klib::core::mb9bf560l::io::detail::alternate::base_timer::tioa1s<0b10>,
            klib::core::mb9bf560l::io::detail::alternate::external_interrupt::eint05s<0b10>
        >;
    };

    struct p36 {
        // P2.3
        using port = io::periph::gpio2;

        // bit number in port
        constexpr static uint32_t number = 3;

        // bit number in port for the analog selection
        constexpr static uint32_t analog_number = 5;

        // all the alternate functions of this pin
        using alternate = std::tuple<
            klib::core::mb9bf560l::io::detail::alternate::multi_functional_serial::sck0b<0b01>,
            klib::core::mb9bf560l::io::detail::alternate::base_timer::tiob1s<0b10>,
            klib::core::mb9bf560l::io::detail::alternate::external_interrupt::eint06s<0b10>
        >;
    };

    struct p37 {
        // P0.4
        using port = io::periph::gpio0;

        // bit number in port
        constexpr static uint32_t number = 4;

        // all the alternate functions of this pin
        // WARNING: changing this pin will turn off debugging support if you are sure
        // you should uncomment the following lines
        // using alternate = std::tuple<
        //     klib::core::mb9bf560l::io::detail::alternate::system_function::jtagen0b<true>
        // >;
    };

    struct p38 {
        // P0.3
        using port = io::periph::gpio0;

        // bit number in port
        constexpr static uint32_t number = 3;

        // all the alternate functions of this pin
        // WARNING: changing this pin will turn off debugging support if you are sure
        // you should uncomment the following lines
        // using alternate = std::tuple<
        //     klib::core::mb9bf560l::io::detail::alternate::system_function::jtagen0b<true>
        // >;
    };

    struct p39 {
        // P0.2
        using port = io::periph::gpio0;

        // bit number in port
        constexpr static uint32_t number = 2;

        // WARNING: changing this pin will turn off support for JTAG debugging
        // all the alternate functions of this pin
        using alternate = std::tuple<
            klib::core::mb9bf560l::io::detail::alternate::system_function::jtagen1s<true>
        >;
    };

    struct p40 {
        // P0.1
        using port = io::periph::gpio0;

        // bit number in port
        constexpr static uint32_t number = 1;

        // all the alternate functions of this pin
        // WARNING: changing this pin will turn off debugging support if you are sure
        // you should uncomment the following lines
        // using alternate = std::tuple<
        //     klib::core::mb9bf560l::io::detail::alternate::system_function::jtagen0b<true>
        // >;
    };

    struct p41 {
        // P0.0
        using port = io::periph::gpio0;

        // bit number in port
        constexpr static uint32_t number = 0;

        // all the alternate functions of this pin
        // WARNING: changing this pin will turn off debugging support if you are sure
        // you should uncomment the following lines
        // using alternate = std::tuple<
        //     klib::core::mb9bf560l::io::detail::alternate::system_function::jtagen1s<true>
        // >;
    };

    struct p42 {
        // P6.6
        using port = io::periph::gpio6;

        // bit number in port
        constexpr static uint32_t number = 6;

        // bit number in port for the analog selection
        constexpr static uint32_t analog_number = 9;

        // all the alternate functions of this pin
        using alternate = std::tuple<
            klib::core::mb9bf560l::io::detail::alternate::external_interrupt::eint10s<0b10>,
            klib::core::mb9bf560l::io::detail::alternate::system_function::croute<0b10>
        >;
    };

    struct p43 {
        // P6.1
        using port = io::periph::gpio6;

        // bit number in port
        constexpr static uint32_t number = 1;

        // bit number in port for the analog selection
        constexpr static uint32_t analog_number = 14;

        // all the alternate functions of this pin
        using alternate = std::tuple<
            klib::core::mb9bf560l::io::detail::alternate::can_adc_trigger_qprc::adtrg0s<0b0110>,
            klib::core::mb9bf560l::io::detail::alternate::can_adc_trigger_qprc::adtrg1s<0b0110>,
            klib::core::mb9bf560l::io::detail::alternate::can_adc_trigger_qprc::adtrg2s<0b0110>,
            klib::core::mb9bf560l::io::detail::alternate::multi_functional_serial::sot1b<0b10>,
            klib::core::mb9bf560l::io::detail::alternate::external_interrupt::eint15s<0b10>,
            klib::core::mb9bf560l::io::detail::alternate::system_function::usbp0e<true>,
            klib::core::mb9bf560l::io::detail::alternate::multi_function_timer::ic00s<0b011>
        >;
    };

    struct p44 {
        // P6.0
        using port = io::periph::gpio6;

        // bit number in port
        constexpr static uint32_t number = 0;

        // all the alternate functions of this pin
        using alternate = std::tuple<
            klib::core::mb9bf560l::io::detail::alternate::multi_functional_serial::sck1b<0b10>,
            klib::core::mb9bf560l::io::detail::alternate::system_function::nmis<true>,
            klib::core::mb9bf560l::io::detail::alternate::multi_function_timer::ic01s<0b011>
        >;
    };

    struct p45 {
        // USBVCC
    };

    struct p46 {
        // P8.0
        using port = io::periph::gpio8;

        // bit number in port
        constexpr static uint32_t number = 0;

        // all the alternate functions of this pin
        using alternate = std::tuple<
            klib::core::mb9bf560l::io::detail::alternate::special::usb0c<true>
        >;
    };

    struct p47 {
        // P8.1
        using port = io::periph::gpio8;

        // bit number in port
        constexpr static uint32_t number = 1;

        // all the alternate functions of this pin
        using alternate = std::tuple<
            klib::core::mb9bf560l::io::detail::alternate::special::usb0c<true>
        >;
    };

    struct p48 {
        // VSS
    };
}

#endif