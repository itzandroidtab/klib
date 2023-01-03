#ifndef KLIB_LPC1788_HPP
#define KLIB_LPC1788_HPP

#include <klib/irq.hpp>

#include "lpc178x7x.h"

namespace klib::lpc1788 {
    // irq for the lpc1788
    using irq = klib::irq<40 + 16>;

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
}

#endif
