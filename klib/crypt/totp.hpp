#ifndef KLIB_TOTP_HPP
#define KLIB_TOTP_HPP

#include <cstdint>

#include "hotp.hpp"

namespace klib::crypt {
    /**
     * @brief Totp implementation for one time passwords
     * 
     * @tparam T 
     * @tparam Digits 
     */
    template <typename T, uint32_t Digits = 6>
    class totp {
    public:
        static uint32_t hash(const uint8_t *const key, const uint32_t length, const uint64_t current_time, const uint64_t time_step = 30, const uint64_t start_time = 0) {
            // calculate the counter value and pass it to hotp
            return hotp<T, Digits>::hash(key, length, (current_time - start_time) / time_step);
        }
    };
}

#endif