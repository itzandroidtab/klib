#ifndef KLIB_LPC55S66_HPP
#define KLIB_LPC55S66_HPP

#include <klib/irq.hpp>

#include "lpc55s66.h"

namespace klib::lpc55s66 {
    // irq for the lpc55s66
    using irq0 = klib::irq::KLIB_IRQ<0, 16 + 40>;
    using irq1 = klib::irq::KLIB_IRQ<1, 16 + 40>;

    /**
     * @brief Get the current cpu id
     *
     * @return uint32_t
     */
    static uint32_t get_cpu_id() {
        // pointer to the vector table offset register
        const volatile uint32_t *const vtor = ((volatile uint32_t*)0xE000ED08);

        // check if the current vector table points to irq0 or irq1
        if ((*vtor) == reinterpret_cast<const uint32_t>(irq0::begin())) {
            // vector table matches irq0. We are running on CPU0
            return 0;
        }

        // not CPU0. That only leaves CPU1
        return 1;
    }

    /**
     * @brief Enable a interrupt
     *
     * @tparam Irq
     */
    template <uint32_t Irq>
    static void enable_irq() {
        static_assert(Irq >= static_cast<uint32_t>(irq0::arm_vector::count), "Invalid IRQ given to enable");

        // enable the irq
        NVIC_EnableIRQ(static_cast<IRQn_Type>(Irq - static_cast<uint32_t>(irq0::arm_vector::count)));
    }

    /**
     * @brief Disable a interrupt
     *
     * @tparam Irq
     */
    template <uint32_t Irq>
    static void disable_irq() {
        static_assert(Irq >= static_cast<uint32_t>(irq0::arm_vector::count), "Invalid IRQ given to disable");

        // disable the irq
        NVIC_DisableIRQ(static_cast<IRQn_Type>(Irq - static_cast<uint32_t>(irq0::arm_vector::count)));
    }

    /**
     * @brief Global enable interrupts.
     *
     */
    static void enable_irq() {
        __enable_irq();
    }

    /**
     * @brief Global disable interrupts. Prevents any interrupt from triggering
     *
     */
    static void disable_irq() {
        __disable_irq();
    }

    /**
     * @brief Set the priority of a interrupt
     *
     * @tparam Irq
     */
    template <irq::arm_vector Irq, uint8_t Priority>
    static void interrupt_priority() {
        static_assert(static_cast<uint32_t>(Irq) >= static_cast<uint32_t>(irq::arm_vector::count), "Invalid IRQ given to set priority");
        static_assert(Priority < (1U << __NVIC_PRIO_BITS), "Invalid priority given to set priority");

        // set the priority
        NVIC_SetPriority(
            static_cast<IRQn_Type>(
                static_cast<uint32_t>(Irq) - static_cast<uint32_t>(irq::arm_vector::count)), 
            Priority
        );
    }

    /**
     * @brief Set the priority of a interrupt
     *
     * @tparam Irq
     */
    template <uint32_t Irq, uint8_t Priority>
    static void interrupt_priority() {
        static_assert(Irq >= static_cast<uint32_t>(irq::arm_vector::count), "Invalid IRQ given to set priority");
        static_assert(Priority < (1U << __NVIC_PRIO_BITS), "Invalid priority given to set priority");

        // set the priority
        NVIC_SetPriority(static_cast<IRQn_Type>(Irq - static_cast<uint32_t>(irq::arm_vector::count)), Priority);
    }
}

#endif
