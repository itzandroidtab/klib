#include <rtos/rtos.hpp>

#include "syscall.hpp"

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
    static Return syscall_invoke(detail::syscalls number, Args... args) {
        // Call the target specific syscall invoke function
        return klib::target::rtos::detail::syscall_invoke<Return, Args...>(
            static_cast<uint32_t>(number), args...
        );
    }

    bool create_task(detail::base_task* task) {
        // invoke the create_task syscall
        return syscall_invoke<bool, detail::base_task*>(detail::syscalls::create_task, task);
    }

    bool delete_task(detail::base_task* task) {
        // invoke the delete_task syscall
        return syscall_invoke<bool, detail::base_task*>(detail::syscalls::delete_task, task);
    }

    void yield() {
        // invoke the yield syscall
        syscall_invoke<void, rtos::waitable*>(detail::syscalls::yield, nullptr);
    }

    void yield(rtos::waitable& waitable) {
        // invoke the yield syscall
        syscall_invoke<void, rtos::waitable*>(detail::syscalls::yield, &waitable);
    }

    void wakeup_highest_priority_waiter(rtos::waitable& waitable) {
        // invoke the yield syscall
        syscall_invoke<void, rtos::waitable*>(detail::syscalls::wakeup_highest_priority_waiter, &waitable);
    }

    void sleep(klib::time::ms time) {
        if (time.value == 0) {
            // no need to sleep
            return;
        }

        // invoke the sleep syscall
        syscall_invoke<void, uint32_t>(detail::syscalls::sleep, time.value);
    }

    klib::time::ms get_time() {
        // invoke the get_time syscall
        return klib::time::ms(syscall_invoke<uint32_t>(detail::syscalls::get_time));
    }

    void* malloc(uint32_t size) {
        // invoke the malloc syscall
        return syscall_invoke<void*, uint32_t>(detail::syscalls::malloc, size);
    }

    void free(const void *const ptr) {
        // invoke the free syscall
        syscall_invoke<void, const void*>(detail::syscalls::free, ptr);
    }
}
