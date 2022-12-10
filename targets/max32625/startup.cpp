#include <cstdint>

#include <klib/systick.hpp>
#include <klib/core_clock.hpp>
#include <coprocessor/coprocessor.hpp>

#include <max32625.hpp>

// disable the constructor does not take arguments error in vscode
#ifdef __INTELLISENSE__
    #pragma diag_suppress 1094
#endif

void __attribute__((__constructor__(101))) __target_startup() {
    using target = klib::max32625;

    // change the core clock to the correct frequency
    klib::clock::set(96'000'000);

    if constexpr (TARGET_FPU_ENABLED) {
        // using to make the access easier to the coprocessor
        using coprocessor = klib::arm::coprocessor;

        // enable the floating point coprocessors
        coprocessor::set<10>(coprocessor::access::full, &SCB->CPACR);
        coprocessor::set<11>(coprocessor::access::full, &SCB->CPACR);
    }

    // init the systick timer
    klib::systick::init<target::irq>();

    // enable the systick timer
    klib::systick::enable();

    // enable MPU, bus and usage faults in seperate interrupts
    // SCB->SHCSR = 0b1011;
}