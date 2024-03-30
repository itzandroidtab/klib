#ifndef KLIB_LPC1752_UART_HPP
#define KLIB_LPC1752_UART_HPP

#include <cstdint>

#include <lpc1752.hpp>

namespace klib::lpc1752::io::periph {
    struct uart0 {
        // peripheral id (e.g uart0, uart1)
        constexpr static uint32_t id = 0;

        // interrupt id (including the arm vector table)
        constexpr static uint32_t interrupt_id = 21;

        // port to the UART hardware
        static inline UART0_Type *const port = UART0;
    };

    struct uart1 {
        // peripheral id (e.g uart0, uart1)
        constexpr static uint32_t id = 1;

        // interrupt id (including the arm vector table)
        constexpr static uint32_t interrupt_id = 22;

        // port to the UART hardware
        static inline UART1_Type *const port = UART1;
    };

    struct uart2 {
        // peripheral id (e.g uart0, uart1)
        constexpr static uint32_t id = 2;

        // interrupt id (including the arm vector table)
        constexpr static uint32_t interrupt_id = 23;

        // port to the UART hardware
        static inline UART0_Type *const port = UART2;
    };

    struct uart3 {
        // peripheral id (e.g uart0, uart1)
        constexpr static uint32_t id = 3;

        // interrupt id (including the arm vector table)
        constexpr static uint32_t interrupt_id = 24;

        // port to the UART hardware
        static inline UART0_Type *const port = UART3;
    };
}

#endif