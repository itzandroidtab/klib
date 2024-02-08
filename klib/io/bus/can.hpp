#ifndef KLIB_CAN_HPP
#define KLIB_CAN_HPP

#include <cstdint>

#include <klib/queue.hpp>

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

    /**
     * @brief Simpel helper class to extend the internal buffers for hardware that 
     * only has limited buffer support. This class adds support for TxSize and RxSize 
     * amount of buffers. 
     * 
     * @warning This does not sort messages on can priority (or any priority) but on 
     * FIFO.
     * 
     * @tparam Can 
     * @tparam TxSize 
     * @tparam RxSize 
     */
    template <typename Can, uint32_t TxSize = 32, uint32_t RxSize = 32>
    class helper {
    protected:
        // queues to store the data
        static inline queue<can::frame, TxSize, queue_optimization::read> transmit;
        static inline queue<can::frame, RxSize, queue_optimization::write> receive;

        // flag if we are already transmitting
        static inline bool is_sending = false;

        // flag to signal if we are "busy" and cannot process 
        // more messages because the queue is full
        static inline volatile bool is_full = false;

        // flag if the queue is empty
        static inline volatile bool is_empty = true;

        static void receive_handler() {
            // read the frame to empty the buffer on 
            // the hardware
            const auto frame = Can::read();

            // check if we can store the data
            if (receive.full()) {
                // queue is full, exit
                return;
            }

            // push the data in the queue
            receive.push(frame);

            // mark the buffer as not empty
            is_empty = false;
        }

        static void transmit_handler() {
            // check if we have any frames left to send
            if (transmit.empty()) {
                // mark we are not busy anymore and 
                // we need a restart next time we are 
                // sending data
                is_sending = false;

                // nothing left to send, exit
                return;
            }

            // write as many frames as possible
            while (!Can::is_busy() && !transmit.empty()) {
                // get a frame from the queue
                const auto frame = transmit.copy_and_pop();

                // clear the is_full flag
                is_full = false;

                // write the frame to the hardware
                Can::template write<true>(frame);
            }
        }

    public:
        template <uint32_t Frequency = 500'000>
        static void init() {
            // init the can hardware with our handlers
            Can::template init<Frequency>(transmit_handler, receive_handler);
        }

        /**
         * @brief Returns if we have data available. This function 
         * is non blocking
         * 
         * @return true 
         * @return false 
         */
        static bool has_data() {
            // return we have data in the queue
            return !is_empty;
        }

        /**
         * @brief Read a frame from the queue
         * 
         * @warning the can interrupt should be suppressed while
         * this function called.
         * 
         * @return klib::io::can::frame 
         */
        static can::frame read() {
            // get the frame
            const auto frame = receive.copy_and_pop();

            // update the is_empty flag
            is_empty = receive.empty();

            // return the frame
            return frame;
        }

        /**
         * @brief Function that returns if we have space in the 
         * transmit queue. Function is non blocking
         * 
         * @return true 
         * @return false 
         */
        static bool is_busy() {
            return is_full;
        }

        /**
         * @brief Write a frame to the queue. Might send the 
         * data immediately. 
         * 
         * @warning the can interrupt should be suppressed while
         * this function called.
         * 
         * @param frame 
         */
        template <bool Async = true>
        static void write(const klib::io::can::frame& frame) {
            // check if the template parameter is correct
            static_assert(Async, "Can helper only supports async writes");

            // check if we can write to the queue
            if (is_full) {
                return;
            }
            
            // push our frame to the queue
            transmit.push(frame);

            // update the flag if the queue is full
            is_full = transmit.full();

            // check if we should start the interrupt chain
            if (!is_sending && !Can::is_busy()) {
                // call the handler to write the data to the hardware 
                transmit_handler();
            }
        }
    };
}

#endif