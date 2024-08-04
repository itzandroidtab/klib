#ifndef KLIB_ATSAM4S2B_PWM_HPP
#define KLIB_ATSAM4S2B_PWM_HPP

#include <cstdint>

#include <targets/core/atmel/atsam4s/pwm.hpp>

namespace klib::atsam4s2b::io::periph::detail::pwm {
    enum class mode {
        pwmh = 0,
        pwml = 1
    };

    template <typename Pin, mode Type, typename Periph, uint32_t Channel>
    struct pwm {
        // pin of the peripheral
        using pin = Pin;

        // type of the pin
        constexpr static mode type = Type;

        // alternate function
        using periph = Periph;

        // channel used for the pwm
        constexpr static uint32_t channel = Channel;
    };
}

namespace klib::atsam4s2b::io::periph::wlcsp_64 {
    struct pwm0 {
        // peripheral id (e.g pwm0, pwm1)
        constexpr static uint32_t id = 0;

        // interrupt id (including the arm vector table)
        constexpr static uint32_t interrupt_id = (31 + 16);

        // power bit position
        constexpr static uint32_t clock_id = 31;

        // port to the PWM hardware
        static inline PWM_Type *const port = PWM;

        // available channels in pwm mode
        constexpr static uint32_t max_pwm_channels = 4;

        // pins selectable as a pwm output
        using pwm_pins = std::tuple<
            // pwm0h
            detail::pwm::pwm<pins::package::wlcsp_64::pc2, detail::pwm::mode::pwmh, core::atsam4s::io::detail::alternate::func_1, 0>,
            detail::pwm::pwm<pins::package::wlcsp_64::pg4, detail::pwm::mode::pwmh, core::atsam4s::io::detail::alternate::func_2, 0>,
            detail::pwm::pwm<pins::package::wlcsp_64::pd6, detail::pwm::mode::pwmh, core::atsam4s::io::detail::alternate::func_2, 0>,
            detail::pwm::pwm<pins::package::wlcsp_64::pc6, detail::pwm::mode::pwmh, core::atsam4s::io::detail::alternate::func_1, 0>,
            // pwm1h
            detail::pwm::pwm<pins::package::wlcsp_64::pb1, detail::pwm::mode::pwmh, core::atsam4s::io::detail::alternate::func_1, 1>,
            detail::pwm::pwm<pins::package::wlcsp_64::ph4, detail::pwm::mode::pwmh, core::atsam4s::io::detail::alternate::func_2, 1>,
            detail::pwm::pwm<pins::package::wlcsp_64::pe5, detail::pwm::mode::pwmh, core::atsam4s::io::detail::alternate::func_2, 1>,
            detail::pwm::pwm<pins::package::wlcsp_64::pc8, detail::pwm::mode::pwmh, core::atsam4s::io::detail::alternate::func_1, 1>,
            // pwm2h
            detail::pwm::pwm<pins::package::wlcsp_64::pd2, detail::pwm::mode::pwmh, core::atsam4s::io::detail::alternate::func_1, 2>,
            detail::pwm::pwm<pins::package::wlcsp_64::pf5, detail::pwm::mode::pwmh, core::atsam4s::io::detail::alternate::func_2, 2>,
            detail::pwm::pwm<pins::package::wlcsp_64::pg5, detail::pwm::mode::pwmh, core::atsam4s::io::detail::alternate::func_2, 2>,
            detail::pwm::pwm<pins::package::wlcsp_64::ph1, detail::pwm::mode::pwmh, core::atsam4s::io::detail::alternate::func_2, 2>,
            // pwm3h
            detail::pwm::pwm<pins::package::wlcsp_64::ph2, detail::pwm::mode::pwmh, core::atsam4s::io::detail::alternate::func_2, 3>,
            detail::pwm::pwm<pins::package::wlcsp_64::pg6, detail::pwm::mode::pwmh, core::atsam4s::io::detail::alternate::func_2, 3>,
            detail::pwm::pwm<pins::package::wlcsp_64::pe7, detail::pwm::mode::pwmh, core::atsam4s::io::detail::alternate::func_3, 3>,
            detail::pwm::pwm<pins::package::wlcsp_64::pb7, detail::pwm::mode::pwmh, core::atsam4s::io::detail::alternate::func_2, 3>,
            // pwm0l
            detail::pwm::pwm<pins::package::wlcsp_64::pg8, detail::pwm::mode::pwml, core::atsam4s::io::detail::alternate::func_2, 0>,
            detail::pwm::pwm<pins::package::wlcsp_64::pa1, detail::pwm::mode::pwml, core::atsam4s::io::detail::alternate::func_2, 0>,
            // pwm1l
            detail::pwm::pwm<pins::package::wlcsp_64::ph8, detail::pwm::mode::pwml, core::atsam4s::io::detail::alternate::func_2, 1>,
            detail::pwm::pwm<pins::package::wlcsp_64::pc4, detail::pwm::mode::pwml, core::atsam4s::io::detail::alternate::func_1, 1>,
            // pwm2l
            detail::pwm::pwm<pins::package::wlcsp_64::pd7, detail::pwm::mode::pwml, core::atsam4s::io::detail::alternate::func_3, 2>,
            detail::pwm::pwm<pins::package::wlcsp_64::pe2, detail::pwm::mode::pwml, core::atsam4s::io::detail::alternate::func_1, 2>,
            detail::pwm::pwm<pins::package::wlcsp_64::pb5, detail::pwm::mode::pwml, core::atsam4s::io::detail::alternate::func_1, 2>,
            // pwm3l
            detail::pwm::pwm<pins::package::wlcsp_64::ph6, detail::pwm::mode::pwml, core::atsam4s::io::detail::alternate::func_3, 3>
        >;
    };
}

namespace klib::atsam4s2b::io {
    using pwm_clock = klib::core::atsam4s::io::detail::pwm::pwm_clock;

    /**
     * @brief Pin that uses a pwm to toggle the output.
     *
     * @note When Using a clka or clkb all channels that use the clock will
     * have their timing changed when selected
     * @warning When disabling the pwm the output of the gpio is not changed.
     */
    template <typename Pin, typename Pwm, uint32_t Frequency, uint8_t Bits, pwm_clock UseClock = pwm_clock::clka>
    using pin_pwm = core::atsam4s::io::pwm<Pin, Pwm, Frequency, Bits, UseClock>;
}

#endif