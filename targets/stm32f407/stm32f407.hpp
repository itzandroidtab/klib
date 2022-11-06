#ifndef KLIB_STM32F407_HPP
#define KLIB_STM32F407_HPP

#include <klib/irq.hpp>

// #include "stm32f407.h"

namespace klib::stm32f407 {
    // irq for the stm32f407
    using irq = klib::irq<88>;

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
