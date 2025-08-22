#ifndef KLIB_CYPRESS_MB9BF560L_PORT_HPP
#define KLIB_CYPRESS_MB9BF560L_PORT_HPP

#include <cstdint>

#include <klib/klib.hpp>
#include <klib/math.hpp>

namespace klib::core::mb9bf560l::io::detail::pins {
    // special case for the special port settings register
    constexpr static uint8_t spsr_id = 255;

    /**
     * @brief Helper struct for the peripheral helper
     * 
     * @tparam Id 
     * @tparam Offset 
     * @tparam Bits 
     */
    template <uint8_t Id, uint32_t Offset, uint8_t Bits>
    struct peripheral_helper_impl {
        // the target peripheral
        constexpr static uint8_t id = Id;

        // offset into the register
        constexpr static uint32_t offset = Offset;

        // amount of bits in the register
        constexpr static uint32_t bits = Bits;
    };

    /**
     * @brief Helper struct to define a peripheral alternate function
     * 
     * @tparam Periph 
     * @tparam Value 
     */
    template <typename Periph, uint32_t Value>
    struct peripheral_helper {
        // using to get the register
        using periph = Periph;

        // value needed to switch the pin to the correct
        // alternate function
        constexpr static uint32_t value = Value;
    };

    /**
     * @brief Helper function to clear a specific alternate function. Note this 
     * does not check for any valid inputs as it is only run on runtime
     * 
     * @param id 
     * @param offset 
     * @param bits 
     * @param value 
     */
    static void peripheral_helper_clear_alternate(volatile uint32_t *const ptr, const uint32_t offset, const uint32_t bits, const uint32_t value) {
        // get the current value in the register
        uint32_t reg = (*ptr);

        // get the value from the register
        uint32_t v = (reg >> offset) & (klib::exp2(bits) - 1);

        // check if the value matches the one from the alternate function
        if (v == value) {
            // we have a match, set the alternate function to the default (0)
            reg &= ~((klib::exp2(bits) - 1) << offset);
    
            // write the data back into the register
            (*ptr) = reg;
        }
    }

    /**
     * @brief Helper function to clear all alternate functions of a pin
     * 
     * @note expects a pin with alternate functions
     * 
     * @tparam Index 
     * @tparam Pin
     */
    template <uint32_t Index, typename Pin>
    static void peripheral_helper_clear_alternate_functions() {
        // go through all items in the alternate functions tuple
        using alternate = std::tuple_element_t<Index, typename Pin::alternate>;

        // make sure the input id is valid
        static_assert(alternate::periph::id <= 20 || alternate::periph::id == spsr_id, "Invalid peripheral id");

        // check what peripheral id we need to clear
        if constexpr (alternate::periph::id == spsr_id) {
            // clear the alternate function if it is set
            peripheral_helper_clear_alternate(
                &Pin::port::port->SPRS, alternate::periph::offset, alternate::periph::bits, alternate::value
            );
        }
        else {
            // get the pointer to the pin select we need to write to
            volatile uint32_t *const epfr = &Pin::port::port->EPFR00;

            // clear the alternate function if it is set
            peripheral_helper_clear_alternate(
                &epfr[alternate::periph::id], alternate::periph::offset, alternate::periph::bits, alternate::value
            );
        }

        // check the next index
        if constexpr ((Index + 1) < std::tuple_size<typename Pin::alternate>::value) {
            // call the function again with the next pin
            peripheral_helper_clear_alternate_functions<Index + 1, Pin>();
        }
    }

    /**
     * @brief Helper function to write set a alterernate function. Note this 
     * does not check for any valid inputs as it is only run on runtime
     * 
     * @param ptr 
     * @param offset 
     * @param bits 
     * @param value 
     */
    static void peripheral_helper_set_alternate(volatile uint32_t *const ptr, const uint32_t offset, const uint32_t bits, const uint32_t value) {
        // get the current value in the register
        uint32_t reg = (*ptr);

        // get the value from the register
        uint32_t v = (reg >> offset) & (klib::exp2(bits) - 1);

        // we have a match, set the alternate function to the default (0)
        reg &= ~((klib::exp2(bits) - 1) << offset);

        // write the value where we just cleared the data
        reg |= value << offset;

        // write the data back into the register
        (*ptr) = reg;
    }

