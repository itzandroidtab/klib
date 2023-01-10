// include the header with the global reset handler and the default handler
#include <klib/entry/entry.hpp>

// array with the initial vector table. Interrupt handlers should all point 
// to the default handler as the irq class should be used to register the 
// the handlers for interrupts. 
void (*const volatile __vectors[])(void) [[gnu::section(".vectors")]] = {
    (void (*)(void)) &__stack_end,
    __reset_handler,    // The reset handler
    __default_handler,  // The NMI handler
    __default_handler,  // The hard fault handler
    0,                  // Reserved
    0,                  // Reserved
    0,                  // Reserved
    0,                  // Reserved
    0,                  // Reserved
    0,                  // Reserved
    0,                  // Reserved
    __default_handler,  // SVCall handler
    0,                  // Reserved
    0,                  // Reserved
    __default_handler,  // The PendSV handler
    __default_handler   // The SysTick handler
};