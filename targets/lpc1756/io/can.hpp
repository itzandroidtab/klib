#ifndef KLIB_LPC1756_CAN_HPP
#define KLIB_LPC1756_CAN_HPP

#include <cstdint>

#include <klib/math.hpp>
#include <klib/io/bus/can.hpp>
#include <klib/io/core_clock.hpp>

#include <lpc1756.hpp>
#include "clocks.hpp"
#include "power.hpp"

#include "pins.hpp"

namespace klib::lpc1756::io::periph::detail::can {
    enum class mode {
        rd,
        td,
    };

    template <typename Pin, mode Type, typename Periph>
    struct can {
        // pin of the peripheral
        using pin = Pin;

        // type of the pin
        constexpr static mode type = Type;

        // alternate function
        using periph = Periph;
    };
}

namespace klib::lpc1756::io::periph::lqfp_80 {
    struct can0_0 {
        // peripheral id (e.g can0, can1)
        constexpr static uint32_t id = 0;

        // peripheral clock bit position
        constexpr static uint32_t clock_id = 13;

        // port to the CAN hardware
        static inline CAN1_Type *const port = CAN1;

        // P0.0 and P0.1
        using rd = detail::can::can<pins::package::lqfp_80::p37, detail::can::mode::rd, io::detail::alternate::func_1>;
        using td = detail::can::can<pins::package::lqfp_80::p38, detail::can::mode::td, io::detail::alternate::func_1>;
    };

    struct can0_1 {
        // peripheral id (e.g can0, can1)
        constexpr static uint32_t id = 0;

        // peripheral clock bit position
        constexpr static uint32_t clock_id = 13;

        // port to the CAN hardware
        static inline CAN1_Type *const port = CAN1;

        // P0.21 (not available on 80 pin package) and P0.22
        // TODO: decide/check if rd of the other configuration can be used
        using td = detail::can::can<pins::package::lqfp_80::p44, detail::can::mode::td, io::detail::alternate::func_3>;
    };

    struct can1 {
        // peripheral id (e.g can0, can1)
        constexpr static uint32_t id = 1;

        // peripheral clock bit position
        constexpr static uint32_t clock_id = 14;

        // port to the CAN hardware
        static inline CAN1_Type *const port = CAN2;

        // P2.7 and P2.8
        using rd = detail::can::can<pins::package::lqfp_80::p51, detail::can::mode::rd, io::detail::alternate::func_1>;
        using td = detail::can::can<pins::package::lqfp_80::p50, detail::can::mode::td, io::detail::alternate::func_1>;

        // 80 pin package does not have the following so only 1 configuration for can1
        // P0.4 and P0.5
    };
}

namespace klib::lpc1756::io {
    template <typename Can>
    class can {
    protected:
        // baud rate parameters
        constexpr static uint8_t prop = 0;
        constexpr static uint8_t seg1 = 0b1100;
        constexpr static uint8_t seg2 = 0b001;
        constexpr static uint8_t sjw = 0b00;

        /**
         * @brief buffer used in hardware
         * 
         */
        struct hw_buffer {
            volatile uint32_t TFI;
            volatile uint32_t TID;
            volatile uint32_t TDA;
            volatile uint32_t TDB;
        };

        /**
         * @brief Clocks for the can can acceptence filter
         * 
         */
        struct can_filter {
            // peripheral clock bit position
            constexpr static uint32_t clock_id = 15;
        };

