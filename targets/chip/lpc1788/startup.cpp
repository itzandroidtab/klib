#include <cstdint>

#include "lpc1788.hpp"

#include <klib/io/systick.hpp>
#include <klib/io/core_clock.hpp>

#include <io/clocks.hpp>
#include <io/system.hpp>

// disable the constructor does not take arguments error in vscode
#ifdef __INTELLISENSE__
    #pragma diag_suppress 1094
#endif

void __attribute__((__constructor__(101))) __target_startup() {
    namespace target = klib::lpc1788;
    using clock = target::io::system::clock;

    // setup the flash wait state to 5 + 1 CPU clocks
    target::io::system::flash::setup<5>();

    // setup the clock to 120Mhz (in this example we are using a 12Mhz
    // oscillator). FCCO needs to be in range 156 - 320Mhz
    // ((10 * 2 * 12Mhz) / 1 = 240Mhz) / 2 = 120Mhz
    clock::set_main<
        clock::source::main, 12'000'000, 10, clock::pre_divider::div_1, 2
    >();

    // set the global peripheral clock divider to 1. All the
    // drivers expect this value. At the moment they do not
    // read the divider. TODO: add support for dynamic dividers
    target::io::clocks::set<target::io::clocks::divider::div_1>();

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