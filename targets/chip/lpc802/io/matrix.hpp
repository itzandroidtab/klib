#ifndef KLIB_LPC802_MATRIX_HPP
#define KLIB_LPC802_MATRIX_HPP

#include <cstdint>
#include <cstddef>
#include <type_traits>

#include <lpc802.hpp>

#include "clocks.hpp"

namespace klib::lpc802::io::periph {
    struct matrix0 {
        // peripheral id (e.g matrix0, matrix1)
        constexpr static uint32_t id = 0;

        // peripheral clock bit position
        constexpr static uint32_t clock_id = 7;

        // port to the matrix peripheral.
        static inline SWM0_Type *const port = SWM0;
    };
}

namespace klib::lpc802::io {
    template <typename Matrix>
    class matrix {
    public:
        /**
         * @brief Pin functions in the flex matrix
         *
         */
        enum class flex_matrix {
            // first register
            uart0_tx = 0,
            uart0_rx = 1,
            uart0_rts = 2,
            uart0_cts = 3,

            // second register
            uart0_sclk = 4,
            uart1_tx = 5,
            uart1_rx = 6,
            uart1_sclk = 7,

            // third register
            spi0_sck = 8,
            spi0_mosi = 9,
            spi0_miso = 10,
            spi0_ssel0 = 11,

            // forth register
            spi0_ssel1 = 12,
            timer0_capture0 = 13,
            timer0_capture1 = 14,
            timer0_capture2 = 15,

            // fifth register
            timer0_mat0 = 16,
            timer0_mat1 = 17,
            timer0_mat2 = 18,
            timer0_mat3 = 19,

            // sixth register
            i2c0_sda = 20,
            i2c0_scl = 21,
            acmp = 22,
            clkout = 23,

            // seventh register
            gpio_int_bmat = 24,
            levelshift_in0 = 25,
            levelshift_in1 = 26,
            levelshift_out0 = 27,

            // eight register
            levelshift_out1 = 28
        };

        /**
         * @brief Pin functions in the fixed matrix
         *
         */
        enum class fixed_matrix {
            acmp_i1 = 0,
            acmp_i2 = 1,
            acmp_i3 = 2,
            acmp_i4 = 3,
            swclk = 4,
            swdio = 5,
            resetn = 6,
            clkin = 7,
            wclkin = 8,
            vddcmp = 9,
            adc0 = 10,
            adc1 = 11,
            adc2 = 12,
            adc3 = 13,
            adc4 = 14,
            adc5 = 15,
            adc6 = 16,
            adc7 = 17,
            adc8 = 18,
            adc9 = 19,
            adc10 = 20,
            adc11 = 21
        };

    protected:
        /**
         * @brief Get the register offset for the pinassignment register used for the flex option
         *
         * @tparam Flex
         * @return constexpr uint32_t
         */
        template <flex_matrix Flex>
        constexpr static uint32_t get_register_offset() {
            // calculate the register index
            constexpr auto v = static_cast<uint32_t>(Flex) / 4;

            // check if the flex option is valid
            static_assert(v < 8, "Invalid pin flex option");

            // check what pointer to return
            switch (v) {
                case 1:
                    return offsetof(SWM0_Type, PINASSIGN1_PINASSIGN_PINASSIGN_DATA) / 4;
                case 2:
                    return offsetof(SWM0_Type, PINASSIGN2_PINASSIGN_PINASSIGN_DATA) / 4;
                case 3:
                    return offsetof(SWM0_Type, PINASSIGN3_PINASSIGN_PINASSIGN_DATA) / 4;
                case 4:
                    return offsetof(SWM0_Type, PINASSIGN4_PINASSIGN_PINASSIGN_DATA) / 4;
                case 5:
                    return offsetof(SWM0_Type, PINASSIGN5_PINASSIGN_PINASSIGN_DATA) / 4;
                case 6:
                    return offsetof(SWM0_Type, PINASSIGN6_PINASSIGN_PINASSIGN_DATA) / 4;
                case 7:
                    return offsetof(SWM0_Type, PINASSIGN7_PINASSIGN_PINASSIGN_DATA) / 4;
                default:
                case 0:
                    return offsetof(SWM0_Type, PINASSIGN0_PINASSIGN_PINASSIGN_DATA) / 4;
            }
        }

