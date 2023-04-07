#include <cstdint>

#include <klib/io/systick.hpp>
#include <klib/io/core_clock.hpp>
#include <coprocessor/coprocessor.hpp>

#include "lpc1756.hpp"
#include "io/system.hpp"

// disable the constructor does not take arguments error in vscode
#ifdef __INTELLISENSE__
    #pragma diag_suppress 1094
#endif

void __attribute__((__constructor__(101))) __target_startup() {
    namespace target = klib::lpc1756;

    // setup the flash wait state to 4 + 1 CPU clocks
    target::io::system::flash::setup<4>();

    // setup the clock to 96Mhz (this is using a 12Mhz oscillator)
    // (((15 + 1) * 2 * 12Mhz) / (0 + 1) = 384Mhz) / (3 + 1) = 96Mhz
    target::io::system::clock::set<96'000'000, 15, 0, 3>();

    // setup the irq handler before main is called. This 
    // moves the vector table to ram so it can be changed
    // at runtime. When no interrupts are used this 
    // function call can be removed. By default interrupts
    // are mapped to a function that halts the whole cpu.
    target::irq::init();

    // init the systick timer
    klib::io::systick::init<target::irq, true>();

    // enable the systick timer
    klib::io::systick::enable();

    // enable MPU, bus and usage faults in seperate inpterrupts
    // SCB->SHCSR = 0b1011;
}