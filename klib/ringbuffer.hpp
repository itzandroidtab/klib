#ifndef KLIB_RINGBUFFER_HPP
#define KLIB_RINGBUFFER_HPP

#include <cstdint>

namespace klib {
    /**
     * Simple ringbuffer implementation. When not using a MaxSize 
     * that is a power of 2 the operator[] the compiler cannot use
     * a and to substitutes the modulo
     *
     * @tparam T
     * @tparam MaxSize
     */
    template<typename T, uint32_t MaxSize>
    class ringbuffer {
    protected:
        T buffer[MaxSize] = {};

        size_t head = 0;
        size_t tail = 0;
        size_t used = 0;

        /**
         * @brief Goto the index at which a item can be stored.
         * 
         * @return size_t 
         */
        constexpr void advance() {
            // check if the ringbuffer is already full
            if (full()) {
                // increment the tail
                tail = (tail + 1) % MaxSize;
            }
            else {
                // add to the used count    
                used++;
            }

            // increment the head
            head = (head + 1) % MaxSize;
        }

    public:
        /**
         * @brief Default constructor.
         * 
         */
        constexpr ringbuffer() = default;

        /**
         * @brief Add an item to the ringbuffer.
         *
         * @param val
         */
        constexpr void push(const T &val) {
            // store the data in the buffer
            buffer[head] = val;

            // advance the head ot the next position
            advance();
        }

        /**
         * @brief Emplace an item into the ringbuffer.
         * 
         * @tparam Args 
         * @param args 
         */
        template<typename ...Args>
        constexpr void emplace(Args&& ...args) {
            // store the data in the buffer
            buffer[head] = T(std::forward<Args>(args)...);

            // advance the head ot the next position
            advance();
        }

        /**
         * @brief Copy the last item from the buffer and pop.
         *
         * @return
         */
        constexpr T copy_and_pop() {
            if (empty()) {
                return {};
            }

            // get the current value
            const T item = buffer[tail]; 

            // move the tail
            tail = (tail + 1) % MaxSize;

            // decrement the amount of items we have
            used--;

            // return the item
            return item;
        }

        /**
         * @brief clear the ringbuffer.
         *
         * @return
         */
        constexpr void clear() {
            head = 0;
            tail = 0;
            used = 0;
        }

        /**
         * @brief Get the value at the given index.
         *
         * @param index
         * @return
         */
        constexpr T &operator[](const size_t index) {
            return buffer[(tail + index) % MaxSize];
        }

        /**
         * @brief Get the value at the given index.
         *
         * @param index
         * @return
         */
        constexpr T const &operator[](const size_t index) const {
            return buffer[(tail + index) % MaxSize];
        }

        /**
         * @brief Return whether the ringbuffer is empty.
         *
         * @return
         */
        constexpr bool empty() const {
            return !size();
        }

        /**
         * @brief Return whether the ringbuffer is full.
         *
         * @return
         */
        constexpr bool full() const {
            return size() >= max_size();
        }

        /**
         * @brief Get the current size of the ringbuffer.
         *
         * @return
         */
        constexpr size_t size() const {
            return used;
        }

        /**
         * @brief Get the maximum size of the ringbuffer.
         *
         * @return
         */
        constexpr size_t max_size() const {
            return MaxSize;
        }
    };
}

#endif