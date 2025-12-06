#ifndef KLIB_RTOS_TASK_HPP
#define KLIB_RTOS_TASK_HPP

#include <klib/klib.hpp>
#include <rtos/rtos.hpp>

#include "base_task.hpp"
#include "syscall.hpp"

namespace klib::rtos {
    /**
     * @brief Task to run in the RTOS with a fixed stack size
     * 
     * @tparam StackSize 
     */
    template <uint8_t Priority = 0, uint32_t StackSize = 128>
    class task: public detail::base_task {
    protected:
        // stack for this task
        size_t stack[StackSize] = {};

        /**
         * @brief Helper trampoline function that calls the actual task function
         * after the function has returned we delete the task from the scheduler
         * 
         * @tparam Args 
         * @param func 
         * @param task 
         * @param parameters 
         */
        template <typename... Args>
        static void task_trampoline(rtos::detail::base_task* task, void (*func)(Args...), Args... parameters) {
            // call the function with the parameter
            func(parameters...);

            // mark the task for deletion
            syscall::delete_task(task);
        }

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
        task(void (*func)(Args...), Args&&... parameters): 
            detail::base_task(Priority)
        {
            static_assert(sizeof...(parameters) <= 2, "A maximum of 2 parameters are supported for task functions");

            // type aliases for clarity
            using trampoline_t = void (*)(rtos::detail::base_task*, void (*)(Args...), Args...);

            // initialize the stack with default values and update the stack pointer
            stack_pointer = klib::target::rtos::detail::setup_task_stack(
                static_cast<trampoline_t>(task_trampoline<Args...>), 
                stack, StackSize, 
                static_cast<rtos::detail::base_task*>(this),
                static_cast<void (*)(Args...)>(func), 
                std::forward<Args>(parameters)...
            );
        }
    };
}

#endif