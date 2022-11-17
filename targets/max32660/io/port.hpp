#ifndef KLIB_MAX32660_PORT_HPP
#define KLIB_MAX32660_PORT_HPP

#include <tuple>
#include <klib/masked_irq.hpp>

#include "periph.hpp"
#include "pins.hpp"

namespace klib::max32660::io::detail::port {
    template <uint32_t Port>
    const GPIO0_Type *port = nullptr;

    // port when using the gpio0
    template <>
    GPIO0_Type *const port<0> = GPIO0;
}

namespace klib::max32660::io::detail::pins {
    template <typename Port>
    const GPIO0_Type *port = nullptr;

    template <>
    GPIO0_Type *const port<io::detail::gpio0> = GPIO0;

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
        // set the 3 function registers
        if constexpr (std::is_same_v<Periph, detail::alternate::func_1>) {
            // setup alternate function 1
            detail::pins::port<typename Pin::port>->EN2_CLR = mask<Pin>;
            detail::pins::port<typename Pin::port>->EN1_CLR = mask<Pin>;
            detail::pins::port<typename Pin::port>->EN_CLR = mask<Pin>;
        }
        else if (std::is_same_v<Periph, detail::alternate::func_2>) {
            // setup alternate function 2
            detail::pins::port<typename Pin::port>->EN2_CLR = mask<Pin>;
            detail::pins::port<typename Pin::port>->EN1_SET = mask<Pin>;
            detail::pins::port<typename Pin::port>->EN_CLR = mask<Pin>;
        }
        else if (std::is_same_v<Periph, detail::alternate::func_3>) {
            // setup alternate function 3
            detail::pins::port<typename Pin::port>->EN2_CLR = mask<Pin>;
            detail::pins::port<typename Pin::port>->EN1_SET = mask<Pin>;
            detail::pins::port<typename Pin::port>->EN_SET = mask<Pin>;
        }
        else {
            // setup normal gpio function
            detail::pins::port<typename Pin::port>->EN2_CLR = mask<Pin>;
            detail::pins::port<typename Pin::port>->EN1_CLR = mask<Pin>;
            detail::pins::port<typename Pin::port>->EN_SET = mask<Pin>;
        }
    }
}

namespace klib::max32660::io {
    /**
     * @brief Interrupt handler for a port. Uses masked register to handle the interrupts
     * 
     * @tparam Port 
     */
    template <typename Port>
    class port_interrupt {
    protected:
        static klib::masked_register_irq masked;

        /**
         * @brief Interrupt handler
         * 
         */
        static void irq_handler() {
            // handle the interrupts that are registered
            masked.handle_irq(0, 0);
        }

    public:
        static void init() {
            // register our handler
            max32660::irq::template register_irq<Port::irq_id>(irq_handler);
        }

        /**
         * @brief Register a interrupt for a specific pin
         * 
         * @tparam Pin 
         * @param callback 
         */
        template <typename Pin>
        static void register_irq(masked_register_irq::interrupt_callback callback) {

        }

        /**
         * @brief Register multiple pins to a callback
         * 
         * @tparam Pins 
         * @param callback 
         */
        template <typename ...Pins>
        static void register_irq(masked_register_irq::interrupt_callback callback) {
            // (register_irq<Pins>(), ...);
        }
    };
}

namespace klib::max32660::io {
    template <typename Pin>
    class pin_in {
    public:
        constexpr static void init() {
            // clear all the alternate functions
            detail::pins::set_peripheral<Pin, io::detail::alternate::none>();
        }

        constexpr static bool get() {
            // get the status of the pin
            return detail::pins::port<typename Pin::port>->IN & detail::pins::mask<Pin>;
        }

        template <bool Val>
        constexpr static void pullup_enable() {
            // enable/disable the pullups
            if constexpr (Val) {
                detail::pins::port<typename Pin::port>->PS |= detail::pins::mask<Pin>;
                detail::pins::port<typename Pin::port>->PAD_CFG1 |= detail::pins::mask<Pin>;
            }
            else {
                detail::pins::port<typename Pin::port>->PAD_CFG1 &= ~detail::pins::mask<Pin>;
            }
        }

        template <bool Val>
        constexpr static void pulldown_enable() {
            // enable/disable the pulldowns
            if constexpr (Val) {
                detail::pins::port<typename Pin::port>->PS &= ~detail::pins::mask<Pin>;
                detail::pins::port<typename Pin::port>->PAD_CFG1 |= detail::pins::mask<Pin>;
            }
            else {
                detail::pins::port<typename Pin::port>->PAD_CFG1 &= ~detail::pins::mask<Pin>;
            }            
        }
    };

