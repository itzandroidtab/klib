#ifndef KLIB_UNITS_HPP
#define KLIB_UNITS_HPP

#include <cstdint>
#include <type_traits>

#include "math.hpp"

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
     * @brief Available time units
     * 
     */
    enum class time_unit: uint8_t {
        ns = 0,
        us,
        ms,
        s
    };

    /**
     * @brief Declarations for all the time units
     * 
     */
    struct ns;
    struct us;
    struct ms;
    struct s;

    /**
     * @brief Definitions for all the time units
     * 
     */
    struct ns {
        uint32_t value;

        constexpr ns(): value(0) {}
        constexpr ns(uint32_t ns): value(ns) {}
        constexpr operator us() const;
        constexpr operator ms() const;
        constexpr operator s() const;
    };

    struct us {
        uint32_t value;

        constexpr us(): value(0) {}
        constexpr us(uint32_t us): value(us) {}
        constexpr operator ns() const;
        constexpr operator ms() const;
        constexpr operator s() const;
    };

    struct ms {
        uint32_t value;

        constexpr ms(): value(0) {}
        constexpr ms(uint32_t ms): value(ms) {}
        constexpr operator ns() const;
        constexpr operator us() const;
        constexpr operator s() const;
    };

    struct s {
        uint32_t value;

        constexpr s(): value(0) {}
        constexpr s(uint32_t s): value(s) {}
        constexpr operator ns() const;
        constexpr operator us() const;
        constexpr operator ms() const;
    };

    /**
     * @brief Concept to check if we have a time unit
     * 
     * @tparam T 
     */
    template <typename T>
    concept is_time_unit = (
        std::is_same_v<T, ns> || std::is_same_v<T, us> || 
        std::is_same_v<T, ms> || std::is_same_v<T, s>
    );

    /**
     * @brief Converts a time struct to a time unit
     * 
     * @tparam T 
     * @return consteval 
     */
    template <typename T>
    consteval time_unit time_to_unit() requires is_time_unit<T> {
        if constexpr (std::is_same_v<T, ns>) {
            return time_unit::ns;
        }
        else if constexpr (std::is_same_v<T, us>) {
            return time_unit::us;
        }
        else if constexpr (std::is_same_v<T, ms>) {
            return time_unit::ms;
        }
        else {
            // not anything else return seconds
            return time_unit::s;
        }
    }

    /**
     * @brief Gets the conversion factor to convert between 
     * time units. Only returns the difference between the
     * units. Does not return the direction
     * 
     * @tparam T 
     * @tparam G 
     * @return consteval 
     */
    template <typename T, typename G>
    consteval uint32_t conversion_factor() requires is_time_unit<T> && is_time_unit<G> {
        const uint32_t value[] = {
            1000 * 1000 * 1000,
            1000 * 1000,
            1000,
            1
        };

        // get the enum value of the type we have
        const uint8_t from = static_cast<uint8_t>(time_to_unit<T>());
        const uint8_t to = static_cast<uint8_t>(time_to_unit<G>());

        // return the difference
        return klib::max(value[from], value[to]) / klib::min(value[from], value[to]);
    }

    /**
     * @brief Operators for all the time units
     * 
     */
    constexpr ns operator"" _ns(const uint64_t value) {
        // get rid of the uint64_t straight away
        return ns(static_cast<uint32_t>(value));
    }

    constexpr ns::operator us() const {
        return (value * conversion_factor<ns, us>());
    } 

    constexpr ns::operator ms() const {
        return (value * conversion_factor<ns, ms>());
    } 

    constexpr ns::operator s() const {
        return (value * conversion_factor<ns, s>());
    }

    constexpr us operator"" _us(const uint64_t value) {
        // get rid of the uint64_t straight away
        return us(static_cast<uint32_t>(value));
    }

    constexpr us::operator ns() const {
        return (value * conversion_factor<us, ns>());
    } 

    constexpr us::operator ms() const {
        return (value / conversion_factor<us, ms>());
    } 

    constexpr us::operator s() const {
        return (value / conversion_factor<us, s>());
    }

    constexpr ms operator"" _ms(const uint64_t value) {
        // get rid of the uint64_t straight away
        return ms(static_cast<uint32_t>(value));
    }

    constexpr ms::operator ns() const {
        return (value * conversion_factor<ms, ns>());
    } 

    constexpr ms::operator us() const {
        return (value * conversion_factor<ms, us>());
    }

    constexpr ms::operator s() const {
        return (value / conversion_factor<ms, s>());
    }

    constexpr s operator"" _s(const uint64_t value) {
        // get rid of the uint64_t straight away
        return s(static_cast<uint32_t>(value));
    }

    constexpr s::operator ns() const {
        return (value * conversion_factor<s, ns>());
    } 

    constexpr s::operator us() const {
        return (value * conversion_factor<s, us>());
    }

    constexpr s::operator ms() const {
        return (value * conversion_factor<s, ms>());
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
