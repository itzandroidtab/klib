#ifndef KLIB_MASKED_IRQ_HPP
#define KLIB_MASKED_IRQ_HPP

#include <cstdint>

#include <klib/klib.hpp>
#include <klib/math.hpp>

namespace klib {
    /**
     * @brief Masked interrupt list. Calls interrupt based on the input and 
     * the mask. Can be used as a helper class to call functions that need 
     * to be called when specific bits are set
     * 
     */
    class masked_irq {
    public:
        // using for the array of callbacks
        using interrupt_callback = void (*)(const uint32_t status_register, const uint32_t interrupt_mask);
        
    protected:
        // max amount of interrupts that fit 
        constexpr static uint32_t max_irq_amount = 32;

        // array with all the function callbacks. 
        interrupt_callback callbacks[max_irq_amount] = {};

    public:
        /**
         * @brief Handle all the bits in the status register (masked by the interrupt mask) by 
         * calling the callback function.
         * 
         * @warning Does not clear any bits in the status register of the handler that calls
         * this function
         * 
         * @param status_register 
         * @param interrupt_mask 
         */
        void handle_irq(const uint32_t status_register, const uint32_t interrupt_mask) {
            // Reverse the bit order for the count trailing zero's.
            uint32_t masked_register = klib::rbit(status_register & interrupt_mask);

            // amount of trailing zeros in the status register
            uint8_t trailing_zeros = 0;

            // loop until we dont have any bits set anymore (32 == zero's)
            while ((trailing_zeros = __CLZ(masked_register)) < 32) {
                // get the current bit to handle in the callback list
                const auto bit = static_cast<uint8_t>(trailing_zeros);

                // check if the callback is used
                if (callbacks[bit] != nullptr) {
                    callbacks[bit](status_register, interrupt_mask);
                }

                // clear the bit from the masked register
                masked_register &= ~(1 << (31 - bit));
            }
        }

        /**
         * @brief Register a callback
         * 
         * @tparam Irq 
         * @param callback 
         */
        template <uint32_t Irq>
        void register_irq(const interrupt_callback &callback) {
            static_assert(Irq < max_irq_amount, "Invalid IRQ given to register");

            // set the callback in the list of callbacks
            callbacks[Irq] = callback;
        }

        /**
         * @brief Clear a callback
         * 
         * @tparam Irq 
         */
        template <uint32_t Irq>
        void unregister_irq() {
            static_assert(Irq < max_irq_amount, "Invalid IRQ given to unregister");

            // clear the callback
            callbacks[Irq] = nullptr;
        }
    };
}

#endif