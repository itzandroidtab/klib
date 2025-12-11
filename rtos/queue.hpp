#ifndef KLIB_RTOS_QUEUE_HPP
#define KLIB_RTOS_QUEUE_HPP

#include <atomic>

#include <klib/ringbuffer.hpp>

#include "waitable.hpp"
#include "mutex.hpp"

namespace klib::rtos {
    /**
     * @brief Queue class for RTOS usage implemented using a ringbuffer
     * 
     * @tparam Size 
     */
    template <typename T, size_t MaxSize>
    class queue: public waitable {
    protected:
        std::atomic_bool waiting;

        // mutex to protect access to the queue
        rtos::mutex mutex;

        // ringbuffer to store the data
        klib::ringbuffer<T, MaxSize> buffer;

    public:
        /**
         * @brief Add an item to the queue.
         *
         * @param val
         */
        constexpr void push(const T &val) {
            mutex.lock();

            // wait until we have space in the buffer
            while (buffer.full()) {
                // mark as waiting
                waiting.store(true);

                // unlock the mutex while waiting
                mutex.unlock();

                // yield to other tasks
                rtos::syscall::yield(*this);

                // lock the mutex again
                mutex.lock();
            }

            // push the item to the buffer
            buffer.push(val);

            // clear the waiting flag for the other side if it was waiting
            // on data to be available in the buffer
            waiting.store(false);

            // unlock the mutex
            mutex.unlock();
        }

        /**
         * @brief Copy the last item from the buffer and pop.
         *
         * @return
         */
        constexpr T copy_and_pop() {
            // lock the mutex while accessing the buffer
            mutex.lock();

            // wait until we have data in the buffer
            while (buffer.empty()) {
                // mark as waiting
                waiting.store(true);

                // unlock the mutex while waiting
                mutex.unlock();

                // yield to other tasks
                rtos::syscall::yield(*this);

                // lock the mutex again
                mutex.lock();
            }

            // copy and pop the item
            T item = buffer.copy_and_pop();

            // clear the waiting flag for the other side if it was waiting 
            // on space in the buffer
            waiting.store(false);

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

        /**
         * @brief 
         * 
         * @return true 
         * @return false 
         */
        virtual bool is_waiting() const override {
            return waiting.load();
        }
    };
}

#endif