#ifndef KLIB_CLOCK_HPP
#define KLIB_CLOCK_HPP

#include <cstdint>

namespace klib::io {
    class clock {
    protected:
        // clock frequency of the cpu
        static inline uint32_t core_clock;

    public:
        /**
         * @brief Get the core clock speed of the cpu
         * 
         */
        static uint32_t get() {
            return core_clock;
        }

        /**
         * @brief Change the core clock
         * 
         * @param clock 
         */
        static void set(const uint32_t clock) {
            core_clock = clock;
        }
    };
}

#endif