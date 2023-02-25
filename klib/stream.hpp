#ifndef KLIB_STREAM_HPP
#define KLIB_STREAM_HPP

// include the stream specific implementations
#include <klib/comm/streams/swo_stream.hpp>

// prevent the segger stream from being included
// if we do not have segger support enabled
#if (KLIB_SEGGER_SUPPORT == true)
    #include <klib/comm/streams/rtt_stream.hpp>
#endif

namespace klib {
    // check if we have a default stream we need to add.
    // make sure that when RTT is setup we check if we 
    // have segger support enabled.
    #if defined(KLIB_DEFAULT_COUT) && !(KLIB_DEFAULT_COUT == rtt && KLIB_SEGGER_SUPPORT == false)
        // create a cout to print data to the host machine	
        constexpr klib::KLIB_DEFAULT_COUT::cout cout;
    #endif
    #if defined(KLIB_DEFAULT_CIN) && !(KLIB_DEFAULT_CIN == rtt && KLIB_SEGGER_SUPPORT == false)
        // create a cin to print data to the host machine
        constexpr klib::KLIB_DEFAULT_CIN::cin cin;
    #endif
}

#endif
