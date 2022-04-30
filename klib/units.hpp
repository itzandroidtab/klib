#ifndef KLIB_UNITS_HPP
#define KLIB_UNITS_HPP

#include <cstdint>

namespace klib::time {
    struct ms {
        uint32_t value;

        constexpr ms() : value(0) {}
        constexpr ms(uint32_t ms) : value(ms) {}
    };

    constexpr ms operator"" _ms(const uint64_t value) {
        // get rid of the uint64_t straight away
        return ms(static_cast<uint32_t>(value));
    }

    struct s {
        uint32_t value;

        constexpr s() : value(0) {}
        constexpr s(uint32_t s) : value(s) {}
    };

    constexpr s operator"" _s(const uint64_t value) {
        // get rid of the uint64_t straight away
        return s(static_cast<uint32_t>(value));
    }
}

#endif
