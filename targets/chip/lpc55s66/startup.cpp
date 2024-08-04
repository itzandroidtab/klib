#include <cstdint>

#include <klib/io/systick.hpp>
#include <klib/io/core_clock.hpp>

#include <coprocessor/coprocessor.hpp>
#include <io/system.hpp>

// disable the constructor does not take arguments error in vscode
#ifdef __INTELLISENSE__
    #pragma diag_suppress 1094
#endif

void __attribute__((__constructor__(101))) __target_startup() {
    namespace target = klib::lpc55s66;

    // using to make the access easier to the coprocessor
    using coprocessor = klib::arm::coprocessor;

    // the default cpu speed is 48 mhz (for now as
    // the boot rom is switching to that)
    klib::io::clock::set(48'000'000);

    // check if we need to enable the fpu
    if constexpr (TARGET_FPU_ENABLED) {
        // enable the floating point coprocessors
        coprocessor::set<10>(coprocessor::access::full, &SCB->CPACR);
        coprocessor::set<11>(coprocessor::access::full, &SCB->CPACR);
    }

    // allow full access to both casper and the power
    // quad coprocessors
    coprocessor::set<1>(coprocessor::access::full, &SCB->CPACR);
    coprocessor::set<0>(coprocessor::access::full, &SCB->CPACR);

    // enable dap on both cpus
    target::io::system::debug::enable<0>();
    target::io::system::debug::enable<1>();

    // setup the irq handler before main is called. This
    // moves the vector table to ram so it can be changed
    // at runtime. When no interrupts are used this
    // function call can be removed. By default interrupts
    // are mapped to a function that halts the whole cpu.
    // this call does nothing when a flash handler is
    // configured
    klib::irq::boot_helper::init<target::irq>();

    // check if we can enable the systick. If we enable it
    // here when we have a flash irq handler we will cause
    // a hang in the default irq handler
    if constexpr (klib::irq::boot_helper::in_ram<target::irq>()) {
        // init the systick timer
        klib::io::systick<>::init<target::irq, true>();

        // enable the systick timer
        klib::io::systick<>::enable();
    }

    // enable MPU, bus and usage faults in separate inpterrupts
    SCB->SHCSR = 0b111 << 16;
}
