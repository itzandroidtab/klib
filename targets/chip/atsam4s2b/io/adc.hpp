#ifndef KLIB_ATSAM4S2B_ADC_HPP
#define KLIB_ATSAM4S2B_ADC_HPP

#include <targets/core/atmel/atsam4s/adc.hpp>

#include "pins.hpp"

namespace klib::atsam4s2b::io::periph::wlcsp_64 {
    struct adc0 {
        // peripheral id (e.g adc0, adc1)
        constexpr static uint32_t id = 0;

        // peripheral clock bit position
        constexpr static uint32_t clock_id = 29;
        
        // interrupt id (including the arm vector table)
        constexpr static uint32_t interrupt_id = 29 + 16;

        // port to the adc hardware
        static inline ADC_Type *const port = ADC;
    };
}

namespace klib::atsam4s2b::io {
    template <typename Adc>
    using adc = core::atsam4s::io::adc<Adc>;

    template <typename Adc, typename Pin>
    using adc_channel = core::atsam4s::io::adc_channel<Adc, Pin>;
}

#endif