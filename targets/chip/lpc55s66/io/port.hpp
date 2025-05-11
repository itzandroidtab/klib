#ifndef KLIB_LPC55S66_PORT_HPP
#define KLIB_LPC55S66_PORT_HPP

#include <cstdint>
#include <cstddef>

#include <lpc55s66.hpp>
#include "clocks.hpp"

namespace klib::lpc55s66::io::detail::alternate {
    // alternate functions for all the gpio
    // default function (view reference manual for
    // default functions for every pin)
    struct none {
        // peripheral id (e.g func0, func1)
        constexpr static uint32_t id = 0;
    };

    // alternate function 1
    struct func_1 {
        // peripheral id (e.g func0, func1)
        constexpr static uint32_t id = 1;
    };

    // alternate function 2
    struct func_2 {
        // peripheral id (e.g func0, func1)
        constexpr static uint32_t id = 2;
    };

    // alternate function 3
    struct func_3 {
        // peripheral id (e.g func0, func1)
        constexpr static uint32_t id = 3;
    };

    // alternate function 4
    struct func_4 {
        // peripheral id (e.g func0, func1)
        constexpr static uint32_t id = 4;
    };

    // alternate function 5
    struct func_5 {
        // peripheral id (e.g func0, func1)
        constexpr static uint32_t id = 5;
    };

    // alternate function 6
    struct func_6 {
        // peripheral id (e.g func0, func1)
        constexpr static uint32_t id = 6;
    };

    // alternate function 7
    struct func_7 {
        // peripheral id (e.g func0, func1)
        constexpr static uint32_t id = 7;
    };

    // alternate function 8
    struct func_8 {
        // peripheral id (e.g func0, func1)
        constexpr static uint32_t id = 8;
    };

    // alternate function 9
    struct func_9 {
        // peripheral id (e.g func0, func1)
        constexpr static uint32_t id = 9;
    };
}

namespace klib::lpc55s66::io::detail::pins {
    // get the pin mask of a pin number
    template <typename Pin>
    constexpr uint32_t mask = 1U << Pin::number;

    /**
     * @brief Helper function to set a pin to a specific peripheral
     *
     * @tparam Pin
     * @tparam Periph
     */
    template <typename Pin, typename Periph>
    static void set_peripheral() {
        // get the register offset for the pin selection
        constexpr uint32_t pin_offset = (Pin::port::id * 32) + Pin::number;

        // get the pointer to the pin select we need to write to
        volatile uint32_t *const pin_select = &(
            (reinterpret_cast<volatile uint32_t *const>(IOCON))[pin_offset]
        );

        // clear the previous function and set the new function
        (*pin_select) = ((*pin_select) & ~(0xf)) | Periph::id;
    }

    /**
     * @brief Set the open drain of a pin
     *
     * @tparam Pin
     * @tparam OpenDrain
     */
    template <typename Pin, bool OpenDrain>
    static void set_open_drain() {
        // get the register offset for the pin selection
        constexpr uint32_t pin_offset = (Pin::port::id * 32) + Pin::number;

        // get the pointer to the pin select we need to write to
        volatile uint32_t *const pin_select = &(
            (reinterpret_cast<volatile uint32_t *const>(IOCON))[pin_offset]
        );

        // set or clear based on the value
        if constexpr (OpenDrain) {
            // set the open drain mode
            (*pin_select) = (*pin_select) | 0x1 << 9;
        }
        else {
            // clear the open drain bit
            (*pin_select) = ((*pin_select) & ~(0x1 << 9));
        }
    }

    /**
     * @brief Available pin modes
     *
     */
    enum class mode {
        none = 0b00,
        pulldown = 0b01,
        pullup = 0b10,
        repeater = 0b11,
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
        constexpr uint32_t pin_offset = (Pin::port::id * 32) + Pin::number;

        // get the pointer to the pin select we need to write to
        volatile uint32_t *const pin_select = &(
            (reinterpret_cast<volatile uint32_t *const>(IOCON))[pin_offset]
        );

        // clear the previous pin mode and set the new pin mode
        (*pin_select) = ((*pin_select) & ~(0x7 << 4)) | static_cast<uint8_t>(Value) << 4;
    }

