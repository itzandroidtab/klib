#ifndef KLIB_LPC1756_PORT_HPP
#define KLIB_LPC1756_PORT_HPP

#include <cstdint>
#include <cstddef>

#include <lpc1756.hpp>

#include <klib/irq_helper.hpp>

#include "clocks.hpp"

namespace klib::lpc1756::io::detail::alternate {
    // alternate functions for all the gpio
    // default function (view reference manual for 
    // default functions for every pin)
    struct none {};

    // alternate function 1
    struct func_1 {};

    // alternate function 2
    struct func_2 {};

    // alternate function 3
    struct func_3 {};
}

namespace klib::lpc1756::io::detail::pins {
    // get the pin mask of a pin number
    template <typename Pin>
    constexpr uint32_t mask = 1U << Pin::number;

    /**
     * @brief Get the pointer to the correct pin select register
     * 
     * @tparam Pin 
     * @return uint32_t* const 
     */
    template <typename Pin>
    constexpr uint32_t get_pinselect_offset() {
        // get the offset by checking the id of the port and if the pin number is above 15
        constexpr uint32_t offset = (Pin::port::id * 2) + (Pin::number >= (32 / 2));

        // make sure the offset is valid
        static_assert(
            offset == 0 || offset == 1 || offset == 2 || 
            offset == 3 || offset == 4 || offset == 7 || 
            offset == 9,
            "Pin offset is not correct" 
        );

        // check what pointer to return
        return offset;
    }

    /**
     * @brief Helper function to set a pin to a specific peripheral
     * 
     * @tparam Pin 
     * @tparam Periph 
     */
    template <typename Pin, typename Periph>    
    static void set_peripheral() {
        // get the register offset for the pin selection
        constexpr uint32_t pin_offset = get_pinselect_offset<Pin>();

        // set the 3 function registers
        if constexpr (std::is_same_v<Periph, io::detail::alternate::func_1>) {
            // setup alternate function 1
            PINCONNECT->PINSEL[pin_offset] = (
                PINCONNECT->PINSEL[pin_offset] & ~(0b11 << ((Pin::number * 2)) % 32)) | (0b01 << ((Pin::number * 2)) % 32
            );
        }
        else if constexpr (std::is_same_v<Periph, io::detail::alternate::func_2>) {
            // setup alternate function 2
            PINCONNECT->PINSEL[pin_offset] = (
                PINCONNECT->PINSEL[pin_offset] & ~(0b11 << ((Pin::number * 2)) % 32)) | (0b10 << ((Pin::number * 2)) % 32
            );
        }
        else if constexpr (std::is_same_v<Periph, io::detail::alternate::func_3>) {
            // setup alternate function 3
            PINCONNECT->PINSEL[pin_offset] |= (0b11 << ((Pin::number * 2)) % 32);
        }
        else {
            // setup normal gpio function
            PINCONNECT->PINSEL[pin_offset] &= ~(0b11 << ((Pin::number * 2)) % 32);
        }
    }

    /**
     * @brief Available pin modes
     * 
     */
    enum class mode {
        pullup = 0b00,
        repeater = 0b01,
        none = 0b10,
        pulldown = 0b11
    };

    /**
     * @brief Helper function to set the mode of a pin. 
     * 
     * @tparam Pin 
     * @tparam Value 
     */
    template <typename Pin, mode Value>
    static void set_pinmode() {
        // get the register offset for the pin selection
        constexpr uint32_t pin_offset = get_pinselect_offset<Pin>();

        // clear the previous value and set the new value
        PINCONNECT->PINMODE[pin_offset] = (PINCONNECT->PINMODE[pin_offset] & ~(
            (static_cast<uint8_t>(Value) & 0b11) << ((Pin::number * 2)) % 32)) | 
            ((static_cast<uint8_t>(Value) & 0b11) << ((Pin::number * 2)
        ) % 32);
    }

