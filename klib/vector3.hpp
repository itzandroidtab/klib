#ifndef KLIB_VECTOR3_HPP
#define KLIB_VECTOR3_HPP

#include "math.hpp"

namespace klib {
    /**
     * Templated 2-dimensional vector
     * class.
     *
     * @tparam T
     */
    template <typename T>
    struct vector3 {
        T x;
        T y;
        T z;

        /**
         * Default; initialize to zero.
         */
        constexpr vector3() : x(0), y(0), z(0) {}

        /**
         * Initialize the vector with the given
         * values.
         *
         * @param x
         * @param y
         */
        constexpr vector3(const T x, const T y, const T z)
            : x(x), y(y), z(z) {}

        /**
         * Initialize both values with
         * the given value.
         *
         * @param val
         */
        constexpr explicit vector3(const T val)
            : x(val), y(val), z(val) {}

        /**
         * @param other
         * @return
         */
        constexpr vector3<T> operator+(const vector3<T> &other) const {
            return vector3<T>(x + other.x, y + other.y, z + other.z);
        }

        /**
         * @param other
         * @return
         */
        constexpr vector3<T> &operator+=(const vector3<T> &other) {
            x += other.x;
            y += other.y;
            z += other.z;

            return *this;
        }

        /**
         * @param other
         * @return
         */
        constexpr vector3<T> operator+(const T other) const {
            return vector3<T>(x + other, y + other, z + other);
        }

        /**
         * @param other
         * @return
         */
        constexpr vector3<T> &operator+=(const T other) {
            x += other;
            y += other;
            z += other;

            return *this;
        }

        /**
         * @param other
         * @return
         */
        constexpr vector3<T> operator-(const vector3<T> &other) const {
            return vector3<T>(x - other.x, y - other.y, z - other.z);
        }

        /**
         * @param other
         * @return
         */
        constexpr vector3<T> &operator-=(const vector3<T> &other) {
            x -= other.x;
            y -= other.y;
            z -= other.z;

            return *this;
        }

        /**
         * @param other
         * @return
         */
        constexpr vector3<T> operator-(const T other) const {
            return vector3<T>(x - other, y - other, z - other);
        }

        /**
         * @param other
         * @return
         */
        constexpr vector3<T> &operator-=(const T other) {
            x -= other;
            y -= other;
            z -= other;

            return *this;
        }

        /**
         * @param other
         * @return
         */
        constexpr vector3<T> operator*(const vector3<T> &other) const {
            return vector3<T>(x * other.x, y * other.y, z * other.z);
        }

        /**
         * @param other
         * @return
         */
        constexpr vector3<T> &operator*=(const vector3<T> &other) {
            x *= other.x;
            y *= other.y;
            z *= other.z;

            return *this;
        }

        /**
         * @param other
         * @return
         */
        constexpr vector3<T> operator*(const T other) const {
            return vector3<T>(x * other, y * other, z * other);
        }

        /**
         * @param other
         * @return
         */
        constexpr vector3<T> &operator*=(const T other) {
            x *= other;
            y *= other;
            z *= other;

            return *this;
        }

        /**
         * @param other
         * @return
         */
        constexpr vector3<T> operator/(const vector3<T> &other) const {
            return vector3<T>(x / other.x, y / other.y, z / other.z);
        }

        /**
         * @param other
         * @return
         */
        constexpr vector3<T> &operator/=(const vector3<T> &other) {
            x /= other.x;
            y /= other.y;
            z /= other.z;

            return *this;
        }

        /**
         * @param other
         * @return
         */
        constexpr vector3<T> operator/(const T other) const {
            return vector3<T>(x / other, y / other, z / other);
        }

        /**
         * @param other
         * @return
         */
        constexpr vector3<T> &operator/=(const T other) {
            x /= other;
            y /= other;
            z /= other;

            return *this;
        }

        /**
         * @param other
         * @return bool
         */
        constexpr bool operator==(const vector3<T> other) const {
            return (x == other.x) && (y == other.y) && (z == other.z);
        }

        /**
         * @param other
         * @return bool
         */
        constexpr bool operator!=(const vector3<T> other) const {
            return !operator==(other);
        }

        /**
         * Helper function for casting a
         * vector to another vector.
         *
         * @tparam U
         * @return
         */
        template <typename U>
        constexpr vector3<U> cast() const {
            return vector3<U>(
                static_cast<U>(x),
                static_cast<U>(y),
                static_cast<U>(z)
            );
        }
    };

    // some nice aliases
    using vector3i = vector3<int32_t>;
    using vector3u = vector3<uint32_t>;
}

#endif
