#ifndef KLIB_TMPM373_HPP
#define KLIB_TMPM373_HPP

#include <klib/irq.hpp>

#include "tmpm373.h"

namespace klib::tmpm373 {
    // irq for the tmpm373 (see page 59 of the datasheet for the interrupt list)
    using irq = klib::KLIB_IRQ<0, 16 + 77>;

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
}

#endif
