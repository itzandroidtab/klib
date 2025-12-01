#ifndef KLIB_ARM_RTOS_CORTEX_M3_HPP
#define KLIB_ARM_RTOS_CORTEX_M3_HPP

#include <cstdint>
#include <cstddef>
#include <type_traits>

#include <klib/klib.hpp>
#include <klib/rtos/base_task.hpp>

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
    template <typename T, typename = std::enable_if_t<sizeof(T) <= 4>>
    size_t* setup_task_stack(void (*func)(T arg), T parameter, size_t* stack, const size_t stack_size) {
        // Point to the top of the aligned stack
        full_context* context = reinterpret_cast<full_context*>(
            reinterpret_cast<uint8_t*>(stack) + (stack_size * sizeof(size_t)) - sizeof(full_context)
        );

        // initialize the hardware context
        context->hardware.r0 = parameter;
        context->hardware.r1 = 1;
        context->hardware.r2 = 2;
        context->hardware.r3 = 3;
        context->hardware.r12 = 12;
        context->hardware.lr = thread_return;
        context->hardware.pc = reinterpret_cast<uint32_t>(func);

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
        detail::base_task** current_task,
        detail::base_task** next_task
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
            : "I" (offsetof(detail::base_task, stack_pointer))
            : "r2", "r3", "r4", "r5", "r6", "r7", "r8", "r9", "r10", "r11", "memory"
        );
    }
}

#endif