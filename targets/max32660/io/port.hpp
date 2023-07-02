#ifndef KLIB_MAX32660_PORT_HPP
#define KLIB_MAX32660_PORT_HPP

#include <tuple>

#include <max32660.hpp>

#include <klib/irq_helper.hpp>

namespace klib::max32660::io::detail::alternate {
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

// peripheral namespace for perihperals not affected by chip packages
namespace klib::max32660::io::periph {
    struct gpio0 {
        // peripheral id (e.g gpio0, gpio1)
        constexpr static uint32_t id = 0;

        // peripheral interrupt position
        constexpr static uint32_t irq_id = 40;

        // port to the gpio hardware
        static inline GPIO0_Type *const port = GPIO0;
    };

    struct gpio0_wakeup {
        // peripheral id (e.g gpio0, gpio1)
        constexpr static uint32_t id = 0;

        // peripheral interrupt position
        constexpr static uint32_t irq_id = 70;

        // port to the gpio hardware
        static inline GPIO0_Type *const port = GPIO0;
    };
}

namespace klib::max32660::io::detail::pins {
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
        if constexpr (std::is_same_v<Periph, io::detail::alternate::func_1>) {
            // setup alternate function 1"
            Pin::port::port->EN2_CLR = mask<Pin>;
            Pin::port::port->EN1_CLR = mask<Pin>;
            Pin::port::port->EN_CLR = mask<Pin>;
        }
        else if constexpr (std::is_same_v<Periph, io::detail::alternate::func_2>) {
            // setup alternate function 2
            Pin::port::port->EN2_CLR = mask<Pin>;
            Pin::port::port->EN1_SET = mask<Pin>;
            Pin::port::port->EN_CLR = mask<Pin>;
        }
        else if constexpr (std::is_same_v<Periph, io::detail::alternate::func_3>) {
            // setup alternate function 3
            Pin::port::port->EN2_CLR = mask<Pin>;
            Pin::port::port->EN1_SET = mask<Pin>;
            Pin::port::port->EN_SET = mask<Pin>;
        }
        else {
            // setup normal gpio function
            Pin::port::port->EN2_CLR = mask<Pin>;
            Pin::port::port->EN1_CLR = mask<Pin>;
            Pin::port::port->EN_SET = mask<Pin>;
        }
    }
}

namespace klib::max32660::io {
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
            const uint32_t status = Port::port->INT_STAT;

            // interrupt mask
            const uint32_t mask = Port::port->INT_EN;

            // clear the interrupts we are handling
            Port::port->INT_CLR = status & mask;

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
            // register our handler
            irq::template register_irq<Port::irq_id>(irq_handler);

