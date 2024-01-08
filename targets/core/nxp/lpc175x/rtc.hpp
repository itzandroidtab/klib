#ifndef KLIB_NXP_LPC175X_RTC_HPP
#define KLIB_NXP_LPC175X_RTC_HPP

#include <cstdint>

#include <klib/klib.hpp>
#include <klib/units.hpp>
#include <klib/io/rtc.hpp>

#include <io/power.hpp>

namespace klib::core::lpc175x::io {
    template <typename Rtc>
    class rtc {
    public:
        /**
         * @brief Initialize the RTC
         * 
         * @param cal_value 
         * @param cal_direction
         */
        static void init(const uint16_t cal_value = 0, const bool cal_direction = 1) {
            // enable the power for the rtc
            target::io::power_control::enable<Rtc>();

            // check if the rtc is already on (and not in reset)
            if (Rtc::port->CCR & 0x1 && !(Rtc::port->CCR & (0x1 << 1))) {
                // rtc already on. No need for more configuration
                return;
            }

            // disable the oscillator fail detect interrupt
            Rtc::port->RTC_AUXEN = 0x00;

            // clear the fail detect flag
            Rtc::port->RTC_AUX = 0x1 << 4;

            // disable all the counter increment interrupts
            Rtc::port->CIIR = 0xff;

            // disable the alarm
            Rtc::port->AMR = 0xff;

            // setup the calibration if we have any
            Rtc::port->CALIBRATION = cal_value | (cal_direction << 17);

            // enable the rtc
            Rtc::port->CCR = 0x1 | ((cal_value == 0) ? (0x1 << 4) : 0x00);
        }

        /**
         * @brief Returns if the current rtc time is valid.
         * 
         * @return true 
         * @return false 
         */
        static bool is_valid() {
            return Rtc::port->YEAR < 1970;
        }

        /**
         * @brief Get the current epoch time (currently we are using a 32 bit 
         * values for the second counter. As we do not care about negative 
         * values we should not have the 2038 issue. This does cause a issue if
         * we want to have epoch values before 1970)
         * 
         * @return klib::time::s 
         */
        static klib::time::s get() {
            // convert the current date and time to epoch seconds
            return klib::io::rtc::datetime_to_epoch(
                Rtc::port->YEAR, Rtc::port->MONTH, 
                Rtc::port->DOM, Rtc::port->HRS, 
                Rtc::port->MIN, Rtc::port->SEC
            );
        }

        /**
         * @brief Set the current epoch time
         * 
         * @param time 
         */
        static void set(const klib::time::s time) {
            // get the amount of days in the epoch time
            uint32_t days = (time.value / (24 * 60 * 60));
            uint32_t years = 1970;

            // 01-01-1970 was a thursday (3). Add that to our epoch
            // days to calculate the current day of the week
            Rtc::port->DOW = (days + 3) % 7;
            
            // add one to the days
            days += 1;

            // check all the years 
            while (true) {
                // check if the current year is a leap year
                const uint32_t y = (
                    ((years & 0b11) == 0) ? 
                    klib::io::rtc::days_year + 1: klib::io::rtc::days_year
                );

                // check if the amount of days in the year
                // fits in the amount of days we have left
                if (days < y) {
                    break;
                }

                // increment the years
                years++;
                days -= y;
            }

            // set the year
            Rtc::port->YEAR = years;
            Rtc::port->DOY = days;

            uint32_t months = 1;

            // get the amount of months
            for (uint32_t i = 0; i < (sizeof(klib::io::rtc::month_days) / sizeof(klib::io::rtc::month_days[0])); i++) {
                // check if we have more days than the 
                // current month has
                if (days < klib::io::rtc::month_days[i]) {
                    break;
                }

                // increment the months
                months++;
                days -= klib::io::rtc::month_days[i];
            }

            // set the month and the days
            Rtc::port->MONTH = months;
            Rtc::port->DOM = days;

            // get the seconds left over
            const uint32_t seconds = time.value % (24 * 60 * 60);

            // set the hours, minutes and seconds
            Rtc::port->HRS = seconds / (60 * 60);
            Rtc::port->MIN = (seconds % (60 * 60)) / 60;
            Rtc::port->SEC = seconds % 60;
        }
    };
}

#endif