    /**
     * @brief Helper function to write set a alterernate function
     *
     * @tparam Pin
     * @tparam Periph
     */
    template <typename Pin, typename Periph>
    static void peripheral_helper_setup() {
        // make sure the input id is valid
        static_assert(Periph::periph::id <= 20 || Periph::periph::id == spsr_id, "Invalid peripheral id");

        // clear any other alternate functions for this pin before we set the 
        // new one. This is only needed when the user application switches 
        // the alternate function of a pin more than once. We do it here just
        // in case to make sure we do not have any issues with other peripherals
        peripheral_helper_clear_alternate_functions<0, Pin>();

        // check what peripheral id we need to set
        if constexpr (Periph::periph::id == spsr_id) {
            // set the value from the peripheral into the register
            peripheral_helper_set_alternate(
                &Pin::port::port->SPRS, Periph::periph::offset, Periph::periph::bits, Periph::value
            );
        }
        else {
            // get the pointer to the pin select we need to write to
            volatile uint32_t *const epfr = &Pin::port::port->EPFR00;

            // set the value from the peripheral into the register
            peripheral_helper_set_alternate(
                &epfr[Periph::periph::id], Periph::periph::offset, Periph::periph::bits, Periph::value
            );
        }
    }
}

namespace klib::core::mb9bf560l::io::detail::alternate {
    // alternate functions for all the gpio
    // default function (view reference manual for
    // default functions for every pin)
    struct none {};

    // alternate function for analog pins
    struct analog {};

    // For some reason this chip does not have the
    // alternate functions tied to the port/pin.
    // (probably due to only having 1 gpio port). This
    // is quite annoying as we need to keep track what
    // pin goes where. We also need to clear any other
    // alternate functions the pin can be when changing
    // to another alternate function as this is not
    // cleared automaticly.

    // The one that decided to make the gpio peripheral 
    // this way should reconsider his job of making IP 
    // blocks. This is extremly anoying as we need to 
    // clear all the other functions this pin can have.
    namespace special {
        // register SPRS (note we use ID 255 for this register)
        // alternate function for usb1c
        template <uint8_t Value>
        struct usb1c: public detail::pins::peripheral_helper<detail::pins::peripheral_helper_impl<detail::pins::spsr_id, 5, 1>, Value> {};

        // alternate function for usb0c
        template <uint8_t Value>
        struct usb0c: public detail::pins::peripheral_helper<detail::pins::peripheral_helper_impl<detail::pins::spsr_id, 4, 1>, Value> {};

        // alternate function for mainxc
        template <uint8_t Value>
        struct mainxc: public detail::pins::peripheral_helper<detail::pins::peripheral_helper_impl<detail::pins::spsr_id, 2, 2>, Value> {};

        // alternate function for subxc
        template <uint8_t Value>
        struct subxc: public detail::pins::peripheral_helper<detail::pins::peripheral_helper_impl<detail::pins::spsr_id, 0, 2>, Value> {};
    }

    namespace system_function {
        // register EPFR00
        // alternate function for trc3e
        template <bool Value>
        struct trc3e: public detail::pins::peripheral_helper<detail::pins::peripheral_helper_impl<0, 27, 1>, Value> {};

        // alternate function for trc2e
        template <bool Value>
        struct trc2e: public detail::pins::peripheral_helper<detail::pins::peripheral_helper_impl<0, 26, 1>, Value> {};

        // alternate function for trc1e
        template <bool Value>
        struct trc1e: public detail::pins::peripheral_helper<detail::pins::peripheral_helper_impl<0, 25, 1>, Value> {};

        // alternate function for trc0e
        template <bool Value>
        struct trc0e: public detail::pins::peripheral_helper<detail::pins::peripheral_helper_impl<0, 24, 1>, Value> {};

        // alternate function for jtagen1s
        template <bool Value>
        struct jtagen1s: public detail::pins::peripheral_helper<detail::pins::peripheral_helper_impl<0, 17, 1>, Value> {};

        // alternate function for jtagen0b
        template <bool Value>
        struct jtagen0b: public detail::pins::peripheral_helper<detail::pins::peripheral_helper_impl<0, 16, 1>, Value> {};

        // alternate function for usbp1e
        template <bool Value>
        struct usbp1e: public detail::pins::peripheral_helper<detail::pins::peripheral_helper_impl<0, 13, 1>, Value> {};

        // alternate function for usbp0e
        template <bool Value>
        struct usbp0e: public detail::pins::peripheral_helper<detail::pins::peripheral_helper_impl<0, 9, 1>, Value> {};

        // alternate function for suboute
        template <uint8_t Value>
        struct suboute: public detail::pins::peripheral_helper<detail::pins::peripheral_helper_impl<0, 6, 2>, Value> {};

