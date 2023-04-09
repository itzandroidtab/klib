#ifndef KLIB_CAN_HPP
#define KLIB_CAN_HPP

#include <cstdint>

namespace klib::io::can {
    // max data size for the 11/29 bit can frame
    constexpr uint32_t max_frame_size = 8;

    /**
     * @brief Can frame for 11 and 29 bit frames
     * 
     */
    struct frame {
        // address in the can frame. This is for
        // extended and normal frames
        uint32_t address;

        // flag if we have a extended or normal 
        // frame
        bool extended;

        // amount of data in the data array
        uint8_t size;

        // data of the frame
        uint8_t data[max_frame_size] = {};

        // flag if the frame is a remote frame (RTR)
        bool remote = false;
    };
}

#endif