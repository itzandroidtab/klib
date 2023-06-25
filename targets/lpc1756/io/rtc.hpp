#ifndef KLIB_LPC1756_RTC_HPP
#define KLIB_LPC1756_RTC_HPP

#include <cstdint>

#include <lpc1756.hpp>
#include <klib/units.hpp>

#include "power.hpp"

namespace klib::lpc1756::io::periph {
    struct rtc0 {
        // peripheral id (e.g rtc0, rtc1)
        constexpr static uint32_t id = 0;

        // peripheral clock bit position
        constexpr static uint32_t clock_id = 9;

        // interrupt id (including the arm vector table)
        constexpr static uint32_t interrupt_id = 33;

        // port to the RTC hardware
        static inline RTC_Type *const port = RTC;
    };
}

namespace klib::lpc1756::io {
    template <typename Rtc>
    class rtc {
    protected:
        // amount of days in a month without the leapyears
        constexpr static uint8_t month_days[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

        /**
         * @brief Convert seperate date fields to the epoch time
         * 
         * @param year 1970 - 4095
         * @param month 1 - 12
         * @param day 1 - (28, 29, 30, 31)
         * @param hours 0 - 23
         * @param minutes 0 - 59
         * @param seconds 0 - 59
         * @return klib::time::s 
         */
        static klib::time::s datetime_to_epoch(const uint16_t year, const uint8_t month, const uint8_t day, 
            const uint8_t hours, const uint8_t minutes, const uint8_t seconds) 
        {
            uint32_t days = 0;

            // amount of days in a year (non leap years)
            constexpr auto days_year = (
                month_days[0] + month_days[1] + month_days[2] + month_days[3] +
                month_days[4] + month_days[5] + month_days[6] + month_days[7] +
                month_days[8] + month_days[9] + month_days[10] + month_days[11]
            );

            // calculate the amount of days till the current year
            for (uint16_t y = 1970; y < year; y++) {
                // add all the months
                days += days_year;

                // check if the current year is a leap year (we do the same 
                // thing as the hardware does by checking only the lowest 2 
                // bits. This will work fine between 1901 - 2099 but will 
                // fail at year 2100)
                if ((y & 0b11) == 0) {
                    // add a day for a leap year
                    days++;
                }
            }

            // add the amount of days till the current month
            for (uint8_t m = 0; m < (month - 1); m++) {
                // add the current month
                days += month_days[m];
            }

            // add the days but remove 1 for the current day
            days += (static_cast<int8_t>(day) - 1);

            // convert the days with the hours, minutes and seconds to the epoch time
            return {(days * (60 * 60 * 24)) + (hours * (60 * 60)) + (minutes * 60) + seconds};
        }

    public:
        template <uint16_t CalValue = 0, bool CalDirection = 1>
        static void init() {
            // enable the power for the rtc
            power_control::enable<Rtc>();

            // check if the rtc is already on
            if (Rtc::port->CCR & 0x1) {
                // rtc already on. No need for more configuration
                return;
            }

            // setup the calibration if we have any
            Rtc::port->CALIBRATION = CalValue | (CalDirection << 17);

            // enable the rtc
            Rtc::port->CCR = 0x1 | ((CalValue == 0) ? (0x1 << 4) : 0x00);
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
            return datetime_to_epoch(
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
            // TODO: implement this
        }
    };
}

#endif