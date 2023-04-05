#include <cstdint>

#include <klib/io/systick.hpp>

#include "lpc802.hpp"
#include "io/system.hpp"

// disable the constructor does not take arguments error in vscode
#ifdef __INTELLISENSE__
    #pragma diag_suppress 1094
#endif

void __attribute__((__constructor__(101))) __target_startup() {
    namespace target = klib::lpc802;

    // change the clock frequency to 15Mhz
    target::io::system::clock::set<target::io::system::clock::clock_source::mhz_15>();

    // setup the irq handler before main is called. This 
    // moves the vector table to ram so it can be changed
    // at runtime. When no interrupts are used this 
    // function call can be removed. By default interrupts
    // are mapped to a function that halts the whole cpu.
    target::irq::init();

    // init the systick timer
    klib::systick::init<target::irq>(klib::clock::get() / 2);

    // enable the systick timer
    klib::systick::enable();

    // enable MPU, bus and usage faults in seperate interrupts
    // SCB->SHCSR = 0b1011;
}

