#ifndef KLIB_ATMEL_ATSAM4S_PORT_INTERRUPT_HPP
#define KLIB_ATMEL_ATSAM4S_PORT_INTERRUPT_HPP

#include <cstdint>

#include <klib/klib.hpp>
#include <klib/irq_helper.hpp>

#include "clocks.hpp"

namespace klib::core::atsam4s::io {
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
            // get the interrupt status from the port. This will automatically 
            // clear the interrupts for all the pins that are triggered
            const uint32_t status = Port::port->ISR;
            const uint32_t mask = Port::port->IMR;

            // handle the interrupts that are registered
            helper.handle_irq(status, mask);
        }

    public:
        // using for the interrupt callback
        using interrupt_callback = irq_helper::interrupt_callback;

        /**
         * @brief Init the port interrupt
         *
         */
        static void init() {
            // enable the power for the pio
            target::io::power_control::enable<Port>();

            // register the interrupt handler for the port to our local handler
            target::irq::template register_irq<Port::interrupt_id>(irq_handler);

            // enable the interrupt on the port
            target::enable_irq<Port::interrupt_id>();
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
            dual_edge,
            low_level,
            high_level,
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

            // set the edge select based on the edge mode
            if constexpr ((Edge == edge::falling) || (Edge == edge::rising) || (Edge == edge::dual_edge)) {
                // set the pin to edge select
                Pin::port::port->ESR = detail::pins::mask<Pin>;
            }
            else {
                // set the pin to level select
                Pin::port::port->LSR = detail::pins::mask<Pin>;
            }

            // check if we have a falling or rising edge
            if constexpr ((Edge == edge::falling) || (Edge == edge::low_level)) {
                // enable falling edge or low level
                Pin::port::port->FELLSR = detail::pins::mask<Pin>;

                // enable the additional interrupt modes register to trigger only 
                // on the falling edge/low level
                Pin::port::port->AIMER = detail::pins::mask<Pin>;
            }
            else if constexpr ((Edge == edge::rising) || (Edge == edge::high_level)) {
                // enable rising edge or high level
                Pin::port::port->REHLSR = detail::pins::mask<Pin>;

                // enable the additional interrupt modes register to trigger only 
                // on the rising edge/high level
                Pin::port::port->AIMER = detail::pins::mask<Pin>;
            }
            else {
                // clear the additional interrupt modes register to trigger on both edges
                Pin::port::port->AIMDR = detail::pins::mask<Pin>;
            }

            // enable the interrupt for the pin
            Pin::port::port->IER = detail::pins::mask<Pin>;

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
            // disable the interrupt for the pin
            Pin::port::port->IDR = detail::pins::mask<Pin>;
        }
    };
}

#endif