#ifndef KLIB_COPROCESSOR_HPP
#define KLIB_COPROCESSOR_HPP

#include <cstdint>

namespace klib::arm {
    class coprocessor {
    public:
        /**
         * @brief Access rights
         * 
         */
        enum class access {
            denied = 0x0,
            privileged = 0x1,
            full = 0x3
        };

        /**
         * @brief Enable a coprocessor
         * 
         * @tparam CoProcessorId 
         * @param right access right for the coprocessor
         * @param cpacr pointer to the cpacr register
         */
        template <uint32_t CoProcessorId>
        static void set(const access right, volatile uint32_t *const cpacr) {
            static_assert(CoProcessorId < 14, "Max amount of coprocessors is 13");

            // clear the coprocessors first to clear the previous value
            (*cpacr) &= ~(0x3 << (CoProcessorId * 2));

            // set the access right in the register
            (*cpacr) |= static_cast<uint8_t>(right) << (CoProcessorId * 2);
        }
    };
}

#endif