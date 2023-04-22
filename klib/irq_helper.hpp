#ifndef KLIB_IRQ_HELPER_HPP
#define KLIB_IRQ_HELPER_HPP

#include <cstdint>

#include <klib/math.hpp>

namespace klib {
    /**
     * @brief Masked interrupt list. Calls interrupt based on the input and 
     * the mask. Can be used as a helper class to call functions that need 
     * to be called when specific bits are set
     * 
     * @tparam Size 
     */
    template <uint32_t Size = 32>
    class irq_helper {
    public:
        // using for the array of callbacks
        using interrupt_callback = void(*)();

        // size of the masked interrupt
        constexpr static uint32_t size = Size;

    protected:
        // make sure the size is within a 32bit value and at least 1 bit
        static_assert(Size <= 32, "Masked interrupt only supports up to 32 bits");
        static_assert(Size >= 1, "Masked interrupt needs at least 1 item");

        // array with all the function callbacks. 
        interrupt_callback callbacks[Size] = {};

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
            // create the mask for the input size
            constexpr uint32_t size_mask = static_cast<uint32_t>(klib::exp2<uint64_t>(Size) - 1);

            // Reverse the bit order for the count trailing zero's.
            uint32_t masked_register = status_register & interrupt_mask & size_mask;

            // amount of trailing zeros in the status register
            uint8_t trailing_zeros = 0;

            // loop until we dont have any bits set anymore (32 == zero's)
            while ((trailing_zeros = klib::ctz(masked_register)) < 32) {
                // get the current bit to handle in the callback list
                const auto bit = static_cast<uint8_t>(trailing_zeros);

                // check if the callback is used
                if (callbacks[bit] != nullptr) {
                    // call the callback
                    callbacks[bit]();
                }

                // clear the bit from the masked register
                masked_register &= ~(1 << bit);
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
            static_assert(Irq < Size, "Invalid IRQ given to register");

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
            static_assert(Irq < Size, "Invalid IRQ given to unregister");

            // clear the callback
            callbacks[Irq] = nullptr;
        }
    };

    /**
     * @brief Masked interrupt list. Calls interrupt based on the input and 
     * the mask. Can be used as a helper class to call functions that need 
     * to be called when specific bits are set
     * 
     * @note seperate class to prevent any overhead for virtual functions
     * 
     * @tparam Size 
     */
    template <uint32_t Size = 32>
    class irq_helper_status {
    public:
        // using for the array of callbacks
        using interrupt_callback = void(*)(const uint32_t, const uint32_t);

        // size of the masked interrupt
        constexpr static uint32_t size = Size;

    protected:
        // make sure the size is within a 32bit value and at least 1 bit
        static_assert(Size <= 32, "Masked interrupt only supports up to 32 bits");
        static_assert(Size >= 1, "Masked interrupt needs at least 1 item");

        // array with all the function callbacks. 
        interrupt_callback callbacks[Size] = {};

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
            // create the mask for the input size
            constexpr uint32_t size_mask = static_cast<uint32_t>(klib::exp2<uint64_t>(Size) - 1);

            // Reverse the bit order for the count trailing zero's.
            uint32_t masked_register = status_register & interrupt_mask & size_mask;

            // amount of trailing zeros in the status register
            uint8_t trailing_zeros = 0;

            // loop until we dont have any bits set anymore (32 == zero's)
            while ((trailing_zeros = klib::ctz(masked_register)) < 32) {
                // get the current bit to handle in the callback list
                const uint8_t bit = trailing_zeros;

                // check if the callback is used
                if (callbacks[bit] != nullptr) {
                    // call the callback
                    callbacks[bit](status_register, interrupt_mask);
                }

                // clear the bit from the masked register
                masked_register &= ~(0x1 << bit);
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
            static_assert(Irq < Size, "Invalid IRQ given to register");

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
            static_assert(Irq < Size, "Invalid IRQ given to unregister");

            // clear the callback
            callbacks[Irq] = nullptr;
        }
    };
}

#endif
