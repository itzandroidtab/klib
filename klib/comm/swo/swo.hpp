#ifndef KLIB_SWO_HPP
#define KLIB_SWO_HPP

#include <cstdint>
#include <type_traits>

namespace klib::swo {
    namespace detail {
        // ITM stimulus port
        struct stim_type {
            union {
                uint8_t u8;
                uint16_t u16;
                uint32_t u32;
            };   
        };
    }

    class swo {
    private:
        constexpr static uint8_t stim_size = 32;

        // pointer to the stimulus ports
        static volatile inline detail::stim_type *const stim = ((volatile detail::stim_type*)0xE0000000);

        // pointer to the itm tcr register
        static volatile inline uint32_t *const tcr = ((volatile uint32_t*)0xE0000E80);

        // pointer to the itm ter register
        static volatile inline uint32_t *const ter = ((volatile uint32_t*)0xE0000E80);

        // pointer to the debug exception and monitor control register
        static volatile inline uint32_t *const demcr = ((volatile uint32_t*)0xE000EDFC);

    public:
        /**
         * Write data into the swo register
         *
         * @param Channel
         * @param Blocking
         * @tparam T
         */
        template <
            uint8_t Channel = 0, 
            bool Blocking = true, 
            typename T,
            typename = std::enable_if_t<sizeof(T) <= 4>
        >
        static void write(const T data) {
            static_assert(Channel < stim_size, "Stimulus port only supports up to stim_size of channels");

            // check if ITM is enabled. This class expects the debugger
            // to enable ITM. When it is not enabled it will skip.
            if (!enabled()) {
                return;
            }

            // check if we need to block or not
    	    if constexpr (Blocking) {
                // check if the previous transfer has finished
                while (!stim[Channel].u32) {
                    // wait until it is done
                }              
            }
            else {
                // check if we can write data
                if (!stim[Channel].u32) {
                    // we cannot as we are not blocking return
                    return;
                }
            }

            // write the data based on the correct size
            if constexpr (sizeof(T) == 1) {
                stim[Channel].u8 = *(reinterpret_cast<const uint8_t*>(&data));
            }
            else if constexpr (sizeof(T) == 2) {
                stim[Channel].u16 = *(reinterpret_cast<const uint16_t*>(&data));
            }
            else if constexpr (sizeof(T) > 2) {
                stim[Channel].u32 = *(reinterpret_cast<const uint32_t*>(&data));
            }
        }

        /**
         * @brief Return if the output channel is enabled. (host needs to 
         * enable ITM for this to return true)
         * 
         * @return true 
         * @return false 
         */
        static bool enabled() {
            return ((*tcr & 0x1) && (*demcr & (1 << 24)) && (*ter & 0x1));
        }
    };
}

#endif