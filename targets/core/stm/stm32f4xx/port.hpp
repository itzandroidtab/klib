#ifndef KLIB_STM_STM32F4XX_PORT_HPP
#define KLIB_STM_STM32F4XX_PORT_HPP

#include <cstdint>

#include <klib/klib.hpp>

namespace klib::core::stm32f4xx::io::detail::alternate {
    // alternate functions for all the gpio
    // default function (view reference manual for
    // default functions for every pin)
    struct none {};
    
    // alternate function 0
    struct func_0 {
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

    // alternate function 10
    struct func_10 {
        // peripheral id (e.g func0, func1)
        constexpr static uint32_t id = 10;
    };

    // alternate function 11
    struct func_11 {
        // peripheral id (e.g func0, func1)
        constexpr static uint32_t id = 11;
    };

    // alternate function 12
    struct func_12 {
        // peripheral id (e.g func0, func1)
        constexpr static uint32_t id = 12;
    };

    // alternate function 13
    struct func_13 {
        // peripheral id (e.g func0, func1)
        constexpr static uint32_t id = 13;
    };

    // alternate function 14
    struct func_14 {
        // peripheral id (e.g func0, func1)
        constexpr static uint32_t id = 14;
    };

    // alternate function 15
    struct func_15 {
        // peripheral id (e.g func0, func1)
        constexpr static uint32_t id = 15;
    };
}

namespace klib::core::stm32f4xx::io::detail::pins {
    // get the pin mask of a pin number
    template <typename Pin>
    constexpr uint32_t mask = 1U << Pin::number;

    /**
     * @brief Direction mode
     * 
     */
    enum class direction_mode {
        input = 0b00,
        output = 0b01,
        alternate_function = 0b10,
        analog = 0b11
    };

    /**
     * @brief Speed modes
     * 
     */
    enum class speed_mode {
        low = 0b00,
        medium = 0b01,
        high = 0b10,
        very_high = 0b11
    };

    /**
     * @brief Set the direction mode of a pin
     * 
     * @tparam Pin 
     * @tparam Mode 
     */
    template <typename Pin, direction_mode Mode>
    static void set_direction_mode() {
        static_assert(Pin::number < 16, "Invalid pin number given to set_direction_mode");

        // get the current register
        const uint32_t reg = Pin::port::port->MODER;

        // set the new direction mode
        Pin::port::port->MODER = (
            (reg & ~(0b11 << (Pin::number * 2))) | 
            (static_cast<uint32_t>(Mode) << (Pin::number * 2))
        );
    }

    /**
     * @brief Set the open drain mode of a pin
     * 
     * @tparam Pin 
     * @tparam OpenDrain 
     */
    template <typename Pin, bool OpenDrain>
    static void set_open_drain() {
        static_assert(Pin::number < 16, "Invalid pin number given to set_open_drain");

        // set or clear based on the value
        if constexpr (OpenDrain) {
            Pin::port::port->OTYPER |= mask<Pin>;
        }
        else {
            Pin::port::port->OTYPER &= ~(mask<Pin>);
        }
    }

    /**
     * @brief Set the speed object
     * 
     * @tparam Pin 
     * @tparam Speed 
     */
    template <typename Pin, speed_mode Speed>
    static void set_speed() {
        static_assert(Pin::number < 16, "Invalid pin number given to set_speed");
        
        // get the current register
        const uint32_t reg = Pin::port::port->OSPEEDR;

        // set the new speed
        Pin::port::port->OSPEEDR = (
            (reg & ~(0b11 << (Pin::number * 2))) |
            (static_cast<uint8_t>(Speed) << (Pin::number * 2))
        );
    }

    /**
     * @brief Pull-up/pull-down mode
     * 
     */
    enum class pull_mode {
        none = 0b00,
        pullup = 0b01,
        pulldown = 0b10
    };

