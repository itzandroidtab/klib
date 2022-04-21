#ifndef KLIB_MAX32660_PIO_HPP
#define KLIB_MAX32660_PIO_HPP

#include <cstdint>

namespace klib::max32660::io::detail {
    // alternate ports for all the gpio
    struct pio0 {};

    // alternate functions for all the gpio
    // default function
    struct periph_func_none {};

    // alternate function 1
    struct periph_func_1 {};

    // alternate function 2
    struct periph_func_2 {};

    // alternate function 3
    struct periph_func_3 {};

    namespace swd {
        enum class mode {
            dio,
            clk
        };

        template <typename Pin, mode Type, typename Periph>
        struct swd {
            // pin of the peripheral
            using pin = Pin;

            // type of the pin
            constexpr static mode type = Type;

            // alternate function
            using periph = Periph;
        };
    }

    namespace uart {
        enum class mode {
            txd,
            rxd,
            cts,
            rts
        };

        template <typename Pin, mode Type, typename Periph>
        struct uart {
            // pin of the peripheral
            using pin = Pin;

            // type of the pin
            constexpr static mode type = Type;

            // alternate function
            using periph = Periph;
        };
    }    

    namespace spi {
        enum class mode {
            miso,
            mosi,
            sck,
            cs0
        };

        template <typename Pin, mode Type, typename Periph>
        struct spi {
            // pin of the peripheral
            using pin = Pin;

            // type of the pin
            constexpr static mode type = Type;

            // alternate function
            using periph = Periph;
        };
    }

    namespace i2c {
        enum class mode {
            sda,
            scl
        };

        template <typename Pin, mode Type, typename Periph>
        struct i2c {
            // pin of the peripheral
            using pin = Pin;

            // type of the pin
            constexpr static mode type = Type;

            // alternate function
            using periph = Periph;
        };
    }

    namespace tc {
        enum class mode {
            tmr
        };

        template <typename Pin, mode Type, typename Periph>
        struct tc {
            // pin of the peripheral
            using pin = Pin;

            // type of the pin
            constexpr static mode type = Type;

            // alternate function
            using periph = Periph;
        };
    }
}

#endif