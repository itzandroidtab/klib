#ifndef KLIB_RTOS_MUTEX_HPP
#define KLIB_RTOS_MUTEX_HPP

#include <atomic>

#include "waitable.hpp"
#include "syscall.hpp"

namespace klib::rtos {
    class mutex : public waitable {
    private:
        std::atomic_bool locked;

    public:
        /**
         * @brief Construct a new mutex object
         * 
         */
        mutex(): locked(false) {}

        /**
         * @brief Lock the mutex. This function will block until the mutex
         * is acquired.
         * 
         */
        void lock() {
            // wait until we can lock the mutex
            while (true) {
                // try to lock the mutex
                if (try_lock()) {
                    // locked successfully
                    return;
                }

                // yield to other tasks
                rtos::syscall::yield(*this);
            }
        }

        /**
         * @brief Try to lock the mutex. This function will not block and
         * will return if the mutex could not be acquired.
         * 
         * @return true 
         * @return false 
         */
        bool try_lock() {
            bool expected = false;
            
            // compare and try to set the locked flag to true
            return locked.compare_exchange_strong(expected, true);
        }

        /**
         * @brief Unlock the mutex.
         * 
         */
        void unlock() {
            // unlock the mutex
            locked.store(false);
        }

        /**
         * @brief Function for the scheduler to check if a task is waiting
         * 
         * @return true 
         * @return false 
         */
        virtual bool is_waiting() const override {
            // a mutex is considered waiting if it is locked
            return locked.load();
        }
    };
}

#endif // KLIB_RTOS_MUTEX_HPP