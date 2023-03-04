#ifndef KLIB_ATSAM3X8E_HPP
#define KLIB_ATSAM3X8E_HPP

#include <klib/irq.hpp>

// #include <atsam3x8e.h>

namespace klib::atsam3x8e {
    // irq for the atsam3x8e
    using irq = klib::KLIB_IRQ<16 + 28>;

    /**
     * @brief Enable a interrupt
     * 
     * @tparam Irq 
     */
    template <uint32_t Irq>
    static void enable_irq() {
        // static_assert(Irq >= static_cast<uint32_t>(irq::arm_vector::count), "Invalid IRQ given to enable");

        // // enable the irq
        // NVIC_EnableIRQ(static_cast<IRQn_Type>(Irq - static_cast<uint32_t>(irq::arm_vector::count)));
    }

    /**
     * @brief Disable a interrupt
     * 
     * @tparam Irq 
     */
    template <uint32_t Irq>
    static void disable_irq() {
        // static_assert(Irq >= static_cast<uint32_t>(irq::arm_vector::count), "Invalid IRQ given to disable");

        // // disable the irq
        // NVIC_DisableIRQ(static_cast<IRQn_Type>(Irq - static_cast<uint32_t>(irq::arm_vector::count)));
    }
}

#endif
