#include <cstdint>

#include <klib/core_clock.hpp>
#include <coprocessor/coprocessor.hpp>

// disable the constructor does not take arguments error in vscode
#ifdef __INTELLISENSE__
    #pragma diag_suppress 1094
#endif

void __attribute__((__constructor__(101))) __target_startup() {
    // change the core clock to the correct frequency
    klib::clock::set(96'000'000);

    if constexpr (TARGET_FPU_ENABLED) {
        // using to make the access easier to the coprocessor
        using coprocessor = klib::arm::coprocessor;

        // enable the floating point coprocessors
        coprocessor::enable<10>(coprocessor::access::full, &SCB->CPACR);
        coprocessor::enable<11>(coprocessor::access::full, &SCB->CPACR);
    }
}