#ifndef KLIB_ENTRY_HPP
#define KLIB_ENTRY_HPP

#include <stdint.h>

// extern c for c linkage in c++. No ifdef cplusplus as targets 
// should be implemented using c++
extern "C" {
    // pointer to the end of the stack
    extern uint32_t __stack_end;

    /**
     * @brief Generic reset handler that initializes the .bss and .data
     * segments. It calls all the constructors and runs main. When code
     * for the main needs to be executed the "__constructor__" attribute
     * can be added to the function.
     * 
     */
    void __reset_handler();

    /**
     * @brief Default handler. Should be used to initialize the default
     * arm vector table. 
     * 
     */
    void __default_handler();
}

#endif