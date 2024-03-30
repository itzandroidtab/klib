#ifndef KLIB_LPC1756_DEV_USB_PINS_HPP
#define KLIB_LPC1756_DEV_USB_PINS_HPP

#include <io/pins.hpp>

namespace klib::lpc1756::pins::board::dev_usb {
    // using the lqfp_80 pins.
    using p0_0 = package::lqfp_80::p37;
    using p0_1 = package::lqfp_80::p38;
    using p0_2 = package::lqfp_80::p79;
    using p0_3 = package::lqfp_80::p80;
    using p0_6 = package::lqfp_80::p64;
    using p0_7 = package::lqfp_80::p63;
    using p0_8 = package::lqfp_80::p62;
    using p0_9 = package::lqfp_80::p61;
    using p0_10 = package::lqfp_80::p39;
    using p0_11 = package::lqfp_80::p40;
    using p0_15 = package::lqfp_80::p47;
    using p0_16 = package::lqfp_80::p48;
    using p0_17 = package::lqfp_80::p46;
    using p0_18 = package::lqfp_80::p45;
    using p0_22 = package::lqfp_80::p44;
    using p0_25 = package::lqfp_80::p7;
    using p0_26 = package::lqfp_80::p6;
    using p0_29 = package::lqfp_80::p22;
    using p0_30 = package::lqfp_80::p23;

    using p1_0 = package::lqfp_80::p76;
    using p1_1 = package::lqfp_80::p75;
    using p1_4 = package::lqfp_80::p74;
    using p1_8 = package::lqfp_80::p73;
    using p1_9 = package::lqfp_80::p72;
    using p1_10 = package::lqfp_80::p71;
    using p1_14 = package::lqfp_80::p70;
    using p1_15 = package::lqfp_80::p69;
    using p1_18 = package::lqfp_80::p25;
    using p1_19 = package::lqfp_80::p26;
    using p1_20 = package::lqfp_80::p27;
    using p1_22 = package::lqfp_80::p28;
    using p1_23 = package::lqfp_80::p29;
    using p1_24 = package::lqfp_80::p30;
    using p1_25 = package::lqfp_80::p31;
    using p1_26 = package::lqfp_80::p32;
    using p1_28 = package::lqfp_80::p35;
    using p1_29 = package::lqfp_80::p36;
    using p1_30 = package::lqfp_80::p18;
    using p1_31 = package::lqfp_80::p17;

    using p2_0 = package::lqfp_80::p60;
    using p2_1 = package::lqfp_80::p59;
    using p2_2 = package::lqfp_80::p58;
    using p2_3 = package::lqfp_80::p55;
    using p2_4 = package::lqfp_80::p54;
    using p2_5 = package::lqfp_80::p53;
    using p2_6 = package::lqfp_80::p52;
    using p2_7 = package::lqfp_80::p51;
    using p2_8 = package::lqfp_80::p50;
    using p2_9 = package::lqfp_80::p49;
    using p2_10 = package::lqfp_80::p41;

    using p4_28 = package::lqfp_80::p65;
    using p4_29 = package::lqfp_80::p68;
}

namespace klib::lpc1756::io::periph {
    namespace lqfp_80 {};

    // move target package peripherals into current namespace
    using namespace lqfp_80;
}

#endif