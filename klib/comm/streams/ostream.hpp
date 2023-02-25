#ifndef KLIB_OSTREAM_HPP
#define KLIB_OSTREAM_HPP

#include <type_traits>
#include <array>

#include <klib/dynamic_array.hpp>
#include <klib/math.hpp>
#include <klib/string.hpp>

#include "stream_base.hpp"

namespace klib {
    struct _boolalpha {};

    [[maybe_unused]]
    constexpr _boolalpha boolalpha;

    struct _noboolalpha {};

    [[maybe_unused]]
    constexpr _noboolalpha noboolalpha;

    struct _hex {};

    [[maybe_unused]]
    constexpr _hex hex;

    struct _dec {};

    [[maybe_unused]]
    constexpr _dec dec;

    struct _oct {};

    [[maybe_unused]]
    constexpr _oct oct;

    struct _bin {};

    [[maybe_unused]]
    constexpr _bin bin;

    [[maybe_unused]]
    constexpr char endl = '\n';

    /**
     * @brief Write a character to the output stream
     * 
     * @tparam OutputStream 
     * @tparam B 
     * @tparam Boolalpha 
     * @param str 
     * @param c 
     * @return const OutputStream<B, Boolalpha>& 
     */
    template <
        template<klib::base, bool> class OutputStream, 
        klib::base B, bool Boolalpha
    >
    const OutputStream<B, Boolalpha> &operator<<(const OutputStream<B, Boolalpha> &str, char c) {
        str.putc(c);

        return str;
    }

    /**
     * @brief Write a character array to the output stream
     * 
     * @tparam OutputStream 
     * @tparam B 
     * @tparam Boolalpha 
     * @param str 
     * @param s 
     * @return const OutputStream<B, Boolalpha>& 
     */
    template <
        template<klib::base, bool> class OutputStream, 
        klib::base B, bool Boolalpha
    >
    const OutputStream<B, Boolalpha> &operator<<(const OutputStream<B, Boolalpha> &str, const char *s) {
        for (const char *p = s; *p != '\0'; p++) {
            str << *p;
        }

        return str;
    }

    /**
     * @brief Write a number to the output stream. Converts a number using the base
     * 
     * @tparam T 
     * @tparam OutputStream 
     * @tparam B 
     * @tparam Boolalpha 
     * @tparam Boolalpha,
     * @param str 
     * @param v 
     * @return const OutputStream<B, Boolalpha>& 
     */
    template <
        typename T,
        template<klib::base, bool> class OutputStream, 
        klib::base B, bool Boolalpha,
        typename = std::enable_if_t<
            std::is_integral_v<T> && (sizeof(T) <= 8)
        >
    >
    const OutputStream<B, Boolalpha> &operator<<(const OutputStream<B, Boolalpha> &str, T v) {
        // buffer to store the conversion (max of 8 characters per byte + 3 for 
        // the prefix and null-termintator)
        char buf[(sizeof(T) * 8) + sizeof("  ")] = {};
        
        // convert the value to a string
        string::itoa<
            OutputStream<B, Boolalpha>::base, 
            OutputStream<B, Boolalpha>::boolalpha
        >(v, buf);

        // write the buffer to the stream
        str << buf;

        return str;
    }

    /**
     * Helper struct that helps output C-style arrays or a std::array to
     * the the stream. This is meant for debugging, the output is formatted
     * as follows: "{ A, B, C }" where A, B and C are outputted using their shift
     * left operator overload.
     *
     * Usage example:
     * @code
     * int buff[] = {1,2,3};
     * klib::cout << klib::array_values(buff, 3);
     * @endcode
     * @tparam T
     */
    template <typename T>
    struct array_values {
        const T *arr;
        uint32_t size;

        /**
         * Construct the values helper from a C-style
         * array.
         *
         * @param arr
         * @param size
         */
        constexpr array_values(const T *arr, uint32_t size)
            : arr(arr), size(size) {}

        /**
         * Construct the values helper from a std::array.
         *
         * @tparam Size
         * @param arr
         */
        template <uint32_t Size>
        constexpr array_values(const std::array<T, Size> &arr)
            : arr(arr.data()), size(Size) {}

        /**
         * Construct the values helper from a klib::dynamic_array.
         *
         * @tparam Size
         * @param arr
         */
        template <uint32_t Size>
        constexpr array_values(const klib::dynamic_array<T, Size> &arr)
            : arr(arr.data()), size(arr.size()) {}
    };

