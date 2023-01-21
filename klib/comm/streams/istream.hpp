#ifndef KLIB_ISTREAM_HPP
#define KLIB_ISTREAM_HPP

#include <type_traits>

#include <klib/math.hpp>

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
        // Currently, decimal base 10 32 bit is assumed
        char buf[11] = {};
        int end = 0;

        for (uint32_t i = 0; i < 10; i++) {
            char c;

            if (str >> c) {
                buf[i] = c;
            } else {
                end = i - 1;
                buf[i] = '\0';
                break;
            }
        }

        for (int i = end; i >= 0; i--) {
            if (buf[i] == '-') {
                val *= -1;
                break;
            }

            val += klib::pow(10, end - i) * (buf[i] - '0');
        }

        return str;
    }
}

#endif
