#ifndef KLIB_LPC1752_I2C_HPP
#define KLIB_LPC1752_I2C_HPP

#include <cstdint>

#include <lpc1752.hpp>

namespace klib::lpc1752::io::periph {
    struct i2c0 {
        // peripheral id (e.g i2c0, i2c1)
        constexpr static uint32_t id = 0;

        // interrupt id (including the arm vector table)
        constexpr static uint32_t interrupt_id = 26;

        // port to the I2C hardware
        static inline I2C0_Type *const port = I2C0;
    };

    struct i2c1 {
        // peripheral id (e.g i2c0, i2c1)
        constexpr static uint32_t id = 1;

        // interrupt id (including the arm vector table)
        constexpr static uint32_t interrupt_id = 27;

        // port to the I2C hardware
        static inline I2C0_Type *const port = I2C1;
    };

    struct i2c2 {
        // peripheral id (e.g i2c0, i2c1)
        constexpr static uint32_t id = 2;

        // interrupt id (including the arm vector table)
        constexpr static uint32_t interrupt_id = 28;

        // port to the I2C hardware
        static inline I2C0_Type *const port = I2C2;
    };
}

#endif