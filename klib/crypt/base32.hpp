#ifndef KLIB_BASE32_HPP
#define KLIB_BASE32_HPP

#include <cstdint>
#include <algorithm>
#include <span>

#include <klib/string.hpp>

namespace klib::crypt {
    /**
     * @brief Base32
     * 
     * @note implemented using: https://datatracker.ietf.org/doc/html/rfc4648#section-6
     */
    class base32 {
    protected:
        // amount of bits per character
        constexpr static uint32_t bits_per_ch = 5;

        /**
         * @brief Checks if a character is a valid base32 character
         * 
         * @details valid characters are: A-Z and 2-7
         * 
         * @param ch 
         * @return true 
         * @return false 
         */
        constexpr static bool is_valid_character(const char ch) {
            // check if above highest valid character
            if (ch > 'Z') {
                return false;
            }

            // check if below lowest valid number
            if (ch < '2') {
                return false;
            }   

            // check if between the valid numbers and the lowest
            // valid character
            if (ch > '7' && ch < 'A') {
                return false;
            }

            // characters is between A-Z or 2-7
            return true;
        }

        /**
         * @brief Convert a base32 character to a 5 bit value
         * 
         * @warning does not check if character are valid
         * 
         * @param ch 
         * @return constexpr uint8_t 
         */
        constexpr static uint8_t character_to_value(const char ch) {
            if (ch >= 'A' && ch <= 'Z') {
                return ch - 'A';
            }

            return (ch - '2') + 26;
        }

    public:
        /**
         * @brief Decode a base32 string
         * 
         * @param input 
         * @param output
         * @param size 
         * @return uint32_t amount of bytes written in 
         * output (0 if invalid)
         */
        constexpr static uint32_t decode(const char *const input, const std::span<uint8_t> output) {
            // base32 is always 40 bit aligned. Every character is 5 bits.
            // this means if the size is not a multiply of 8 it is a invalid
            // base32 string
            const uint32_t length = klib::string::strlen(input);

            // check the length
            if (((length % 8) != 0) || (!length)) {
                return false;
            } 

            // check if the output size fits
            if ((((length * 5) + 7) / 8) > output.size()) {
                return false;
            }

            // clear the amount of bytes we are writing
            std::fill_n(output.data(), (((length * 5) + 7) / 8), 0x00);

            // flag for if we have detected a padding character
            bool padding_started = false;

            // amount of bytes processed
            uint32_t processed = 0;

            // check if every character is a valid character. The last 
            // characters can be '=' instead of valid characters as this is
            // the value used as padding
            for (uint32_t i = 0; i < length; i++) {
                // check if the character is valid
                const bool valid = is_valid_character(input[i]);
                const bool is_padding = input[i] == '=';

                // check if we got a character that is not padding and not
                // a valid character
                if (!valid && !is_padding) {
                    return false;
                }

                // check if we have a valid character after we detected 
                // padding
                if (valid && padding_started) {
                    return false;
                }

                // do not process the data if we have padding
                if (is_padding) {
                    padding_started = true;

                    // do not process the padding bytes
                    continue;
                }

                // increment the amount of bytes we have processed
                processed++;

                // get the amount of bits before the current data
                const uint32_t bits = i * bits_per_ch;

                // convert the current character to a value
                const uint8_t raw = character_to_value(input[i]);

                // add all the bits to the output
                for (uint32_t j = 0; j < bits_per_ch; /* do not increment here */) {
                    const uint32_t index = ((bits + j) / 8);
                    const uint32_t shift = ((bits + j) % 8);

                    // check how many bits we can update in the current byte
                    const uint32_t update = klib::min(8 - shift, (bits_per_ch - j));

                    // get the mask where we want to write
                    const uint32_t mask = (klib::exp2(update) - 1);

                    // set the bits
                    output[index] |= ((raw >> ((bits_per_ch - j) - update)) & mask) << (8 - (shift + update));

                    // update the bit index
                    j += update;
                }
            }

            // check how many bits are unused
            const uint32_t unused = (processed * 5) % 8;

            // return the amount of bytes written
            return ((((processed * 5) - unused) + 7) / 8);
        }
    };
}

#endif