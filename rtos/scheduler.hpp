#ifndef KLIB_RTOS_HPP
#define KLIB_RTOS_HPP

#include <klib/io/systick.hpp>
#include <klib/dynamic_array.hpp>
#include <klib/units.hpp>

#include <rtos/rtos.hpp>

#include "task.hpp"
#include "syscall.hpp"

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
            // check if we have any tasks when we are called
            // by the systick interrupt
            if (tasks.size() == 0) {
                // no tasks to schedule
                return;
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
            // get the current time
            const auto now = io::systick<CpuId, SYSTICK_CALLBACK_ENABLED>::get_runtime();

            // schedule the next task. We use the priority to pick
            // the next task to run. We always pick the first task
            // with the highest priority that is not sleeping. If no
            // task is ready we run the idle task.
            if (current_task == nullptr) {
                next_task = &idle_task;
            }
            else if (current_task->is_sleeping && current_task->wakup_time > now) {
                // current task is sleeping, switch to idle
                next_task = &idle_task;
            }
            else if (current_task->waitable != nullptr) {
                next_task = &idle_task;
            }
            else {
                next_task = current_task;
            }

            // TODO: implement a better scheduling algorithm
            for (size_t i = 0; i < tasks.size(); i++) {
                // get a reference to the task
                auto& task = *tasks[i];

                // no need to check the next_task again. We checked
                // it above to determine if we need to switch to the
                // idle task
                if (&task == next_task) {
                    continue;
                }

                // check if the task is sleeping
                if (task.is_sleeping) {
                    // check if the sleep time has elapsed
                    if (task.wakup_time <= now) {
                        // wake up the task
                        task.is_sleeping = false;
                    }
                    else {
                        // task is still sleeping
                        continue;
                    }
                }

                // check if the task is waiting on a waitable
                if (task.waitable != nullptr) {
                    if (task.waitable->is_waiting()) {
                        // task is waiting on a waitable
                        continue;
                    }
                    else {
                        // clear the waitable as it is no longer waiting
                        task.waitable = nullptr;
                    }
                }

                // task is not sleeping, check priority
                if (task.current_priority >= next_task->current_priority) {
                    next_task = &task;
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
        static uint32_t syscall_handler(detail::syscalls number, uint32_t arg0, uint32_t arg1, uint32_t arg2) {
            switch (number) {
                case detail::syscalls::create_task:
                    // add the task to the scheduler
                    tasks.push_back(reinterpret_cast<detail::base_task*>(arg0));
                    break;

                case detail::syscalls::delete_task:
                    // check if we have any tasks
                    if (tasks.empty()) {
                        // no tasks to delete
                        return false;
                    }

                    // find and remove the task from the scheduler. We move backwards
                    // to not mess up the indexing when erasing. Note this is something
                    // that is specific to our dynamic array implementation. 
                    // TODO: update this when changing away from the dynamic array
                    for (uint32_t i = tasks.size(); i > 0; i--) {
                        // get the index
                        const uint32_t index = i - 1;

                        // check if this is the task we want to delete
                        if (tasks[index] != reinterpret_cast<detail::base_task*>(arg0)) {
                            continue;
                        }

                        // erase the provided task
                        tasks.erase(tasks.begin() + index);

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

                    // task not found
                    return false;

                case detail::syscalls::sleep:
                    // set the time to sleep for the current task
                    current_task->wakup_time = (
                        io::systick<CpuId, SYSTICK_CALLBACK_ENABLED>::get_runtime() + 
                        klib::time::ms(arg0)
                    );

                    // mark the task as sleeping
                    current_task->is_sleeping = true;

                    // switch to the next task
                    schedule();
                    break;

                case detail::syscalls::yield:
                    // yield the cpu to the next task
                    current_task->waitable = reinterpret_cast<rtos::waitable*>(arg0);

                    // switch to the next task
                    schedule();
                    break;

                case detail::syscalls::wakeup_highest_priority_waiter:
                    // yield only if a higher priority task is waiting
                    next_task = current_task;

                    // TODO: change to a better data structure for tasks
                    for (const auto& t: tasks) {
                        // skip our current task and skip all tasks that are
                        // sleeping or not waiting
                        if (t == next_task || t->is_sleeping || t->waitable == nullptr) {
                            continue;
                        }

                        // check if the task is waiting on the provided waitable
                        if (t->waitable == reinterpret_cast<rtos::waitable*>(arg0)) {
                            // check if the task has a higher priority
                            if (t->current_priority > next_task->current_priority) {
                                next_task = t;
                            }
                        }
                    }

                    // check if we found a higher priority task
                    if (next_task != current_task) {
                        // switch to the new task by triggering the pendsv interrupt
                        SCB->ICSR = (0x1 << 28);
                    }
                    break;

                case detail::syscalls::get_time:
                    // get the current time in ms
                    return io::systick<CpuId, SYSTICK_CALLBACK_ENABLED>::get_runtime().value;
            }

            return true;
        }
    };
}

#endif