    template <typename Pin>
    class pin_out {
    public:
        constexpr static void init() {
            // clear all the alternate functions
            detail::pins::set_peripheral<Pin, io::detail::alternate::none>();

            // enable the gpio output
            detail::pins::port<typename Pin::port>->OUT_EN_SET = detail::pins::mask<Pin>;
        }

        template <bool Val>
        constexpr static void set() {
            if constexpr (Val) {
                detail::pins::port<typename Pin::port>->OUT_SET = detail::pins::mask<Pin>;
            }
            else {
                detail::pins::port<typename Pin::port>->OUT_CLR = detail::pins::mask<Pin>;
            }
        }

        constexpr static void set(const bool val) {
            if (val) {
                detail::pins::port<typename Pin::port>->OUT_SET = detail::pins::mask<Pin>;
            }
            else {
                detail::pins::port<typename Pin::port>->OUT_CLR = detail::pins::mask<Pin>;
            }
        }
    };

    template <typename Pin>
    class pin_oc {
    public:
        constexpr static void init() {
            // clear all the alternate functions
            detail::pins::set_peripheral<Pin, io::detail::alternate::none>();
        }

        template <bool Val>
        constexpr static void set() {
            if constexpr (Val) {
                // setup the gpio as a output
                detail::pins::port<typename Pin::port>->OUT_EN_SET = detail::pins::mask<Pin>;

                // enable the gpio output
                pin_out<Pin>::template set<true>();
            }
            else {
                // clear the gpio output flag
                detail::pins::port<typename Pin::port>->OUT_EN_CLR = detail::pins::mask<Pin>;
            }
        }

        constexpr static void set(const bool val) {
            if (val) {
                // setup the gpio as a output
                detail::pins::port<typename Pin::port>->OUT_EN_SET = detail::pins::mask<Pin>;

                // enable the gpio output
                pin_out<Pin>::template set<true>();
            }
            else {
                // clear the gpio output flag
                detail::pins::port<typename Pin::port>->OUT_EN_CLR = detail::pins::mask<Pin>;
            }
        }
    };

    template <typename Pin>
    class pin_od {
    public:
        constexpr static void init() {
            // clear all the alternate functions
            detail::pins::set_peripheral<Pin, io::detail::alternate::none>();
        }

        template <bool Val>
        constexpr static void set() {
            if constexpr (Val) {
                // disable the gpio output
                detail::pins::port<typename Pin::port>->OUT_EN_CLR = detail::pins::mask<Pin>;
            }
            else {
                // enable the gpio output
                detail::pins::port<typename Pin::port>->OUT_EN_SET = detail::pins::mask<Pin>;

                pin_out<Pin>::template set<false>();
            }
        }

        constexpr static void set(const bool val) {
            if (val) {
                // disable the gpio output
                detail::pins::port<typename Pin::port>->OUT_EN_CLR = detail::pins::mask<Pin>;
            }
            else {
                // enable the gpio output
                detail::pins::port<typename Pin::port>->OUT_EN_SET = detail::pins::mask<Pin>;

                pin_out<Pin>::template set<false>();
            }
        }
    };

    template <typename Pin>
    class pin_in_out {
    public:
        constexpr static void init() {
            // init using pin_out as we can read the pin anyway
            pin_out<Pin>::init();
        }

        constexpr static bool get() {
            // get the status of the pin
            return pin_in<Pin>::get();
        }

        template <bool Val>
        constexpr static void pullup_enable() {
            pin_in<Pin>::template pullup_enable<Val>();
        }

        template <bool Val>
        constexpr static void pulldown_enable() {
            pin_in<Pin>::template pulldown_enable<Val>();
        }

        template <bool Val>
        constexpr static void set() {
            pin_out<Pin>::template set<Val>();
        }

        constexpr static void set(const bool val) {
            pin_out<Pin>::set(val);
        }
    };

    template <typename Pin>
    class pin_in_out_oc {
    public:
        constexpr static void init() {
            // init using pin_oc as we can read the pin anyway
            pin_oc<Pin>::init();
        }

        constexpr static bool get() {
            // get the status of the pin
            return pin_in<Pin>::get();
        }

        template <bool Val>
        constexpr static void pullup_enable() {
            pin_in<Pin>::template pullup_enable<Val>();
        }

        template <bool Val>
        constexpr static void pulldown_enable() {
            pin_in<Pin>::template pulldown_enable<Val>();
        }

        template <bool Val>
        constexpr static void set() {
            pin_oc<Pin>::template set<Val>();
        }

        constexpr static void set(const bool val) {
            pin_oc<Pin>::set(val);
        }
    };

    template <typename Pin>
    class pin_in_out_od {
    public:
        constexpr static void init() {
            // init using pin_od as we can read the pin anyway
            pin_od<Pin>::init();
        }

        constexpr static bool get() {
            // get the status of the pin
            return pin_in<Pin>::get();
        }

