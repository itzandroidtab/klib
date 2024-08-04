#ifndef KLIB_QUEUE_HPP
#define KLIB_QUEUE_HPP

#include <cstdint>

namespace klib {
    /**
     * @brief Enum for specifying if the queue should use read
     * or write optimisation
     *
     */
    enum class queue_optimization {
        read,
        write
    };

    /**
     * Simple queue implementation.
     *
     * @tparam T
     * @tparam MaxSize
     * @tparam Optimization
     */
    template<typename T, uint32_t MaxSize, queue_optimization Optimization = queue_optimization::write>
    class queue {
    protected:
        T buffer[MaxSize] = {};

        uint32_t index = 0;

    public:
        /**
         * Put an item on the queue.
         *
         * @param item
         */
        void push(const T &item) {
            if constexpr (Optimization == queue_optimization::write) {
                buffer[index] = item;
            }
            else {
                for (uint32_t i = index; i != 0; i--) {
                    buffer[i] = buffer[i - 1];
                }

                buffer[0] = item;
            }

            index++;
        }

        /**
         * Pop an item from the queue.
         */
        void pop() {
            if constexpr (Optimization == queue_optimization::write) {
                for (uint32_t i = 1; i < index; i++) {
                    buffer[i - 1] = buffer[i];
                }
            }

            index--;
        }

        /**
         * Get the next item from the queue
         * and pop.
         */
        T copy_and_pop() {
            const T item = front();

            pop();

            return item;
        }

        /**
         * Get the next in the queue.
         */
        T &front() {
            if constexpr (Optimization == queue_optimization::write) {
                return buffer[0];
            }
            else {
                return buffer[index - 1];
            }
        }

        /**
         * Get the next in the queue.
         */
        T const &front() const {
            if constexpr (Optimization == queue_optimization::write) {
                return buffer[0];
            }
            else {
                return buffer[index - 1];
            }
        }

        /**
         * Get the last item in the queue.
         */
        T &back() {
            if constexpr (Optimization == queue_optimization::write) {
                return buffer[index - 1];
            }
            else {
                return buffer[0];
            }
        }

        /**
         * Get the last item in the queue.
         */
        T const &back() const {
            if constexpr (Optimization == queue_optimization::write) {
                return buffer[index - 1];
            }
            else {
                return buffer[0];
            }
        }

        /**
         * Get the current size of the queue.
         *
         * @return uint32_t
         */
        uint32_t size() const {
            return index;
        }

        /**
         * Is the queue empty?
         *
         * @return
         */
        bool empty() const {
            return size() == 0;
        }

        /**
         * Is the queue full?
         *
         * @return
         */
        bool full() const {
            return size() == max_size();
        }

        /**
         * Clears the queue
         *
         * @brief Clears the queue
         * @details Clears the queue by setting the index to zero
         * @return
         */
        void clear() {
            index = 0;
        }

        /**
         * Get the maximum size of the queue.
         *
         * @return constexpr uint32_t
         */
        constexpr uint32_t max_size() const {
            return MaxSize;
        }

        /**
         * Is this queue write optimized or
         * read optimized?
         *
         * @return constexpr queue_optimization
         */
        constexpr queue_optimization optimized_for() const {
            return Optimization;
        }
    };
}

#endif