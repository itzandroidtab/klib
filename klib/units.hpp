#ifndef KLIB_UNITS_HPP
#define KLIB_UNITS_HPP

#include <cstdint>

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

namespace klib::time {
    struct us;
    struct ms;
    struct s;

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

    constexpr us operator"" _us(const uint64_t value) {
        // get rid of the uint64_t straight away
        return us(static_cast<uint32_t>(value));
    }

    constexpr ms operator"" _ms(const uint64_t value) {
        // get rid of the uint64_t straight away
        return ms(static_cast<uint32_t>(value));
    }

    constexpr s operator"" _s(const uint64_t value) {
        // get rid of the uint64_t straight away
        return s(static_cast<uint32_t>(value));
    }

    constexpr us::operator ms() const {
        return (value / 1000);
    } 

    constexpr us::operator s() const {
        return (value / 1000 / 1000);
    } 

    constexpr ms::operator us() const {
        return (value * 1000);
    }

    constexpr ms::operator s() const {
        return (value / 1000);   
    }

    constexpr s::operator us() const {
        return (value * 1000 * 1000);
    }

    constexpr s::operator ms() const {
        return (value * 1000);
    }

    constexpr us operator-(const us lhs, const us rhs) {
        return lhs.value - rhs.value;
    }

    constexpr ms operator-(const ms lhs, const ms rhs) {
        return lhs.value - rhs.value;
    }

    constexpr s operator-(const s lhs, const s rhs) {
        return lhs.value - rhs.value;
    }
}

#endif
