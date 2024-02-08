#ifndef KLIB_STRING_HPP
#define KLIB_STRING_HPP

#include <cstdint>
#include <klib/units.hpp>
#include <klib/math.hpp>

namespace klib::string {
    /**
     * @brief Returns if a character is binary
     * 
     * @param c 
     * @return true 
     * @return false 
     */
    constexpr bool is_binary(const char c) {
        return (c == '0' || c == '1');
    }

    /**
     * @brief Returns if a character is octal
     * 
     * @param c 
     * @return true 
     * @return false 
     */
    constexpr bool is_octal(const char c) {
        return (c >= '0' && c <= '7');
    }

    /**
     * @brief Returns if a character is a digit
     * 
     * @param c 
     * @return true 
     * @return false 
     */
    constexpr bool is_digit(const char c) {
        return (c >= '0' && c <= '9');
    }

    /**
     * @brief Returns if a character is a digit or within the hex range
     * 
     * @param c 
     * @return true 
     * @return false 
     */
    constexpr bool is_hex(const char c) {
        return is_digit(c) || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F');
    }

    /**
     * @brief Returns if a character is a uppercase character
     * 
     * @param c 
     * @return true 
     * @return false 
     */
    constexpr bool is_upper(const char c) {
        return (c >= 'A' && c <= 'Z');
    }

    /**
     * @brief Returns if a character is lowercase
     * 
     * @param c 
     * @return true 
     * @return false 
     */
    constexpr bool is_lower(const char c) {
        return (c >= 'a' && c <= 'z');
    }

    /**
     * @brief Returns if we have a lower or upper case character
     * 
     * @return true 
     * @return false 
     */
    constexpr bool is_character(const char c) {
        return is_lower(c) || is_upper(c);
    }

    /**
     * @brief Converts a character to uppercase
     * 
     * @param c 
     * @return char 
     */
    constexpr char to_upper(const char c) {
        return (is_lower(c) ? c & (~0x20) : c);
    }

    /**
     * @brief Converts a character to lowercase
     * 
     * @param c 
     * @return char 
     */
    constexpr char to_lower(const char c) {
        return (is_upper(c) ? c | 0x20 : c);
    }

    /**
     * @brief Count the amount of characters before a null terminator
     * 
     * @param str 
     * @return uint32_t 
     */
    constexpr uint32_t strlen(const char* str) {
        uint32_t count = 0;

        // count until the end of the string
        while (*str != '\0') {
            count++;
            str++;
        }

        return count;
    }

    /**
     * @brief Copy a c string to another c string
     * 
     * @param destination 
     * @param source 
     * @return char* 
     */
    constexpr char* strcpy(char *const destination, const char* source) {
        char* ptr = destination; 

        while (*source != '\0') {
            *ptr = *source;

            ptr++;
            source++;
        }

        // add a null terminator after the string
        *ptr = '\0';

        // return the start of the destination
        return destination;
    }
    
    /**
     * @brief Cat a string to the end of another string
     * 
     * @param destination 
     * @param source 
     * @return char* 
     */
    constexpr char* strcat(char* destination, const char* source) {
        // get the length of the string
        const uint32_t len = strlen(destination);

        // copy the source to the destination with the length offset
        strcpy(destination + len, source);

        // return the start of the destination
        return destination;
    }

    /**
     * @brief Pad a string on the left with a character for amount of times
     * 
     * @param str 
     * @param amount 
     * @param ch 
     */
    constexpr void left_pad(char *const str, const uint32_t amount, const char ch = ' ') {
        // get the length of the string
        const uint32_t len = strlen(str);

        // only move if we have data in the string
        if (len > 0) {
            // move the characters to the end of the string
            for (uint32_t i = 0; i < len; i++) {
                str[len + amount - (i + 1)] = str[len - (i + 1)];
            }
        }

        // add a null terminator after the end
        str[len + amount] = '\0';

        // add the character until the start of the original string
        for (uint32_t i = 0; i < amount; i++) {
            str[i] = ch;
        } 
    }