        // alternate function for rtccoe
        template <uint8_t Value>
        struct rtccoe: public detail::pins::peripheral_helper<detail::pins::peripheral_helper_impl<0, 4, 2>, Value> {};

        // alternate function for croute
        template <uint8_t Value>
        struct croute: public detail::pins::peripheral_helper<detail::pins::peripheral_helper_impl<0, 1, 2>, Value> {};

        // alternate function for nmis
        template <bool Value>
        struct nmis: public detail::pins::peripheral_helper<detail::pins::peripheral_helper_impl<0, 0, 1>, Value> {};
    }

    namespace multi_function_timer {
        // register EPFR01
        // alternate function for ic03s
        template <uint8_t Value>
        struct ic03s: public detail::pins::peripheral_helper<detail::pins::peripheral_helper_impl<1, 29, 3>, Value> {};

        // alternate function for ic02s
        template <uint8_t Value>
        struct ic02s: public detail::pins::peripheral_helper<detail::pins::peripheral_helper_impl<1, 26, 3>, Value> {};

        // alternate function for ic01s
        template <uint8_t Value>
        struct ic01s: public detail::pins::peripheral_helper<detail::pins::peripheral_helper_impl<1, 23, 3>, Value> {};

        // alternate function for ic00s
        template <uint8_t Value>
        struct ic00s: public detail::pins::peripheral_helper<detail::pins::peripheral_helper_impl<1, 20, 3>, Value> {};

        // alternate function for frck0s
        template <uint8_t Value>
        struct frck0s: public detail::pins::peripheral_helper<detail::pins::peripheral_helper_impl<1, 18, 2>, Value> {};

        // alternate function for dtti0s
        template <uint8_t Value>
        struct dtti0s: public detail::pins::peripheral_helper<detail::pins::peripheral_helper_impl<1, 16, 2>, Value> {};

        // alternate function for dtti0c
        template <bool Value>
        struct dtti0c: public detail::pins::peripheral_helper<detail::pins::peripheral_helper_impl<1, 12, 1>, Value> {};

        // alternate function for rto05e
        template <uint8_t Value>
        struct rto05e: public detail::pins::peripheral_helper<detail::pins::peripheral_helper_impl<1, 10, 2>, Value> {};

        // alternate function for rto04e
        template <uint8_t Value>
        struct rto04e: public detail::pins::peripheral_helper<detail::pins::peripheral_helper_impl<1, 8, 2>, Value> {};

        // alternate function for rto03e
        template <uint8_t Value>
        struct rto03e: public detail::pins::peripheral_helper<detail::pins::peripheral_helper_impl<1, 6, 2>, Value> {};

        // alternate function for rto02e
        template <uint8_t Value>
        struct rto02e: public detail::pins::peripheral_helper<detail::pins::peripheral_helper_impl<1, 4, 2>, Value> {};

        // alternate function for rto01e
        template <uint8_t Value>
        struct rto01e: public detail::pins::peripheral_helper<detail::pins::peripheral_helper_impl<1, 2, 2>, Value> {};

        // alternate function for rto00e
        template <uint8_t Value>
        struct rto00e: public detail::pins::peripheral_helper<detail::pins::peripheral_helper_impl<1, 0, 2>, Value> {};


        // register EPFR02
        // alternate function for i2c13s
        template <uint8_t Value>
        struct i2c13s: public detail::pins::peripheral_helper<detail::pins::peripheral_helper_impl<2, 29, 3>, Value> {};

        // alternate function for i2c12s
        template <uint8_t Value>
        struct i2c12s: public detail::pins::peripheral_helper<detail::pins::peripheral_helper_impl<2, 26, 3>, Value> {};

        // alternate function for i2c11s
        template <uint8_t Value>
        struct i2c11s: public detail::pins::peripheral_helper<detail::pins::peripheral_helper_impl<2, 23, 3>, Value> {};

        // alternate function for i2c10s
        template <uint8_t Value>
        struct i2c10s: public detail::pins::peripheral_helper<detail::pins::peripheral_helper_impl<2, 20, 3>, Value> {};

        // alternate function for frck1s
        template <uint8_t Value>
        struct frck1s: public detail::pins::peripheral_helper<detail::pins::peripheral_helper_impl<2, 18, 2>, Value> {};

        // alternate function for dtti1s
        template <uint8_t Value>
        struct dtti1s: public detail::pins::peripheral_helper<detail::pins::peripheral_helper_impl<2, 16, 2>, Value> {};

