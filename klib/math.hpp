#ifndef KLIB_MATH_HPP
#define KLIB_MATH_HPP

#include <limits>
#include <type_traits>
#include <cstdint>
#include <cmath>

namespace klib {
    template <
        typename T,
        typename = std::enable_if_t<std::is_floating_point_v<T>>
    >
    constexpr bool isnan(const T arg) {
        return __builtin_isnan(arg);
    }

    template <
        typename T,
        typename = std::enable_if_t<std::is_floating_point_v<T>>
    >
    constexpr bool isinf(const T arg) {
        return __builtin_isinf(arg);
    }    

    /**
     * Generic pow function.
     *
     * @tparam T
     * @param base
     * @param exponent
     * @return
     */
    template<typename T>
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
     * Round the given number up.
     * If the arg is ±infinity, the result is not modified.
     * If the arg is ±0, the result is not modified.
     * If the arg is NaN, NaN is returned.
     *
     * @param arg
     * @return
     */
    constexpr double ceil(double arg) {
        if (isinf(arg)) {
            return arg;
        }

        if (arg == 0.0) {
            return arg;
        }

        if (isnan(arg)) {
            return NAN;
        }

        // Negative ceiling
        if (arg < 0.0) {
            return static_cast<double>(
                static_cast<int64_t>(arg)
            );
        }

        // Positive ceiling
        return static_cast<double>(
            static_cast<int64_t>(arg)
        );
    }

    /**
    * Round the given number up.
    * If the arg is ±infinity, the result is not modified.
    * If the arg is ±0, the result is not modified.
    * If the arg is NaN, NaN is returned.
    *
    * @param arg
    * @return
    */
    constexpr float ceil(float arg) {
        return static_cast<float>(
            ceil(static_cast<double>(arg))
        );
    }

    /**
    * Round the given number up.
    * If the arg is ±infinity, the result is not modified.
    * If the arg is ±0, the result is not modified.
    * If the arg is NaN, NaN is returned.
    *
    * @tparam T
    * @param arg
    * @return
    */
    template<
        typename T,
        typename = std::enable_if_t<std::is_integral_v<T>>
    >
    constexpr double ceil(T arg) {
        return static_cast<double>(arg);
    }

    /**
     * Floor the given number.
     * If the arg is ±infinity, the result is not modified.
     * If the arg is ±0, the result is not modified.
     * If the arg is NaN, NaN is returned.
     *
     * @param arg
     * @return
     */
    constexpr double floor(double arg) {
        if (isinf(arg)) {
            return arg;
        }

        if (arg == 0.0) {
            return arg;
        }

        if (isnan(arg)) {
            return NAN;
        }

        // Negative flooring
        if (arg < 0.0) {
            return static_cast<double>(
                static_cast<int64_t>(arg) - 1
            );
        }

        // Positive flooring
        return static_cast<double>(
            static_cast<int64_t>(arg)
        );
    }

    /**
     * Floor the given number.
     * If the arg is ±infinity, the result is not modified.
     * If the arg is ±0, the result is not modified.
     * If the arg is NaN, NaN is returned.
     *
     * @param arg
     * @return
     */
    constexpr float floor(float arg) {
        return static_cast<float>(
            floor(static_cast<double>(arg))
        );
    }

    /**
     * Floor the given number.
     * If the arg is ±infinity, the result is not modified.
     * If the arg is ±0, the result is not modified.
     * If the arg is NaN, NaN is returned.
     *
     * @tparam T
     * @param arg
     * @return
     */
    template<
        typename T,
        typename = std::enable_if_t<std::is_integral_v<T>>
    >
    constexpr double floor(T arg) {
        return static_cast<double>(arg);
    }

    template<
        typename T,
        typename = std::enable_if_t<std::is_integral_v<T>>
    >
    constexpr T sqrt(T input) {
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

    template<typename T>
    constexpr T min(const T a, const T b) {
        return (a > b) ? b : a;
    }

    template<typename T>
    constexpr T max(const T a, const T b) {
        return (a > b) ? a : b;
    }

    template<typename T>
    constexpr T abs(const T x) {
        if constexpr (std::is_unsigned_v<T>) {
            return x;
        } else {
            return x < 0 ? -x : x;
        }
    }

    constexpr int log2(const uint32_t n) {
        // the buildin clz should get replaced by the 
        // compiler for the clz arm instruction
        return 31 - __builtin_clz(n);
    }

    inline double log(const double n) {
        int l2 = log2(static_cast<uint32_t>(n));
        auto divisor = static_cast<double>(1 << l2);
        double x = n / divisor;    // normalized value between [1.0, 2.0]

        double result = -1.7417939 + (2.8212026 + (-1.4699568 + (0.44717955 - 0.056570851 * x) * x) * x) * x;
        result += static_cast<double>(l2) * 0.69314718; // ln(2) = 0.69314718

        return result;
    }

    inline double log10(const double n) {
        constexpr double ln10 = 2.3025850929940456840179914546844;
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
    template<typename T, typename G = T>
    constexpr T map(const T x, const G in_min, const G in_max, const G out_min, const G out_max){
        return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
    }

}

#endif
