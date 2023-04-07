#include <cstdint>

#include <klib/io/systick.hpp>
#include <klib/io/core_clock.hpp>
#include <coprocessor/coprocessor.hpp>

#include <max32625.hpp>

// disable the constructor does not take arguments error in vscode
#ifdef __INTELLISENSE__
    #pragma diag_suppress 1094
#endif

void __attribute__((__constructor__(101))) __target_startup() {
    namespace target = klib::max32625;

    // change the core clock to the correct frequency
    klib::clock::set(96'000'000);

    if constexpr (TARGET_FPU_ENABLED) {
        // using to make the access easier to the coprocessor
        using coprocessor = klib::arm::coprocessor;

        // enable the floating point coprocessors
        coprocessor::set<10>(coprocessor::access::full, &SCB->CPACR);
        coprocessor::set<11>(coprocessor::access::full, &SCB->CPACR);
    }

    // setup the irq handler before main is called. This 
    // moves the vector table to ram so it can be changed
    // at runtime. When no interrupts are used this 
    // function call can be removed. By default interrupts
    // are mapped to a function that halts the whole cpu.
    target::irq::init();

    // init the systick timer
    klib::io::systick::init<target::irq>();

    // enable the systick timer
    klib::io::systick::enable();

    // enable MPU, bus and usage faults in seperate interrupts
    // SCB->SHCSR = 0b1011;
}