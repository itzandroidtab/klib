#ifndef KLIB_NXP_LPC178X_CLOCKS_HPP
#define KLIB_NXP_LPC178X_CLOCKS_HPP

#include <klib/klib.hpp>

namespace klib::core::lpc178x::io {
    class clocks {
    public:
        /**
         * @brief Available clock dividers
         */
        enum class divider {
            off = 0x0,
            div_1 = 0x1,
            div_2 = 0x2,
            div_3 = 0x3,
            div_4 = 0x4,
        };

        /**
         * @brief Set the global peripheral clock selection
         *
         */
        template <divider Divider = divider::div_1>
        static void set() {
            // enable the peripheral
            SYSCON->PCLKSEL = static_cast<uint32_t>(Divider);
        }

        /**
         * @brief Get the global peripheral clock selection
         *
         * @tparam P
         * @return true
         * @return false
         */
        static divider status() {
            // return the value of the selection
            return static_cast<divider>(SYSCON->PCLKSEL & 0x1f);
        }
    };
}

#endif
