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
        static void init(const uint32_t cal_value = 0, const bool cal_direction = 1) {
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
            Rtc::port->CALIBRATION = (cal_value & 0x1ffff) | (cal_direction << 17);

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
            // convert the epoch to datetime
            const auto datetime = klib::io::rtc::epoch_to_datetime(time);

            // get the amount of days in the epoch time
            const uint32_t days = (time.value / (24 * 60 * 60));

            // 01-01-1970 was a thursday (3). Add that to our epoch
            // days to calculate the current day of the week
            Rtc::port->DOW = (days + 3) % 7;

            // get the amount of leap days in the years that passed
            const uint32_t leap = (((datetime.year - 1970) + ((1970 & 0b11) - 1)) / 4);

            // set the year
            Rtc::port->YEAR = datetime.year;
            Rtc::port->DOY = ((days - leap) - ((datetime.year - 1970) * klib::io::rtc::days_year)) + 1;

            // set the month and the days
            Rtc::port->MONTH = datetime.month;
            Rtc::port->DOM = datetime.day;

            // set the hours, minutes and seconds
            Rtc::port->HRS = datetime.hours;
            Rtc::port->MIN = datetime.minutes;
            Rtc::port->SEC = datetime.seconds;
        }
    };
}

#endif