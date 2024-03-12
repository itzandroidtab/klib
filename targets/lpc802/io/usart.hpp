#ifndef KLIB_LPC802_USART_HPP
#define KLIB_LPC802_USART_HPP

#include <klib/io/core_clock.hpp>

#include "pins.hpp"
#include "matrix.hpp"

namespace klib::lpc802::io::periph {
    /**
     * @brief Struct stores information about the usart0 
     * hardware.
     * 
     */
    struct usart0 {
        // peripheral id (e.g usart0, usart1)
        constexpr static uint32_t id = 0;

        // peripheral clock bit position
        constexpr static uint32_t clock_id = 14;

        // interrupt id for the usart peripheral
        constexpr static uint32_t interrupt_id = 19;

        // port to the usart peripheral.
        static inline USART_Type *const port = USART0;

        // available pin modes on the flex matrix
        constexpr static auto tx = matrix<periph::matrix0>::flex_matrix::uart0_tx;
        constexpr static auto rx = matrix<periph::matrix0>::flex_matrix::uart0_rx;
        constexpr static auto rts = matrix<periph::matrix0>::flex_matrix::uart0_rts;
        constexpr static auto cts = matrix<periph::matrix0>::flex_matrix::uart0_cts;
        constexpr static auto sclk = matrix<periph::matrix0>::flex_matrix::uart0_sclk;
    };

    /**
     * @brief Struct stores information about the usart1
     * hardware.
     * 
     */
    struct usart1 {
        // peripheral id (e.g usart0, usart1)
        constexpr static uint32_t id = 1;

        // peripheral clock bit position
        constexpr static uint32_t clock_id = 15;

        // interrupt id for the usart peripheral
        constexpr static uint32_t interrupt_id = 20;

        // port to the usart peripheral.
        static inline USART1_Type *const port = USART1;

        // available pin modes on the flex matrix
        constexpr static auto tx = matrix<periph::matrix0>::flex_matrix::uart1_tx;
        constexpr static auto rx = matrix<periph::matrix0>::flex_matrix::uart1_rx;
        constexpr static auto sclk = matrix<periph::matrix0>::flex_matrix::uart1_sclk;
    };    
}

namespace klib::lpc802::io {
    template <typename Usart>
    class usart {
    public:
        // using for the array of callbacks
        using interrupt_callback = void(*)();

    protected:
        // callbacks
        static inline interrupt_callback transmit_callback = nullptr;
        static inline interrupt_callback receive_callback = nullptr;

    public:
        template <
            typename Txd, typename Rxd, 
            uint32_t Baudrate = 115'200
        >
        constexpr static void init(const interrupt_callback& transmit = nullptr, const interrupt_callback& receive = nullptr) {
            // set the callbacks
            transmit_callback = transmit;
            receive_callback = receive;

            // enable the usart perihperal clock
            clocks::enable<Usart>();

            // reset the peripheral
            SYSCON->PRESETCTRL0 &= ~(0x1 << Usart::clock_id);
            SYSCON->PRESETCTRL0 |= (0x1 << Usart::clock_id);

            // configure the pins using the switch matrix
            using matrix = matrix<periph::matrix0>;

            // setup the matrix with the supplied pins
            matrix::setup<Txd, Usart::tx>();
            matrix::setup<Rxd, Usart::rx>();

            // set the clock
            if constexpr (Usart::id == 0) {
                SYSCON->UART0CLKSEL = 1;
            }
            else {
                SYSCON->UART1CLKSEL = 1;
            }

            // configure the port
            Usart::port->CFG = (
                0x1 | (0x1 << 2)
            );

            // set the baud rate
            Usart::port->BRG = ((klib::io::clock::get() >> 4) / Baudrate) - 1;

            // register the interrupt handler when we have a 
            // callback registered
            if (receive != nullptr || transmit != nullptr) {
                // register the interrupt for the current peripheral
                irq::register_irq<Usart::interrupt_id>(irq_handler);

                // enable the interrupt
                enable_irq<Usart::interrupt_id>();
            }

            // enable the interrupt bits for receiving data and 
            // when the transmit buffer is empty based on the
            // interrupt callbacks
            Usart::port->INTENSET = (
                ((receive != nullptr) ? 0x1 : 0x0) | 
                ((transmit != nullptr) ? (0x1 << 2) : 0x0)
            );
        }

        /**
         * @brief Returns if we have data available.
         * 
         * @return true 
         * @return false 
         */
        static bool has_data() {
            // check both RBS bits in the status register
            return Usart::port->INTSTAT & 0x1;
        }

        /**
         * @brief Function that returns if the transmitting side is 
         * busy. If this function returns false it means we will 
         * overwrite anything that is currently being transmitted
         * 
         * @return true 
         * @return false 
         */
        static bool is_busy() {
            // return if the transmitter idle status is set
            return !(Usart::port->INTSTAT & (0x1 << 3));
        }

        /**
         * @brief Write a frame to the bus. If no data can be written 
         * the previous data might be lost
         * 
         * @param frame 
         */
        template <bool Async = false>
        static void write(const uint16_t data) {
            // write data to the uart
            Usart::port->TXDAT = data;

            // enable the interrupt if we have a transmit callback
            if (transmit_callback) {
                Usart::port->INTENSET |= (0x1 << 2);
            }

            if constexpr (Async) {
                // wait until we are done writing
                while (Usart::port->INTSTAT & (0x1 << 2)) {
                    // do nothing
                }
            }
        }

        /**
         * @brief Read the data that is in the rx register
         * 
         * @return uint16_t
         */
        static uint16_t read() {
            // reenable the irq if we have a receive callback
            if (receive_callback) {
                Usart::port->INTENSET |= 0x1;
            }

            return Usart::port->RXDAT;
        }

    public:
        /**
         * @brief Interrupt handler. Should not be called by the user
         * 
         */    
        static void irq_handler() {
            // get the status register
            const uint32_t status = Usart::port->STAT;

            // get the interrupt mask
            const uint32_t mask = Usart::port->INTENSET;

            // combine the mask and the status
            const uint32_t masked_status = status & mask;

            // clear the register
            Usart::port->INTENCLR = masked_status;   

            // check if we have received data
            if (masked_status & 0x1 && receive_callback != nullptr) {
                // we have data available and we have a callback. Call the callback
                receive_callback();
            }

            // check if we can transmit more data
            if (masked_status & (0x1 << 2) && transmit_callback != nullptr) {
                // we can transmit more data. Call the callback
                transmit_callback();
            }         
        }
    };
}

#endif