#include <cstdint>

#include <atsam4s2b.hpp>

#include <klib/io/systick.hpp>
#include <klib/io/core_clock.hpp>

#include <io/system.hpp>

// disable the constructor does not take arguments error in vscode
#ifdef __INTELLISENSE__
    #pragma diag_suppress 1094
#endif

void __attribute__((__constructor__(101))) __target_startup() {
    namespace target = klib::atsam4s2b;
    using clock = target::io::system::clock;

    // setup the flash wait state to 5 + 1 CPU clocks
    target::io::system::flash::setup<5>();

    // change the internal rc frequency to 12 mhz
    // clock::set_internal_rc<12'000'000>();

    // setup the clock to 120Mhz (in this example we are using a 12Mhz
    // oscillator)
    // (((12Mhz * 20) = 240Mhz) / 2) = 120Mhz
    clock::set_main<clock::source::main, clock::pll::plla, 12'000'000, 20, 2>();
    // clock::set_main<clock::source::internal, clock::pll::plla, 12'000'000, 20, 2>();

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