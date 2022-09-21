#ifndef KLIB_INVERTED_GPIO_HPP
#define KLIB_INVERTED_GPIO_HPP

namespace klib::io {
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