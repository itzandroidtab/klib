#ifndef KLIB_LPC1756_ADC_HPP
#define KLIB_LPC1756_ADC_HPP

#include <cstdint>

#include <targets/core/nxp/lpc175x/adc.hpp>
#include "pins.hpp"

namespace klib::lpc1756::io::periph::detail::adc {
    template <typename Pin, typename Periph>
    struct adc {
        // pin of the peripheral
        using pin = Pin;

        // alternate function
        using periph = Periph;
    };
}

namespace klib::lpc1756::io::periph::lqfp_80 {
    struct adc0 {
        // peripheral id (e.g adc0, adc1)
        constexpr static uint32_t id = 0;

        // peripheral clock bit position
        constexpr static uint32_t clock_id = 12;

        // interrupt id (including the arm vector table)
        constexpr static uint32_t interrupt_id = 22 + 16;

        // port to the ADC hardware
        static inline ADC_Type *const port = ADC;

        // pins allowed per output pin. Used for determining if a pin is valid on compile time
        using pins = std::tuple<
            detail::adc::adc<pins::package::lqfp_80::p7, core::lpc175x::io::detail::alternate::func_1>,
            detail::adc::adc<pins::package::lqfp_80::p6, core::lpc175x::io::detail::alternate::func_1>,
            detail::adc::adc<pins::package::lqfp_80::p18, core::lpc175x::io::detail::alternate::func_3>,
            detail::adc::adc<pins::package::lqfp_80::p17, core::lpc175x::io::detail::alternate::func_3>,
            detail::adc::adc<pins::package::lqfp_80::p79, core::lpc175x::io::detail::alternate::func_2>
        >;
    };
}

namespace klib::lpc1756::io {
    template <typename Adc>
    using adc = core::lpc175x::io::adc<Adc>;

    template <typename Adc, typename Pin>
    using adc_channel = core::lpc175x::io::adc_channel<Adc, Pin>;
}

#endif