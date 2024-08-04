#ifndef KLIB_SHA1_HPP
#define KLIB_SHA1_HPP

#include <cstdint>
#include <array>

#include <klib/math.hpp>

namespace klib::crypt {
    /**
     * @brief Sha1 hash. C++ implementation inspired by [novan-ve/ft_ssl](https://github.com/novan-ve/ft_ssl)
     *
     */
    class sha1 {
    public:
        constexpr static uint32_t block_size = 64;
        constexpr static uint32_t digest_size = 20;

    protected:
        constexpr static uint32_t hash_size = 5;

        // total amount of bytes given to the hash
        uint64_t size = 0;

        // buffer size for the hash
        uint8_t buffer[block_size] = {};

        // hash values with the initial values
        uint32_t hash_buffer[hash_size] = {
            0x67452301,
            0xEFCDAB89,
            0x98BADCFE,
            0x10325476,
            0xC3D2E1F0,
        };

        uint32_t uint8_to_uint32(const uint8_t *buf) const {
            return (
                static_cast<uint32_t>(buf[3]) << 24 |
                static_cast<uint32_t>(buf[2]) << 16 |
                static_cast<uint32_t>(buf[1]) << 8 |
                static_cast<uint32_t>(buf[0])
            );
        }

        uint32_t rotate_left(const uint32_t value, const uint32_t n) const {
            return (value << n) | (value >> ((sizeof(uint32_t) * 8) - n));
        }

        void calc(const uint32_t *const input) {
            uint32_t w[80] = {};

            for (int i = 0; i < 16; i++) {
                w[i] = klib::bswap32(input[i]);
            }

            for (int i = 16; i < 80; i++) {
                w[i] = rotate_left(w[i - 3] ^ w[i - 8] ^ w[i - 14] ^ w[i - 16], 1);
            }

            uint32_t a = hash_buffer[0];
            uint32_t b = hash_buffer[1];
            uint32_t c = hash_buffer[2];
            uint32_t d = hash_buffer[3];
            uint32_t e = hash_buffer[4];

            for (int i = 0; i < 80; i++) {
                uint32_t f, k;

                if (i >= 0 && i < 20) {
                    f = (b & c) | ((~b) & d);
                    k = 0x5A827999;
                }
                else if (i >= 20 && i < 40) {
                    f = b ^ c ^ d;
                    k = 0x6ED9EBA1;
                }
                else if (i >= 40 && i < 60) {
                    f = (b & c) | (b & d) | (c & d);
                    k = 0x8F1BBCDC;
                }
                else {
                    f = b ^ c ^ d;
                    k = 0xCA62C1D6;
                }

                const uint32_t temp = rotate_left(a, 5) + f + e + k + w[i];
                e = d;
                d = c;
                c = rotate_left(b, 30);
                b = a;
                a = temp;
            }

            hash_buffer[0] += a;
            hash_buffer[1] += b;
            hash_buffer[2] += c;
            hash_buffer[3] += d;
            hash_buffer[4] += e;
        }

    public:
        /**
         * @brief Construct a new sha1 object
         *
         */
        sha1() {}

        /**
         * @brief Construct a new sha1 object and call update on the data
         *
         * @param data
         * @param length
         */
        sha1(const uint8_t *const data, const uint32_t length) {
            update(data, length);
        }

        /**
         * @brief Update the internal structure with the provided
         * data
         *
         * @param data
         * @param length
         */
        void update(const uint8_t *const data, const uint32_t length) {
            uint32_t input[16];
            uint32_t offset = size % block_size;

            for (size_t i = 0; i < length; i++) {
                buffer[offset++] = data[i];
                if (offset % block_size == 0) {
                    for (int j = 0; j < 16; j++) {
                        input[j] = uint8_to_uint32(buffer + (j * 4));
                    }
                    calc(input);
                    offset = 0;
                }
            }
            size += length;
        }

        /**
         * @brief Finalize the hash. After this function is called update
         * should not be called anymore
         *
         * @return digest
         */
        std::array<uint8_t, digest_size> finalize() {
            const uint64_t offset = size % block_size;
            const uint64_t padding_len = offset < 56 ? 56 - offset : 120 - offset;

            uint8_t padding[block_size] = {};
            padding[0] = 0x80;

            update(padding, padding_len);
            size -= padding_len;

            uint32_t input[16] = {};

            for (uint8_t i = 0; i < 14; i++) {
                input[i] = uint8_to_uint32(buffer + (i * 4));
            }

            const uint64_t ssize = klib::bswap64(size * 8);
            input[14] = (uint32_t)(ssize);
            input[15] = (uint32_t)(ssize >> 32);

            // the buffer is full calculate now
            calc(input);

            // copy the hash to the digest
            std::array<uint8_t, digest_size> ret;

            for (uint32_t i = 0; i < hash_size; i++) {
                ret[i * sizeof(uint32_t) + 0] = ((hash_buffer[i] & 0xFF000000) >> 24);
                ret[i * sizeof(uint32_t) + 1] = ((hash_buffer[i] & 0x00FF0000) >> 16);
                ret[i * sizeof(uint32_t) + 2] = ((hash_buffer[i] & 0x0000FF00) >> 8);
                ret[i * sizeof(uint32_t) + 3] = (hash_buffer[i] & 0x000000FF);
            }

            return ret;
        }
    };
}

#endif