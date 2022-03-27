#ifndef KLIB_RTT_HPP
#define KLIB_RTT_HPP

#include <cstdint>
#include "src/RTT/SEGGER_RTT.h"

namespace klib::rtt {
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
        static void write(const T data, const size_t size) {
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
    };
}

#endif