#include <cstdint>

#include <mb9bf566k.hpp>

#include <klib/io/systick.hpp>
#include <coprocessor/coprocessor.hpp>

#include <io/system.hpp>

// disable the constructor does not take arguments error in vscode
#ifdef __INTELLISENSE__
    #pragma diag_suppress 1094
#endif

void __attribute__((__constructor__(101))) __target_startup() {
    namespace target = klib::mb9bf566k;
    namespace system = target::io::system;

    // configure the flash accelerator up to 200 mhz
    system::flash::setup<system::flash::waitstate::upto_200mhz, 0>();

    // setup the clock to 156Mhz (in this example we are using a 12Mhz
    // oscillator)
    // (((12Mhz * 26) = 240Mhz) / 2) = 156Mhz
    system::clock::set_main<system::clock::source::main, system::clock::pll::pll, 12'000'000, 26, 2>();

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