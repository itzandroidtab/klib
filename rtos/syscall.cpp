#include <rtos/rtos.hpp>

#include "syscall.hpp"
#include "scheduler.hpp"

namespace klib::rtos::syscall {
    /**
     * @brief Invoke a syscall with the given arguments
     * 
     * @tparam Return 
     * @tparam Args 
     * @param number 
     * @param args 
     * @return Return 
     */
    template <typename Return, typename... Args>
    static Return syscall_invoke(scheduler::syscalls number, Args... args) {
        // Call the target specific syscall invoke function
        return klib::target::rtos::detail::syscall_invoke<Return, Args...>(
            static_cast<uint32_t>(number), args...
        );
    }

    bool create_task(detail::base_task* task) {
        // invoke the create_task syscall
        return syscall_invoke<bool, detail::base_task*>(scheduler::syscalls::create_task, task);
    }

    bool delete_task(detail::base_task* task) {
        // invoke the delete_task syscall
        return syscall_invoke<bool, detail::base_task*>(scheduler::syscalls::delete_task, task);
    }

    void yield() {
        // invoke the yield syscall
        syscall_invoke<void>(scheduler::syscalls::yield);
    }

    void sleep(klib::time::ms time) {
        // invoke the sleep syscall
        syscall_invoke<void, klib::time::ms>(scheduler::syscalls::sleep, time.value);
    }

    void* malloc(uint32_t size) {
        // invoke the malloc syscall
        return syscall_invoke<void*, uint32_t>(scheduler::syscalls::malloc, size);
    }

    void free(const void *const ptr) {
        // invoke the free syscall
        syscall_invoke<void, const void*>(scheduler::syscalls::free, ptr);
    }
}
