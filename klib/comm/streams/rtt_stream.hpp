#ifndef RTT_STREAM_HPP
#define RTT_STREAM_HPP

#include <klib/comm/segger/rtt.hpp>

#include "stream_base.hpp"
#include "ostream.hpp"
#include "istream.hpp"

namespace klib {
    namespace rtt {
        template <base B = _default_base, bool Boolalpha = _default_boolalpha, uint32_t Digits = 5>
        struct cout: public ostream<B, Boolalpha> {
            template <klib::base IB, bool IBoolalpha, uint32_t IDigits>
            using instance = cout<IB, IBoolalpha, IDigits>;

            void putc(const char c) const {
                rtt::write<0>(&c, sizeof(c));
            }
        };

        class cin: public istream {
        public:
            using instance = cin;

            char getc() const {
                return rtt::getc<0, true>();
            }

            bool hasc() const {
                return rtt::hasc<0>();
            }
        };
    }
}

#endif