#ifndef KLIB_HMAC_HPP
#define KLIB_HMAC_HPP

#include <cstdint>
#include <array>
#include <algorithm>

namespace klib::crypt {
    /**
     * @brief Hmac implementation
     * 
     * @tparam T 
     */
    template <typename T>
    class hmac {
    protected:
        static auto compute_block_sized_key(const uint8_t *const key, uint32_t length) {
            std::array<uint8_t, T::block_size> buffer = {};

            // check if the key is above the block size. If it is we shorten it.
            if (length > T::block_size) {
                T key_hash(key, length);
                auto result = key_hash.finalize();

                // copy the hash into the buffer
                std::copy(result.begin(), result.end(), buffer.begin());
            }
            else {
                // copy the key to the buffer
                std::copy_n(key, length, buffer.begin());
            }

            // return the buffer. Already padded with zero's
            return buffer;
        }

    public:        
        static auto hash(const uint8_t *const key, const uint32_t k_length, const uint8_t *const message, const uint32_t m_length) {
            // compute the block sized key
            const auto block_key = compute_block_sized_key(key, k_length);

            // inner and outer padded key
            T outer;
            T inner;

            for (const auto& b: block_key) {
                const uint8_t tmp = b ^ 0x5c;
                outer.update(&tmp, sizeof(tmp));
            }

            for (const auto& b: block_key) {
                const uint8_t tmp = b ^ 0x36;
                inner.update(&tmp, sizeof(tmp));
            }

            // concatenate the message to the inner
            inner.update(message, m_length);
            const auto result = inner.finalize();

            // concatenate the result of the inner to the outer
            outer.update(result.data(), result.size());
            return outer.finalize();
        }
    };
}

#endif