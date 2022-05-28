#ifndef KLIB_STREAM_HPP
#define KLIB_STREAM_HPP

// include the stream specific implementations
#include <klib/comm/streams/rtt_stream.hpp>
#include <klib/comm/streams/swo_stream.hpp>

namespace klib {
    #if defined(KLIB_DEFAULT_COUT)
        // create a cout to print data to the host machine
        constexpr klib::KLIB_DEFAULT_COUT::cout cout;
    #endif
    #if defined(KLIB_DEFAULT_CIN)
        // create a cin to print data to the host machine
        constexpr klib::KLIB_DEFAULT_CIN::cin cin;
    #endif
}

#endif
