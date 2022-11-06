#ifndef KLIB_MAX32660_PINS_HPP
#define KLIB_MAX32660_PINS_HPP

#include <cstdint>

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

namespace klib::max32660::io::detail {
    // placeholder peripheral for gpio. Actual
    // peripheral with the data for the gpio
    // is in periph.hpp
    struct gpio0 {};
}

namespace klib::max32660::io::pins::package::wlp {
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
        using port = io::detail::gpio0;

        // bit number in port
        constexpr static uint32_t number = 9;
    };

    struct pb4 {
        // P0.8
        using port = io::detail::gpio0;

        // bit number in port
        constexpr static uint32_t number = 8;
    };

    struct pc1 {
        // P0.0
        using port = io::detail::gpio0;

        // bit number in port
        constexpr static uint32_t number = 0;
    };

    struct pc2 {
        // P0.1
        using port = io::detail::gpio0;

        // bit number in port
        constexpr static uint32_t number = 1;
    };

    struct pc3 {
        // P0.6
        using port = io::detail::gpio0;

        // bit number in port
        constexpr static uint32_t number = 6;
    };

    struct pc4 {
        // P0.7
        using port = io::detail::gpio0;
        
        // bit number in port
        constexpr static uint32_t number = 7;
    };

    struct pd1 {
        // P0.2
        using port = io::detail::gpio0;

        // bit number in port
        constexpr static uint32_t number = 2;
    };

    struct pd2 {
        // P0.3
        using port = io::detail::gpio0;

        // bit number in port
        constexpr static uint32_t number = 3;
    };

    struct pd3 {
        // P0.4
        using port = io::detail::gpio0;

        // bit number in port
        constexpr static uint32_t number = 4;
    }; 

    struct pd4 {
        // P0.5
        using port = io::detail::gpio0;

        // bit number in port
        constexpr static uint32_t number = 5;
    };
}

namespace klib::max32660::io::pins::package::tqfn_20 {
    struct p1 {
        // P0.1
        using port = io::detail::gpio0;

        // bit number in port
        constexpr static uint32_t number = 1;
    };

    struct p2 {
        // P0.0
        using port = io::detail::gpio0;

        // bit number in port
        constexpr static uint32_t number = 0;
    };

    struct p3 {
        // P0.13
        using port = io::detail::gpio0;

        // bit number in port
        constexpr static uint32_t number = 13;  
    };

    struct p4 {
        // P0.12
        using port = io::detail::gpio0;

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
        using port = io::detail::gpio0;

        // bit number in port
        constexpr static uint32_t number = 9;
    };

    struct p12 {
        // P0.8
        using port = io::detail::gpio0;

        // bit number in port
        constexpr static uint32_t number = 8;
    };

    struct p13 {
        // P0.11
        using port = io::detail::gpio0;

        // bit number in port
        constexpr static uint32_t number = 11;
    };

    struct p14 {
        // P0.10
        using port = io::detail::gpio0;

        // bit number in port
        constexpr static uint32_t number = 10;
    };

    struct p15 {
        // P0.7
        using port = io::detail::gpio0;

        // bit number in port
        constexpr static uint32_t number = 7;
    }; 

    struct p16 {
        // P0.6
        using port = io::detail::gpio0;

        // bit number in port
        constexpr static uint32_t number = 6;
    };

    struct p17 {
        // P0.5
        using port = io::detail::gpio0;

        // bit number in port
        constexpr static uint32_t number = 5;
    };

    struct p18 {
        // P0.4
        using port = io::detail::gpio0;

        // bit number in port
        constexpr static uint32_t number = 4;
    };

    struct p19 {
        // P0.3
        using port = io::detail::gpio0;

        // bit number in port
        constexpr static uint32_t number = 3;
    };

    struct p20 {
        // P0.2
        using port = io::detail::gpio0;

        // bit number in port
        constexpr static uint32_t number = 2;
    };
}

namespace klib::max32660::io::pins::package::tqfn_24 {
    struct p1 {
        // P0.1
        using port = io::detail::gpio0;

        // bit number in port
        constexpr static uint32_t number = 1;
    };

    struct p2 {
        // P0.0
        using port = io::detail::gpio0;

        // bit number in port
        constexpr static uint32_t number = 0;
    };

    struct p3 {
        // NC
    };

    struct p4 {
        // P0.13
        using port = io::detail::gpio0;

        // bit number in port
        constexpr static uint32_t number = 13;
    };

    struct p5 {
        // P0.12
        using port = io::detail::gpio0;

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
        using port = io::detail::gpio0;

        // bit number in port
        constexpr static uint32_t number = 9;
    };

    struct p14 {
        // P0.8
        using port = io::detail::gpio0;

        // bit number in port
        constexpr static uint32_t number = 8;
    };

    struct p15 {
        // NC
    }; 

    struct p16 {
        // P0.11
        using port = io::detail::gpio0;

        // bit number in port
        constexpr static uint32_t number = 11;
    };

    struct p17 {
        // P0.10
        using port = io::detail::gpio0;

        // bit number in port
        constexpr static uint32_t number = 10;
    };

    struct p18 {
        // P0.7
        using port = io::detail::gpio0;

        // bit number in port
        constexpr static uint32_t number = 7;
    };

    struct p19 {
        // P0.6
        using port = io::detail::gpio0;

        // bit number in port
        constexpr static uint32_t number = 6;
    };

    struct p20 {
        // P0.5
        using port = io::detail::gpio0;

        // bit number in port
        constexpr static uint32_t number = 5;
    };

    struct p21 {
        // P0.4
        using port = io::detail::gpio0;

        // bit number in port
        constexpr static uint32_t number = 4;
    };

    struct p22 {
        // VDD
    };

    struct p23 {
        // P0.3
        using port = io::detail::gpio0;

        // bit number in port
        constexpr static uint32_t number = 3;
    };

    struct p24 {
        // P0.2
        using port = io::detail::gpio0;

        // bit number in port
        constexpr static uint32_t number = 2;
    };
}

#endif