        // alternate function for dtti1c
        template <bool Value>
        struct dtti1c: public detail::pins::peripheral_helper<detail::pins::peripheral_helper_impl<2, 12, 1>, Value> {};

        // alternate function for rto15e
        template <uint8_t Value>
        struct rto15e: public detail::pins::peripheral_helper<detail::pins::peripheral_helper_impl<2, 10, 2>, Value> {};

        // alternate function for rto14e
        template <uint8_t Value>
        struct rto14e: public detail::pins::peripheral_helper<detail::pins::peripheral_helper_impl<2, 8, 2>, Value> {};

        // alternate function for rto13e
        template <uint8_t Value>
        struct rto13e: public detail::pins::peripheral_helper<detail::pins::peripheral_helper_impl<2, 6, 2>, Value> {};

        // alternate function for rto12e
        template <uint8_t Value>
        struct rto12e: public detail::pins::peripheral_helper<detail::pins::peripheral_helper_impl<2, 4, 2>, Value> {};

        // alternate function for rto11e
        template <uint8_t Value>
        struct rto11e: public detail::pins::peripheral_helper<detail::pins::peripheral_helper_impl<2, 2, 2>, Value> {};

        // alternate function for rto10e
        template <uint8_t Value>
        struct rto10e: public detail::pins::peripheral_helper<detail::pins::peripheral_helper_impl<2, 0, 2>, Value> {};


        // register EPFR03
        // alternate function for i2c33s
        template <uint8_t Value>
        struct i2c33s: public detail::pins::peripheral_helper<detail::pins::peripheral_helper_impl<3, 29, 3>, Value> {};

        // alternate function for i2c32s
        template <uint8_t Value>
        struct i2c32s: public detail::pins::peripheral_helper<detail::pins::peripheral_helper_impl<3, 26, 3>, Value> {};

        // alternate function for i2c31s
        template <uint8_t Value>
        struct i2c31s: public detail::pins::peripheral_helper<detail::pins::peripheral_helper_impl<3, 23, 3>, Value> {};

        // alternate function for i2c20s
        template <uint8_t Value>
        struct i2c20s: public detail::pins::peripheral_helper<detail::pins::peripheral_helper_impl<3, 20, 3>, Value> {};

        // alternate function for frck2s
        template <uint8_t Value>
        struct frck2s: public detail::pins::peripheral_helper<detail::pins::peripheral_helper_impl<3, 18, 2>, Value> {};

        // alternate function for dtti2s
        template <uint8_t Value>
        struct dtti2s: public detail::pins::peripheral_helper<detail::pins::peripheral_helper_impl<3, 16, 2>, Value> {};

        // alternate function for dtti2c
        template <bool Value>
        struct dtti2c: public detail::pins::peripheral_helper<detail::pins::peripheral_helper_impl<3, 12, 1>, Value> {};

        // alternate function for rto25e
        template <uint8_t Value>
        struct rto25e: public detail::pins::peripheral_helper<detail::pins::peripheral_helper_impl<3, 10, 2>, Value> {};

        // alternate function for rto24e
        template <uint8_t Value>
        struct rto24e: public detail::pins::peripheral_helper<detail::pins::peripheral_helper_impl<3, 8, 2>, Value> {};

        // alternate function for rto23e
        template <uint8_t Value>
        struct rto23e: public detail::pins::peripheral_helper<detail::pins::peripheral_helper_impl<3, 6, 2>, Value> {};

        // alternate function for rto22e
        template <uint8_t Value>
        struct rto22e: public detail::pins::peripheral_helper<detail::pins::peripheral_helper_impl<3, 4, 2>, Value> {};

        // alternate function for rto21e
        template <uint8_t Value>
        struct rto21e: public detail::pins::peripheral_helper<detail::pins::peripheral_helper_impl<3, 2, 2>, Value> {};

        // alternate function for rto20e
        template <uint8_t Value>
        struct rto20e: public detail::pins::peripheral_helper<detail::pins::peripheral_helper_impl<3, 0, 2>, Value> {};
    }

    namespace base_timer {
        // register EPFR04
        // alternate function for tiob3s
        template <uint8_t Value>
        struct tiob3s: public detail::pins::peripheral_helper<detail::pins::peripheral_helper_impl<4, 28, 2>, Value> {};

        // alternate function for tioa3e
        template <uint8_t Value>
        struct tioa3e: public detail::pins::peripheral_helper<detail::pins::peripheral_helper_impl<4, 26, 2>, Value> {};

        // alternate function for tioa3s
        template <uint8_t Value>
        struct tioa3s: public detail::pins::peripheral_helper<detail::pins::peripheral_helper_impl<4, 24, 2>, Value> {};

