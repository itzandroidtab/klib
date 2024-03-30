#ifdnef KLIB_TARGET_TEMPLATE_HPP
#define KLIB_TARGET_TEMPLATE_HPP

#include <klib/irq.hpp>

namespace klib::template {
    // irq for the target cpu
    using irq = klib::KLIB_IRQ<0, 16 + 16>;

    /**
     * @brief Get the current cpu id
     * 
     * @return uint32_t 
     */
    static uint32_t get_cpu_id() {
        return 0;
    }

    /**
     * @brief Enable a interrupt
     * 
     * @tparam Irq 
     */
    template <uint32_t Irq>
    static void enable_irq() {
        // enable a interrupt
    }

    /**
     * @brief Disable a interrupt
     * 
     * @tparam Irq 
     */
    template <uint32_t Irq>
    static void disable_irq() {
        // disable a interrupt
    }
    
    /**
     * @brief Global enable interrupts.
     * 
     */
    static void enable_irq() {
        // enable the global interrupts
    }

    /**
     * @brief Global disable interrupts. Prevents any interrupt from triggering
     * 
     */
    static void disable_irq() {
        // globally disable the interrupts
    }
}

#endif