        template <bool Val>
        constexpr static void pullup_enable() {
            pin_in<Pin>::template pullup_enable<Val>();
        }

        template <bool Val>
        constexpr static void pulldown_enable() {
            pin_in<Pin>::template pulldown_enable<Val>();
        }

        template <bool Val>
        constexpr static void set() {
            pin_od<Pin>::template set<Val>();
        }

        constexpr static void set(const bool val) {
            pin_od<Pin>::set(val);
        }
    };
}

namespace klib::max32660::io {
    /**
     * @brief Port that combines multiple pins into a single type.
     *
     * @tparam Pins
     */
    template <typename ...Pins>
    class port_in {
    protected:
        // tuple with all the pins
        using pins = std::tuple<Pins...>;

        // using to map to a index in the tuple
        template <uint32_t Index>
        using pin = std::tuple_element_t<Index, pins>;

        // make sure we dont have any more input as the register width
        static_assert(
            (sizeof...(Pins) <= 32) && (sizeof...(Pins) > 0), 
            "Port_in needs at least 1 pin and at max 32 pins"
        );

        // check if all the ports are the same
        static_assert(
            ((std::is_same_v<typename Pins::port, typename pin<0>::port>) && ...), 
            "Port_in only works with pins in the same pio port"
        );

        /**
         * @brief Map the raw input data to the input pins of the type. Data is 
         * arranged by order of input data. Last item in input pins is lsb.
         * 
         * @tparam Index
         * @param raw
         * @return uint32_t
         */
        template <uint32_t Index = 0>
        constexpr static uint32_t map_to_pin_order(uint32_t raw) {
            if constexpr (Index + 1 < sizeof...(Pins)) {
                // recursively get the get input bit and map to input pin using msb
                return (
                    (((raw >> pin<(sizeof...(Pins) - 1) - Index>::number) & 0x1) << Index) | 
                    map_to_pin_order<Index + 1>(raw)
                );
            }
            else {
                // get the correct input bit and use msb to map these to the input pins
                return (
                    ((raw >> pin<(sizeof...(Pins) - 1) - Index>::number) & 0x1) << Index
                );
            }
        }

        /**
         * @brief Create a pin mask of all the input pins in the current type
         * 
         * @tparam Index 
         * @return uint32_t 
         */
        template <uint32_t Index = 0>
        constexpr static uint32_t create_pin_mask() {
            if constexpr (Index + 1 < sizeof...(Pins)) {
                return detail::pins::mask<pin<Index>> | create_pin_mask<Index + 1>();
            }
            else {
                return detail::pins::mask<pin<Index>>;
            }
        }

    public:
        // amount of pins in type
        constexpr static uint32_t pin_count = sizeof...(Pins);

        /**
         * @brief Init all the pins in the type
         * 
         */
        constexpr static void init() {
            // init all the pins
            (pin_in<Pins>::init(), ...);
        }

        /**
         * @brief Get all the pins mapped to the input order
         * 
         * @return constexpr uint32_t 
         * @tparam Raw
         */
        template <bool Raw = false>
        constexpr static uint32_t get() {
            // return the result mapped to the pin order
            if constexpr (Raw) {
                // create a mask of all the pins
                constexpr uint32_t mask = create_pin_mask();

                // read the whole pio and clear pins not in use
                return detail::pins::port<typename pin<0>::port>->IN & mask;
            }
            else {
                // create a mask of all the pins
                constexpr uint32_t mask = create_pin_mask();

                // read the whole pio and clear pins not in use
                return map_to_pin_order(detail::pins::port<typename pin<0>::port>->IN & mask);
            }
        }

        /**
         * @brief Enable/Disable pullups on all pins
         * 
         * @tparam Val 
         */
        template <bool Val>
        constexpr static void pullup_enable() {
            (pin_in<Pins>::template pullup_enable<Val>(), ...);
        }

        /**
         * @brief Enable/Disable pulldowns for all all pins
         * 
         * @tparam Val 
         */
        template <bool Val>
        constexpr static void pulldown_enable() {
            (pin_in<Pins>::template pulldown_enable<Val>(), ...);
        }
    };

    /**
     * Port that combines multiple pins into a single type.
     *
     * @tparam Pins
     */
    template <typename ...Pins>
    class port_out {
    protected:
        // tuple with all the pins
        using pins = std::tuple<Pins...>;

        // using to map to a index in the tuple
        template <uint32_t Index>
        using pin = std::tuple_element_t<Index, pins>;

        // make sure we dont have any more input as the register width
        static_assert(
            sizeof...(Pins) <= 32 && sizeof...(Pins) > 0, 
            "port_out needs at least 1 pin and at max 32 pins"
        );

        // check if all the ports are the same
        static_assert(
            ((std::is_same_v<typename Pins::port, typename pin<0>::port>) && ...), 
            "port_out only works with pins in the same pio port"
        );

