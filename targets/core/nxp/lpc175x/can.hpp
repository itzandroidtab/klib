#ifndef KLIB_NXP_LPC175X_CAN_HPP
#define KLIB_NXP_LPC175X_CAN_HPP

#include <cstdint>

#include <klib/klib.hpp>
#include <klib/math.hpp>
#include <klib/io/bus/can.hpp>
#include <klib/io/core_clock.hpp>
#include <klib/io/peripheral.hpp>

#include <io/power.hpp>

#include "clocks.hpp"
#include "port.hpp"

namespace klib::core::lpc175x::io::detail {
    class can_interrupt {
    public:
        // using for the array of callbacks
        using interrupt_callback = void(*)();

    protected:
        // amount of ports that are handled in this interrupt
        constexpr static uint32_t can_count = 2;

        // interrupt callbacks
        static inline interrupt_callback callbacks[can_count] = {nullptr, nullptr};

    public:
        template <typename Can>
        static void init() {
            // register the interrupt handler
            target::irq::register_irq<Can::interrupt_id>(irq_handler);

            // enable the interrupt
            target::enable_irq<Can::interrupt_id>();            
        }

        /**
         * @brief Register a callback
         * 
         * @tparam Irq 
         * @param callback 
         */
        template <uint32_t CanId>
        static void register_irq(const interrupt_callback& callback) {
            static_assert(CanId < can_count, "Invalid Can Id provided to register");

            // register the callback
            callbacks[CanId] = callback;
        }

        /**
         * @brief Clear a callback
         * 
         * @tparam Irq 
         */
        template <uint32_t CanId>
        static void unregister_irq() {
            static_assert(CanId < can_count, "Invalid Can Id provided to unregister");

            // clear the callback
            callbacks[CanId] = nullptr;
        }

        /**
         * @brief Interrupt handler for the can hardware. This should only be 
         * called from NVIC
         * 
         */
        static void irq_handler() {
            // call every callback we have
            for (uint32_t i = 0; i < can_count; i++) {
                // check if we have a valid callback
                if (callbacks[i]) {
                    // call the callback
                    callbacks[i]();
                }
            }
        }
    };
}

namespace klib::core::lpc175x::io {
    /**
     * @brief lpc1756 can driver
     * 
     * @tparam Can 
     * @tparam CanTxBuffers amount of tx hardware buffers used. Can help
     * when a protocol needs a specific order of messages
     */
    template <typename Can, uint32_t CanTxBuffers = Can::tx_buffers>
    class can {
    public:
        // using for the array of callbacks
        using interrupt_callback = void(*)();

    protected:
        // make sure the amount of tx buffers is supported
        static_assert(CanTxBuffers > 0 && CanTxBuffers <= Can::tx_buffers, "Invalid buffer count");

        // callbacks
        static inline interrupt_callback transmit_callback = nullptr;
        static inline interrupt_callback receive_callback = nullptr;

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

        /**
         * @brief Available transmit buffers
         * 
         */
        enum class buffer_index {
            buffer_0 = 0,
            buffer_1,
            buffer_2,
        };

