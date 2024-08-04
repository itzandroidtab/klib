#ifndef KLIB_HPP
#define KLIB_HPP

namespace klib {
    // add the namespace for our target
    namespace TARGET_CPU {};

    // set the target (TARGET_CPU is defined in cmakelists
    // of the cpu target)
    namespace target = klib::TARGET_CPU;
}

// check if a cpu is defined
#if defined(TARGET_CPU)
    // define to expand strings
    #define XSTR(x) #x
    #define STR(x) XSTR(x)

    // include the generated source file
    #include STR(TARGET_CPU.hpp)

    // clear the defines
    #undef XSTR
    #undef STR
#else
    #error "No target cpu is defined"
#endif

#endif