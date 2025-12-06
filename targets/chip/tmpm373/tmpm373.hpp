#ifndef KLIB_TMPM373_HPP
#define KLIB_TMPM373_HPP

#include <klib/irq.hpp>

#include "tmpm373.h"

namespace klib::tmpm373 {
    // irq for the tmpm373 (see page 86 of the datasheet for the interrupt list)
    using irq = klib::irq::KLIB_IRQ<0, 16 + 77>;

    // amount of cpu cores
    constexpr static uint32_t cpu_cores = 1;

    /**
     * @brief Get the current cpu id
     *
     * @return uint32_t
     */
    static uint32_t get_cpu_id() {
        // NOTE: this mcu only has 1 core. So we always
        // return id 0
        return 0;
    }

    /**
     * @brief Enable a interrupt
     *
     * @tparam Irq
     */
    template <uint32_t Irq>
    static void enable_irq() {
        static_assert(Irq >= static_cast<uint32_t>(irq::arm_vector::count), "Invalid IRQ given to enable");

        // enable the irq
        NVIC_EnableIRQ(static_cast<IRQn_Type>(Irq - static_cast<uint32_t>(irq::arm_vector::count)));
    }

    /**
     * @brief Disable a interrupt
     *
     * @tparam Irq
     */
    template <uint32_t Irq>
    static void disable_irq() {
        static_assert(Irq >= static_cast<uint32_t>(irq::arm_vector::count), "Invalid IRQ given to disable");

        // disable the irq
        NVIC_DisableIRQ(static_cast<IRQn_Type>(Irq - static_cast<uint32_t>(irq::arm_vector::count)));
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