    /**
     * @brief Helper function to change the digimode of a pin
     *
     * @tparam Pin
     * @tparam Enabled
     */
    template <typename Pin, bool Enabled>
    static void set_digimode() {
        // get the register offset for the pin selection
        constexpr uint32_t pin_offset = (Pin::port::id * 32) + Pin::number;

        // get the pointer to the pin select we need to write to
        volatile uint32_t *const pin_select = &(
            (reinterpret_cast<volatile uint32_t *const>(IOCON))[pin_offset]
        );

        // set or clear based on the value
        if constexpr (Enabled) {
            // set the digimode bit
            (*pin_select) = (*pin_select) | 0x1 << 8;
        }
        else {
            // clear the digimode bit
            (*pin_select) = ((*pin_select) & ~(0x1 << 8));
        }
    }
}

// global peripherals, not affected by chip package
namespace klib::lpc55s66::io::periph {
    struct gpio0 {
        // peripheral id (e.g gpio0, gpio1)
        constexpr static uint32_t id = 0;

        // peripheral clock bit position
        constexpr static uint32_t clock_id = 14;

        // peripheral interrupt position
        // constexpr static uint32_t interrupt_id = 40;

        // port to the gpio hardware
        static inline GPIO_Type *const port = GPIO;
    };

    struct gpio1 {
        // peripheral id (e.g gpio0, gpio1)
        constexpr static uint32_t id = 1;

        // peripheral clock bit position
        constexpr static uint32_t clock_id = 15;

        // peripheral interrupt position
        // constexpr static uint32_t interrupt_id = 40;

        // port to the gpio hardware
        static inline GPIO_Type *const port = GPIO;
    };

    struct gpio2 {
        // peripheral id (e.g gpio0, gpio1)
        constexpr static uint32_t id = 2;

        // peripheral clock bit position
        constexpr static uint32_t clock_id = 16;

        // peripheral interrupt position
        // constexpr static uint32_t interrupt_id = 40;

        // port to the gpio hardware
        static inline GPIO_Type *const port = GPIO;
    };

    struct gpio3 {
        // peripheral id (e.g gpio0, gpio1)
        constexpr static uint32_t id = 3;

        // peripheral clock bit position
        constexpr static uint32_t clock_id = 17;

        // peripheral interrupt position
        // constexpr static uint32_t interrupt_id = 40;

        // port to the gpio hardware
        static inline GPIO_Type *const port = GPIO;
    };
}

namespace klib::lpc55s66::io {
    template <typename Pin>
    class pin_in {
    public:
        constexpr static void init() {
            // enable the peripheral clock of the pin
            clocks::enable<typename Pin::port>();

            // clear all the alternate functions
            detail::pins::set_peripheral<Pin, io::detail::alternate::none>();

            // set the pin as a digital input
            detail::pins::set_digimode<Pin, true>();

            // disable the output of the pin
            Pin::port::port->DIR[Pin::port::id] &= ~detail::pins::mask<Pin>;
        }

        constexpr static bool get() {
            // read the pin using the byte register (gives higher
            // performance as we do not need to and with the mask.
            // Gives slower performace when reading multiple pins)
            return GPIO->B[Pin::port::id].B_[Pin::number];
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
            // enable the peripheral clock of the pin
            clocks::enable<typename Pin::port>();

            // clear all the alternate functions
            detail::pins::set_peripheral<Pin, io::detail::alternate::none>();

            // set the direction to output
            Pin::port::port->DIR[Pin::port::id] |= detail::pins::mask<Pin>;
        }

        template <bool Val>
        constexpr static void set() {
            // Use the port id to set or clear using the mask. This is because
            // all the ports are in one structure
            if constexpr (Val) {
                Pin::port::port->SET[Pin::port::id] = detail::pins::mask<Pin>;
            }
            else {
                Pin::port::port->CLR[Pin::port::id] = detail::pins::mask<Pin>;
            }
        }

        constexpr static void set(const bool val) {
            // Use the port id to set or clear using the mask. This is because
            // all the ports are in one structure
            if (val) {
                Pin::port::port->SET[Pin::port::id] = detail::pins::mask<Pin>;
            }
            else {
                Pin::port::port->CLR[Pin::port::id] = detail::pins::mask<Pin>;
            }
        }
    };
}

#endif