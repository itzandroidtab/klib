#ifndef KLIB_STREAM_HPP
#define KLIB_STREAM_HPP

// include the stream specific implementations
#include <klib/comm/streams/swo_stream.hpp>
#include <klib/comm/streams/rtt_stream.hpp>

namespace klib {
    // check if we have a default stream we need to add.
    // make sure that when RTT is setup we check if we 
    // have segger support enabled.
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
