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
         * @brief Construct a new task with parameter for the function
         * 
         * @tparam T 
         * @param func 
         * @param parameter 
         */
        template <typename T>
        task(void (*func)(T arg), const T parameter) {
            // ensure the parameter fits in a register
            static_assert(sizeof(T) <= 4, "Parameter size must be less than or equal to 4 bytes");

            // initialize the stack with default values and update the stack pointer
            stack_pointer = klib::target::rtos::detail::setup_task_stack(func, parameter, stack, StackSize);
        }

        /**
         * @brief Construct a new task without parameter for the function
         * 
         * @param func 
         */
        task(void (*func)()) {
            // initialize the stack with default values and update the stack pointer
            stack_pointer = klib::target::rtos::detail::setup_task_stack(reinterpret_cast<void(*)(int)>(func), 0, stack, StackSize);
        }
    };
}

#endif