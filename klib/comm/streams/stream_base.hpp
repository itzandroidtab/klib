#ifndef KLIB_STREAM_BASE_HPP
#define KLIB_STREAM_BASE_HPP

#include <klib/units.hpp>

namespace klib {
    template <base B = _default_base, bool Boolalpha = _default_boolalpha>
    class ostream {
    public:
        constexpr static klib::base base = B;
        constexpr static bool boolalpha = Boolalpha;

        template <klib::base IB, bool IBoolalpha>
        using instance = ostream<IB, IBoolalpha>;

        // Defined by child implementation
        // Note: no pure abstract or virtual!
        void putc(const char c) const;
    };

    class istream {
    public:
        using instance = istream;

        // Defined by child implementation
        // Note: no pure abstract or virtual!
        char getc() const;

        // Defined by child implementation
        // Note: no pure abstract or virtual!
        bool hasc() const;
    };
}

#endif