    /**
     * @brief Set the pull-up/pull-down mode of a pin
     * 
     * @tparam Pin 
     * @tparam Mode 
     */
    template <typename Pin, pull_mode Mode>
    static void set_pull_mode() {
        static_assert(Pin::number < 16, "Invalid pin number given to set_pull_mode");

        // get the current register
        const uint32_t reg = Pin::port::port->PUPDR;

        // set the new pull mode
        Pin::port::port->PUPDR = (
            (reg & ~(0b11 << (Pin::number * 2))) |
            (static_cast<uint32_t>(Mode) << (Pin::number * 2))
        );
    }

    /**
     * @brief Helper function to set a pin to a specific peripheral
     * 
     * @tparam Pin 
     * @tparam Periph 
     */
    template <typename Pin, typename Periph>
    static void set_peripheral() {
        static_assert(Pin::number < 16, "Invalid pin number given to set_peripheral");

        // set the pin mode based on the alternate function
        if constexpr (std::is_same_v<Periph, detail::alternate::none>) {
            // change to input mode when none is selected
            set_direction_mode<Pin, direction_mode::input>();
        }
        else {
            // set to alternate function mode
            set_direction_mode<Pin, direction_mode::alternate_function>();
        }

        // determine which alternate function register to use
        if constexpr (Pin::number < 8) {
            // get the current register value
            const uint32_t reg = Pin::port::port->AFRL;

            // set the alternate function
            Pin::port::port->AFRL = (
                (reg & ~(0xF << (Pin::number * 4))) |
                (Periph::id << (Pin::number * 4))
            );
        }
        else {
            // get the current register value
            const uint32_t reg = Pin::port::port->AFRH;

            // set the alternate function
            Pin::port::port->AFRH = (
                (reg & ~(0xF << ((Pin::number - 8) * 4))) |
                (Periph::id << ((Pin::number - 8) * 4))
            );
        }
    }
}

namespace klib::core::stm32f4xx::io {
    template <typename Pin>
    class pin_in {
    public:
        /**
         * @brief Initialize the pin as an input
         * 
         */
        constexpr static void init() {
            // set the pin to input mode
            detail::pins::set_direction_mode<Pin, detail::pins::direction_mode::input>();
        }

        /**
         * @brief Get the value of the pin
         * 
         * @return true 
         * @return false 
         */
        constexpr static bool get() {
            // read from the input data register
            return Pin::port::port->IDR & detail::pins::mask<Pin>;
        }

        /**
         * @brief Enable/disable pull-up resistor
         * 
         * @tparam Val 
         */
        template <bool Val>
        constexpr static void pullup_enable() {
            detail::pins::set_pull_mode<Pin,
                Val ? detail::pins::pull_mode::pullup : detail::pins::pull_mode::none
            >();
        }

        /**
         * @brief Enable/disable pull-down resistor
         * 
         * @tparam Val 
         */
        template <bool Val>
        constexpr static void pulldown_enable() {
            detail::pins::set_pull_mode<Pin,
                Val ? detail::pins::pull_mode::pulldown : detail::pins::pull_mode::none
            >();
        }
    };

    template <typename Pin>
    class pin_out {
    public:
        /**
         * @brief Initialize the pin as an output
         * 
         */
        constexpr static void init() {
            // set the pin to output mode
            detail::pins::set_direction_mode<Pin, detail::pins::direction_mode::output>();
        }

        /**
         * @brief Set the pin to a specific value
         * 
         * @tparam Val 
         */
        template <bool Val>
        constexpr static void set() {
            if constexpr (Val) {
                // set the pin high
                Pin::port::port->BSRR = detail::pins::mask<Pin>;
            }
            else {
                // set the pin low
                Pin::port::port->BSRR = ((detail::pins::mask<Pin>) << 16);
            }
        }

        /**
         * @brief Set the pin to a specific value
         * 
         * @param val 
         */
        constexpr static void set(const bool val) {
            if (val) {
                // set the pin high
                Pin::port::port->BSRR = detail::pins::mask<Pin>;
            }
            else {
                // set the pin low
                Pin::port::port->BSRR = ((detail::pins::mask<Pin>) << 16);
            }
        }
    };
}

#endif