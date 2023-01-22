#ifndef KLIB_LPC1756_PORT_HPP
#define KLIB_LPC1756_PORT_HPP

#include <cstdint>
#include <cstddef>

#include <lpc1756.hpp>

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
        switch (offset) {
            case 1:
                return offsetof(PINCONNECT_Type, PINSEL1) / 4;
            case 2:
                return offsetof(PINCONNECT_Type, PINSEL2) / 4;
            case 3:
                return offsetof(PINCONNECT_Type, PINSEL3) / 4;
            case 4:
                return offsetof(PINCONNECT_Type, PINSEL4) / 4;
            case 7:
                return offsetof(PINCONNECT_Type, PINSEL7) / 4;
            case 9:
                return offsetof(PINCONNECT_Type, PINSEL9) / 4;
            default:
            case 0:
                return offsetof(PINCONNECT_Type, PINSEL0) / 4;
        }
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

        // get the pointer to the pin select we need to write to
        volatile uint32_t *const pin_select = &(
            (reinterpret_cast<volatile uint32_t *const>(PINCONNECT))[pin_offset]
        );

        // set the 3 function registers
        if constexpr (std::is_same_v<Periph, io::detail::alternate::func_1>) {
            // setup alternate function 1
            (*pin_select) = ((*pin_select) & ~(0b11 << (Pin::number / 2))) | (0b01 << (Pin::number / 2));;
        }
        else if constexpr (std::is_same_v<Periph, io::detail::alternate::func_2>) {
            // setup alternate function 2
            (*pin_select) = ((*pin_select) & ~(0b11 << (Pin::number / 2))) | (0b10 << (Pin::number / 2));
        }
        else if constexpr (std::is_same_v<Periph, io::detail::alternate::func_3>) {
            // setup alternate function 3
            (*pin_select) |= (0b11 << (Pin::number / 2));
        }
        else constexpr {
            // setup normal gpio function
            (*pin_select) &= ~(0b11 << (Pin::number / 2));
        }
    }
}

// global peripherals, not affected by chip package
namespace klib::lpc1756::io::periph {
    struct gpio0 {
        // peripheral id (e.g gpio0, gpio1)
        constexpr static uint32_t id = 0;

        // peripheral interrupt position
        // constexpr static uint32_t irq_id = 40;

        // port to the gpio hardware
        static inline GPIO0_Type *const port = GPIO0;
    };

    struct gpio1 {
        // peripheral id (e.g gpio0, gpio1)
        constexpr static uint32_t id = 1;

        // peripheral interrupt position
        // constexpr static uint32_t irq_id = 40;

        // port to the gpio hardware
        static inline GPIO0_Type *const port = GPIO1;
    };
    
    struct gpio2 {
        // peripheral id (e.g gpio0, gpio1)
        constexpr static uint32_t id = 2;

        // peripheral interrupt position
        // constexpr static uint32_t irq_id = 40;

        // port to the gpio hardware
        static inline GPIO0_Type *const port = GPIO2;
    };

    struct gpio3 {
        // peripheral id (e.g gpio0, gpio1)
        constexpr static uint32_t id = 3;

        // peripheral interrupt position
        // constexpr static uint32_t irq_id = 40;

        // port to the gpio hardware
        static inline GPIO0_Type *const port = GPIO3;
    };

    struct gpio4 {
        // peripheral id (e.g gpio0, gpio1)
        constexpr static uint32_t id = 4;

        // peripheral interrupt position
        // constexpr static uint32_t irq_id = 40;

        // port to the gpio hardware
        static inline GPIO0_Type *const port = GPIO4;
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
}

#endif