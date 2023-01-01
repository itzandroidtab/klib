#ifndef KLIB_COMBINED_STREAM_HPP
#define KLIB_COMBINED_STREAM_HPP

#include "stream_base.hpp"

namespace klib {
    /**
     * The combined stream class is a helper class that
     * copies the values streamed to it to both streams provided.
     *
     * @tparam S1
     * @tparam S2
     * @tparam Ostream
     */
    template <typename S1, typename S2, typename Ostream = klib::ostream<>>
    class combined_stream : public Ostream {
    protected:
        S1 &s1;
        S2 &s2;

    public:
        combined_stream(S1 &s1, S2 &s2)
            : s1(s1), s2(s2) {}

        void putc(char c) {
            s1.putc(c);
            s2.putc(c);
        }
    };

    /**
     * Helper function to combine two streams.
     * 'Tee' is a reference to the Unix utility that
     * duplicates a stream.
     *
     * @tparam S1
     * @tparam S2
     * @tparam Ostream
     * @param s1
     * @param s2
     * @return
     */
    template <typename S1, typename S2, typename Ostream = klib::ostream<>>
    combined_stream<S1, S2, Ostream> tee(S1 &s1, S2 &s2) {
        return combined_stream(s1, s2);
    }
}

#endif