            // enable the interrupt
            enable_irq<Port::irq_id>();
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
            return Pin::port::port->IN & detail::pins::mask<Pin>;
        }

        template <bool Val>
        constexpr static void pullup_enable() {
            // enable/disable the pullups
            if constexpr (Val) {
                Pin::port::port->PS |= detail::pins::mask<Pin>;
                Pin::port::port->PAD_CFG1 |= detail::pins::mask<Pin>;
            }
            else {
                Pin::port::port->PAD_CFG1 &= ~detail::pins::mask<Pin>;
            }
        }

        template <bool Val>
        constexpr static void pulldown_enable() {
            // enable/disable the pulldowns
            if constexpr (Val) {
                Pin::port::port->PS &= ~detail::pins::mask<Pin>;
                Pin::port::port->PAD_CFG1 |= detail::pins::mask<Pin>;
            }
            else {
                Pin::port::port->PAD_CFG1 &= ~detail::pins::mask<Pin>;
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
            Pin::port::port->OUT_EN_SET = detail::pins::mask<Pin>;
        }

        template <bool Val>
        constexpr static void set() {
            if constexpr (Val) {
                Pin::port::port->OUT_SET = detail::pins::mask<Pin>;
            }
            else {
                Pin::port::port->OUT_CLR = detail::pins::mask<Pin>;
            }
        }

        constexpr static void set(const bool val) {
            if (val) {
                Pin::port::port->OUT_SET = detail::pins::mask<Pin>;
            }
            else {
                Pin::port::port->OUT_CLR = detail::pins::mask<Pin>;
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
                Pin::port::port->OUT_EN_SET = detail::pins::mask<Pin>;

                // enable the gpio output
                pin_out<Pin>::template set<true>();
            }
            else {
                // clear the gpio output flag
                Pin::port::port->OUT_EN_CLR = detail::pins::mask<Pin>;
            }
        }

        constexpr static void set(const bool val) {
            if (val) {
                // setup the gpio as a output
                Pin::port::port->OUT_EN_SET = detail::pins::mask<Pin>;

                // enable the gpio output
                pin_out<Pin>::template set<true>();
            }
            else {
                // clear the gpio output flag
                Pin::port::port->OUT_EN_CLR = detail::pins::mask<Pin>;
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
                Pin::port::port->OUT_EN_CLR = detail::pins::mask<Pin>;
            }
            else {
                // enable the gpio output
                Pin::port::port->OUT_EN_SET = detail::pins::mask<Pin>;

                pin_out<Pin>::template set<false>();
            }
        }

        constexpr static void set(const bool val) {
            if (val) {
                // disable the gpio output
                Pin::port::port->OUT_EN_CLR = detail::pins::mask<Pin>;
            }
            else {
                // enable the gpio output
                Pin::port::port->OUT_EN_SET = detail::pins::mask<Pin>;

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

    template <typename Pin>
    class pin_interrupt {
    public:
        /**
         * @brief Using for the interrupt callback used in the pin irq
         * 
         */
        using interrupt_callback = port_interrupt<typename Pin::port>::interrupt_callback;
        
    protected:
        /**
         * @brief Trigger modes
         * 
         */
        enum class trigger {
            level = 0,
            edge = 1,
        };

        template <trigger Trigger, bool Value, bool DualEdge>
        constexpr static void init(interrupt_callback callback) {
            // init the port
            port_interrupt<typename Pin::port>::init();

            // clear all the alternate functions
            detail::pins::set_peripheral<Pin, io::detail::alternate::none>();

            // set the mode bit in the register
            Pin::port::port->INT_MOD &= ~detail::pins::mask<Pin>;
            Pin::port::port->INT_MOD |= (static_cast<uint32_t>(Trigger) << Pin::number);

            // check what value we should set
            if constexpr (Trigger == trigger::level) {
                Pin::port::port->INT_POL &= ~detail::pins::mask<Pin>;
                Pin::port::port->INT_POL |= (static_cast<uint32_t>(Value) << Pin::number);
            }
            else {
                Pin::port::port->INT_POL &= ~detail::pins::mask<Pin>;
                Pin::port::port->INT_POL |= (static_cast<uint32_t>(Value) << Pin::number);

                Pin::port::port->INT_DUAL_EDGE &= ~detail::pins::mask<Pin>;
                Pin::port::port->INT_DUAL_EDGE |= (static_cast<uint32_t>(DualEdge) << Pin::number);
            }

            // unregister the interrupt from the port
            port_interrupt<typename Pin::port>::template register_irq<Pin>(callback);
        }

    public:
        enum class level {
            low = 0,
            high = 1,
        };

        enum class edge {
            falling = 0,
            rising = 1,
            dual_edge,
        };

        template <level Level>
        constexpr static void init(interrupt_callback callback) {
            // init the level trigger
            init<trigger::level, static_cast<bool>(Level), false>(callback);
        }

        template <edge Edge>
        constexpr static void init(interrupt_callback callback) {
            // init the edge trigger
            init<trigger::edge, static_cast<bool>(Edge), Edge == edge::dual_edge>(callback);
        }

        constexpr static void enable() {
            // enable the interrupt after we have set a callback
            Pin::port::port->INT_EN_SET = detail::pins::mask<Pin>;
        }

        constexpr static void disable() {
            // disable the interrupt
            Pin::port::port->INT_EN_CLR = detail::pins::mask<Pin>;
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
                return pin<0>::port->IN & mask;
            }
            else {
                // create a mask of all the pins
                constexpr uint32_t mask = create_pin_mask();

                // read the whole pio and clear pins not in use
                return map_to_pin_order(pin<0>::port->IN & mask);
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
            pin<0>::port->OUT_SET = positive_mask;                
            // clear the pins
            pin<0>::port->OUT_CLR = negative_mask;
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
                pin<0>::port->OUT_SET = PosMask;
            }

            if constexpr (NegMask) {                   
                // clear the pins
                pin<0>::port->OUT_CLR = NegMask;
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
