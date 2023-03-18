#ifndef KLIB_ATSAM3X8E_RSTC_HPP
#define KLIB_ATSAM3X8E_RSTC_HPP

#include "atsam3x8e.hpp"

namespace klib::atsam3x8e {
    // class rstc {
    //     private:
    //         constexpr static uint32_t key = (RSTC_CR_KEY_PASSWD << 24);
    //     public:
    //         enum class type {
    //             cpu = RSTC_CR_PROCRST_Msk,
    //             peripheral = RSTC_CR_PERRST_Msk,
    //             external = RSTC_CR_EXTRST_Msk
    //         };

    //         enum class source {
    //             general,
    //             backup,
    //             watchdog,
    //             software,
    //             user
    //         };

    //         template <type Rst>
    //         static void reset() {
    //             // reset based on the reset type
    //             RSTC->RSTC_CR = key | static_cast<uint32_t>(Rst);
    //         }

    //         template <source Src, bool Enabled>
    //         static void set_source() {
    //             // make sure the user doesnt enter a invalid source
    //             static_assert(Src == source::user, "ATSAM3X8E only supports source::user as source parameter");

    //             // write the data into the register
    //             RSTC->RSTC_MR = (RSTC->RSTC_MR & ~RSTC_MR_URSTEN_Msk) | Enabled | key;
    //         }

    //         template <uint8_t ExtRstLen>
    //         static void set_ext_rst_length(){
    //             // sets the external reset length (nrst). time is 2^(erstl + 1)
    //             // configurable between 60us and 2 sec
    //             RSTC->RSTC_MR = (RSTC->RSTC_MR & ~RSTC_MR_ERSTL_Msk) | (ExtRstLen & 0x0F) << 8 | key;
    //         }

    //         static source get_rst_cause() {
    //             return static_cast<source>((RSTC->RSTC_SR & RSTC_SR_RSTTYP_Msk) >> RSTC_SR_RSTTYP_Pos);
    //         } 
    // };
}

#endif