        /**
         * @brief Write implementation. Will fill up any of the buffers that is available
         * 
         * @tparam Async 
         * @param frame 
         * @param prio 
         */
        template <bool Async>
        static void write_impl(const klib::io::can::frame& frame, const uint8_t prio = 0) {
            // get the status register
            const uint32_t status = Can::port->SR;

            // pointer to the hardware registers
            hw_buffer* buffer;

            // index of the used buffer
            buffer_index index;

            // check what buffer to write to. Some buffers might be 
            // disabled based on the value in CanTxBuffers.
            if (status & (0x1 << 2)) {
                // point the pointer to the first buffer
                buffer = reinterpret_cast<hw_buffer *const>(
                    const_cast<uint32_t*>(&(Can::port->TFI1))
                );

                index = buffer_index::buffer_0;
            }
            else if (status & (0x1 << 10) && (CanTxBuffers > 1)) {
                buffer = reinterpret_cast<hw_buffer *const>(
                    const_cast<uint32_t*>(&(Can::port->TFI2))
                );

                index = buffer_index::buffer_1;
            }
            else if (status & (0x1 << 18) && (CanTxBuffers > 2)) {
                buffer = reinterpret_cast<hw_buffer *const>(
                    const_cast<uint32_t*>(&(Can::port->TFI3))
                );

                index = buffer_index::buffer_2;
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

            // check if we should enable the transmit done interrupt
            if (transmit_callback) {
                // enable the corresponding interrupt
                switch (index) {
                    case buffer_index::buffer_1:
                        Can::port->IER |= (0x1 << 9);
                        break;
                    case buffer_index::buffer_2:
                        Can::port->IER |= (0x1 << 10);
                        break;
                    case buffer_index::buffer_0:
                    default:
                        Can::port->IER |= (0x1 << 1);
                        break;
                }
            }

            // check if not async
            if constexpr (Async) {
                // do a early return if we are in async mode
                return;
            }

            // bitfield to check
            uint32_t bitfield = 0;

            // get the bit we should wait on
            switch (index) {
                case buffer_index::buffer_1:
                    bitfield = 0x1 << 11;
                    break;
                case buffer_index::buffer_2:
                    bitfield = 0x1 << 19;
                    break;
                default:
                case buffer_index::buffer_0:
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

        static void irq_handler() {
            // get the status register
            const uint32_t status = Can::port->ICR;
            const uint32_t mask = Can::port->IER;

            // get the masked status
            const uint32_t masked_status = status & mask;

            // check what kind of interrupt we have
            if (masked_status & ((0x1 << 1) | (0x1 << 9) | (0x1 << 10))) {
                // get the channels that have a interrupt pending
                uint32_t pending = masked_status & ((0x1 << 1) | (0x1 << 9) | (0x1 << 10));

                // disable the interrupt bits for every enabled 
                // transmit buffer
                Can::port->IER &= ~(pending);

                // check if we have a interrupt callback
                if (transmit_callback) {
                    // amount of trailing zeros in the status register
                    uint8_t trailing_zeros = 0;

                    // notify the interrupt for every transmit (32 == zero's)
                    while ((trailing_zeros = klib::ctz(pending)) < 32) {
                        // call the callback
                        transmit_callback();

                        // clear the bit from the pending interrupts
                        pending &= ~(0x1 << trailing_zeros);
                    }
                }
            }
            
            if (masked_status & (0x1 << 0)) {
                // clear the read interrupt bit until the current data is read
                Can::port->IER &= ~(0x1);

                // notify we have received data. Call the callback if we have one
                if (receive_callback) {
                    // call the callback
                    receive_callback();
                }
            }

            // check if we have passed the error thresholds (protocol defined at 127)
            if (masked_status & (0x1 << 5)) { 
                // get the current error counters
                const uint32_t reg = Can::port->GSR;

                // check if we should enable or disable the bus error interrupt
                if ((((reg >> 16) & 0xff) > 127) || (((reg >> 24) & 0xff) > 127)) {
                    Can::port->IER |= (0x1 << 7);
                }
                else {
                    Can::port->IER &= ~(0x1 << 7);
                }
            }

            // check if we have a bus error (only enabled when the thresholds 
            // are reached)
            if (masked_status & (0x1 << 7)) {
                // stop all the current transmissions when the bus error 
                // interrupt is enabled and we have one
                Can::port->CMR |= (0x1 << 1);
            }
        }

    public:
        template <uint32_t Frequency = 500'000>
        static void init(const interrupt_callback& transmit = nullptr, const interrupt_callback& receive = nullptr) {
            // init using the pconp register
            target::io::power_control::enable<Can>();

            // enable the clock to the peripheral
            clocks::set<Can, clocks::divider::div_1>();

            // enable the clock for the acceptance filter
            clocks::set<can_filter, clocks::divider::div_1>();

            // disable the filter by setting the bypass mode
            CANAF->AFMR = (0x1 << 1);

            // change the pins to the can driver
            io::detail::pins::set_peripheral<typename Can::rd::pin, typename Can::rd::periph>();
            io::detail::pins::set_peripheral<typename Can::td::pin, typename Can::td::periph>();

            // disable the can controller so we can configure it
            Can::port->MOD |= 0x1;

            // calculate the baud rate
            const uint32_t baud_rate = ((klib::io::clock::get() / 1) / (Frequency * (1 + prop + (seg1 + 1) + (seg2 + 1)))) - 1;

            // set the bus timing
            Can::port->BTR = (
                ((Frequency > 100'000) ? 0x0 : 0x1 << 23) |
                (seg1 << 16) | (seg2 << 20) | (sjw << 14) |
                (baud_rate & 0x3ff)
            );

            // set the callbacks
            transmit_callback = transmit;
            receive_callback = receive;

            // enable the interrupt bits (all error and wakeup bits 
            // and the receive interrupt if we are using interrupts)
            Can::port->IER = (
                0b00000110000 | ((receive != nullptr) ? 0x1 : 0x0) 
                // | (0x1 << 3) // data overrun interrupt bit
            );

            // clear any pending interrupts
            (void)Can::port->ICR;

            // register the interrupt
            detail::can_interrupt::template register_irq<Can::id>(irq_handler);

            // init the can interrupt
            detail::can_interrupt::template init<Can>();

            // enable the can controller
            Can::port->MOD = 0x00;
        }

        /**
         * @brief Returns if we have a RX or a TX error
         * 
         * @return true 
         * @return false 
         */
        static bool has_error() {
            return Can::port->GSR & (0x1 << 6);
        }

        /**
         * @brief Returns if we have data available
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
         * no space in the internal buffers
         * 
         * @return true 
         * @return false 
         */
        static bool is_busy() {
            // get the status register
            const uint32_t status = Can::port->SR;

            // check what bits we should check
            if constexpr (CanTxBuffers > 2) {
                // return if any of the buffers is empty
                return (status & ((0x1 << 2) | (0x1 << 10) | (0x1 << 18))) == 0;
            }
            else if constexpr (CanTxBuffers > 1) {
                // return if one of the first 2 buffers is empty
                return (status & ((0x1 << 2) | (0x1 << 10))) == 0;
            }

            // return if the first buffer is empty
            return (status & ((0x1 << 2))) == 0;
        }

        /**
         * @brief Write a frame to the bus. If no data can be written the 
         * packet is lost. If async == false this function blocks until
         * the data is written on the bus
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
         * @warning undefined behaviour if data is read without data in the 
         * hardware buffer. (check has_data() or wait for the receive
         * interrupt)
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

            // check if we should reenable the interrupt
            if (receive_callback) {
                // enable the receive interrupt
                Can::port->IER |= 0x1;
            }

            return frame;
        }
    };
}

#endif