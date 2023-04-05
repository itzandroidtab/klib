#ifndef KLIB_MATH_HPP
#define KLIB_MATH_HPP

#include <limits>
#include <type_traits>
#include <cstdint>

namespace klib {
    /**
     * @brief Return if the input is not a number
     * 
     * @tparam T 
     * @param arg 
     * @return true 
     * @return false 
     */
    template <
        typename T,
        typename = std::enable_if_t<std::is_floating_point_v<T>>
    >
    constexpr bool isnan(const T arg) {
        return __builtin_isnan(arg);
    }

    /**
     * @brief Return if the input is infinite
     * 
     * @tparam T 
     * @param arg 
     * @return true 
     * @return false 
     */
    template <
        typename T,
        typename = std::enable_if_t<std::is_floating_point_v<T>>
    >
    constexpr bool isinf(const T arg) {
        return __builtin_isinf(arg);
    }    

    /**
     * @brief 16 bit byteswap (converts 0xaabb to 0xbbaa)
     * 
     * @param data 
     * @return constexpr uint16_t 
     */
    constexpr uint16_t bswap(const uint16_t data) {
        return __builtin_bswap16(data);
    }

    /**
     * @brief 32 bit byteswap (converts 0xaabb to 0xbbaa)
     * 
     * @param data 
     * @return constexpr uint32_t 
     */
    constexpr uint32_t bswap(const uint32_t data) {
        return __builtin_bswap32(data);
    }

    /**
     * @brief 64 bit byteswap (converts 0xaabb to 0xbbaa)
     * 
     * @param data 
     * @return constexpr uint64_t 
     */
    constexpr uint64_t bswap(const uint64_t data) {
        return __builtin_bswap64(data);
    }

    /**
     * @brief 16 bit byteswap (converts 0xaabb to 0xbbaa). Explicit 16 bit
     * 
     * @tparam T 
     * @param data 
     * @return constexpr uint16_t 
     */
    template <typename T>
    constexpr uint16_t bswap16(const T data) {
        return bswap(static_cast<uint16_t>(data));
    }

    /**
     * @brief 32 bit byteswap (converts 0xaabb to 0xbbaa). Explicit 32 bit
     * 
     * @tparam T 
     * @param data 
     * @return constexpr uint32_t 
     */
    template <typename T>
    constexpr uint32_t bswap32(const T data) {
        return bswap(static_cast<uint32_t>(data));
    }

    /**
     * @brief 32 bit byteswap (converts 0xaabb to 0xbbaa). Explicit 32 bit
     * 
     * @tparam T 
     * @param data 
     * @return constexpr uint64_t 
     */
    template <typename T>
    constexpr uint64_t bswap64(const T data) {
        return bswap(static_cast<uint64_t>(data));
    }


    /**
     * @brief Count leading zero's
     * 
     * @param data 
     * @return constexpr uint32_t 
     */
    constexpr uint32_t clz(const uint32_t data) {
        // handle the 0 case as buildin_clz(0) is undefined
        if (data == 0x00) {
            return 32;
        }

        return __builtin_clz(data);
    }

    /**
     * @brief Count trailing zero's
     * 
     * @param data 
     * @return constexpr uint32_t 
     */
    constexpr uint32_t ctz(const uint32_t data) {
        // handle the 0 case as buildin_ctz(0) is undefined
        if (data == 0x00) {
            return 32;
        }

        return __builtin_ctz(data);
    }

    /**
     * @brief Return the amount of bits set to 1
     * 
     * @param data 
     * @return constexpr uint32_t 
     */
    constexpr uint32_t popcount(const uint32_t data) {
        return __builtin_popcount(data);
    }

    /**
     * @brief Computes 2 raised to the given power
     * 
     * @param exponent 
     * @return constexpr uint32_t 
     */
    template <typename Precision = uint32_t>
    constexpr Precision exp2(const uint32_t exponent) {
        return static_cast<Precision>(1) << exponent;
    }

    /**
     * @brief Generic pow function.
     * 
     * @warning This function is recursive.
     *
     * @tparam T
     * @param base
     * @param exponent
     * @return
     */
    template <typename T>
    constexpr T pow(const T base, const int_fast32_t exponent) {
        if (exponent == 0) {
            return 1;
        }

        if (exponent == 1) {
            return base;
        }

        if ((exponent & 1) == 0) {
            T rest = pow(base, exponent / 2);
            return rest * rest;
        }

        return base * pow(base, exponent - 1);
    }

    /**
     * @brief Round the given number up.
     * If the arg is ±infinity, the result is not modified.
     * If the arg is ±0, the result is not modified.
     * If the arg is NaN, NaN is returned.
     * 
     * @tparam T 
     * @param arg 
     * @return constexpr T 
     */
    template <
        typename T,
        typename = std::enable_if_t<std::is_floating_point_v<T>>
    >
    constexpr T ceil(T arg) {
        if (isinf(arg)) {
            return arg;
        }

        if (arg == 0.0) {
            return arg;
        }

        if (isnan(arg)) {
            return (__builtin_nanf(""));
        }

        // Negative ceiling
        if (arg < 0.0) {
            return static_cast<T>(
                static_cast<int64_t>(arg)
            );
        }

        // Positive ceiling
        return static_cast<T>(
            static_cast<int64_t>(arg)
        );
    }

