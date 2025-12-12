#ifndef KLIB_RTOS_QUEUE_HPP
#define KLIB_RTOS_QUEUE_HPP

#include <atomic>

#include <klib/ringbuffer.hpp>

#include "mutex.hpp"
#include "semaphore.hpp"

namespace klib::rtos {
    /**
     * @brief Queue class for RTOS usage implemented using a ringbuffer
     * 
     * @tparam Size 
     */
    template <typename T, size_t MaxSize>
    class queue {
    protected:
        // mutex to protect access to the queue
        rtos::mutex mutex;

        // semaphore to count available data and available space
        rtos::semaphore data_available;
        rtos::semaphore space_available;
        
        // ringbuffer to store the data
        klib::ringbuffer<T, MaxSize> buffer;

    public:
        /**
         * @brief Construct a new queue object
         * 
         */
        queue():
            mutex(),
            data_available(0),
            space_available(MaxSize),
            buffer()
        {}

        /**
         * @brief Add an item to the queue.
         *
         * @param val
         */
        void push(const T &val) {
            // wait until we have space in the buffer
            space_available.decrement();

            // lock the mutex while accessing the buffer
            mutex.lock();

            // push the item to the buffer
            buffer.push(val);

            // signal that data is available
            data_available.increment();

            // unlock the mutex
            mutex.unlock();
        }

        /**
         * @brief Copy the last item from the buffer and pop.
         *
         * @return
         */
        T copy_and_pop() {
            // wait until we have data in the buffer
            data_available.decrement();

            // lock the mutex while accessing the buffer
            mutex.lock();

            // copy and pop the item
            T item = buffer.copy_and_pop();

            // signal that space is available
            space_available.increment();

            // unlock the mutex
            mutex.unlock();

            // return the item
            return item;
        }

        /**
         * @brief clear the queue.
         *
         * @return
         */
        constexpr void clear() {
            // lock the mutex while clearing the buffer
            mutex.lock();
            
            // clear the buffer
            buffer.clear();

            // clear the semaphores
            space_available = rtos::semaphore(MaxSize);
            data_available = rtos::semaphore(0);

            // unlock the mutex
            mutex.unlock();
        }

        /**
         * @brief Return whether the queue is empty.
         *
         * @return
         */
        constexpr bool empty() {
            // lock the mutex while accessing the buffer
            mutex.lock();

            // check if the buffer is empty
            const bool is_empty = buffer.empty();

            // unlock the mutex
            mutex.unlock();

            return is_empty;
        }

        /**
         * @brief Return whether the queue is full.
         *
         * @return
         */
        constexpr bool full() {
            // lock the mutex while accessing the buffer
            mutex.lock();

            // check if the buffer is full
            const bool is_full = buffer.full();

            // unlock the mutex
            mutex.unlock();

            return is_full;
        }

        /**
         * @brief Get the current size of the queue.
         *
         * @return
         */
        constexpr size_t size() {
            // lock the mutex while accessing the buffer
            mutex.lock();

            // get the size of the buffer
            const size_t current_size = buffer.size();

            // unlock the mutex
            mutex.unlock();

            return current_size;
        }

        /**
         * @brief Get the maximum size of the queue.
         *
         * @return
         */
        constexpr size_t max_size() const {
            return MaxSize;
        }
    };
}

#endif