#ifndef KLIB_CYPRESS_MB9BF560L_PORT_HPP
#define KLIB_CYPRESS_MB9BF560L_PORT_HPP

#include <cstdint>

#include <klib/klib.hpp>

namespace klib::core::mb9bf560l::io::detail::alternate {
    // alternate functions for all the gpio
    // default function (view reference manual for 
    // default functions for every pin)
    struct none {};

    // alternate function for analog pins
    struct analog {};
}

namespace klib::core::mb9bf560l::io::detail::pins {
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
        // check if we have a analog pin
        constexpr static bool has_analog = requires() {
            // if we have a analog number we have a analog pin
            (void)Pin::analog_number;
        };

        // make sure we are not setting a pin that does not have
        // analog to analog
        static_assert(
            !std::is_same_v<Periph, io::detail::alternate::analog> || 
            (std::is_same_v<Periph, io::detail::alternate::analog> && has_analog),
            "Pin does not support analog"
        );

        // always disable analog mode if we are not switching to analog mode
        if (!std::is_same_v<Periph, io::detail::alternate::analog>) {
            // disable the analog mode if it is available on the pin
            if constexpr (has_analog) {
                // disable the analog mode
                Pin::port::port->ADE &= (~detail::pins::mask<Pin>);
            }
        }

        // get the pointer to the pin select we need to write to
        volatile uint32_t *const pfr = (reinterpret_cast<volatile uint32_t *const>(Pin::port::port->PFR0));

        // set the alternate function
        if constexpr (std::is_same_v<Periph, io::detail::alternate::none>) {
            // enable control using the PIO. Disables peripheral 
            // control of the pin
            pfr[Pin::port::id] &= (~detail::pins::mask<Pin>);
        }
        else if constexpr (std::is_same_v<Periph, io::detail::alternate::analog>) {
            // set the pin to analog mode
            Pin::port::port->ADE |= detail::pins::mask<Pin>;
        }
    }
}

namespace klib::core::mb9bf560l::io {
    template <typename Pin>
    class pin_in {
    public:
        constexpr static void init() {
            // clear all the alternate functions
            detail::pins::set_peripheral<Pin, io::detail::alternate::none>();

            // get the pointer to the pin select we need to write to
            volatile uint32_t *const ddr = (reinterpret_cast<volatile uint32_t *const>(Pin::port::port->DDR0));

            // disable the output on the pin
            ddr[Pin::port::id] &= (~detail::pins::mask<Pin>);
        }

        constexpr static bool get() {
            // get the pointer to the pin select we need to write to
            volatile uint32_t *const pdir = (reinterpret_cast<volatile uint32_t *const>(Pin::port::port->PDIR0));

            // get the status of the pin
            return pdir[Pin::port::id] & detail::pins::mask<Pin>;
        }

        template <bool Val>
        constexpr static void pullup_enable() {
            // get the pointer to the pin select we need to write to
            volatile uint32_t *const pcr = (reinterpret_cast<volatile uint32_t *const>(Pin::port::port->PCR0));

            if constexpr (Val) {
                pcr[Pin::port::id] |= detail::pins::mask<Pin>;
            }
            else {
                pcr[Pin::port::id] &= (~detail::pins::mask<Pin>);
            }
        }    
    };
 
    template <typename Pin>
    class pin_out {
    public:
        constexpr static void init() {
            // clear all the alternate functions
            detail::pins::set_peripheral<Pin, io::detail::alternate::none>();

            // get the pointer to the pin select we need to write to
            volatile uint32_t *const ddr = (reinterpret_cast<volatile uint32_t *const>(Pin::port::port->DDR0));

            // // enable the gpio output
            ddr[Pin::port::id] |= detail::pins::mask<Pin>;
        }

        template <bool Val>
        constexpr static void set() {
            // get the pointer to the pin select we need to write to
            volatile uint32_t *const pdor = (reinterpret_cast<volatile uint32_t *const>(Pin::port::port->PDOR0));

            if constexpr (Val) {
                pdor[Pin::port::id] |= detail::pins::mask<Pin>;
            }
            else {
                pdor[Pin::port::id] &= (~detail::pins::mask<Pin>);
            }
        }

        constexpr static void set(const bool val) {
            // get the pointer to the pin select we need to write to
            volatile uint32_t *const pdor = (reinterpret_cast<volatile uint32_t *const>(Pin::port::port->PDOR0));

            if (val) {
                pdor[Pin::port::id] |= detail::pins::mask<Pin>;
            }
            else {
                pdor[Pin::port::id] &= (~detail::pins::mask<Pin>);
            }
        }
    };
}

#endif