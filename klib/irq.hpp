#ifndef KLIB_IRQ_HPP
#define KLIB_IRQ_HPP

#include <cstdint>

#include <klib/entry/entry.hpp>

#include "math.hpp"
#include "lookuptable.hpp"

namespace klib::detail {
    /**
     * @brief Available irq types. Flash irq types 
     * have different requirements for initialization
     * that should be done by the user
     * 
     */
    enum class type {
        ram,
        flash
    };
}

namespace klib {
    /**
     * @brief Irq boot helper. Will initialize any ram irq handler
     * 
     */
    class boot_helper {
    public:
        /**
         * @brief Returns if the irq handler is in ram
         * 
         * @tparam Irq 
         * @return true 
         * @return false 
         */
        template <typename Irq>
        constexpr static bool in_ram() {
            // check if we have a ram handler
            return Irq::type == klib::detail::type::ram;
        } 

        /**
         * @brief Init the ram handler if we have one
         * 
         * @tparam Irq 
         * @tparam Systick 
         */
        template <typename Irq>
        static void init() {
            // check if we have a ram handler
            constexpr bool ram = in_ram<Irq>();

            // call init only when we have a ram handler
            if constexpr (ram) {
                Irq::init();
            }
        }
    };

    /**
     * @brief IRQ handler that relocates the vector table.
     * 
     * @brief Minimum allignment is 32 words (128 bytes) this allows up to 16 interrupts (+ 
     * default arm interrupts). For more interrupts the amount should be aligned to the next
     * power of 2. E.g. 44 should get aligned by 64 words (256 bytes).
     * 
     * @tparam CpuId 
     * @tparam IrqCount 
     * @tparam Alignment 
     */
    template <uint32_t CpuId, uint16_t IrqCount, uint32_t Alignment = klib::max(klib::exp2(32 - klib::clz(IrqCount * sizeof(uint32_t))), (32 * sizeof(uint32_t)))>
    class irq_ram {
    public:
        // using for the array of callbacks
        using interrupt_callback = void (*)();

        // the irq type
        constexpr static detail::type type = detail::type::ram;

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
        
    protected:
        // array with all the function callbacks. 
        alignas(Alignment) static inline volatile interrupt_callback callbacks[IrqCount] = {};

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
         * @tparam UpdateVectorTable 
         * @param stack_end 
         */
        template <bool UpdateVectorTable = true>
        static void init(const uint32_t* stack_end = &__stack_end) {
            // set the first position to the stack pointer
            callbacks[static_cast<uint8_t>(arm_vector::stack_ptr)] = reinterpret_cast<interrupt_callback>(stack_end);

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

            // check if we need to set the vtor register
            if constexpr (UpdateVectorTable) {
                // move the vector table to the callback address
                (*vtor) = reinterpret_cast<volatile uint32_t>(&callbacks);
            }
        }