        /**
         * @brief Validate if a flex offset and index are supported for a specific pin
         *
         * @tparam Pin
         * @tparam Offset
         * @tparam index
         * @return true
         * @return false
         */
        template <typename Pin, uint32_t Offset, uint32_t index>
        constexpr static bool validate_flex_parameter() {
            // check if the supplied pin is allowed for the offset and index
            switch (Offset) {
                case 0:
                    return (
                        ((Pin::number >= 0 && Pin::number <= 5) ||
                        (Pin::number >= 7 && Pin::number <= 17)) &&
                        Pin::port::id == 0
                    );
                case 1:
                    return (
                        ((Pin::number >= 0 && Pin::number <= 5) ||
                        (Pin::number >= 7 && Pin::number <= 17)) &&
                        Pin::port::id == 0
                    );
                case 2:
                    return (
                        (Pin::number == 5 ||
                        (Pin::number >= 7 && Pin::number <= 17)) &&
                        Pin::port::id == 0
                    );
                case 3:
                    return (
                        (Pin::number == 5 ||
                        (Pin::number >= 7 && Pin::number <= 17)) &&
                        Pin::port::id == 0
                    );
                case 4:
                    return (
                        (Pin::number == 5 ||
                        (Pin::number >= 7 && Pin::number <= 17)) &&
                        Pin::port::id == 0
                    );
                case 5:
                    return (
                        (Pin::number == 5 ||
                        (Pin::number >= 7 && Pin::number <= 17)) &&
                        Pin::port::id == 0
                    );
                case 6:
                    if constexpr (index == 0) {
                        return (
                            (Pin::number == 5 ||
                            (Pin::number >= 7 && Pin::number <= 17)) &&
                            Pin::port::id == 0
                        );
                    }
                    else {
                        return (
                            ((Pin::number >= 0 && Pin::number <= 5) ||
                            (Pin::number >= 7 && Pin::number <= 30)) &&
                            Pin::port::id == 0
                        );
                    }
                case 7:
                    return (
                        ((Pin::number >= 0 && Pin::number <= 5) ||
                        (Pin::number >= 7 && Pin::number <= 30)) &&
                        Pin::port::id == 0
                    );
                default:
                    // unknown offset. This should never happen but
                    // return we have a invalid flex output if it does
                    return false;
            }
        }

    public:
        /**
         * @brief Setup a pin using the flexable pin matrix
         *
         * @tparam Pin
         * @param flex
         */
        template <typename Pin, flex_matrix Flex>
        static void setup() {
            // enable the matrix perihperal clock
            clocks::enable<Matrix>();

            // get the register offset for the flex option
            constexpr uint32_t offset = get_register_offset<Flex>();

            // get the pointer to the pin select we need to write to
            volatile uint32_t *const matrix_register = &(
                (reinterpret_cast<volatile uint32_t *const>(Matrix::port))[offset]
            );

            // get the index for the option
            constexpr uint32_t index = static_cast<uint32_t>(Flex) % 4;

            // check if the pin is a valid option for the current flex option
            static_assert(
                validate_flex_parameter<Pin, offset, index>(),
                "Flex function is not supported on pin"
            );

            // clear the previous value and set our new value
            (*matrix_register) = ((*matrix_register) & ~(0xff << (index * 8))) | (Pin::number << (index * 8));

            // disable the matrix perihperal clock
            clocks::disable<Matrix>();
        }

        /**
         * @brief Setup a pin using the fixed pin matrix
         *
         * @param flex
         */
        template <fixed_matrix Fixed>
        static void setup() {
            // enable the matrix perihperal clock
            clocks::enable<Matrix>();

            // get the integer value of the selection
            Matrix::port->PINENABLE0 |= static_cast<uint32_t>(Fixed);

            // disable the matrix perihperal clock
            clocks::disable<Matrix>();
        }

        /**
         * @brief Clear all the alternate functions for the provided pin
         *
         * @tparam Pin
         */
        template <typename Pin>
        static void clear() {
            // enable the matrix perihperal clock
            clocks::enable<Matrix>();

            // get a pointer to the matrix
            volatile uint8_t *const matrix_array = (
                reinterpret_cast<volatile uint8_t *const>(Matrix::port)
            );

            // search in the array for the pin
            for (uint32_t i = 0; i < (8 * sizeof(uint32_t)); i++) {
                // check if the current item matches the array value
                if (Pin::number == matrix_array[i]) {
                    // clear the pin from the matrix
                    matrix_array[i] = 0xff;
                }
            }

            // disable the matrix perihperal clock
            clocks::disable<Matrix>();
        }
    };
}

#endif