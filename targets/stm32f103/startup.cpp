#include <cstdint>

#include "stm32f103.hpp"

#include <klib/io/systick.hpp>
#include <klib/io/core_clock.hpp>

// disable the constructor does not take arguments error in vscode
#ifdef __INTELLISENSE__
    #pragma diag_suppress 1094
#endif

void __attribute__((__constructor__(101))) __target_startup() {
    namespace target = klib::stm32f103;

    // default frequency is 8 mhz
    klib::io::clock::set(8'000'000);

    // setup the irq handler before main is called. This 
    // moves the vector table to ram so it can be changed
    // at runtime. When no interrupts are used this 
    // function call can be removed. By default interrupts
    // are mapped to a function that halts the whole cpu.
    target::irq::init();

    // init the systick timer
    klib::io::systick<>::init<target::irq, true>();

    // enable the systick timer
    klib::io::systick<>::enable();

    // enable MPU, bus and usage faults in separate inpterrupts
    SCB->SHCSR = 0b111 << 16;
}