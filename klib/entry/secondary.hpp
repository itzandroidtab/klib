#ifndef KLIB_SECONDARY_HPP
#define KLIB_SECONDARY_HPP

#include <cstdint>

namespace klib::entry {
    /**
     * @brief A secondary memory loader. This loader should be 
     * called afer the memories are initialized by the user 
     * code. Constructor(102) is reserved for this function.
     *
     */
    void secondary_memory_loader();
}

#endif