    /**
     * @brief Pad a string on the right with a character for amount of times
     * 
     * @param str 
     * @param amount 
     * @param ch 
     */
    constexpr void right_pad(char *const str, const uint32_t amount, const char ch = ' ') {
        // get the length of the string
        const uint32_t len = strlen(str);

        // add the amount of character
        for (uint32_t i = 0; i < amount; i++) {
            str[len + i] = ch;
        }

        // add a null terminator after the end
        str[len + amount] = '\0';
    }

    /**
     * @brief Set the width of a string by appending characters to the left or right
     * 
     * @param str 
     * @param width 
     */
    template <bool LeftPad = true>
    constexpr void set_width(char *const str, const uint32_t width, const char ch = ' ') {
        // get the length of the string
        const uint32_t len = strlen(str);

        // skip if the length is more than the width
        if (len >= width) {
            return;
        }

        if constexpr (LeftPad) {
            // do a leftpad
            left_pad(str, width - len, ch);
        }
        else {
            // do a rightpad
            right_pad(str, width - len, ch);
        }
    }
}

namespace klib::string::detail {
    template <base B = _default_base>
    constexpr uint8_t add_prefix(char *const str) {
        // add the correct prefix for every base
        if constexpr (B == base::BIN) {
            strcpy(str, "0b");

            return 2;
        }
        else if constexpr (B == base::OCT) {
            strcpy(str, "0o");

            return 2;
        }
        else if constexpr (B == base::HEX) {
            strcpy(str, "0x");

            return 2;
        }

        // decimal does not get any prefix
        return 0;
    }

    template <base B = _default_base, typename T = int>
    constexpr uint32_t count_chars(T value) {
        uint32_t chars = 0;

        // check if the value is signed
        if constexpr (std::is_signed<T>::value) {
            // check if the value is negative
            if (value < 0) {
                // change the negative value to positive
                value *= -1;

                // add 1 for the '-' sign
                chars += 1;
            }
        }

        while (static_cast<uint32_t>(value) > 0) {
            chars += 1;

            if constexpr (B == base::BIN) {
                value >>= 1;
            }
            else if constexpr (B == base::OCT) {
                value >>= 3;
            }
            else if constexpr (B == base::DEC) {
                value /= 10;
            }
            else {
                value >>= 4;
            }
        }

        return chars;
    }

    template <base B = _default_base, typename T = int>
    constexpr T stoi_impl(const char *const str) {
        // get a pointer to the string
        const char* s = str;
        T value = 0;

        // iterate until the end of the string
        while (*s != '\0') {
            // handle all the bases
            if constexpr (B == base::BIN) {
                // check if the input is valid
                if (is_binary(*s)) {
                    value = (value << 1) + ((*s) - '0');
                }
            }
            else if constexpr (B == base::OCT) {
                // check if the input is valid
                if (is_octal(*s)) {
                    value = (value << 3) + ((*s) - '0');
                }
            }
            else if constexpr (B == base::DEC) {
                // check if the input is valid
                if (is_digit(*s)) {
                    value = ((*s) - '0') + (value * 10);
                }
            }
            else if constexpr (B == base::HEX) {
                // check if the input is valid
                if (is_digit(*s)) {
                    value = ((*s) - '0') + (value << 4);
                }
                else if (is_hex(*s)) {
                    value = ((to_lower(*s) - 'a') + 10) + (value << 4);
                }
            }

            // go to the next character
            s++;
        }

        return value;
    }

    template <base B = _default_base>
    constexpr uint32_t get_base() {
        if constexpr (B == base::BIN) {
            return 2;
        }
        else if constexpr (B == base::OCT) {
            return 8;            
        }
        else if constexpr (B == base::DEC) {
            return 10;
        }
        else {
            return 16;
        }
    }

