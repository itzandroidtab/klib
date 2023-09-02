#include <cstdint>

#include <max32660.hpp>

#include <klib/io/systick.hpp>
#include <coprocessor/coprocessor.hpp>
#include <io/system.hpp>

// disable the constructor does not take arguments error in vscode
#ifdef __INTELLISENSE__
    #pragma diag_suppress 1094
#endif

void __attribute__((__constructor__(101))) __target_startup() {
    namespace target = klib::max32660;

    // set the 96 Mhz clock source
    target::io::system::clock::set<target::io::system::clock::clock_source::hfio>();

    // enable the swd interface (should be on by default)
    target::io::system::control::enable_swd<true>();

    if constexpr (TARGET_FPU_ENABLED) {
        // using to make the access easier to the coprocessor
        using coprocessor = klib::arm::coprocessor;

        // enable the floating point coprocessors
        coprocessor::set<10>(coprocessor::access::full, &SCB->CPACR);
        coprocessor::set<11>(coprocessor::access::full, &SCB->CPACR);
        
        // enable the fpu (should be on by default)
        target::io::system::control::enable_fpu<true>();
    }

    // flush the cache to start in a known state
    target::io::system::control::flush_cache();

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