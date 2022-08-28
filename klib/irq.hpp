#ifndef KLIB_IRQ_HPP
#define KLIB_IRQ_HPP

#include <cstdint>

#include <klib/entry/entry.hpp>

namespace klib {
    /**
     * @brief IRQ handler that relocates the vector table.
     * 
     * @tparam IrqCount 
     * @tparam Allignment 
     */
    template <uint32_t IrqCount, uint32_t Allignment>
    class irq {
    public:
        // using for the array of callbacks
        using interrupt_callback = void (*)();

        /**
         * @brief Available arm vector entries
         * 
         */
        enum class arm_vector: uint8_t {
            stack_ptr = 0,
            reset = 1,
            nmi = 2,
            hard_fault = 3,
            memory_managagement_fault = 4,
            bus_fault = 5,
            usage_fault = 6,
            svcall = 11,
            pendsv = 14,
            systick = 15,
            // end of the arm vector table. vector count should not 
            // be used as a valid vector entry.
            count
        };

        // make sure we have at least enough entries to fit the arm vector table
        static_assert(IrqCount >= static_cast<const uint8_t>(arm_vector::count), 
            "Invalid IRQ count, cannot fit the arm vector table"
        );
        
    private:
        // array with all the function callbacks. 
        alignas(Allignment) static inline volatile interrupt_callback callbacks[IrqCount] = {};

        // pointer to the vector table offset register
        static volatile inline uint32_t *const vtor = ((volatile uint32_t*)0xE000ED08);

        /**
         * @brief Reserved handler. Used to fill the vector table with a valid pointer
         * 
         */
        static void reserved() {
            // loop as some arm interrupt has happend that is not registered. Probably 
            // a hardfault, busfault or a usagefault.
            while (true) {}
        }

        /**
         * @brief Default handler that just returns as soon as its called
         * 
         */
        static void default_handler() {
            // return straight away
            return;
        }

    public:
        /**
         * @brief Relocate the interrupt table and init all functions to the default handler
         * 
         */
        static void init() {
            // set the first position to the stack pointer
            callbacks[static_cast<uint8_t>(arm_vector::stack_ptr)] = reinterpret_cast<interrupt_callback>(&__stack_end);

            // setup the arm vector table
            for (uint8_t i = static_cast<const uint8_t>(arm_vector::reset); 
                 i < static_cast<const uint8_t>(arm_vector::count); i++) 
            {
                callbacks[i] = &reserved;
            }

            // reset all the other handlers to the default handler
            for (uint32_t i = static_cast<const uint8_t>(arm_vector::count); i < IrqCount; i++) {
                callbacks[i] = &default_handler;
            }

            // move the vector table to the callback address
            (*vtor) = reinterpret_cast<volatile uint32_t>(&callbacks);
        }

        /**
         * @brief Register a callback
         * 
         * @tparam Irq 
         * @param callback 
         */
        template <uint32_t Irq>
        static void register_irq(const interrupt_callback &callback) {
            static_assert(Irq < IrqCount, "Invalid IRQ given to register");

            // set the callback in the list of callbacks
            callbacks[Irq] = callback;
        }

        /**
         * @brief Register a arm vector callback
         * 
         * @tparam Irq 
         * @param callback 
         */
        template <arm_vector Irq>
        static void register_irq(const interrupt_callback &callback) {
            static_assert(Irq < arm_vector::count, "Count can not not be used as a arm vector entry");

            // register arm vector as a callback
            return register_irq<static_cast<uint32_t>(Irq)>(callback);
        }

        /**
         * @brief Clear a callback
         * 
         * @tparam Irq 
         */
        template <uint32_t Irq>
        static void unregister_irq() {
            static_assert(Irq < IrqCount, "Invalid IRQ given to unregister");

            // clear the callback
            callbacks[Irq] = default_handler;
        }
    };
}

#endif