#ifndef KLIB_RTC_HPP
#define KLIB_RTC_HPP

#include <cstdint>

#include <klib/units.hpp>

namespace klib::io::rtc {
    // amount of days in a month without the leapyears
    constexpr static uint8_t month_days[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

    // amount of days in a year (non leap years)
    constexpr static auto days_year = (
        month_days[0] + month_days[1] + month_days[2] + month_days[3] +
        month_days[4] + month_days[5] + month_days[6] + month_days[7] +
        month_days[8] + month_days[9] + month_days[10] + month_days[11]
    );

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
}

#endif