        // alternate function for tiob2s
        template <uint8_t Value>
        struct tiob2s: public detail::pins::peripheral_helper<detail::pins::peripheral_helper_impl<4, 20, 2>, Value> {};

        // alternate function for tioa2e
        template <uint8_t Value>
        struct tioa2e: public detail::pins::peripheral_helper<detail::pins::peripheral_helper_impl<4, 18, 2>, Value> {};

        // alternate function for tiob1s
        template <uint8_t Value>
        struct tiob1s: public detail::pins::peripheral_helper<detail::pins::peripheral_helper_impl<4, 12, 2>, Value> {};

        // alternate function for tioa1e
        template <uint8_t Value>
        struct tioa1e: public detail::pins::peripheral_helper<detail::pins::peripheral_helper_impl<4, 10, 2>, Value> {};

        // alternate function for tioa1s
        template <uint8_t Value>
        struct tioa1s: public detail::pins::peripheral_helper<detail::pins::peripheral_helper_impl<4, 8, 2>, Value> {};

        // alternate function for tiob0s
        template <uint8_t Value>
        struct tiob0s: public detail::pins::peripheral_helper<detail::pins::peripheral_helper_impl<4, 4, 3>, Value> {};

        // alternate function for tioa0e
        template <uint8_t Value>
        struct tioa0e: public detail::pins::peripheral_helper<detail::pins::peripheral_helper_impl<4, 2, 2>, Value> {};


        // register EPFR05
        // alternate function for tiob7s
        template <uint8_t Value>
        struct tiob7s: public detail::pins::peripheral_helper<detail::pins::peripheral_helper_impl<5, 28, 2>, Value> {};

        // alternate function for tioa7e
        template <uint8_t Value>
        struct tioa7e: public detail::pins::peripheral_helper<detail::pins::peripheral_helper_impl<5, 26, 2>, Value> {};

        // alternate function for tioa7s
        template <uint8_t Value>
        struct tioa7s: public detail::pins::peripheral_helper<detail::pins::peripheral_helper_impl<5, 24, 2>, Value> {};

        // alternate function for tiob6s
        template <uint8_t Value>
        struct tiob6s: public detail::pins::peripheral_helper<detail::pins::peripheral_helper_impl<5, 20, 2>, Value> {};

        // alternate function for tioa6e
        template <uint8_t Value>
        struct tioa6e: public detail::pins::peripheral_helper<detail::pins::peripheral_helper_impl<5, 18, 2>, Value> {};

        // alternate function for tiob5s
        template <uint8_t Value>
        struct tiob5s: public detail::pins::peripheral_helper<detail::pins::peripheral_helper_impl<5, 12, 2>, Value> {};

        // alternate function for tioa5e
        template <uint8_t Value>
        struct tioa5e: public detail::pins::peripheral_helper<detail::pins::peripheral_helper_impl<5, 10, 2>, Value> {};

        // alternate function for tioa5s
        template <uint8_t Value>
        struct tioa5s: public detail::pins::peripheral_helper<detail::pins::peripheral_helper_impl<5, 8, 2>, Value> {};

        // alternate function for tiob4s
        template <uint8_t Value>
        struct tiob4s: public detail::pins::peripheral_helper<detail::pins::peripheral_helper_impl<5, 4, 2>, Value> {};

        // alternate function for tioa4e
        template <uint8_t Value>
        struct tioa4e: public detail::pins::peripheral_helper<detail::pins::peripheral_helper_impl<5, 2, 2>, Value> {};
    }

    namespace external_interrupt {
        // register EPFR06
        // alternate function for eint15s
        template <uint8_t Value>
        struct eint15s: public detail::pins::peripheral_helper<detail::pins::peripheral_helper_impl<6, 30, 2>, Value> {};

        // alternate function for eint14s
        template <uint8_t Value>
        struct eint14s: public detail::pins::peripheral_helper<detail::pins::peripheral_helper_impl<6, 28, 2>, Value> {};

        // alternate function for eint13s
        template <uint8_t Value>
        struct eint13s: public detail::pins::peripheral_helper<detail::pins::peripheral_helper_impl<6, 26, 2>, Value> {};

        // alternate function for eint12s
        template <uint8_t Value>
        struct eint12s: public detail::pins::peripheral_helper<detail::pins::peripheral_helper_impl<6, 24, 2>, Value> {};

        // alternate function for eint11s
        template <uint8_t Value>
        struct eint11s: public detail::pins::peripheral_helper<detail::pins::peripheral_helper_impl<6, 22, 2>, Value> {};

