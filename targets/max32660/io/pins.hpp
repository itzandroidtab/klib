#ifndef KLIB_MAX32660_PINS_HPP
#define KLIB_MAX32660_PINS_HPP

#include <type_traits>

#include "max32660.h"
#include "pio.hpp"

namespace klib::max32660::io::pins::detail {
    // get the pin mask of a pin number
    template<typename Pin>
    constexpr uint32_t mask = 1U << Pin::number;

    // default type when using the port
    template<typename Pin>
    const GPIO0_Type *port = nullptr;

    // port when using the pio0
    template<>
    GPIO0_Type *const port<io::detail::pio0> = GPIO0;
}

namespace klib::max32660::io {
    /**
     * @brief Set the peripheral of a pin
     * 
     * @tparam Pin 
     * @tparam Periph 
     */
    template<typename Pin, typename Periph>    
    static void set_peripheral() {
        // set the 3 function registers
        if constexpr (std::is_same_v<Periph, detail::periph_func_1>) {
            // setup alternate function 1
            pins::detail::port<typename Pin::port>->EN2_CLR = pins::detail::mask<Pin>;
            pins::detail::port<typename Pin::port>->EN1_CLR = pins::detail::mask<Pin>;
            pins::detail::port<typename Pin::port>->EN_CLR = pins::detail::mask<Pin>;
        }
        else if (std::is_same_v<Periph, detail::periph_func_2>) {
            // setup alternate function 2
            pins::detail::port<typename Pin::port>->EN2_CLR = pins::detail::mask<Pin>;
            pins::detail::port<typename Pin::port>->EN1_SET = pins::detail::mask<Pin>;
            pins::detail::port<typename Pin::port>->EN_CLR = pins::detail::mask<Pin>;
        }
        else if (std::is_same_v<Periph, detail::periph_func_3>) {
            // setup alternate function 3
            pins::detail::port<typename Pin::port>->EN2_CLR = pins::detail::mask<Pin>;
            pins::detail::port<typename Pin::port>->EN1_SET = pins::detail::mask<Pin>;
            pins::detail::port<typename Pin::port>->EN_SET = pins::detail::mask<Pin>;
        }
        else {
            // setup normal gpio function
            pins::detail::port<typename Pin::port>->EN2_CLR = pins::detail::mask<Pin>;
            pins::detail::port<typename Pin::port>->EN1_CLR = pins::detail::mask<Pin>;
            pins::detail::port<typename Pin::port>->EN_SET = pins::detail::mask<Pin>;
        }
    }
}

namespace klib::max32660::io::pins::wlp {
    struct pa1 {
        // 32KOUT
    };

    struct pa2 {
        // 32KIN
    };

    struct pa3 {
        // VDD
    };

    struct pa4 {
        // VCORE
    };

    struct pb1 {
        // RSTN
    };

    struct pb2 {
        // VSS
    };

    struct pb3 {
        // P0.9
        using port = io::detail::pio0;

        // bit number in port
        constexpr static uint32_t number = 9;
    };

    struct pb4 {
        // P0.8
        using port = io::detail::pio0;

        // bit number in port
        constexpr static uint32_t number = 8;
    };

    struct pc1 {
        // P0.0
        using port = io::detail::pio0;

        // bit number in port
        constexpr static uint32_t number = 0;
    };

    struct pc2 {
        // P0.1
        using port = io::detail::pio0;

        // bit number in port
        constexpr static uint32_t number = 1;
    };

    struct pc3 {
        // P0.6
        using port = io::detail::pio0;

        // bit number in port
        constexpr static uint32_t number = 6;
    };

    struct pc4 {
        // P0.7
        using port = io::detail::pio0;
        
        // bit number in port
        constexpr static uint32_t number = 7;
    };

    struct pd1 {
        // P0.2
        using port = io::detail::pio0;

        // bit number in port
        constexpr static uint32_t number = 2;
    };

    struct pd2 {
        // P0.3
        using port = io::detail::pio0;

        // bit number in port
        constexpr static uint32_t number = 3;
    };

    struct pd3 {
        // P0.4
        using port = io::detail::pio0;

        // bit number in port
        constexpr static uint32_t number = 4;
    }; 

    struct pd4 {
        // P0.5
        using port = io::detail::pio0;

        // bit number in port
        constexpr static uint32_t number = 5;
    };
}

namespace klib::max32660::io::pins::tqfn_20 {
    struct p1 {
        // P0.1
        using port = io::detail::pio0;

        // bit number in port
        constexpr static uint32_t number = 1;
    };

    struct p2 {
        // P0.0
        using port = io::detail::pio0;

