#ifndef KLIB_UNITS_HPP
#define KLIB_UNITS_HPP

#include <cstdint>
#include <type_traits>

namespace klib {
    enum class base {
        HEX,
        DEC,
        OCT,
        BIN
    };

    namespace {
        constexpr base _default_base = base::DEC;
        constexpr bool _default_boolalpha = false;
    }
}

namespace klib::angle {
    /**
     * @brief Degree unit. Range 0 - 360 
     * 
     */
    struct degree {
        uint16_t value;

        constexpr degree(): value(0) {}
        constexpr degree(uint16_t deg): value(deg) {}
    };
}

namespace klib::time {
    /**
     * @brief Declarations for all the time units
     * 
     */
    struct us;
    struct ms;
    struct s;

    /**
     * @brief Definitions for all the time units
     * 
     */
    struct us {
        uint32_t value;

        constexpr us() : value(0) {}
        constexpr us(uint32_t us) : value(us) {}
        constexpr operator ms() const;
        constexpr operator s() const;
    };

    struct ms {
        uint32_t value;

        constexpr ms() : value(0) {}
        constexpr ms(uint32_t ms) : value(ms) {}
        constexpr operator us() const;
        constexpr operator s() const;
    };

    struct s {
        uint32_t value;

        constexpr s() : value(0) {}
        constexpr s(uint32_t s) : value(s) {}
        constexpr operator us() const;
        constexpr operator ms() const;
    };

    /**
     * @brief Concept to check if we have a time unit
     * 
     * @tparam T 
     */
    template <typename T>
    concept is_time_unit = std::is_same_v<T, us> || std::is_same_v<T, ms> || std::is_same_v<T, s>;

    /**
     * @brief Operators for all the time units
     * 
     */
    constexpr us operator"" _us(const uint64_t value) {
        // get rid of the uint64_t straight away
        return us(static_cast<uint32_t>(value));
    }

    constexpr us::operator ms() const {
        return (value / 1000);
    } 

    constexpr us::operator s() const {
        return (value / 1000 / 1000);
    }

    constexpr ms operator"" _ms(const uint64_t value) {
        // get rid of the uint64_t straight away
        return ms(static_cast<uint32_t>(value));
    }

    constexpr ms::operator us() const {
        return (value * 1000);
    }

    constexpr ms::operator s() const {
        return (value / 1000);   
    }

    constexpr s operator"" _s(const uint64_t value) {
        // get rid of the uint64_t straight away
        return s(static_cast<uint32_t>(value));
    }

    constexpr s::operator us() const {
        return (value * 1000 * 1000);
    }

    constexpr s::operator ms() const {
        return (value * 1000);
    }

    /**
     * @brief Operators for the time units
     * 
     */
    template <typename T, typename G>
    constexpr T operator-(const T lhs, const G rhs) requires is_time_unit<T> && is_time_unit<G> {
        // cast the rhs to the type on the left and substract
        return lhs.value - static_cast<T>(rhs).value;
    }

    template <typename T, typename G>
    constexpr T operator+(const T lhs, const G rhs) requires is_time_unit<T> && is_time_unit<G> {
        // cast the rhs to the type on the left and substract
        return lhs.value + static_cast<T>(rhs).value;
    }

    template <typename T>
    constexpr bool operator==(const T lhs, const T rhs) requires is_time_unit<T> {
        // compare both values
        return lhs.value == rhs.value;
    }

    template <typename T>
    constexpr bool operator!=(const T lhs, const T rhs) requires is_time_unit<T> {
        // use the == operator and invert it
        return !operator==(lhs, rhs);
    }

    template <typename T>
    constexpr bool operator>(const T lhs, const T rhs) requires is_time_unit<T> {
        return lhs.value > rhs.value;
    }

    template <typename T>
    constexpr bool operator>=(const T lhs, const T rhs) requires is_time_unit<T> {
        return lhs.value >= rhs.value;
    }

    template <typename T>
    constexpr bool operator<(const T lhs, const T rhs) requires is_time_unit<T> {
        return lhs.value < rhs.value;
    }

    template <typename T>
    constexpr bool operator<=(const T lhs, const T rhs) requires is_time_unit<T> {
        return lhs.value <= rhs.value;
    }
}

#endif