        // alternate function for eint10s
        template <uint8_t Value>
        struct eint10s: public detail::pins::peripheral_helper<detail::pins::peripheral_helper_impl<6, 20, 2>, Value> {};

        // alternate function for eint09s
        template <uint8_t Value>
        struct eint09s: public detail::pins::peripheral_helper<detail::pins::peripheral_helper_impl<6, 18, 2>, Value> {};

        // alternate function for eint08s
        template <uint8_t Value>
        struct eint08s: public detail::pins::peripheral_helper<detail::pins::peripheral_helper_impl<6, 16, 2>, Value> {};

        // alternate function for eint07s
        template <uint8_t Value>
        struct eint07s: public detail::pins::peripheral_helper<detail::pins::peripheral_helper_impl<6, 14, 2>, Value> {};

        // alternate function for eint06s
        template <uint8_t Value>
        struct eint06s: public detail::pins::peripheral_helper<detail::pins::peripheral_helper_impl<6, 12, 2>, Value> {};

        // alternate function for eint05s
        template <uint8_t Value>
        struct eint05s: public detail::pins::peripheral_helper<detail::pins::peripheral_helper_impl<6, 10, 2>, Value> {};

        // alternate function for eint04s
        template <uint8_t Value>
        struct eint04s: public detail::pins::peripheral_helper<detail::pins::peripheral_helper_impl<6, 8, 2>, Value> {};

        // alternate function for eint03s
        template <uint8_t Value>
        struct eint03s: public detail::pins::peripheral_helper<detail::pins::peripheral_helper_impl<6, 6, 2>, Value> {};

        // alternate function for eint02s
        template <uint8_t Value>
        struct eint02s: public detail::pins::peripheral_helper<detail::pins::peripheral_helper_impl<6, 4, 2>, Value> {};

        // alternate function for eint01s
        template <uint8_t Value>
        struct eint01s: public detail::pins::peripheral_helper<detail::pins::peripheral_helper_impl<6, 2, 2>, Value> {};

        // alternate function for eint00s
        template <uint8_t Value>
        struct eint00s: public detail::pins::peripheral_helper<detail::pins::peripheral_helper_impl<6, 0, 2>, Value> {};
    }

    namespace multi_functional_serial {
        // register EPFR07
        // alternate function for sck3b
        template <uint8_t Value>
        struct sck3b: public detail::pins::peripheral_helper<detail::pins::peripheral_helper_impl<7, 26, 2>, Value> {};

        // alternate function for sot3b
        template <uint8_t Value>
        struct sot3b: public detail::pins::peripheral_helper<detail::pins::peripheral_helper_impl<7, 24, 2>, Value> {};

        // alternate function for sin3s
        template <uint8_t Value>
        struct sin3s: public detail::pins::peripheral_helper<detail::pins::peripheral_helper_impl<7, 22, 2>, Value> {};

        // alternate function for sck2b
        template <uint8_t Value>
        struct sck2b: public detail::pins::peripheral_helper<detail::pins::peripheral_helper_impl<7, 20, 2>, Value> {};

        // alternate function for sot2b
        template <uint8_t Value>
        struct sot2b: public detail::pins::peripheral_helper<detail::pins::peripheral_helper_impl<7, 18, 2>, Value> {};

        // alternate function for sin2s
        template <uint8_t Value>
        struct sin2s: public detail::pins::peripheral_helper<detail::pins::peripheral_helper_impl<7, 16, 2>, Value> {};

        // alternate function for sck1b
        template <uint8_t Value>
        struct sck1b: public detail::pins::peripheral_helper<detail::pins::peripheral_helper_impl<7, 14, 2>, Value> {};

        // alternate function for sot1b
        template <uint8_t Value>
        struct sot1b: public detail::pins::peripheral_helper<detail::pins::peripheral_helper_impl<7, 12, 2>, Value> {};

        // alternate function for sin1s
        template <uint8_t Value>
        struct sin1s: public detail::pins::peripheral_helper<detail::pins::peripheral_helper_impl<7, 10, 2>, Value> {};

        // alternate function for sck0b
        template <uint8_t Value>
        struct sck0b: public detail::pins::peripheral_helper<detail::pins::peripheral_helper_impl<7, 8, 2>, Value> {};

        // alternate function for sot0b
        template <uint8_t Value>
        struct sot0b: public detail::pins::peripheral_helper<detail::pins::peripheral_helper_impl<7, 6, 2>, Value> {};