        /**
         * @brief Map the ordered input data to the order of the pio.
         * 
         * @tparam Index
         * @param raw
         * @return uint32_t
         */
        template <uint32_t Index = 0>
        constexpr static uint32_t map_to_pio_order(uint32_t mask) {
            // check if we are at the last entry
            if constexpr (Index + 1 < sizeof...(Pins)) {
                // recursively get the get input bit and map to input pin using msb
                return (
                    (((mask >> Index) & 0x1) << pin<(sizeof...(Pins) - 1) - Index>::number) | 
                    map_to_pio_order<Index + 1>(mask)
                );
            }
            else {
                // get the correct input bit and use msb to map these to the input pins
                return (
                    ((mask >> Index) & 0x1) << pin<(sizeof...(Pins) - 1) - Index>::number
                );
            }
        }

        /**
         * @brief Create a pin mask of all the output pins in the current type
         * 
         * @tparam Index 
         * @return uint32_t 
         */
        template <uint32_t Index = 0>
        constexpr static uint32_t create_pin_mask() {
            if constexpr (Index + 1 < sizeof...(Pins)) {
                return detail::pins::mask<pin<Index>> | create_pin_mask<Index + 1>();
            }
            else {
                return detail::pins::mask<pin<Index>>;
            }
        }

        /**
         * @brief Write directly to the pio
         * 
         * @param positive_mask 
         * @param negative_mask 
         */
        constexpr static void set_pio(const uint32_t positive_mask, const uint32_t negative_mask) {
            // set the pins
            detail::pins::port<typename pin<0>::port>->OUT_SET = positive_mask;                
            // clear the pins
            detail::pins::port<typename pin<0>::port>->OUT_CLR = negative_mask;
        }
        
        /**
         * @brief Write directly to the pio
         * 
         * @tparam PosMask 
         * @tparam NegMask 
         */
        template <uint32_t PosMask, uint32_t NegMask>
        constexpr static void set_pio() {
            // check if we need to call set
            if constexpr (PosMask) {
                // set the pins;
                detail::pins::port<typename pin<0>::port>->OUT_SET = PosMask;
            }

            if constexpr (NegMask) {                   
                // clear the pins
                detail::pins::port<typename pin<0>::port>->OUT_CLR = NegMask;
            }
        }

    public:
        // amount of pins in type
        constexpr static uint32_t pin_count = sizeof...(Pins);

        /**
         * @brief Init all the pins in the type
         * 
         */
        constexpr static void init() {
            // init all the pins
            (pin_out<Pins>::init(), ...);
        }

        /**
         * @brief Write data ordered by pin order to the pins
         * 
         * @details Set data in a port of pins. When not using raw values the 
         * input is mapped to the order of the pins using msb.
         * 
         * @tparam IsRaw 
         * @param val 
         */
        template <bool IsRaw = false>
        constexpr static void set(const uint32_t val) {
            // create a pin mask
            constexpr uint32_t mask = create_pin_mask();
            
            // check if we need to remap the input to raw data
            if constexpr (IsRaw) {
                // data is already raw data. Use the pinmask and map to output
                const uint32_t positive_mask = (val & mask);
                const uint32_t negative_mask = ((~val) & mask);

                // write data to the pio
                set_pio(positive_mask, negative_mask);
            }
            else {
                // map the data the pio order and write as raw data
                const uint32_t raw = map_to_pio_order(val);

                // data is already raw data. Use the pinmask and map to output
                const uint32_t positive_mask = (raw & mask);
                const uint32_t negative_mask = ((~raw) & mask);

                // write data to the pio
                set_pio(positive_mask, negative_mask);
            }
        }

        /**
         * @brief Write data ordered by pin order to the pins
         * 
         * @details Set data in a port of pins. When not using raw values the 
         * input is mapped to the order of the pins using msb.
         * 
         * @tparam Val 
         * @tparam IsRaw 
         */
        template <uint32_t Val, bool IsRaw = false>
        constexpr static void set() {
            // create a pin mask
            constexpr uint32_t mask = create_pin_mask();

            // check if we need to remap the input to raw data
            if constexpr (IsRaw) {
                // data is already raw data. Use the pinmask and map to output
                constexpr uint32_t positive_mask = (Val & mask);
                constexpr uint32_t negative_mask = ((~Val) & mask);

                // write data to pio
                set_pio<positive_mask, negative_mask>();
            }
            else {
                constexpr uint32_t raw = map_to_pio_order(Val);

                // get all the pins we need to clear and set
                constexpr uint32_t positive_mask = (raw & mask);
                constexpr uint32_t negative_mask = ((~raw) & mask);

                // write data to pio
                set_pio<positive_mask, negative_mask>();
            }
        }
    };
}


#endif
