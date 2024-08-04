#ifndef KLIB_TARGET_TEMPLATE_HPP
#define KLIB_TARGET_TEMPLATE_HPP

#include <cstdint>

namespace klib::target_template::io::detail::alternate {
    // alternate functions for all the gpio
    // default function.
    struct none {};

    // alternate function 1
    struct func_1 {};
}

namespace klib::target_template::io::periph {
    /**
     * @brief struct with all the information about gpio0
     *
     */
    struct gpio0 {
        // peripheral id (e.g gpio0, gpio1)
        constexpr static uint32_t id = 0;

        // peripheral interrupt position (numbering starts with the
        // last item of the arm vector table)
        constexpr static uint32_t interrupt_id = 16;

        // on hardware this should have the port to
        // write/read the hardware with
        static inline void *const port = (nullptr);
    };
}

namespace klib::target_template::io {
    /**
     * @brief Class to read a digital pin
     *
     * @tparam Pin
     */
    template <typename Pin>
    class pin_in {
        /**
         * @brief Init the pin
         *
         */
        constexpr static void init() {}
    };

    /**
     * @brief Class to write to a digital pin
     *
     * @tparam Pin
     */
    template <typename Pin>
    class pin_out {
        /**
         * @brief Init the pin
         *
         */
        constexpr static void init() {}
    };

    /**
     * @brief Class to create a open collector digital output
     *
     * @tparam Pin
     */
    template <typename Pin>
    class pin_oc {
        /**
         * @brief Init the pin
         *
         */
        constexpr static void init() {}
    };

    /**
     * @brief Class to create a open drain digital output
     *
     * @tparam Pin
     */
    template <typename Pin>
    class pin_od {
        /**
         * @brief Init the pin
         *
         */
        constexpr static void init() {}
    };

    /**
     * @brief Class to create a pin that can be written and read from
     *
     * @tparam Pin
     */
    template <typename Pin>
    class pin_in_out {
        /**
         * @brief Init the pin
         *
         */
        constexpr static void init() {}
    };

    /**
     * @brief Class to create a open collector pin that can be written and read from
     *
     * @tparam Pin
     */
    template <typename Pin>
    class pin_in_out_oc {
        /**
         * @brief Init the pin
         *
         */
        constexpr static void init() {}
    };

    /**
     * @brief Class to create a open drain pin that can be written and read from
     *
     * @tparam Pin
     */
    template <typename Pin>
    class pin_in_out_od {
        /**
         * @brief Init the pin
         *
         */
        constexpr static void init() {}
    };

    /**
     * @brief Class to trigger a callback when a certain criteria is met
     *
     * @tparam Pin
     */
    template <typename Pin>
    class pin_interrupt {
        /**
         * @brief Init the pin
         *
         */
        constexpr static void init() {}
    };
}

#endif