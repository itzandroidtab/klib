#ifndef KLIB_STREAM_BASE_HPP
#define KLIB_STREAM_BASE_HPP

#include <klib/vector2.hpp>

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

    template<base B = _default_base, bool Boolalpha = _default_boolalpha>
    class ostream {
    public:
        constexpr static klib::base base = B;
        constexpr static bool boolalpha = Boolalpha;

        template<klib::base IB, bool IBoolalpha>
        using instance = ostream<IB, IBoolalpha>;

        // Defined by child implementation
        // Note: no pure abstract or virtual!
        void putc(const char c) const;

        void setpos(const vector2u &pos) {}
    };

    class istream {
    public:
        using instance = istream;

        // Defined by child implementation
        // Note: no pure abstract or virtual!
        char getc() const;
    };
}

#endif
