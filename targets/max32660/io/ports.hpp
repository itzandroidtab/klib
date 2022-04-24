#ifndef LLIB_PORTS_HPP
#define LLIB_PORTS_HPP

#include <tuple>
#include <klib/math.hpp>

namespace klib::max32660::io {
    /**
     * Port that combines multiple pins into a single type.
     *
     * @tparam Pins
     */
    template<typename ...Pins>
    class port_in {
    protected:
        // tuple with all the pins
        using pins = std::tuple<Pins...>;

        // using to map to a index in the tuple
        template<size_t Index>
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
        template<size_t Index = 0>
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
        template<size_t Index = 0>
        constexpr static uint32_t create_pin_mask() {
            if constexpr (Index + 1 < sizeof...(Pins)) {
                return io::pins::detail::mask<pin<Index>> | create_pin_mask<Index + 1>();
            }
            else {
                return io::pins::detail::mask<pin<Index>>;
            }
        }

    public:
        // amount of pins in type
        constexpr static size_t pin_count = sizeof...(Pins);

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
                return io::pins::detail::port<typename pin<0>::port>->IN & mask;
            }
            else {
                // create a mask of all the pins
                constexpr uint32_t mask = create_pin_mask();

                // read the whole pio and clear pins not in use
                const uint32_t v = io::pins::detail::port<typename pin<0>::port>->IN & mask;

                return map_to_pin_order(v);
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
     * Port that combines multiple llib::pin_out pins
     * into a single type.
     *
     * @tparam Pins
     */
    template<typename ...Pins>
    class port_out {
    protected:
        // tuple with all the pins
        using pins = std::tuple<Pins...>;

        // using to map to a index in the tuple
        template<size_t Index>
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
        template<size_t Index = 0>
        constexpr static uint32_t map_to_pio_order(uint32_t mask) {
            // check if we are at the last entry
            if constexpr (Index + 1 < sizeof...(Pins)) {
                // recursively get the get input bit and map to input pin using msb
                return (
                    ((mask >> Index) << pin<(sizeof...(Pins) - 1) - Index>::number) | 
                    map_to_pio_order<Index + 1>(mask)
                );
            }
            else {
                // get the correct input bit and use msb to map these to the input pins
                return (
                    (mask >> Index) << pin<(sizeof...(Pins) - 1) - Index>::number
                );
            }
        }

        /**
         * @brief Create a pin mask of all the output pins in the current type
         * 
         * @tparam Index 
         * @return uint32_t 
         */
        template<size_t Index = 0>
        constexpr static uint32_t create_pin_mask() {
            if constexpr (Index + 1 < sizeof...(Pins)) {
                return io::pins::detail::mask<pin<Index>> | create_pin_mask<Index + 1>();
            }
            else {
                return io::pins::detail::mask<pin<Index>>;
            }
        }

        /**
         * @brief Write directly to the pio
         * 
         * @param positive_mask 
         * @param negative_mask 
         */
        constexpr static void set_pio(const uint32_t positive_mask, const uint32_t negative_mask) {
            // check if we need to call set
            if (positive_mask) {
                // set the pins;
                io::pins::detail::port<typename pin<0>::port>->OUT_SET = positive_mask;
            }

            if (negative_mask) {                   
                // clear the pins
                io::pins::detail::port<typename pin<0>::port>->OUT_CLR = negative_mask;
            }
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
                io::pins::detail::port<typename pin<0>::port>->OUT_SET = PosMask;
            }

            if constexpr (NegMask) {                   
                // clear the pins
                io::pins::detail::port<typename pin<0>::port>->OUT_CLR = NegMask;
            }
        }

    public:
        // amount of pins in type
        constexpr static size_t pin_count = sizeof...(Pins);

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

#endif //LLIB_PORTS_HPP
