#ifndef KLIB_RTOS_SEMAPHORE_HPP
#define KLIB_RTOS_SEMAPHORE_HPP

#include <atomic>
#include <cstdint>

#include "waitable.hpp"
#include "syscall.hpp"

namespace klib::rtos {
    class semaphore : public waitable {
    private:
        std::atomic<uint32_t> count;

    public:
        /**
         * @brief Construct a new semaphore object
         * 
         * @param initial_count 
         */
        semaphore(const uint32_t initial_count = 0): 
            count(initial_count) 
        {}

        /**
         * @brief Decrement the semaphore
         * 
         */
        void decrement() {
            uint32_t expected;

            while (true) {
                // try to acquire the semaphore
                if (try_decrement()) {
                    return;
                }

                // yield to other tasks if no resources available
                rtos::syscall::yield(*this);
            }
        }

        /**
         * @brief Try to decrement the semaphore
         * 
         * @return true 
         * @return false 
         */
        bool try_decrement() {
            uint32_t expected = count.load();

            while (expected > 0) {
                if (count.compare_exchange_strong(expected, expected - 1)) {
                    return true;
                }
            }

            return false;
        }

        /**
         * @brief Increment the semaphore
         * 
         */
        void increment() {
            count.fetch_add(1);
        }

        /**
         * @brief Returns whether the semaphore is being waited on
         * 
         * @return true 
         * @return false 
         */
        virtual bool is_waiting() const override {
            return count.load() == 0;
        }
    };
}

#endif // KLIB_RTOS_SEMAPHORE_HPP
