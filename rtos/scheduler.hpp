#ifndef KLIB_RTOS_HPP
#define KLIB_RTOS_HPP

#include <klib/io/systick.hpp>
#include <klib/dynamic_array.hpp>
#include <klib/units.hpp>

#include <rtos/rtos.hpp>

#include "task.hpp"
#include "syscall.hpp"

namespace klib::rtos {
    class scheduler {
    public:
        /**
         * @brief All the available syscalls
         * 
         */
        enum class syscalls {
            create_task = 0,
            delete_task = 1,
            yield = 2,
            sleep = 3,
            malloc = 4,
            free = 5
        };

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
            // check if we have any tasks when we are called
            // by the systick interrupt
            if (tasks.size() == 0) {
                // no tasks to schedule
                return;
            }

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
            // schedule the next task. We use the priority to pick
            // the next task to run. We always pick the first task
            // with the highest priority that is not sleeping. If no
            // task is ready we run the idle task.
            next_task = (current_task == nullptr ? &idle_task : (
                current_task->time_to_sleep.value == 0 ? current_task : &idle_task
            ));

            // TODO: implement a better scheduling algorithm
            for (size_t i = 0; i < tasks.size(); i++) {
                if (tasks[i]->time_to_sleep.value > 0) {
                    // task is sleeping
                    continue;
                }

                // task is not sleeping, check priority
                if (tasks[i]->current_priority > next_task->current_priority) {
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
        template <uint32_t CpuId, typename Irq>
        static void start() {
            // update the callback to our scheduler
            io::systick<CpuId, SYSTICK_CALLBACK_ENABLED>::set_callback(schedule_irq);

            // register our interrupt to the pendsv interrupt
            Irq::template register_irq<Irq::arm_vector::pendsv>(pendsv);

            // register the syscall handler
            Irq::template register_irq<Irq::arm_vector::svcall>(klib::target::rtos::detail::syscall_handler<scheduler>);

            // add the idle task to the scheduler. This task is always ready to run
            create_task(&idle_task);

            // start the scheduler from the target
            klib::target::rtos::detail::scheduler_start();
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
        
    public:
        /**
         * @brief Syscall handler, called when a syscall is invoked 
         * by the target implementation. Parameters need to be passed
         * in registers r1-r3. These are converted to the correct
         * types based on the syscall.
         * 
         * @param number 
         * @param arg0 
         * @param arg1 
         * @param arg2 
         * @return uint32_t 
         */
        static uint32_t syscall_handler(syscalls number, uint32_t arg0, uint32_t arg1, uint32_t arg2) {
            switch (number) {
                case syscalls::create_task:
                    // add the task to the scheduler
                    tasks.push_back(reinterpret_cast<detail::base_task*>(arg0));
                    break;

                case syscalls::delete_task:
                    // find and remove the task from the scheduler. We move backwards
                    // to not mess up the indexing when erasing. Note this is something
                    // that is specific to our dynamic array implementation. 
                    // TODO: update this when changing away from the dynamic array
                    for (size_t i = 0; i < tasks.size(); i++) {
                        if (tasks[i] == reinterpret_cast<detail::base_task*>(arg0)) {
                            // erase the provided task
                            tasks.erase(tasks.begin() + i);

                            // check if we are running the task we are deleting
                            if (current_task == reinterpret_cast<detail::base_task*>(arg0)) {
                                // clear the current task and switch to the 
                                // next one
                                current_task = nullptr;

                                // switch to the next task
                                schedule();
                            }
                            
                            // task found and deleted
                            return true;
                        }
                    }

                    // task not found
                    return false;

                case syscalls::yield:
                    // yield the cpu to the next task
                    schedule();
                    break;

                case syscalls::sleep:
                    // set the time to sleep for the current task
                    current_task->time_to_sleep.value = arg0;

                    // switch to the next task
                    schedule();
                    break;
            }

            return true;
        }
    };
}

#endif