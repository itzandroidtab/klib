#ifndef KLIB_HOTP_HPP
#define KLIB_HOTP_HPP

#include <cstdint>

#include <klib/math.hpp>

#include "hmac.hpp"

namespace klib::crypt {
    /**
     * @brief Hotp implementation
     * 
     * @tparam T 
     * @tparam Digits 
     */
    template <typename T, uint32_t Digits = 6>
    class hotp {
    public:
        static uint32_t hash(const uint8_t *const key, const uint32_t length, const uint64_t counter) {
            // convert the counter from little endian to big endian
            const uint64_t be_counter = klib::to_big_endian<uint64_t>(counter);

            // first phase. get the digest of the message using the key
            const auto digest = hmac<T>::hash(key, length, reinterpret_cast<const uint8_t*>(&be_counter), sizeof(be_counter));

            // truncate the hash
            const uint8_t offset = digest[19] & 0xf;

            const uint32_t result = (
                ((digest[offset + 0] & 0x7f) << 24) |
                ((digest[offset + 1] & 0xff) << 16) |
                ((digest[offset + 2] & 0xff) << 8) |
                (digest[offset + 3] & 0xff)
            );

            // return the result mod digits
            return result % klib::pow(10, Digits);
        }
    };
}

#endif