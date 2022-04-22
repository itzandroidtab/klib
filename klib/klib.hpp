#ifndef KLIB_HPP
#define KLIB_HPP

namespace klib {
    // add the namespace for our target
    namespace TARGET_CPU {
        // check if we are using a board definition
        #ifdef TARGET_BOARD
        namespace io::pins {
            namespace board::TARGET_BOARD {};

            // add board to pin namespace
            using namespace board::TARGET_BOARD;
        }
        #endif
    };
    
    // set the target (TARGET_CPU is defined in cmakelists 
    // of the cpu target)
    namespace target = klib::TARGET_CPU;
}

// import our board files when we have them
#if defined(TARGET_CPU) && defined(TARGET_BOARD)
    // define to expand strings
    #define XSTR(x) #x
    #define STR(x) XSTR(x)

    // use define to expand target board define
    #include STR(board/TARGET_BOARD/pins.hpp)
#endif

#endif