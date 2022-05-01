#ifndef SWO_STREAM_HPP
#define SWO_STREAM_HPP

#include <klib/comm/swo/swo.hpp>

#include "stream_base.hpp"
#include "ostream.hpp"
#include "istream.hpp"

namespace klib {
    namespace swo {
        template<base B = _default_base, bool Boolalpha = _default_boolalpha>
        struct cout: public ostream<B, Boolalpha> {
            template<klib::base IB, bool IBoolalpha>
            using instance = cout<IB, IBoolalpha>;

            void putc(const char c) const {
                swo::write<0, true>(c);
            }
        };
    }
}

#endif