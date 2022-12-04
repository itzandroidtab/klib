#ifdnef KLIB_TARGET_TEMPLATE_HPP
#define KLIB_TARGET_TEMPLATE_HPP

#include <klib/irq.hpp>

namespace klib::template {
    // irq for the target cpu
    using irq = klib::irq<32>;

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
}

#endif