    /**
     * @brief Set the open drain of a pin
     * 
     * @tparam Pin 
     * @tparam OpenDrain 
     */
    template <typename Pin, bool OpenDrain>
    static void set_open_drain() {        
        // set or clear based on the value
        if constexpr (OpenDrain) {
            PINCONNECT->PINMODE[Pin::port::id] |= mask<Pin>;
        }
        else {
            PINCONNECT->PINMODE[Pin::port::id] &= ~(mask<Pin>);
        }
    }
}

// global peripherals, not affected by chip package
namespace klib::lpc1756::io::periph {
    struct gpio0 {
        // peripheral id (e.g gpio0, gpio1)
        constexpr static uint32_t id = 0;

        // interrupt id (including the arm vector table)
        constexpr static uint32_t interrupt_id = 37;

        // port to the gpio hardware
        static inline GPIO0_Type *const port = GPIO0;
    };

    struct gpio1 {
        // peripheral id (e.g gpio0, gpio1)
        constexpr static uint32_t id = 1;

        // port to the gpio hardware
        static inline GPIO0_Type *const port = GPIO1;
    };
    
    struct gpio2 {
        // peripheral id (e.g gpio0, gpio1)
        constexpr static uint32_t id = 2;

        // interrupt id (including the arm vector table)
        constexpr static uint32_t interrupt_id = 37;

        // port to the gpio hardware
        static inline GPIO0_Type *const port = GPIO2;
    };

    struct gpio3 {
        // peripheral id (e.g gpio0, gpio1)
        constexpr static uint32_t id = 3;

        // port to the gpio hardware
        static inline GPIO0_Type *const port = GPIO3;
    };

    struct gpio4 {
        // peripheral id (e.g gpio0, gpio1)
        constexpr static uint32_t id = 4;

        // port to the gpio hardware
        static inline GPIO0_Type *const port = GPIO4;
    };
}

namespace klib::lpc1756::io::detail {
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
        // amount of can devices that can be serviced in the single interrupt
        constexpr static uint32_t port_count = 2;

        // interrupt callbacks
        static inline interrupt_callback callbacks[port_count] = {nullptr, nullptr};

    public:
        template <typename Port>
        static void init() {
            // enable the clock to the peripheral
            clocks::set<ext3_interrupt, clocks::divider::div_1>();

            // register the interrupt handler
            irq::register_irq<Port::interrupt_id>(irq_handler);

            // enable the interrupt
            enable_irq<Port::interrupt_id>();
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

namespace klib::lpc1756::io {
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
}

namespace klib::lpc1756::io {
    template <typename Pin>
    class pin_in {
    public:
        constexpr static void init() {
            // clear all the alternate functions
            detail::pins::set_peripheral<Pin, io::detail::alternate::none>();

            // set the pin as a input (clear the pin int the direction mask)
            Pin::port::port->DIR &= ~(detail::pins::mask<Pin>);
        }

        constexpr static bool get() {
            // get the status of the pin
            return Pin::port::port->PIN & detail::pins::mask<Pin>;
        }

        template <bool Val>
        constexpr static void pullup_enable() {
            detail::pins::set_pinmode<Pin, 
                Val ? detail::pins::mode::pullup : detail::pins::mode::none
            >();
        }

        template <bool Val>
        constexpr static void pulldown_enable() {
            detail::pins::set_pinmode<Pin, 
                Val ? detail::pins::mode::pulldown : detail::pins::mode::none
            >();
        }        
    };
 
    template <typename Pin>
    class pin_out {
    public:
        constexpr static void init() {
            // clear all the alternate functions
            detail::pins::set_peripheral<Pin, io::detail::alternate::none>();

            // enable the gpio output
            Pin::port::port->DIR |= detail::pins::mask<Pin>;
        }

        template <bool Val>
        constexpr static void set() {
            if constexpr (Val) {
                Pin::port::port->SET = detail::pins::mask<Pin>;
            }
            else {
                Pin::port::port->CLR = detail::pins::mask<Pin>;
            }
        }

        constexpr static void set(const bool val) {
            if (val) {
                Pin::port::port->SET = detail::pins::mask<Pin>;
            }
            else {
                Pin::port::port->CLR = detail::pins::mask<Pin>;
            }
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