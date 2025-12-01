#ifndef KLIB_RTOS_TASK_HPP
#define KLIB_RTOS_TASK_HPP

#include <klib/klib.hpp>
#include <rtos/rtos.hpp>

#include "base_task.hpp"

namespace klib::rtos {
    /**
     * @brief Task to run in the RTOS with a fixed stack size
     * 
     * @tparam StackSize 
     */
    template <uint32_t StackSize = 128>
    class task: public detail::base_task {
    public:
        using task_func = void(*)();

    protected:
        // stack for this task
        size_t stack[StackSize] = {};

    public:
        /**
         * @brief Construct a task that runs the given function. When the function 
         * exits the task is deleted.
         * 
         * @tparam Args 
         * @param func 
         * @param parameters 
         */
        template <typename... Args>
        task(void (*func)(Args...), Args&&... parameters) {
            static_assert(sizeof...(parameters) <= 4, "A maximum of 4 parameters are supported for task functions");

            // type alias for type deduction
            using func_t = void (*)(Args...);

            // initialize the stack with default values and update the stack pointer
            klib::target::rtos::detail::setup_task_stack(static_cast<func_t>(func), stack, StackSize, std::forward<Args>(parameters)...);
        }
    };
}

#endif