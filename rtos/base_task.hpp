#ifndef KLIB_RTOS_BASE_TASK_HPP
#define KLIB_RTOS_BASE_TASK_HPP

#include <cstdint>
#include <cstdlib>

#include <klib/units.hpp>

namespace klib::rtos::detail {
    /**
     * @brief Task to run in the RTOS
     * 
     * @tparam StackSize 
     */
    class base_task {
    public:
        // time to sleep for the task
        volatile klib::time::ms time_to_sleep;

        // pointer to the current stack position
        size_t* stack_pointer;

        // current priority of the task
        uint8_t current_priority;

        // the base priority of the task
        const uint8_t priority;

        /**
         * @brief Construct a new base task
         * 
         * @param priority 
         */
        base_task(uint8_t priority = 0): 
            time_to_sleep(0),
            stack_pointer(nullptr),
            current_priority(priority),
            priority(priority)
        {}
    };
}

#endif