        // alternate function for sin0s
        template <uint8_t Value>
        struct sin0s: public detail::pins::peripheral_helper<detail::pins::peripheral_helper_impl<7, 4, 2>, Value> {};


        // register EPFR08
        // alternate function for cts5s
        template <uint8_t Value>
        struct cts5s: public detail::pins::peripheral_helper<detail::pins::peripheral_helper_impl<8, 30, 2>, Value> {};

        // alternate function for rts5e
        template <uint8_t Value>
        struct rts5e: public detail::pins::peripheral_helper<detail::pins::peripheral_helper_impl<8, 28, 2>, Value> {};

        // alternate function for sck7b
        template <uint8_t Value>
        struct sck7b: public detail::pins::peripheral_helper<detail::pins::peripheral_helper_impl<8, 26, 2>, Value> {};

        // alternate function for sot7b
        template <uint8_t Value>
        struct sot7b: public detail::pins::peripheral_helper<detail::pins::peripheral_helper_impl<8, 24, 2>, Value> {};

        // alternate function for sin7s
        template <uint8_t Value>
        struct sin7s: public detail::pins::peripheral_helper<detail::pins::peripheral_helper_impl<8, 22, 2>, Value> {};

        // alternate function for sck6b
        template <uint8_t Value>
        struct sck6b: public detail::pins::peripheral_helper<detail::pins::peripheral_helper_impl<8, 20, 2>, Value> {};

        // alternate function for sot6b
        template <uint8_t Value>
        struct sot6b: public detail::pins::peripheral_helper<detail::pins::peripheral_helper_impl<8, 18, 2>, Value> {};

        // alternate function for sin6s
        template <uint8_t Value>
        struct sin6s: public detail::pins::peripheral_helper<detail::pins::peripheral_helper_impl<8, 16, 2>, Value> {};

        // alternate function for sck5b
        template <uint8_t Value>
        struct sck5b: public detail::pins::peripheral_helper<detail::pins::peripheral_helper_impl<8, 14, 2>, Value> {};

        // alternate function for sot5b
        template <uint8_t Value>
        struct sot5b: public detail::pins::peripheral_helper<detail::pins::peripheral_helper_impl<8, 12, 2>, Value> {};

        // alternate function for sin5s
        template <uint8_t Value>
        struct sin5s: public detail::pins::peripheral_helper<detail::pins::peripheral_helper_impl<8, 10, 2>, Value> {};

        // alternate function for sck4b
        template <uint8_t Value>
        struct sck4b: public detail::pins::peripheral_helper<detail::pins::peripheral_helper_impl<8, 8, 2>, Value> {};

        // alternate function for sot4b
        template <uint8_t Value>
        struct sot4b: public detail::pins::peripheral_helper<detail::pins::peripheral_helper_impl<8, 6, 2>, Value> {};

        // alternate function for sin4s
        template <uint8_t Value>
        struct sin4s: public detail::pins::peripheral_helper<detail::pins::peripheral_helper_impl<8, 4, 2>, Value> {};

        // alternate function for cts4s
        template <uint8_t Value>
        struct cts4s: public detail::pins::peripheral_helper<detail::pins::peripheral_helper_impl<8, 2, 2>, Value> {};

        // alternate function for rts4e
        template <uint8_t Value>
        struct rts4e: public detail::pins::peripheral_helper<detail::pins::peripheral_helper_impl<8, 0, 2>, Value> {};
    }

    namespace can_adc_trigger_qprc {
        // register EPFR09
        // alternate function for ctx1e
        template <uint8_t Value>
        struct ctx1e: public detail::pins::peripheral_helper<detail::pins::peripheral_helper_impl<9, 30, 2>, Value> {};

        // alternate function for crx1s
        template <uint8_t Value>
        struct crx1s: public detail::pins::peripheral_helper<detail::pins::peripheral_helper_impl<9, 28, 2>, Value> {};

        // alternate function for ctx0e
        template <uint8_t Value>
        struct ctx0e: public detail::pins::peripheral_helper<detail::pins::peripheral_helper_impl<9, 26, 2>, Value> {};

        // alternate function for crx0s
        template <uint8_t Value>
        struct crx0s: public detail::pins::peripheral_helper<detail::pins::peripheral_helper_impl<9, 24, 2>, Value> {};

        // alternate function for adtrg2s
        template <uint8_t Value>
        struct adtrg2s: public detail::pins::peripheral_helper<detail::pins::peripheral_helper_impl<9, 20, 4>, Value> {};

