#ifndef KLIB_ARM_RTOS_CORTEX_M3_HPP
#define KLIB_ARM_RTOS_CORTEX_M3_HPP

#include <cstdint>
#include <cstddef>
#include <type_traits>
#include <bit>

#include <klib/klib.hpp>
#include <rtos/base_task.hpp>

namespace klib::rtos::cortex_m3 {
    // data to tell the handler to return from either the PSP or the MSP
    constexpr static uint32_t thread_return = 0xfffffffd;
    constexpr static uint32_t main_return = 0xfffffff9;

    /**
     * @brief Registers the hardware pushes to the stack on exception entry
     * 
     */
    struct hardware_context {
        uint32_t r0;
        uint32_t r1;
        uint32_t r2;
        uint32_t r3;
        uint32_t r12;
        uint32_t lr;
        uint32_t pc;
        uint32_t xpsr;
    };

    /**
     * @brief Registers that need to be saved manually
     * 
     */
    struct software_context {
        uint32_t r4;
        uint32_t r5;
        uint32_t r6;
        uint32_t r7;
        uint32_t r8;
        uint32_t r9;
        uint32_t r10;
        uint32_t r11;
    };

    /**
     * @brief The full context saved on the stack for task switching
     * 
     */
    struct full_context {
        software_context software;
        hardware_context hardware;
    };
}

namespace klib::rtos::cortex_m3::detail {
    /**
     * @brief helper function to store up to 4 parameters in the 
     * hardware context. Values not used are set to 0
     */
    template <typename... Args>
    constexpr void fill_function_parameters(hardware_context &regs, Args... args) {
        // make sure we have at most 4 arguments
        static_assert(sizeof...(Args) <= 4, "A maximum of 4 arguments can be assigned to r0-r3");

        // check if the arguments are not bigger than uint32_t
        static_assert((... && (sizeof(Args) <= sizeof(uint32_t))), "All argument types must be less than or equal to 4 bytes");

        // create an array with the argument values casted to uint32_t using bit_cast
        const uint32_t values[sizeof...(Args)] = { std::bit_cast<uint32_t>(args)... };

        // assign the values to the registers, missing values are set to 0
        regs.r0 = (sizeof...(Args) > 0) ? values[0] : 0;
        regs.r1 = (sizeof...(Args) > 1) ? values[1] : 0;
        regs.r2 = (sizeof...(Args) > 2) ? values[2] : 0;
        regs.r3 = (sizeof...(Args) > 3) ? values[3] : 0;
    }
}

namespace klib::rtos::cortex_m3 {
    /**
     * @brief Setup the task stack with the initial context
     * 
     * @tparam T 
     * @param func 
     * @param parameter 
     * @param stack 
     * @param stack_size 
     * @return uint32_t*
     */
    template <typename... Args>
    size_t* setup_task_stack(void (*func)(Args...), size_t* stack, const size_t stack_size, Args&&... parameters) {
        // Point to the top of the aligned stack
        full_context* context = reinterpret_cast<full_context*>(
            reinterpret_cast<uint8_t*>(stack) + (stack_size * sizeof(size_t)) - sizeof(full_context)
        );

        // fill in the function parameters into r0-r3
        detail::fill_function_parameters(context->hardware, parameters...);

        // initialize the hardware context
        context->hardware.r12 = 12;
        context->hardware.lr = thread_return;
        context->hardware.pc = std::bit_cast<uint32_t>(func);

        // for the cortex m3 make we need to set the thumb bit in the xPSR register
        context->hardware.xpsr = 0x1 << 24;

        // initialize the software context
        context->software.r4 = 4;
        context->software.r5 = 5;
        context->software.r6 = 6;
        context->software.r7 = 7;
        context->software.r8 = 8;
        context->software.r9 = 9;
        context->software.r10 = 10;
        context->software.r11 = 11;

        // return the new stack pointer
        return reinterpret_cast<size_t*>(context);
    }

