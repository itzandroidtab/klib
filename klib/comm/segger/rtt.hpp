#ifndef KLIB_RTT_HPP
#define KLIB_RTT_HPP

#include <cstdint>

// prevent the segger header from being included
// if we do not have segger support enabled
#if (KLIB_SEGGER_SUPPORT == true)
    #include "src/RTT/SEGGER_RTT.h"
#endif

namespace klib::rtt {
#if (KLIB_SEGGER_SUPPORT == true)
    /**
     * @brief Rtt class that can communicate with a host
     * 
     */
    class rtt {
    public:
        /**
         * @brief Only needed to make the write and getc consistent timing on every call
         * as init is called in write and getc if not initialized
         * 
         */
        static void init() {
            // init the segger library
            SEGGER_RTT_Init();
        }

        /**
         * @brief Write a data to a defined channel
         * 
         * @tparam Channel 
         * @param data 
         * @param size 
         */
        template <uint8_t Channel = 0, typename T>
        static void write(const T data, const uint32_t size) {
            // write the character to the rtt buffer
            SEGGER_RTT_Write(Channel, reinterpret_cast<const uint8_t*>(&data), size);
        }

        /**
         * @brief Get a character from the SEGGER RTT buffer. 
         * 
         * @tparam Channel 
         * @tparam Blocking 
         * @return char
         */
        template <uint8_t Channel = 0, bool Blocking = true>
        static char getc() {
            char ret = 0;

            if constexpr (Blocking) {
                // try to read a character from the RTT buffer
                while (SEGGER_RTT_Read(Channel, &ret, sizeof(ret)) <= 0) {
                    // wait until we have a character
                }
            }
            else {
                // read from the RTT buffer and just return if we dont have anything
                SEGGER_RTT_Read(Channel, &ret, sizeof(ret));
            }

            return ret;
        }

        /**
         * @brief Check if a character is available in the RTT buffer
         * 
         * @tparam Channel 
         * @return true 
         * @return false 
         */
        template <uint8_t Channel = 0>
        static bool hasc() {
            // return if we have data available
            return SEGGER_RTT_HasData(Channel);
        }
    };

#else
    /**
     * @brief Class to prevent compile errors while we 
     * do not have segger rtt support enabled
     * 
     */
    class rtt {
    public:
        static void init() {
            // do nothing
        }

        template <uint8_t Channel = 0, typename T>
        static void write(const T data, const uint32_t size) {
            // do nothing
        }

        template <uint8_t Channel = 0, bool Blocking = true>
        static char getc() {
            // return a null character
            return 0;
        }

        template <uint8_t Channel = 0>
        static bool hasc() {
            // return we do not have data
            return false;
        }
    };
#endif
}

#endif