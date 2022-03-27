#ifndef KLIB_HPP
#define KLIB_HPP

namespace klib {
    // add the namespace for our target
    namespace TARGET_CPU {};
    
    // set the target (TARGET_CPU is defined in cmakelists 
    // of the cpu target)
    namespace target = klib::TARGET_CPU;
}

#endif