    /**
     * @brief Helper function to switch between tasks. This function saves 
     * the context onto the process stack and loads the context of the next
     * task from its stack.
     * 
     * @param current_task
     * @param next_task
     */
    static void __attribute__((naked)) switch_task(
        rtos::detail::base_task** current_task,
        rtos::detail::base_task** next_task
    ) {
        __asm__ volatile(
            // save context of current and next task is not a nullptr
            "ldr r2, [r0]\n"
            "cbz r2, switch_task_next_task\n"

            // we have a task, save its context. Get the process stack pointer
            "mrs r2, psp\n"
            
            // push the registers onto the stack that are not saved by hardware
            "stmdb r2!, {r4-r11}\n" 

            // store the stack pointer with the offset in %0
            "ldr r3, [r0]\n"
            "str r2, [r3, %0]\n"

            "switch_task_next_task:\n"

            // store the next task into the current task pointer
            "ldr r3, [r1]\n"
            "str r3, [r0]\n"

            // get the stack pointer of the next task
            "ldr r2, [r3, %0]\n"

            // pop the registers from the stack that are not saved by hardware
            "ldmia r2!, {r4-r11}\n" 
            "msr psp, r2\n"

            // return
            "bx lr\n"
            :
            : "I" (offsetof(rtos::detail::base_task, stack_pointer))
            : "r2", "r3", "r4", "r5", "r6", "r7", "r8", "r9", "r10", "r11", "memory"
        );
    }

    /**
     * @brief Start the scheduler by switching to unprivileged mode and 
     * switching to the process stack pointer
     * 
     */
    static void scheduler_start() {
        // set the stack pointer so we dont access any wrong memory
        // before starting the scheduler. Wrong memory in this case
        // is whatever is left in the process stack pointer
        uint32_t psp_stack[32] = {};
        
        // switch to the array for the process stack pointer
        __set_PSP(reinterpret_cast<uint32_t>(&psp_stack[(sizeof(psp_stack) / sizeof(psp_stack[0]))]));

        // switch to the process stack pointer and start executing in unprivileged mode
        asm volatile(
            "msr control, %0\n"
            "isb\n"
            :
            : "r" (0x3) // unprivileged, use PSP
            : "memory"
        );

        // endless loop to wait on the start of the scheduler
        while (true) {
            // wait for a interrupt. The systick callback
            // should schedule what task to run
            asm volatile("wfi");
        }
    }

    template <typename Scheduler>
    static __attribute__((naked)) void sycall_handler() {
        // get the values from r0-r3 and the syscall number from r4
        __asm__ volatile(
            // store the link register and r4
            "push {r4, lr}\n"

            // get the r0-r3 values from the process stack
            "mrs r4, psp\n"
            "ldr r0, [r4, #0]\n"
            "ldr r1, [r4, #4]\n"
            "ldr r2, [r4, #8]\n"
            "ldr r3, [r4, #12]\n"

            // call the scheduler syscall handler
            "blx %[syscall_handler]\n"

            // copy the result (in r0) to the process stack so it is
            // returned to the caller
            "str r0, [r4, #0]\n"

            // pop r4 and the link register
            "pop {r4, lr}\n"

            // return from exception
            "bx lr\n"
            :
            : [syscall_handler]"r"(Scheduler::syscall_handler)
            : "r0", "r1", "r2", "r3", "r4", "memory"
        );
    }

    template <typename Return, typename... Args>
    static Return syscall_invoke(uint8_t syscall_number, Args... args) {
        // make sure we have at most 3 arguments
        static_assert(sizeof...(Args) <= 3, "A maximum of 3 arguments can be used in a syscall");

        // check if the arguments are not bigger than uint32_t
        static_assert((... && (sizeof(Args) <= sizeof(uint32_t))), "All argument types must be less than or equal to 4 bytes");

        // convert the arguments to uint32_t values
        const uint32_t values[3] = { std::bit_cast<uint32_t>(args)... };
    
        // invoke the syscall
        uint32_t ret;

        // call the svc instruction - use svc #0 and pass syscall number in r7 (common convention)
        asm volatile(
            "mov r0, %[number]\n"
            "mov r1, %[arg1]\n"
            "mov r2, %[arg2]\n"
            "mov r3, %[arg3]\n"
            "svc #0\n"
            "mov %[ret], r0\n"
            : [ret]"=r"(ret)
            : 
                [number]"r"(syscall_number), 
                [arg1]"r"(values[0]), 
                [arg2]"r"(values[1]), 
                [arg3]"r"(values[2])
            : "r0", "r1", "r2", "r3", "memory"
        );
    
        // convert the return value based on type
        if constexpr (std::is_void_v<Return>) {
            return;
        } else if constexpr (std::is_same_v<Return, bool>) {
            return static_cast<bool>(ret);
        } else if constexpr (sizeof(Return) == sizeof(uint32_t)) {
            return std::bit_cast<Return>(ret);
        } else {
            return static_cast<Return>(ret);
        }
    }
}

#endif