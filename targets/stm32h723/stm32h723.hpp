#ifndef KLIB_STM32H723_HPP
#define KLIB_STM32H723_HPP

#include <klib/irq.hpp>

// #include "stm32h723.h"

namespace klib::stm32h723 {
    // irq for the stm32h723
    using irq = klib::irq<178>;

    // /**
    //  * @brief Enable a interrupt
    //  * 
    //  * @tparam Irq 
    //  */
    // template <uint32_t Irq>
    // static void enable_irq() {
    //     static_assert(Irq >= static_cast<uint32_t>(irq::arm_vector::count), "Invalid IRQ given to enable");

    //     // enable the irq
    //     NVIC_EnableIRQ(static_cast<IRQn_Type>(Irq - static_cast<uint32_t>(irq::arm_vector::count)));
    // }

    // /**
    //  * @brief Disable a interrupt
    //  * 
    //  * @tparam Irq 
    //  */
    // template <uint32_t Irq>
    // static void disable_irq() {
    //     static_assert(Irq >= static_cast<uint32_t>(irq::arm_vector::count), "Invalid IRQ given to disable");

    //     // disable the irq
    //     NVIC_DisableIRQ(static_cast<IRQn_Type>(Irq - static_cast<uint32_t>(irq::arm_vector::count)));
    // }
}

#endif