        /**
         * @brief Return the beginning of the callback array
         * 
         * @return auto*
         */
        static auto* begin() {
            return callbacks;
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

    /**
     * @brief IRQ handler that relocates the vector table to a table in flash. This frees 
     * the memory otherwise allocated for the vector table.
     * 
     * @brief Minimum allignment is 32 words (128 bytes) this allows up to 16 interrupts (+ 
     * default arm interrupts). For more interrupts the amount should be aligned to the next
     * power of 2. E.g. 44 should get aligned by 64 words (256 bytes).
     * 
     * @tparam CpuId 
     * @tparam IrqCount 
     * @tparam Alignment 
     */
    template <uint32_t CpuId, uint16_t IrqCount, uint32_t Alignment = klib::max(klib::exp2(32 - klib::clz(IrqCount * sizeof(uint32_t))), (32 * sizeof(uint32_t)))>
    class irq_flash {
    public:
        // using for the array of callbacks
        using interrupt_callback = void (*)();

        // the irq type
        constexpr static detail::type type = detail::type::flash;

        // amount of interrupts
        constexpr static uint16_t irq_count = IrqCount;

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

        // export the alignment so the user can use it to 
        // align the vector table
        constexpr static uint32_t allignment = Alignment;

        // make sure we have at least enough entries to fit the arm vector table
        static_assert(IrqCount >= static_cast<const uint8_t>(arm_vector::count), 
            "Invalid IRQ count, cannot fit the arm vector table"
        );

    protected:
        // pointer to the vector table offset register
        static volatile inline uint32_t *const vtor = ((volatile uint32_t*)0xE000ED08);

    public:
        /**
         * @brief Set the vector table to the array provided by the user
         * 
         * @tparam UpdateVectorTable 
         * @param vectors 
         */
        template <bool UpdateVectorTable = true>
        static void init(const interrupt_callback *const vectors) {
            // check if we need to set the vtor register
            if constexpr (UpdateVectorTable) {
                // move the vector table to the callback address
                (*vtor) = reinterpret_cast<volatile uint32_t>(vectors);
            }
        }

        /**
         * @brief Return the beginning of the callback array
         * 
         * @return auto* 
         */
        static auto* begin() {
            return (*vtor);
        }

        /**
         * @brief Dummy function to keep the klib library happy
         * 
         * @tparam Irq 
         * @param callback 
         */
        template <uint32_t Irq>
        static void register_irq(const interrupt_callback &callback) {
            static_assert(Irq < IrqCount, "Invalid IRQ given to register");
        }

        /**
         * @brief Dummy function to keep the klib library happy
         * 
         * @tparam Irq 
         * @param callback 
         */
        template <arm_vector Irq>
        static void register_irq(const interrupt_callback &callback) {
            static_assert(Irq < arm_vector::count, "Count can not not be used as a arm vector entry");
        }

        /**
         * @brief Dummy function to keep the klib library happy
         * 
         * @tparam Irq 
         */
        template <uint32_t Irq>
        static void unregister_irq() {
            static_assert(Irq < IrqCount, "Invalid IRQ given to unregister");
        }
    };

    /**
     * @brief IRQ handler that allows for hooks when a interrupt is called. Uses irq_ram in the background.
     * 
     * @brief Minimum allignment is 32 words (128 bytes) this allows up to 16 interrupts (+ 
     * default arm interrupts). For more interrupts the amount should be aligned to the next
     * power of 2. E.g. 44 should get aligned by 64 words (256 bytes).
     * 
     * @tparam CpuId 
     * @tparam IrqCount 
     * @tparam klib::max(klib::exp2(32 - klib::clz(IrqCount * sizeof(uint32_t))), (32 * sizeof(uint32_t))) 
     */
    template <uint32_t CpuId, uint16_t IrqCount, uint32_t Alignment = klib::max(klib::exp2(32 - klib::clz(IrqCount * sizeof(uint32_t))), (32 * sizeof(uint32_t)))>
    class irq_hooked: public irq_ram<CpuId, IrqCount, Alignment> {
    public:
        // hook function prototype
        using hook_function = void(*)(uint32_t);

        // the irq type
        constexpr static detail::type type = detail::type::ram;

        // using for the array of callbacks
        using interrupt_callback = irq_ram<CpuId, IrqCount, Alignment>::interrupt_callback;

        // using for the arm vector enum
        using arm_vector = irq_ram<CpuId, IrqCount, Alignment>::arm_vector;

    protected:
        // entry and exit hooks
        static inline hook_function entry_hook = nullptr;
        static inline hook_function exit_hook = nullptr;

        /**
         * @brief Interrupt callback that calls the corresponding interrupt in the ram table
         * 
         */
        static void irq_handler() {
            uint32_t irq;

            // get the current interrupt using some assembly
            asm volatile("mrs %0, ipsr" : "=r"(irq));

            // check if we have a entry hook
            if (entry_hook != nullptr) {
                // call the entry hook
                entry_hook(irq);
            }

            // call the callback
            irq_ram<CpuId, IrqCount, Alignment>::callbacks[irq]();

            // check if we have a exit hook
            if (exit_hook != nullptr) {
                // call the exit hook
                exit_hook(irq);
            }
        }

        /**
         * @brief Function to set the lookup table at compile time
         * 
         * @return constexpr auto 
         */
        constexpr static auto set_func(uint32_t) {
            // return the irq handler for every item in the lookup table
            return irq_handler;
        }

        // array with all the function callbacks. 
        alignas(Alignment) constexpr static auto hooked_callbacks = lookuptable<IrqCount, interrupt_callback>(set_func);

    public:
        /**
         * @brief Relocate the interrupt table and init all functions to the default handler
         * 
         * @tparam UpdateVectorTable 
         * @param stack_end 
         */
        template <bool UpdateVectorTable = true>
        static void init(const uint32_t* stack_end = &__stack_end) {
            // update the hooks
            set_hook(nullptr, nullptr);

            // initialize the irq_ram before we move it to our own table
            irq_ram<CpuId, IrqCount, Alignment>::template init<UpdateVectorTable>(stack_end);

            // check if we need to set the vtor register
            if constexpr (UpdateVectorTable) {
                // move the vector table to the hooked array instead of the ram array
                (*irq_ram<CpuId, IrqCount, Alignment>::vtor) = reinterpret_cast<volatile uint32_t>(hooked_callbacks.begin());
            }
        }

        /**
         * @brief Return the beginning of the callback array
         * 
         * @return auto* 
         */
        static auto* begin() {
            return hooked_callbacks.begin();
        }

        /**
         * @brief Set the entry and exit hooks
         * 
         * @param entry 
         * @param exit 
         */
        static void set_hook(const hook_function& entry = nullptr, const hook_function& exit = nullptr) {
            // update the hooks
            entry_hook = entry;
            exit_hook = exit;
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

            // call the ram register
            irq_ram<CpuId, IrqCount, Alignment>::template register_irq<Irq>(callback);
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

            // call the ram register
            irq_ram<CpuId, IrqCount, Alignment>::template register_irq<Irq>(callback);
        }

        /**
         * @brief Clear a callback
         * 
         * @tparam Irq 
         */
        template <uint32_t Irq>
        static void unregister_irq() {
            static_assert(Irq < IrqCount, "Invalid IRQ given to unregister");

            // call the ram unregister
            irq_ram<CpuId, IrqCount, Alignment>::template unregister_irq<Irq>();
        }
    };
}

#endif