    /**
     * @brief Floor the given number.
     * If the arg is ±infinity, the result is not modified.
     * If the arg is ±0, the result is not modified.
     * If the arg is NaN, NaN is returned.
     * 
     * @tparam T 
     * @param arg 
     * @return constexpr double 
     */
    template <
        typename T,
        typename = std::enable_if_t<std::is_floating_point_v<T>>
    >
    constexpr T floor(const T arg) {
        if (isinf(arg)) {
            return arg;
        }

        if (arg == 0.0) {
            return arg;
        }

        if (isnan(arg)) {
            return (__builtin_nanf(""));
        }

        // Negative flooring
        if (arg < 0.0) {
            return static_cast<T>(
                static_cast<int64_t>(arg) - 1
            );
        }

        // Positive flooring
        return static_cast<T>(
            static_cast<int64_t>(arg)
        );
    }

    /**
     * @brief Calculate squre root of input 
     * 
     * @tparam T 
     * @param input 
     * @return constexpr T 
     */
    template <
        typename T,
        typename = std::enable_if_t<std::is_unsigned_v<T>>
    >
    constexpr T sqrt(const T input) {
        // https://stackoverflow.com/questions/1100090/looking-for-an-efficient-integer-square-root-algorithm-for-arm-thumb2
        T op = input;
        T res = 0;
        T one = 1uL << (sizeof(T) * 8 - 2); // Amount of bits - 2


        // "one" starts at the highest power of four <= than the argument.
        while (one > op) {
            one >>= 2;
        }

        while (one != 0) {
            if (op >= res + one) {
                op = op - (res + one);
                res = res + 2 * one;
            }

            res >>= 1;
            one >>= 2;
        }

        return res;
    }

    /**
     * @brief Get the lowest value between the two inputs
     * 
     * @tparam T 
     * @tparam G
     * @param a 
     * @param b 
     * @return constexpr auto 
     */
    template <typename T, typename G>
    constexpr auto min(const T a, const G b) {
        return (a > b) ? b : a;
    }

    /**
     * @brief Get the max between the two input values
     * 
     * @tparam T 
     * @tparam G
     * @param a 
     * @param b 
     * @return constexpr auto 
     */
    template <typename T, typename G>
    constexpr auto max(const T a, const G b) {
        return (a > b) ? a : b;
    }

    /**
     * @brief Return the absolute value of the input (positive only)
     * 
     * @tparam T 
     * @param x 
     * @return T 
     */
    template <typename T>
    constexpr T abs(const T x) {
        if constexpr (std::is_unsigned_v<T>) {
            return x;
        } else {
            return x < 0 ? -x : x;
        }
    }

    /**
     * @brief Returns the binary (base-2) logarithm of the input
     * 
     * @param n 
     * @return constexpr int 
     */
    constexpr int log2(const uint32_t n) {
        // the buildin clz should get replaced by the 
        // compiler for the clz arm instruction
        return 31 - __builtin_clz(n);
    }

    /**
     * @brief Returns the natural logarithm of the input
     * 
     * @tparam P
     * @tparam T 
     * @param n 
     * @return constexpr T 
     */
    template <typename P = float, typename T>
    constexpr P log(const T n) {
        const int l2 = log2(static_cast<uint32_t>(n));
        const auto divisor = static_cast<P>(1 << l2);
        const P x = n / divisor;    // normalized value between [1.0, 2.0]

        P result = static_cast<P>(-1.7417939) + (static_cast<P>(2.8212026) + 
            (static_cast<P>(-1.4699568) + (static_cast<P>(0.44717955) - static_cast<P>(0.056570851) * x) * x) * x) * x;
        result += static_cast<P>(l2) * 0.69314718; // ln(2) = 0.69314718

        return result;
    }

    /**
     * @brief Returns the common (base-10) logarithm 
     * 
     * @tparam P 
     * @tparam T 
     * @param n 
     * @return constexpr T 
     */
    template <typename P = float, typename T>
    constexpr P log10(const T n) {
        constexpr P ln10 = 2.3025850929940456840179914546844;
        return log(n) / ln10;
    }

    /**
     * Map a range of values to another range of values.
     * E.g. map 0...255 tot 0...1024.
     * @tparam T
     * @tparam G
     * @param x
     * @param in_min
     * @param in_max
     * @param out_min
     * @param out_max
     * @return
     */
    template <typename T, typename G = T>
    constexpr T map(const T x, const G in_min, const G in_max, const G out_min, const G out_max){
        return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
    }
}

#endif