    /**
     * Output a array_values object.
     *
     * @tparam OutputStream
     * @tparam T
     * @param str
     * @param values
     * @return
     */
    template <
        typename T,
        template<klib::base, bool> class OutputStream, 
        klib::base B, bool Boolalpha
    >
    const OutputStream<B, Boolalpha> &operator<<(const OutputStream<B, Boolalpha> &str, const array_values<T> &values) {
        str << "{ ";
        for (uint32_t i = 0; i < values.size; ++i) {
            str << values.arr[i];

            if (i + 1 != values.size) {
                str << ", ";
            }
        }
        str << " }";

        return str;
    }

    /**
     * Output a double precision floating point
     * number.
     *
     * @tparam OutputStream
     * @param str
     * @param v
     * @return
     */
    template <
        template<klib::base, bool> class OutputStream, 
        klib::base B, bool Boolalpha,
        typename T,
        typename = std::enable_if_t<std::is_floating_point_v<T>>
    >
    const OutputStream<B, Boolalpha> &operator<<(const OutputStream<B, Boolalpha> &str, T v) {
        char buffer[32];

        if (isnan(v)) {
            return str << "NaN";
        } 
        else if (isinf(v)) {
            return str << "Inf";
        } 
        else if (v == static_cast<T>(0.f)) {
            return str << "0";
        } 
        else {
            char *p = buffer;
            int i = 0, k = 0;

            while (static_cast<int>(v) > 0) {
                v /= 10;
                i++;
            }

            *(p + i) = '.';

            v *= 10;
            auto n = static_cast<int>(v);
            v -= n;

            while ((n > 0) || (i > k)) {
                if (k == i) {
                    k++;
                }

                *(p + k) = '0' + n;
                v *= 10;
                n = static_cast<int>(v);
                v -= n;
                k++;
            }

            // Null-terminated string
            *(p + k) = '\0';
        }

        str << buffer;

        return str;
    }

    /**
     * Output numbers as hexadecimal for this
     * stream.
     *
     * @tparam OutputStream
     * @return
     */
    template <
        template<klib::base, bool> class OutputStream, 
        klib::base B, bool Boolalpha
    >
    auto operator<<(const OutputStream<B, Boolalpha>&, const _hex) {
        return typename OutputStream<B, Boolalpha>::template instance<base::HEX, OutputStream<B, Boolalpha>::boolalpha>();
    }

    /**
     * Output numbers as decimal for this
     * stream.
     *
     * @tparam OutputStream
     * @return
     */
    template <
        template<klib::base, bool> class OutputStream, 
        klib::base B, bool Boolalpha
    >
    auto operator<<(const OutputStream<B, Boolalpha>&, _dec) {
        return typename OutputStream<B, Boolalpha>::template instance<base::DEC, OutputStream<B, Boolalpha>::boolalpha>();
    }

    /**
     * Output numbers as octal for
     * this stream.
     *
     * @tparam OutputStream
     * @return
     */
    template <
        template<klib::base, bool> class OutputStream, 
        klib::base B, bool Boolalpha
    >
    auto operator<<(const OutputStream<B, Boolalpha>&, _oct) {
        return typename OutputStream<B, Boolalpha>::template instance<base::OCT, OutputStream<B, Boolalpha>::boolalpha>();
    }

    /**
     * Output numbers as binary for
     * this stream.
     *
     * @tparam OutputStream
     * @return
     */
    template <
        template<klib::base, bool> class OutputStream, 
        klib::base B, bool Boolalpha
    >
    auto operator<<(const OutputStream<B, Boolalpha>&, _bin) {
        return typename OutputStream<B, Boolalpha>::template instance<base::BIN, OutputStream<B, Boolalpha>::boolalpha>();
    }

    /**
     * Output booleans as 'true' and 'false'
     * for this stream.
     *
     * @tparam OutputStream
     * @return
     */
    template <
        template<klib::base, bool> class OutputStream, 
        klib::base B, bool Boolalpha
    >
    auto operator<<(const OutputStream<B, Boolalpha>&, _boolalpha) {
        return typename OutputStream<B, Boolalpha>::template instance<OutputStream<B, Boolalpha>::base, true>();
    }

    /**
     * Output booleans as '1' and '0'
     * for this stream.
     *
     * @tparam OutputStream
     * @return
     */
    template <
        template<klib::base, bool> class OutputStream, 
        klib::base B, bool Boolalpha
    >
    auto operator<<(const OutputStream<B, Boolalpha>&, _noboolalpha) {
        return typename OutputStream<B, Boolalpha>::template instance<OutputStream<B, Boolalpha>::base, false>();
    }    
}

#endif