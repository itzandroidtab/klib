#ifndef KLIB_ISTREAM_HPP
#define KLIB_ISTREAM_HPP

#include <type_traits>

#include <klib/string.hpp>

#include "stream_base.hpp"

namespace klib {
    template <
        typename InputStream, 
        typename = std::enable_if<std::is_base_of<klib::istream, InputStream>::value>::type
    >
    InputStream &operator>>(InputStream &str, char &c) {
        c = str.getc();

        return str;
    }

    template <
        typename InputStream,
        typename = std::enable_if<std::is_base_of<klib::istream, InputStream>::value>::type
    >
    InputStream &operator>>(InputStream &str, unsigned char &c) {
        c = str.getc();

        return str;
    }

    template <
        typename InputStream,
        typename = std::enable_if<std::is_base_of<klib::istream, InputStream>::value>::type    
    >
    InputStream &operator>>(InputStream &str, int &val) {
        // Currently, decimal base 10 32 bit is assumed + \r\n + null terminator
        char buf[(sizeof(uint32_t) * 2) + (2 + 1)] = {};
        char c;

        // wait on the first character
        while (true) {
            // get the character
            str >> c;

            // check if we have a valid digit
            if (string::is_digit(c)) {
                // we have a digit
                buf[0] = c;

                // stop searching for valid digits
                break;
            }
        }

        for (uint32_t i = 1; i < (sizeof(buf)); i++) {
            // check if more characters are available in the buffer
            if (!str.hasc()) {
                // stop the loop
                break;
            }

            // get the character
            str >> c;

            // check if we have a digit
            if (!string::is_digit(c)) {
                // stop the loop
                break;
            }

            // set the character in the array
            buf[i] = c;
        }

        // set the value
        val = string::stoi(buf);

        return str;
    }
}

#endif