        // alternate function for adtrg1s
        template <uint8_t Value>
        struct adtrg1s: public detail::pins::peripheral_helper<detail::pins::peripheral_helper_impl<9, 16, 4>, Value> {};

        // alternate function for adtrg0s
        template <uint8_t Value>
        struct adtrg0s: public detail::pins::peripheral_helper<detail::pins::peripheral_helper_impl<9, 12, 4>, Value> {};

        // alternate function for qzin1s
        template <uint8_t Value>
        struct qzin1s: public detail::pins::peripheral_helper<detail::pins::peripheral_helper_impl<9, 10, 2>, Value> {};

        // alternate function for qbin1s
        template <uint8_t Value>
        struct qbin1s: public detail::pins::peripheral_helper<detail::pins::peripheral_helper_impl<9, 8, 2>, Value> {};

        // alternate function for qain1s
        template <uint8_t Value>
        struct qain1s: public detail::pins::peripheral_helper<detail::pins::peripheral_helper_impl<9, 6, 2>, Value> {};

        // alternate function for qzin0s
        template <uint8_t Value>
        struct qzin0s: public detail::pins::peripheral_helper<detail::pins::peripheral_helper_impl<9, 4, 2>, Value> {};

        // alternate function for qbin0s
        template <uint8_t Value>
        struct qbin0s: public detail::pins::peripheral_helper<detail::pins::peripheral_helper_impl<9, 2, 2>, Value> {};

        // alternate function for qain0s
        template <uint8_t Value>
        struct qain0s: public detail::pins::peripheral_helper<detail::pins::peripheral_helper_impl<9, 0, 2>, Value> {};
    };

    namespace multi_functional_serial {
        // register EPFR16
        // alternate function for scs6b
        template <uint8_t Value>
        struct scs6b: public detail::pins::peripheral_helper<detail::pins::peripheral_helper_impl<16, 0, 2>, Value> {};
    }
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
            "Pin does not support analog mode"
        );

        // always disable analog mode if we are not switching to analog mode
        if constexpr (!std::is_same_v<Periph, io::detail::alternate::analog>) {
            // disable the analog mode if it is available on the pin
            if constexpr (has_analog) {
                // disable the analog mode
                Pin::port::port->ADE &= (~(0x1 << Pin::analog_number));
            }
        }

        // get the pointer to the pin select we need to write to
        volatile uint32_t *const pfr = &Pin::port::port->PFR0;

        // set the alternate function
        if constexpr (std::is_same_v<Periph, io::detail::alternate::none>) {
            // enable control using the PIO. Disables peripheral
            // control of the pin
            pfr[Pin::port::id] &= (~detail::pins::mask<Pin>);
        }
        else if constexpr (std::is_same_v<Periph, io::detail::alternate::analog>) {
            // set the pin to analog mode
            Pin::port::port->ADE |= (0x1 << Pin::analog_number);
        }
        else {
            // disable control using the PIO. Enables peripheral
            // control of the pin
            pfr[Pin::port::id] |= detail::pins::mask<Pin>;

            // all the other peripherals are configured using the
            // peripheral helper
            detail::pins::peripheral_helper_setup<Pin, Periph>();
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
            volatile uint32_t *const ddr = &Pin::port::port->DDR0;

            // disable the output on the pin
            ddr[Pin::port::id] &= (~detail::pins::mask<Pin>);
        }

        constexpr static bool get() {
            // get the pointer to the pin select we need to write to
            volatile uint32_t *const pdir = &Pin::port::port->PDIR0;

            // get the status of the pin
            return pdir[Pin::port::id] & detail::pins::mask<Pin>;
        }

        template <bool Val>
        constexpr static void pullup_enable() {
            // get the pointer to the pin select we need to write to
            volatile uint32_t *const pcr = &Pin::port::port->PCR0;

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
            volatile uint32_t *const ddr = &Pin::port::port->DDR0;

            // // enable the gpio output
            ddr[Pin::port::id] |= detail::pins::mask<Pin>;
        }

        template <bool Val>
        constexpr static void set() {
            // get the pointer to the pin select we need to write to
            volatile uint32_t *const pdor = &Pin::port::port->PDOR0;

            if constexpr (Val) {
                pdor[Pin::port::id] |= detail::pins::mask<Pin>;
            }
            else {
                pdor[Pin::port::id] &= (~detail::pins::mask<Pin>);
            }
        }

        constexpr static void set(const bool val) {
            // get the pointer to the pin select we need to write to
            volatile uint32_t *const pdor = &Pin::port::port->PDOR0;

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