        // bit number in port
        constexpr static uint32_t number = 0;
    };

    struct p3 {
        // P0.13
        using port = io::detail::pio0;

        // bit number in port
        constexpr static uint32_t number = 13;  
    };

    struct p4 {
        // P0.12
        using port = io::detail::pio0;

        // bit number in port
        constexpr static uint32_t number = 12;
    };

    struct p5 {
        // RSTN
    };

    struct p6 {
        // VSS
    };

    struct p7 {
        // 32KOUT
    };

    struct p8 {
        // 32KIN
    };

    struct p9 {
        // VDD
    };

    struct p10 {
        // VCORE
    };

    struct p11 {
        // P0.9
        using port = io::detail::pio0;

        // bit number in port
        constexpr static uint32_t number = 9;
    };

    struct p12 {
        // P0.8
        using port = io::detail::pio0;

        // bit number in port
        constexpr static uint32_t number = 8;
    };

    struct p13 {
        // P0.11
        using port = io::detail::pio0;

        // bit number in port
        constexpr static uint32_t number = 11;
    };

    struct p14 {
        // P0.10
        using port = io::detail::pio0;

        // bit number in port
        constexpr static uint32_t number = 10;
    };

    struct p15 {
        // P0.7
        using port = io::detail::pio0;

        // bit number in port
        constexpr static uint32_t number = 7;
    }; 

    struct p16 {
        // P0.6
        using port = io::detail::pio0;

        // bit number in port
        constexpr static uint32_t number = 6;
    };

    struct p17 {
        // P0.5
        using port = io::detail::pio0;

        // bit number in port
        constexpr static uint32_t number = 5;
    };

    struct p18 {
        // P0.4
        using port = io::detail::pio0;

        // bit number in port
        constexpr static uint32_t number = 4;
    };

    struct p19 {
        // P0.3
        using port = io::detail::pio0;

        // bit number in port
        constexpr static uint32_t number = 3;
    };

    struct p20 {
        // P0.2
        using port = io::detail::pio0;

        // bit number in port
        constexpr static uint32_t number = 2;
    };
}

namespace klib::max32660::io::pins::tqfn_24 {
    struct p1 {
        // P0.1
        using port = io::detail::pio0;

        // bit number in port
        constexpr static uint32_t number = 1;
    };

    struct p2 {
        // P0.0
        using port = io::detail::pio0;

        // bit number in port
        constexpr static uint32_t number = 0;
    };

    struct p3 {
        // NC
    };

    struct p4 {
        // P0.13
        using port = io::detail::pio0;

        // bit number in port
        constexpr static uint32_t number = 13;
    };

    struct p5 {
        // P0.12
        using port = io::detail::pio0;

        // bit number in port
        constexpr static uint32_t number = 12;
    };

    struct p6 {
        // RSTN
    };

    struct p7 {
        // VSS
    };

    struct p8 {
        // 32KOUT
    };

    struct p9 {
        // 32KIN
    };

    struct p10 {
        // VSS
    };

    struct p11 {
        // VDD
    };

    struct p12 {
        // VCORE
    };

    struct p13 {
        // P0.9
        using port = io::detail::pio0;

        // bit number in port
        constexpr static uint32_t number = 9;
    };

    struct p14 {
        // P0.8
        using port = io::detail::pio0;

        // bit number in port
        constexpr static uint32_t number = 8;
    };

    struct p15 {
        // NC
    }; 

    struct p16 {
        // P0.11
        using port = io::detail::pio0;

        // bit number in port
        constexpr static uint32_t number = 11;
    };

    struct p17 {
        // P0.10
        using port = io::detail::pio0;

        // bit number in port
        constexpr static uint32_t number = 10;
    };

    struct p18 {
        // P0.7
        using port = io::detail::pio0;

        // bit number in port
        constexpr static uint32_t number = 7;
    };

    struct p19 {
        // P0.6
        using port = io::detail::pio0;

        // bit number in port
        constexpr static uint32_t number = 6;
    };

    struct p20 {
        // P0.5
        using port = io::detail::pio0;

        // bit number in port
        constexpr static uint32_t number = 5;
    };

    struct p21{
        // P0.4
        using port = io::detail::pio0;

        // bit number in port
        constexpr static uint32_t number = 4;
    };

    struct p22 {
        // VDD
    };

    struct p23 {
        // P0.3
        using port = io::detail::pio0;

        // bit number in port
        constexpr static uint32_t number = 3;
    };

    struct p24 {
        // P0.2
        using port = io::detail::pio0;

        // bit number in port
        constexpr static uint32_t number = 2;
    };
}

#endif