    template <base B = _default_base, bool BoolAlpha = _default_boolalpha, typename T = int>
    constexpr void itoa_impl(T value, char *const str) {
        // handle the boolalpha case first
        if constexpr (BoolAlpha) {
            if (value == 0) {
                strcpy(str, "false");
            }
            else {
                strcpy(str, "true");
            }

            return;
        }

        // Handle 0 explicitly, otherwise 0 is not printed
        if (value == 0) {
            // add the prefix 
            const auto added = add_prefix<B>(str);

            // add 0 after the prefix
            str[added] = '0';

            // add the null terminator
            str[added + 1] = '\0';  

            return;
        }

        uint32_t index = 0;

        // check if we need to add a negative sign
        if constexpr (B == base::DEC && std::is_signed<T>::value) {
            if (value < 0) {
                // make the value positive
                value *= -1;

                // add a minus sign
                str[index] = '-';

                // go to the next character
                index++;
            }
        }

        // count the amount of space we need for all the character
        const uint32_t count = count_chars<B>(value);

        // add a prefix
        index += add_prefix<B>(&str[index]);

        // get the prefix to do the modulo of
        constexpr auto b = get_base<B>();

        // loop until we dont have any more characters left
        for (uint32_t i = 0; i < count; i++) {
            // get the remainder
            const T remainder = static_cast<uint32_t>(value) % b;

            // add the letter to the string
            if (remainder > 9) {
                str[index + (count - i - 1)] = (remainder - 10) + 'a';
            }
            else {
                str[index + (count - i - 1)] = remainder + '0';
            }

            // remove the remainder from the value
            value /= b;
        }

        // add a null terminator
        str[index + count] = '\0';
    }

    template <uint32_t Digits, typename T = int>
    constexpr void stoa_impl(T value, char *const str) {
        if (klib::isnan(value)) {
            strcpy(str, "NaN");
        }
        else if (klib::isinf(value)) {
            strcpy(str, "Inf");
        } 
        else if (value == static_cast<T>(0.f)) {
            strcpy(str, "0");
        } 
        else {
            int i = 0, k = 0;

            do {
                value /= 10;
                i++;
            }
            while (static_cast<int>(value) > 0);

            // add the decimal seperator
            *(str + i) = '.';

            value *= 10;
            auto n = static_cast<int>(value);
            value -= n;

            while ((k - i) <= static_cast<int>(Digits)) {
                if (k == i) {
                    k++;
                }

                *(str + k) = '0' + n;
                
                value *= 10;
                n = static_cast<int>(value);
                value -= n;
                k++;
            }

            // Null-terminate the string
            *(str + k) = '\0';
        }
    }
}

namespace klib::string {
    /**
     * @brief custom stoi implementation that is constexpr.
     * 
     * @param str 
     * @param base
     * @return int 
     */
    template <base B = _default_base>
    constexpr int stoi(const char *const str) {
        // return the implementation
        return detail::stoi_impl<B, int>(str);
    }

    /**
     * @brief Convert a integer to a string
     * 
     * @tparam B 
     * @tparam Boolalpha 
     * @tparam T 
     * @tparam typename 
     * @param value 
     * @param str 
     */
    template <
        base B = _default_base, bool Boolalpha = _default_boolalpha, 
        typename T = int, typename = std::enable_if_t<std::is_integral_v<T>>
    >
    constexpr void itoa(const T value, char *const str) {
        // return the implementation
        return detail::itoa_impl<B, Boolalpha, T>(value, str);
    }

    /**
     * @brief Convert a float to a string
     * 
     * @tparam Digits 
     * @tparam T 
     * @tparam T,
     * typename 
     * @param value 
     * @param str 
     */
    template <
        uint32_t Digits = 5, typename T = float, 
        typename = std::enable_if_t<std::is_floating_point_v<T>>
    >
    constexpr void stoa(const T value, char *const str) {
        // return the implementation
        return detail::stoa_impl<Digits, T>(value, str);
    }
}

#endif