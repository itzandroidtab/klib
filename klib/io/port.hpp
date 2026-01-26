#ifndef KLIB_PORT_HPP
#define KLIB_PORT_HPP

namespace klib::io {
    /**
     * @brief Dummy pin to subsitute a hardware pin. Pin can be used as a 
     * dummy output or a dDummy input with a constant value
     * 
     * @tparam Value 
     */
    template <bool Value = false>
    class pin_dummy {
    public:
        // using for the array of callbacks
        using interrupt_callback = void(*)();

        /**
         * @brief Enum for the dummy edge interrupts
         * 
         */
        enum class edge {
            falling,
            rising,
            dual_edge,
            low_level,
            high_level,
        };

        /**
         * @brief Init the fake pin
         * 
         */
        constexpr static void init() {}

        /**
         * @brief Init the fake pin using a edge trigger
         * 
         * @note Will not do anything
         * 
         * @tparam Edge 
         * @param callback 
         */
        template <edge Edge>
        constexpr static void init(interrupt_callback callback) {}

        /**
         * @brief Get the value of the pin
         * 
         * @return true 
         * @return false 
         */
        constexpr static bool get() {
            return Value;
        }

        /**
         * @brief Enable/Disable the pullup
         * 
         * @tparam Val 
         */
        template <bool Val>
        constexpr static void pullup_enable() {}

        /**
         * @brief Enable/Disable the pulldown
         * 
         * @tparam Val 
         */
        template <bool Val>
        constexpr static void pulldown_enable() {}

        /**
         * @brief Set the pin to a specific value 
         * 
         * @tparam Val 
         */
        template <bool Val>
        constexpr static void set() {}

        /**
         * @brief Set the pin to a specific value
         * 
         * @param val 
         */
        constexpr static void set(const bool val) {}
    };

    /**
     * @brief Decorator that inverts the value the pin is set at.
     * 
     * @tparam Pin 
     */
    template <typename Pin>
    class pin_inverted {
    public:
        /**
         * @brief Set a pin with the oposite value that is entered
         * 
         * @tparam Val 
         */
        template <bool Val>
        static void set() {
            Pin::template set<!Val>();
        }

        /**
         * @brief Set a pin with the oposite value that is entered
         * 
         * @tparam Val 
         */
        static void set(const bool val) {
            Pin::set(!val);
        }
    };
}

#endif