#ifndef KLIB_LPC55S66_SYSTEM_HPP
#define KLIB_LPC55S66_SYSTEM_HPP

#include <lpc55s66.hpp>

#include <klib/io/core_clock.hpp>

namespace klib::lpc55s66::io::system {
    class cpu1 {
    public:
        /**
         * @brief Start CPU1
         * 
         * @param vectors 
         */
        static void start(const auto* vectors) {
            // set the vector table for CPU1
            SYSCON->CPBOOT = reinterpret_cast<uint32_t>(vectors);

            // enable CPU1 in the configuration register
            SYSCON->CPUCFG |= 0x1 << 2;

            // enable the cpu and reset it
            SYSCON->CPUCTRL = (0x1 << 5) | (0x1 << 3) | (0xc0c4 << 16);

            // disable the reset on CPU1 and enable the CPU1 clock
            SYSCON->CPUCTRL = (0x1 << 3) | (0xc0c4 << 16);
        }
    };

    class debug {
    public:
        /**
         * @brief Enable DAP on the provided cpu
         * 
         * @tparam CpuId 
         */
        template <uint32_t CpuId>
        static void enable() {
            if constexpr (CpuId == 0) {
                // enable debug features on CPU0
                SYSCON->DEBUG_FEATURES = 0xaa | (SYSCON->DEBUG_FEATURES & (~0xff));
                SYSCON->DEBUG_FEATURES_DP = 0xaa | (SYSCON->DEBUG_FEATURES_DP & (~0xff));

                // enable swd on CPU0
                SYSCON->CODESECURITYPROTCPU0 = 0x12345678;
            }
            else {
                // enable debug features on CPU1
                SYSCON->DEBUG_FEATURES = (0xa << 8) | (SYSCON->DEBUG_FEATURES & (~0xf00));
                SYSCON->DEBUG_FEATURES_DP = (0xa << 8) | (SYSCON->DEBUG_FEATURES_DP & (~0xf00));

                // enable swd on CPU1
                SYSCON->CODESECURITYPROTCPU1 = 0x12345678;
            }
        }
    };
}

#endif