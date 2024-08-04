#ifndef KLIB_NXP_LPC175X_PORT_INTERRUPT_HPP
#define KLIB_NXP_LPC175X_PORT_INTERRUPT_HPP

#include <cstdint>

#include <klib/klib.hpp>
#include <klib/irq_helper.hpp>

#include "clocks.hpp"

namespace klib::core::lpc175x::io::detail {
    /**
     * @brief External 3 interrupt handler. Handles the GPIO interrupt
     * by calling the correct callback for every port
     *
     * @todo: add support for the EXT3 interrupt
     *
     */
    class ext3_interrupt {
    public:
        // using for the array of callbacks
        using interrupt_callback = void(*)();

        // clock id of the gpio interrupts
        constexpr static uint32_t clock_id = 17;

    protected:
        // amount of ports that are handled in this interrupt
        constexpr static uint32_t port_count = 2;

        // interrupt callbacks
        static inline interrupt_callback callbacks[port_count] = {nullptr, nullptr};

    public:
        template <typename Port>
        static void init() {
            // enable the clock to the peripheral
            clocks::template set<ext3_interrupt, clocks::divider::div_1>();

            // register the interrupt handler
            target::irq::template register_irq<Port::interrupt_id>(irq_handler);

            // enable the interrupt
            target::enable_irq<Port::interrupt_id>();
        }

        /**
         * @brief Register a callback
         *
         * @tparam Irq
         * @param callback
         */
        template <typename Port>
        static void register_irq(const interrupt_callback& callback) {
            static_assert((Port::id / 2) < port_count, "Invalid Can Id provided to register");

            // register the callback
            callbacks[Port::id / 2] = callback;
        }

        /**
         * @brief Clear a callback
         *
         * @tparam Irq
         */
        template <typename Port>
        static void unregister_irq() {
            static_assert((Port::id / 2) < port_count, "Invalid Can Id provided to unregister");

            // clear the callback
            callbacks[Port::id / 2] = nullptr;
        }

        /**
         * @brief Interrupt handler for the can hardware. This should only be
         * called from NVIC
         *
         */
        static void irq_handler() {
            // get the port status register
            const uint32_t status = GPIOINT->STATUS;

            // check what port triggered the interrupt
            if ((status & 0x1) && callbacks[0]) {
                // call the port0 callback
                callbacks[0]();
            }

            if ((status & 0x2) && callbacks[1]) {
                // call the port1 callback
                callbacks[1]();
            }
        }
    };
}

namespace klib::core::lpc175x::io {
    /**
     * @brief Interrupt handler for a port. Uses irq helper to handle the interrupts
     *
     * @tparam Port
     */
    template <typename Port>
    class port_interrupt {
    protected:
        using irq_helper = klib::irq_helper<32>;

        // irq helper to handle all interrups
        static inline irq_helper helper;

        /**
         * @brief Interrupt handler
         *
         */
        static void irq_handler() {
            // get the interrupt status from the port
            const uint32_t rising_status = GPIOINT->PORT[Port::id / 2].STATR;
            const uint32_t falling_status = GPIOINT->PORT[Port::id / 2].STATF;

            // interrupt masks
            const uint32_t rising_mask = GPIOINT->PORT[Port::id / 2].ENR;
            const uint32_t falling_mask = GPIOINT->PORT[Port::id / 2].ENF;

            // clear the interrupt flags
            GPIOINT->PORT[Port::id / 2].CLR = (
                (rising_status | falling_status) & (rising_mask | falling_mask)
            );

            // handle the interrupts that are registered
            helper.handle_irq(rising_status | falling_status, rising_mask | falling_mask);
        }

    public:
        // using for the interrupt callback
        using interrupt_callback = irq_helper::interrupt_callback;

        /**
         * @brief Init the port interrupt
         *
         */
        static void init() {
            // register the interrupt
            detail::ext3_interrupt::template register_irq<Port>(irq_handler);

            // init the can interrupt
            detail::ext3_interrupt::template init<Port>();
        }

        /**
         * @brief Register a interrupt for a specific pin
         *
         * @tparam Pin
         * @param callback
         */
        template <typename Pin>
        static void register_irq(irq_helper::interrupt_callback callback) {
            // check if the port is correct
            static_assert(std::is_same_v<typename Pin::port, Port>, "Pin is not for this port");

            // register the interrupt with a specific pin
            helper.register_irq<Pin::number>(callback);
        }

        /**
         * @brief Unregister a interrupt for a specific pin
         *
         * @tparam Pin
         */
        template <typename Pin>
        static void unregister_irq() {
            // check if the port is correct
            static_assert(std::is_same_v<typename Pin::port, Port>, "Pin is not for this port");

            // clear the interrupt from the helper irq
            helper.unregister_irq<Pin::number>();
        }
    };

    template <typename Pin>
    class pin_interrupt {
    public:
        /**
         * @brief Using for the interrupt callback used in the pin irq
         *
         */
        using interrupt_callback = port_interrupt<typename Pin::port>::interrupt_callback;

    public:
        /**
         * @brief Supported trigger modes
         *
         */
        enum class edge {
            falling,
            rising,
            dual_edge
        };

        /**
         * @brief Initialize a pin with a callback
         *
         * @tparam Edge
         * @param callback
         */
        template <edge Edge>
        constexpr static void init(interrupt_callback callback) {
            // setup the pin as input
            pin_in<Pin>::init();

            // check what interrupt to
            if constexpr (Edge == edge::rising) {
                GPIOINT->PORT[Pin::port::id / 2].ENR |= detail::pins::mask<Pin>;
                GPIOINT->PORT[Pin::port::id / 2].ENF &= ~detail::pins::mask<Pin>;
            }
            else if constexpr (Edge == edge::falling) {
                GPIOINT->PORT[Pin::port::id / 2].ENR &= ~detail::pins::mask<Pin>;
                GPIOINT->PORT[Pin::port::id / 2].ENF |= detail::pins::mask<Pin>;
            }
            else {
                GPIOINT->PORT[Pin::port::id / 2].ENR |= detail::pins::mask<Pin>;
                GPIOINT->PORT[Pin::port::id / 2].ENF |= detail::pins::mask<Pin>;
            }

            // register the interrupt in the port
            port_interrupt<typename Pin::port>::template register_irq<Pin>(callback);

            // init the port
            port_interrupt<typename Pin::port>::init();
        }

        /**
         * @brief Disable the interrupt on the pin
         *
         */
        constexpr static void disable() {
            // clear the pin from the interrupt list
            GPIOINT->PORT[Pin::port::id / 2].ENR &= ~detail::pins::mask<Pin>;
            GPIOINT->PORT[Pin::port::id / 2].ENF &= ~detail::pins::mask<Pin>;
        }
    };
}

#endif