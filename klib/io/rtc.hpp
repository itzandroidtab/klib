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
     * @brief Date time structure
     * 
     */
    struct datetime {
        // year, minimum is 1970 (as that is the epoch time)
        uint16_t year;
        
        // month, range 1 - 12
        uint8_t month;

        // day of the month. range 1 - (28, 29, 30, 31)
        uint8_t day;

        // hour, range 0 - 23
        uint8_t hours;

        // minutes, range 0 - 59
        uint8_t minutes;

        // seconds, range 0 - 59
        uint8_t seconds;

        // generate a default == operator
        bool operator==(datetime const&) const = default;
    };

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

        // check if the current year is a leap year and we are in/passed 
        // february (feb = 1, we used the same leap year check as above)
        if ((year & 0b11) == 0 && (month > 2)) {
            // add a day for a leap year
            days++;
        }

        // add the days but remove 1 for the current day
        days += (static_cast<int8_t>(day) - 1);

        // convert the days with the hours, minutes and seconds to the epoch time
        return {(days * (60 * 60 * 24)) + (hours * (60 * 60)) + (minutes * 60) + seconds};
    }

    /**
     * @brief Converts a epoch time to a datetime
     * 
     * @param time 
     * @return datetime 
     */
    static datetime epoch_to_datetime(klib::time::s time) {
        datetime ret = {};

        // get the amount of days in the epoch time
        uint32_t days = (time.value / (24 * 60 * 60));
        uint32_t years = 1970;

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
        ret.year = years;

        uint32_t months = 1;

        // get the amount of months
        for (uint32_t i = 0; i < (sizeof(klib::io::rtc::month_days) / sizeof(klib::io::rtc::month_days[0])); i++) {
            // check if the current year is a leap year and we are checking 
            // february (feb = 1, we used the same leap year check as above)
            const bool leap_month = ((years & 0b11) == 0 && (i == 1));

            // check if we have more days than the 
            // current month has
            if (days < (klib::io::rtc::month_days[i] + leap_month)) {
                break;
            }

            // increment the months
            months++;
            days -= (klib::io::rtc::month_days[i] + leap_month);
        }

        // set the month and the days
        ret.month = months;
        ret.day = days + 1;

        // get the seconds left over
        const uint32_t seconds = time.value % (24 * 60 * 60);

        // set the hours, minutes and seconds
        ret.hours = seconds / (60 * 60);
        ret.minutes = (seconds % (60 * 60)) / 60;
        ret.seconds = seconds % 60;

        return ret;
    }
}

#endif