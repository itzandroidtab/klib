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
        volatile klib::time::ms time_to_sleep = {};

        // pointer to the current stack position
        size_t* stack_pointer;

        // priority of the task
        uint8_t priority;
    };
}

#endif