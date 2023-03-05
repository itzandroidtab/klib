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

    template <uint32_t Digits>
    struct precision {};

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
     * @return const OutputStream<B, Boolalpha, Digits>& 
     */
    template <
        template<klib::base, bool, uint32_t> class OutputStream, 
        klib::base B, bool Boolalpha, uint32_t Digits
    >
    const OutputStream<B, Boolalpha, Digits> &operator<<(const OutputStream<B, Boolalpha, Digits> &str, char c) {
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
     * @return const OutputStream<B, Boolalpha, Digits>& 
     */
    template <
        template<klib::base, bool, uint32_t> class OutputStream, 
        klib::base B, bool Boolalpha, uint32_t Digits
    >
    const OutputStream<B, Boolalpha, Digits> &operator<<(const OutputStream<B, Boolalpha, Digits> &str, const char *s) {
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
     * @return const OutputStream<B, Boolalpha, Digits>& 
     */
    template <
        typename T,
        template<klib::base, bool, uint32_t> class OutputStream, 
        klib::base B, bool Boolalpha, uint32_t Digits,
        typename = std::enable_if_t<
            std::is_integral_v<T> && (sizeof(T) <= 8)
        >
    >
    const OutputStream<B, Boolalpha, Digits> &operator<<(const OutputStream<B, Boolalpha, Digits> &str, T v) {
        // buffer to store the conversion (max of 8 characters per byte + 3 for 
        // the prefix and null-termintator)
        char buf[(sizeof(T) * 8) + sizeof("  ")] = {};
        
        // convert the value to a string
        string::itoa<
            OutputStream<B, Boolalpha, Digits>::base, 
            OutputStream<B, Boolalpha, Digits>::boolalpha
        >(v, buf);

        // write the buffer to the stream
        str << buf;

        return str;
    }

    /**
     * Write a float/double to the output stream
     *
     * @tparam OutputStream
     * @param str
     * @param v
     * @return
     */
    template <
        template<klib::base, bool, uint32_t> class OutputStream, 
        klib::base B, bool Boolalpha, uint32_t Digits,
        typename T,
        typename = std::enable_if_t<std::is_floating_point_v<T>>
    >
    const OutputStream<B, Boolalpha, Digits> &operator<<(const OutputStream<B, Boolalpha, Digits> &str, T v) {
        // buffer to store the conversion (max of digits after the comma + 8 characters
        // per byte + 3 for the prefix and null-termintator)
        char buf[Digits + (sizeof(T) * 8) + sizeof("  ")] = {};
        
        // convert the value to a string
        string::stoa<Digits>(v, buf);

        // write the buffer to the stream
        str << buf;

        // return the string
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
        template<klib::base, bool, uint32_t> class OutputStream, 
        klib::base B, bool Boolalpha, uint32_t Digits
    >
    const OutputStream<B, Boolalpha, Digits> &operator<<(const OutputStream<B, Boolalpha, Digits> &str, const array_values<T> &values) {
        str << "{";

        for (uint32_t i = 0; i < values.size; ++i) {
            str << values.arr[i];

            if (i + 1 != values.size) {
                str << ", ";
            }
        }
        
        str << "}";

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
        template<klib::base, bool, uint32_t> class OutputStream, 
        klib::base B, bool Boolalpha, uint32_t Digits
    >
    auto operator<<(const OutputStream<B, Boolalpha, Digits>&, const _hex) {
        return typename OutputStream<B, Boolalpha, Digits>::template instance<base::HEX, OutputStream<B, Boolalpha, Digits>::boolalpha, OutputStream<B, Boolalpha, Digits>::digits>();
    }

    /**
     * Output numbers as decimal for this
     * stream.
     *
     * @tparam OutputStream
     * @return
     */
    template <
        template<klib::base, bool, uint32_t> class OutputStream, 
        klib::base B, bool Boolalpha, uint32_t Digits
    >
    auto operator<<(const OutputStream<B, Boolalpha, Digits>&, _dec) {
        return typename OutputStream<B, Boolalpha, Digits>::template instance<base::DEC, OutputStream<B, Boolalpha, Digits>::boolalpha, OutputStream<B, Boolalpha, Digits>::digits>();
    }

    /**
     * Output numbers as octal for
     * this stream.
     *
     * @tparam OutputStream
     * @return
     */
    template <
        template<klib::base, bool, uint32_t> class OutputStream, 
        klib::base B, bool Boolalpha, uint32_t Digits
    >
    auto operator<<(const OutputStream<B, Boolalpha, Digits>&, _oct) {
        return typename OutputStream<B, Boolalpha, Digits>::template instance<base::OCT, OutputStream<B, Boolalpha, Digits>::boolalpha, OutputStream<B, Boolalpha, Digits>::digits>();
    }

    /**
     * Output numbers as binary for
     * this stream.
     *
     * @tparam OutputStream
     * @return
     */
    template <
        template<klib::base, bool, uint32_t> class OutputStream, 
        klib::base B, bool Boolalpha, uint32_t Digits
    >
    auto operator<<(const OutputStream<B, Boolalpha, Digits>&, _bin) {
        return typename OutputStream<B, Boolalpha, Digits>::template instance<base::BIN, OutputStream<B, Boolalpha, Digits>::boolalpha, OutputStream<B, Boolalpha, Digits>::digits>();
    }

    /**
     * Output booleans as 'true' and 'false'
     * for this stream.
     *
     * @tparam OutputStream
     * @return
     */
    template <
        template<klib::base, bool, uint32_t> class OutputStream, 
        klib::base B, bool Boolalpha, uint32_t Digits
    >
    auto operator<<(const OutputStream<B, Boolalpha, Digits>&, _boolalpha) {
        return typename OutputStream<B, Boolalpha, Digits>::template instance<OutputStream<B, Boolalpha, Digits>::base, true, OutputStream<B, Boolalpha, Digits>::digits>();
    }

    /**
     * Output booleans as '1' and '0'
     * for this stream.
     *
     * @tparam OutputStream
     * @return
     */
    template <
        template<klib::base, bool, uint32_t> class OutputStream, 
        klib::base B, bool Boolalpha, uint32_t Digits
    >
    auto operator<<(const OutputStream<B, Boolalpha, Digits>&, _noboolalpha) {
        return typename OutputStream<B, Boolalpha, Digits>::template instance<OutputStream<B, Boolalpha, Digits>::base, false, OutputStream<B, Boolalpha, Digits>::digits>();
    }

    /**
     * Set the precision to 
     * for this stream.
     *
     * @tparam OutputStream
     * @return
     */
    template <
        template<klib::base, bool, uint32_t> class OutputStream, 
        klib::base B, bool Boolalpha, uint32_t Digits, 
        uint32_t NDigits
    >
    auto operator<<(const OutputStream<B, Boolalpha, Digits>&, const precision<NDigits>) {
        return typename OutputStream<B, Boolalpha, Digits>::template instance<OutputStream<B, Boolalpha, Digits>::base, OutputStream<B, Boolalpha, Digits>::boolalpha, NDigits>();
    }
}

#endif