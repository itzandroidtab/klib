#ifndef KLIB_RTOS_SYSCALL_HPP
#define KLIB_RTOS_SYSCALL_HPP

#include <cstdint>

#include <klib/units.hpp>

#include "base_task.hpp"
#include "waitable.hpp"

namespace klib::rtos::detail {
    /**
     * @brief All the available syscalls
     * 
     */
    enum class syscalls {
        create_task,
        delete_task,
        yield,
        sleep,
        get_time,
        malloc,
        free,
    };
}

namespace klib::rtos::syscall {
    /**
     * @brief Create a task object
     * 
     * @param task 
     * @return successful creation
     */
    bool create_task(detail::base_task* task);

    /**
     * @brief Delete a task
     * 
     * @param task 
     * @return true 
     * @return false 
     */
    bool delete_task(detail::base_task* task);

    /**
     * @brief Yield the CPU to the next task in the scheduler
     * 
     */
    void yield();

    /**
     * @brief Yield the CPU and wait on a waitable object
     * 
     * @param waitable 
     */
    void yield(rtos::waitable& waitable);

    /**
     * @brief Sleep the current task for the given time
     * 
     * @param time 
     */
    void sleep(klib::time::ms time);

    /**
     * @brief Get the time object
     * 
     * @return klib::time::ms 
     */
    klib::time::ms get_time();

    /**
     * @brief Allocate size amount of memory
     * 
     */
    void* malloc(uint32_t size);

    /**
     * @brief Free the memory pointer by ptr
     * 
     */
    void free(const void *const ptr);
}

#endif