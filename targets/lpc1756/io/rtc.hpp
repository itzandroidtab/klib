#ifndef KLIB_LPC1756_RTC_HPP
#define KLIB_LPC1756_RTC_HPP

#include <cstdint>

#include <lpc1756.hpp>
#include <klib/units.hpp>

namespace klib::lpc1756::io::periph {
    struct rtc0 {
        // peripheral id (e.g rtc0, rtc1)
        constexpr static uint32_t id = 0;

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
         * @param year 
         * @param month 
         * @param day 
         * @param hours 
         * @param minutes 
         * @param seconds 
         * @return klib::time::s 
         */
        static klib::time::s datetime_to_epoch(const uint16_t year, const uint8_t month, const uint8_t day, 
            const uint8_t hours, const uint8_t minutes, const uint8_t seconds) 
        {
            uint32_t days = 0;

            // convert the year and 
            for (uint16_t y = 1970; y < year; y++) {
                // add all the days of this year
                for (uint8_t m = 0; (m < sizeof(month_days)) && (m < (month - 1)); m++) {
                    // add the amount of days in the current month
                    days += month_days[m];
                }

                // check if the current year is a leap year (we do the same 
                // thing as the hardware does by checking only the lowest 2 
                // bits. This will work fine between 1901 - 2099 but will 
                // fail at year 2100)
                if ((y & 0b11) == 0) {
                    // add a day for a leap year
                    days++;
                } 
            }

            // add the days but remove 1 for the current day
            days += (static_cast<int8_t>(day) - 1);

            // convert the days with the hours, minutes and seconds to the epoch time
            return {(days * (60 * 60 * 24)) + (hours * (60 * 60)) + (minutes * 60) + seconds};
        }

    public:
        static void init() {
            // do nothing
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