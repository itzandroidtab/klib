#ifndef KLIB_SERVO_HPP
#define KLIB_SERVO_HPP

#include <stdint.h>

#include <klib/math.hpp>
#include <klib/units.hpp>

namespace klib::io {
    template<typename Pin, klib::time::us MinPulse = 500, klib::time::us MaxPulse = 2500>
    class servo {
    protected:
        /**
         * @brief Calculate the duty cycle needed for a specific position in the servo.
         * 
         * Range is from 0 - 180 degrees
         * 
         * @param pos 
         * @return constexpr uint16_t 
         */
        constexpr static uint16_t to_dutycyle(const uint32_t pos) {
            // Returns an duty cycle corresponding to the position, frequency and timer bits
            return ((pos * (MaxPulse.value - MinPulse.value) / 180) + MinPulse.value) / ((1'000'000 / Pin::frequency) / (klib::exp2(Pin::bits) - 1));
        }

    public:
        /**
         * @brief Calculate and set the duty cycle for the position. Range is from 0 - 180 degrees
         * 
         * @tparam Pos 
         */
        template<klib::degree Pos>
        constexpr static void set() {
            Pin::template dutycycle<to_dutycyle(Pos.value)>();
        }

        /**
         * @brief Calculate and set the duty cycle for the position. Range is from 0 - 180 degrees
         * 
         * @param Pos 
         */
        constexpr static void set(klib::degree pos) {
            Pin::dutycycle(to_dutycyle(pos.value));
        }
    };
}

#endif