        template <bool Async>
        static void write_impl(const klib::io::can::frame& frame, const uint8_t prio = 0) {
            // get the status register
            const uint32_t status = Can::port->SR;

            // pointer to the hardware registers
            hw_buffer* buffer;

            // index of the used buffer
            uint8_t index;

            // check what buffer to write to
            if (status & (0x1 << 2)) {
                // point the pointer to the first buffer
                buffer = reinterpret_cast<hw_buffer *const>(
                    const_cast<uint32_t*>(&(Can::port->TFI1))
                );

                index = 0;
            }
            else if (status & (0x1 << 10)) {
                buffer = reinterpret_cast<hw_buffer *const>(
                    const_cast<uint32_t*>(&(Can::port->TFI2))
                );

                index = 1;
            }
            else if (status & (0x1 << 18)) {
                buffer = reinterpret_cast<hw_buffer *const>(
                    const_cast<uint32_t*>(&(Can::port->TFI3))
                );

                index = 2;
            }
            else {
                // if no buffer is available return
                return;
            }

            // set the frame information
            buffer->TFI = (
                (frame.extended << 31) | (klib::min((frame.size & 0xf), 8) << 16) | 
                prio | (frame.remote << 30)
            );

            // write the address
            buffer->TID = (frame.extended ? 
                (frame.address & 0x1fffffff) : (frame.address & 0x7ff)
            );

            // only write the data if we do not have RTR
            if (!frame.remote) {
                // write the data to the hardware buffer
                buffer->TDA = (*reinterpret_cast<const uint32_t *const>(frame.data));
                buffer->TDB = (*reinterpret_cast<const uint32_t *const>(&(frame.data[4])));
            }

            // start the write for the selected buffer
            Can::port->CMR = 0x1 | (0x1 << (static_cast<uint8_t>(index) + 5));

            // check if not async
            if constexpr (Async) {
                return;
            }

            // bitfield to check
            uint32_t bitfield = 0;

            // get the 
            switch (index) {
                case 1:
                    bitfield = 0x1 << 11;
                    break;
                case 2:
                    bitfield = 0x1 << 19;
                    break;
                default:
                case 0:
                    bitfield = 0x1 << 3;
                    break;
            }

            // wait until we are done sending or a error
            while ((Can::port->SR & bitfield) == 0) {
                // wait until we are done transmitting

                // check for any errors
                if (Can::port->SR & (0x1 << 22)) {
                    // we have a error, return
                    return;
                }
            }
        }

    public:
        template <uint32_t Frequency = 500'000>
        static void init() {
            // init using the pconp register
            power_control::enable<Can>();

            // enable the clock to the peripheral
            clocks::enable<Can>();

            // enable the clock for the acceptance filter
            clocks::enable<can_filter>();

            // disable the filter by setting the bypass mode
            CANAF->AFMR = (0x1 << 1);

            // change the pins to the can driver
            io::detail::pins::set_peripheral<typename Can::rd::pin, typename Can::rd::periph>();
            io::detail::pins::set_peripheral<typename Can::td::pin, typename Can::td::periph>();

            // disable the can controller so we can configure it
            Can::port->MOD |= 0x1;

            // calculate the baud rate
            const uint32_t baud_rate = (klib::io::clock::get() / (Frequency * (1 + prop + (seg1 + 1) + (seg2 + 1)))) - 1;

            // set the bus timing
            Can::port->BTR = (
                ((Frequency > 100'000) ? 0x0 : 0x1 << 23) |
                (0b1100 << 16) | (0b001 << 20) | (sjw << 14) |
                (baud_rate & 0x1ff)
            );

            // set the can error warning limits
            // TODO: set the error warning limits
            // Can::port->EWL;

            // enable the can controller
            Can::port->MOD = 0x00;
        }

        /**
         * @brief Returns if we have data available. This function 
         * is non blocking
         * 
         * @return true 
         * @return false 
         */
        static bool has_data() {
            // check both RBS bits in the status register
            return Can::port->SR & (0x1 | 0x1 << 8);
        }

        /**
         * @brief Function that returns if the transmitting side is 
         * busy. If this function returns false it means there is 
         * no space in the internal buffers. Function is non blocking
         * 
         * @return true 
         * @return false 
         */
        static bool is_busy() {
            // get the status register
            const uint32_t status = Can::port->SR;

            // return if any of the buffers is empty
            return (status & ((0x1 << 2) | (0x1 << 10) | (0x1 << 18))) == 0;
        }

        /**
         * @brief Write a frame to the bus. If no data can be written the 
         * packet is lost
         * 
         * @param frame 
         */
        template <bool Async = false>
        static void write(const klib::io::can::frame& frame) {
            // we have space in buffer 1
            write_impl<Async>(frame);
        }

        /**
         * @brief Read a frame from the can hardware
         * 
         * @return klib::io::can::frame 
         */
        static klib::io::can::frame read() {
            // create the frame
            klib::io::can::frame frame;

            // read the data from the registers
            const uint32_t rfs = Can::port->RFS;

            frame.size = (rfs >> 16) & 0xf;
            frame.remote = (rfs >> 30) & 0x1;
            frame.extended = (rfs >> 31);

            // get the address
            frame.address = Can::port->RID;

            // read the data to the frame
            (*reinterpret_cast<uint32_t *const>(frame.data)) = Can::port->RDA;
            (*reinterpret_cast<uint32_t *const>(&(frame.data[4]))) = Can::port->RDB;

            // release the receive buffer
            Can::port->CMR = (0x1 << 2);

            return frame;
        }
    };
}

#endif