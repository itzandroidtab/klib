#ifndef KLIB_STM32F103_PORT_HPP
#define KLIB_STM32F103_PORT_HPP

#include <cstdint>
#include <cstddef>

#include <stm32f103.hpp>

namespace klib::stm32f103::io::detail::alternate {
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

namespace klib::stm32f103::io::detail::pins {
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

    }

    /**
     * @brief Direction
     * 
     */
    enum class direction_mode {
        input = 0b00,
        output_10mhz = 0b01,
        output_2mhz = 0b10,
        output_50mhz = 0b11
    };

    /**
     * @brief Set the pinmode of a pin
     * 
     * @tparam Pin 
     * @tparam Mode 
     */
    template <typename Pin, direction_mode Mode>
    static void set_pinmode() {
        // check if we need to write to the low or high register
        if constexpr (Pin::number < 8) {
            // get the current register
            const uint32_t reg = Pin::port::port->CRL;

            // set the new pinmode
            Pin::port::port->CRL = (
                (reg & ~(0b11 << (Pin::number * 4))) | 
                (static_cast<uint32_t>(Mode) << (Pin::number * 4))
            );
        }
        else {
            // get the current register
            const uint32_t reg = Pin::port::port->CRH;

            // set the new pinmode
            Pin::port::port->CRH = (
                (reg & ~(0b11 << ((Pin::number - 8) * 4))) | 
                (static_cast<uint32_t>(Mode) << ((Pin::number - 8) * 4))
            );
        }
    }

    /**
     * @brief Set the config of a pin (changes if input or output)
     * 
     * TODO: change this to a enum as input
     * 
     * @tparam Pin 
     * @tparam Config 
     */
    template <typename Pin, uint8_t Config>
    static void set_config() {
        // check if we need to write to the low or high register
        if constexpr (Pin::number < 8) {
            // get the current register
            const uint32_t reg = Pin::port::port->CRL;

            // set the new pinmode
            Pin::port::port->CRL = (
                (reg & ~(0b11 << ((Pin::number * 4) + 2))) | 
                (Config << ((Pin::number * 4) + 2))
            );
        }
        else {
            // get the current register
            const uint32_t reg = Pin::port::port->CRH;

            // set the new pinmode
            Pin::port::port->CRH = (
                (reg & ~(0b11 << (((Pin::number - 8) * 4) + 2))) | 
                (Config << (((Pin::number - 8) * 4) + 2))
            );
        }
    }
}

// global peripherals, not affected by chip package
namespace klib::stm32f103::io::periph {
    struct gpio0 {
        // peripheral id (e.g gpio0, gpio1)
        constexpr static uint32_t id = 0;

        // peripheral interrupt position
        // constexpr static uint32_t interrupt_id = 40;

        // port to the gpio hardware
        static inline GPIOA_Type *const port = GPIOA;
    };

    struct gpio1 {
        // peripheral id (e.g gpio0, gpio1)
        constexpr static uint32_t id = 1;

        // peripheral interrupt position
        // constexpr static uint32_t interrupt_id = 40;

        // port to the gpio hardware
        static inline GPIOA_Type *const port = GPIOB;
    };
    
    struct gpio2 {
        // peripheral id (e.g gpio0, gpio1)
        constexpr static uint32_t id = 2;

        // peripheral interrupt position
        // constexpr static uint32_t interrupt_id = 40;

        // port to the gpio hardware
        static inline GPIOA_Type *const port = GPIOC;
    };

    struct gpio3 {
        // peripheral id (e.g gpio0, gpio1)
        constexpr static uint32_t id = 3;

        // peripheral interrupt position
        // constexpr static uint32_t interrupt_id = 40;

        // port to the gpio hardware
        static inline GPIOA_Type *const port = GPIOD;
    };

    struct gpio4 {
        // peripheral id (e.g gpio0, gpio1)
        constexpr static uint32_t id = 4;

        // peripheral interrupt position
        // constexpr static uint32_t interrupt_id = 40;

        // port to the gpio hardware
        static inline GPIOA_Type *const port = GPIOE;
    };
}

namespace klib::stm32f103::io {
    template <typename Pin>
    class pin_in {
    public:
        constexpr static void init() {
            // TODO: move this to clocking file
            // enable clock for port
            RCC->APB2ENR |= 0x1 << (2 + Pin::port::id);

            // clear all the alternate functions
            detail::pins::set_peripheral<Pin, io::detail::alternate::none>();

            // set the pin as a input
            detail::pins::set_pinmode<Pin, detail::pins::direction_mode::input>();

            // set the config to floating input
            detail::pins::set_config<Pin, 0b01>();
        }

        constexpr static bool get() {
            // get the status of the pin
            return Pin::port::port->IDR & detail::pins::mask<Pin>;
        }

        template <bool Val>
        constexpr static void pullup_enable() {
            if constexpr (Val) {
                // enable the pullup
                Pin::port::port->ODR |= detail::pins::mask<Pin>;

                // change to input with pull-up/pull-down
                detail::pins::set_config<Pin, 0b10>();
            }
            else {
                // change back to floating
                detail::pins::set_config<Pin, 0b01>();
            }
        }

        template <bool Val>
        constexpr static void pulldown_enable() {
            if constexpr (Val) {
                // enable the pulldown
                Pin::port::port->ODR &= ~detail::pins::mask<Pin>;

                // change to input with pull-up/pull-down
                detail::pins::set_config<Pin, 0b10>();
            }
            else {
                // change back to floating
                detail::pins::set_config<Pin, 0b01>();
            }
        }    
    };

    template <typename Pin>
    class pin_out {
    public:
        constexpr static void init() {
            // TODO: move this to clocking file
            // enable clock for port
            RCC->APB2ENR |= 0x1 << (2 + Pin::port::id);

            // clear all the alternate functions
            detail::pins::set_peripheral<Pin, io::detail::alternate::none>();

            // enable the gpio output
            detail::pins::set_pinmode<Pin, detail::pins::direction_mode::output_10mhz>();

            // set the config to general purpose output push-pull
            detail::pins::set_config<Pin, 0b00>();
        }

        template <bool Val>
        constexpr static void set() {
            if constexpr (Val) {
                Pin::port::port->BSRR = detail::pins::mask<Pin>;
            }
            else {
                Pin::port::port->BRR = detail::pins::mask<Pin>;
            }
        }

        constexpr static void set(const bool val) {
            if (val) {
                Pin::port::port->BSRR = detail::pins::mask<Pin>;
            }
            else {
                Pin::port::port->BRR = detail::pins::mask<Pin>;
            }
        }
    };
}

#endif