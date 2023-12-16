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
    // (((9 + 1) * 2 * 12Mhz) / (0 + 1) = 240Mhz) / (1 + 1) = 120Mhz
    clock::set_main<
        clock::source::main, 120'000'000, 9, clock::pre_divider::div_1, 1
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
    target::irq::init();

    // init the systick timer
    klib::io::systick<>::init<target::irq, true>();

    // enable the systick timer
    klib::io::systick<>::enable();

    // enable MPU, bus and usage faults in separate inpterrupts
    SCB->SHCSR = 0b111 << 16;
}