#ifndef KLIB_ATSAM4S2B_I2C_HPP
#define KLIB_ATSAM4S2B_I2C_HPP

#include <cstdint>

#include <atsam4s2b.hpp>

namespace klib::atsam4s2b::io::periph::detail::i2c {
    enum class mode {
        sda,
        scl
    };

    template <typename Pin, mode Type, typename Periph>
    struct i2c {
        // pin of the peripheral
        using pin = Pin;

        // type of the pin
        constexpr static mode type = Type;

        // alternate function
        using periph = Periph;
    };
}

namespace klib::atsam4s2b::io::periph::wlcsp_64 {
    struct i2c0 {
        // peripheral id (e.g i2c0, i2c1)
        constexpr static uint32_t id = 0;

        // peripheral clock bit position
        constexpr static uint32_t clock_id = 19;

        // interrupt id (including the arm vector table)
        constexpr static uint32_t interrupt_id = 35;

        // port to the I2C hardware
        static inline TWI0_Type *const port = TWI0;

        using sda = detail::i2c::i2c<pins::package::wlcsp_64::pe1, detail::i2c::mode::sda, core::atsam4s::io::detail::alternate::func_1>;
        using scl = detail::i2c::i2c<pins::package::wlcsp_64::pg2, detail::i2c::mode::scl, core::atsam4s::io::detail::alternate::func_1>;
    };

    struct i2c1 {
        // peripheral id (e.g i2c0, i2c1)
        constexpr static uint32_t id = 1;

        // peripheral clock bit position
        constexpr static uint32_t clock_id = 20;

        // interrupt id (including the arm vector table)
        constexpr static uint32_t interrupt_id = 36;

        // port to the I2C hardware
        static inline TWI1_Type *const port = TWI1;

        using sda = detail::i2c::i2c<pins::package::wlcsp_64::ph1, detail::i2c::mode::sda, core::atsam4s::io::detail::alternate::func_1>;
        using scl = detail::i2c::i2c<pins::package::wlcsp_64::pa1, detail::i2c::mode::scl, core::atsam4s::io::detail::alternate::func_1>;
    };
}

#endif