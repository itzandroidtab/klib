#ifndef KLIB_RTOS_HPP
#define KLIB_RTOS_HPP

#include <klib/io/systick.hpp>
#include <klib/dynamic_array.hpp>
#include <klib/units.hpp>

#include <rtos/rtos.hpp>

#include "task.hpp"

namespace klib::rtos {
    template <uint32_t CpuId, typename Irq>
    class scheduler {
    protected:
        // make sure we can register our callback with the systick handler
        static_assert(SYSTICK_CALLBACK_ENABLED, "Systick callback needs to be enabled to switch tasks");

        // all the task we have
        static inline klib::dynamic_array<detail::base_task*, 16> tasks = {};

        // current and next task index
        static inline detail::base_task* current_task = nullptr;
        static inline detail::base_task* next_task = nullptr;

        /**
         * @brief Idle function. This function waits for interrupts and 
         * does nothing. This function is used when no other task is ready
         * to run.
         * 
         */
        static void idle() {
            while (true) {
                // wait for an interrupt
                asm volatile("wfi");
            }
        }

        // create an idle task that runs when no other task is available
        static inline auto idle_task = task<>(idle);

        /**
         * @brief Scheduler that is called on every systick interrupt. This
         * function decrements the sleep time for all tasks and calls the
         * scheduler to pick the next task.
         * 
         */
        static void schedule_irq() {
            // TODO: implement tick count for tasks
            // decrement sleep time for all tasks
            for (size_t i = 0; i < tasks.size(); i++) {
                if (tasks[i]->time_to_sleep.value > 0) {
                    tasks[i]->time_to_sleep.value--;
                }
            }

            // call the scheduler to pick the next task
            schedule();
        }

        /**
         * @brief Rtos scheduler. Checks which task to run next and
         * triggers a context switch if needed.
         *
         */
        static void schedule() {
            if (!tasks.size()) {
                return;
            }

            // TODO: implement a better scheduling algorithm
            // check if we have a task that is not sleeping
            for (size_t i = 0; i < tasks.size(); i++) {
                if (tasks[i]->time_to_sleep.value == 0) {
                    next_task = tasks[i];
                    break;
                }
            }

            // check if we need to switch
            if (current_task != next_task) {
                // switch to the new task by triggering the pendsv interrupt
                SCB->ICSR = (0x1 << 28);
            }
        }

        /**
         * @brief Switch to the next task
         * 
         */
        static void pendsv() {
            // call the context switcher from the target. Note: we cannot
            // do too much work in the pendsv handler as it will modify registers
            // that are needed by the context switcher
            klib::target::rtos::detail::switch_task(&current_task, &next_task);
        }

    public:
        /**
         * @brief Start the scheduler
         *
         */
        static void start() {
            // update the callback to our scheduler
            io::systick<CpuId, SYSTICK_CALLBACK_ENABLED>::set_callback(schedule_irq);

            // register our interrupt to the pendsv interrupt
            // Irq::template register_irq<Irq::arm_vector::pendsv>(pendsv_handler);
            Irq::template register_irq<Irq::arm_vector::pendsv>(pendsv);

            // add the idle task to the scheduler. This task is always ready to run
            create_task(&idle_task);

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

        /**
         * @brief Create and add a task to the scheduler
         *
         * @param stack
         * @param task
         */
        static void create_task(detail::base_task* task) {
            // add the task to the queue
            tasks.push_back(task);
        }

        /**
         * @brief Sleep the current task for the given time
         * 
         */
        static void sleep(klib::time::ms time) {
            // TODO: change this to use syscalls. This should stop
            // the current task from running and switch to another task
            // if available. As we are in unprivileged mode we cannot
            // trigger a context switch ourselves.

            // set the time to sleep for the current task
            current_task->time_to_sleep.value = time.value;

            // wait until the time to sleep is over
            while (current_task->time_to_sleep.value > 0) {
                // wait for the systick to wake us up
                asm volatile("wfi");
            }
        }
    };
}

#endif