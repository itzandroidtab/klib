#ifndef KLIB_MAX32660_I2C_HPP
#define KLIB_MAX32660_I2C_HPP

#include <cstdint>

#include <klib/io/core_clock.hpp>
#include <max32660.hpp>

#include "clocks.hpp"
#include "port.hpp"

namespace klib::max32660::io::periph::detail::i2c {
    enum class mode {
        sda,
        scl
    };

    template <typename Pin, mode Type, typename Periph>
    struct i2c {
        // pin of the peripheral
        using pin = Pin;

        // type of the pin
        constexpr static mode type = Type;

        // alternate function
        using periph = Periph;
    };
}

namespace klib::max32660::io::periph::wlp {
    struct i2c0 {
        // peripheral id (e.g i2c0, i2c1)
        constexpr static uint32_t id = 0;

        // peripheral clock bit position
        constexpr static uint32_t clock_id = 13;

        // peripheral interrupt position
        constexpr static uint32_t irq_id = 29;

        // port to the i2c hardware
        static inline I2C0_Type *const port = I2C0;

        using sda = detail::i2c::i2c<pins::package::wlp::pb3, detail::i2c::mode::sda, io::detail::alternate::func_1>;
        using scl = detail::i2c::i2c<pins::package::wlp::pb4, detail::i2c::mode::scl, io::detail::alternate::func_1>;
    };

    struct i2c1 {
        // peripheral id (e.g i2c0, i2c1)
        constexpr static uint32_t id = 1;

        // peripheral clock bit position
        constexpr static uint32_t clock_id = 28;

        // peripheral interrupt position
        constexpr static uint32_t irq_id = 52;

        // port to the i2c hardware
        static inline I2C0_Type *const port = I2C1;

        using sda = detail::i2c::i2c<pins::package::wlp::pd2, detail::i2c::mode::sda, io::detail::alternate::func_1>;
        using scl = detail::i2c::i2c<pins::package::wlp::pd1, detail::i2c::mode::scl, io::detail::alternate::func_1>;
    };
}

namespace klib::max32660::io::periph::tqfn_24 {
    struct i2c0 {
        // peripheral id (e.g i2c0, i2c1)
        constexpr static uint32_t id = 0;

        // peripheral clock bit position
        constexpr static uint32_t clock_id = 13;

        // peripheral interrupt position
        constexpr static uint32_t irq_id = 29;

        // port to the i2c hardware
        static inline I2C0_Type *const port = I2C0;

        using sda = detail::i2c::i2c<pins::package::tqfn_24::p13, detail::i2c::mode::sda, io::detail::alternate::func_1>;
        using scl = detail::i2c::i2c<pins::package::tqfn_24::p14, detail::i2c::mode::scl, io::detail::alternate::func_1>;
    };

    struct i2c1 {
        // peripheral id (e.g i2c0, i2c1)
        constexpr static uint32_t id = 1;

        // peripheral clock bit position
        constexpr static uint32_t clock_id = 28;

        // peripheral interrupt position
        constexpr static uint32_t irq_id = 52;

        // port to the i2c hardware
        static inline I2C0_Type *const port = I2C1;

        using sda = detail::i2c::i2c<pins::package::tqfn_24::p23, detail::i2c::mode::sda, io::detail::alternate::func_1>;
        using scl = detail::i2c::i2c<pins::package::tqfn_24::p24, detail::i2c::mode::scl, io::detail::alternate::func_1>;
    };
}

namespace klib::max32660::io {
    template <typename I2c>
    class i2c {
    public:
        /**
         * @brief Available speed modes.
         * 
         * @details 
         * standard = 100kbps
         * fast = 400kbps
         * fast plus = 1Mbps
         * high = 3.4Mbps
         */
        enum class speed {
            standard = 100'000,
            fast = 400'000,
            fast_plus = 1'000'000,
            high = 3'400'000,
        };

    protected:
        // error mask
        constexpr static uint32_t error_mask = (0x1 << 8) | (0x1 << 9) | 
            (0x1 << 10) | (0x1 << 11) | (0x1 << 12) | (0x1 << 13) | (0x1 << 14);

        struct clock_info {
            // high and low clock periods for the sda/scl
            uint16_t high_clk;
            uint16_t low_clk;

            // high speed clock periods
            uint8_t hs_high_clk;
            uint8_t hs_low_clk;

            // bit that signifies if the high speed mode is selected
            bool hs_bit;
        };

        template <speed Speed>
        constexpr static clock_info calculate_clock() {
            // check if we need to use high speed
            if constexpr (Speed == speed::high) {
                // calculate the amount of ticks a clock pulse takes
                uint32_t ticks = (klib::io::clock::get() / 2) / static_cast<uint32_t>(speed::fast);

                // calculate timing based on 30% dutycycle
                uint32_t hs_low_clk = ((ticks * 2) / 3) - 1;
                uint32_t hs_high_clk = ((ticks * 2) / 3) - 1;

                // calculate the clock info on fast speed
                clock_info t = calculate_clock<speed::fast>(); 

                // return the timings
                return {t.high_clk, t.low_clk, hs_low_clk, hs_high_clk, true};
            }
            else {
                // calculate the amount of ticks a clock pulse takes
                uint32_t ticks = (klib::io::clock::get() / 2) / static_cast<uint32_t>(Speed);

                // calculate timing based on 50% dutycycle
                uint16_t highclks = ((ticks >> 1) - 1);
                
                // return the timings
                return {highclks, highclks, 0, 0, false};
            }
        }

        /**
         * @brief Send a slave address to the i2c bus using a repeated start or a normal start
         * 
         * @tparam RepeatedStart 
         * @param address 
         * @param read 
         * @return true 
         * @return false 
         */
        template <bool RepeatedStart>
        constexpr static bool send_slave_address(const uint8_t address, const bool read) {
            // check if we need to write a repeated start or a normal start
            if constexpr (!RepeatedStart) {
                // send a start
                I2c::port->MASTER_CTRL |= 0x1;

                // wait until the repeated start is done
                while (I2c::port->MASTER_CTRL & (0x1 << 1)) {
                    // do nothing
                }

                // write the slave address to the fifo and leave the R/W bit set to 1
                I2c::port->FIFO = (address << 1) | read;
            }
            else {
                // send a repeated start
                I2c::port->MASTER_CTRL |= (0x1 << 1);

                // wait until the repeated start is done
                while (I2c::port->MASTER_CTRL & (0x1 << 1)) {
                    // do nothing
                }

                // write the slave address to the fifo and leave the R/W bit set to 1
                I2c::port->FIFO = (address << 1) | read;
            }

            // wait until we get a ack or nack on the address
            while (!(I2c::port->INT_FL0 & (1 << 7 | 1 << 10))) {
                // TODO: fix the issue where this loop gets endless
                // do nothing
            }

            // check if we received a nack
            if (I2c::port->INT_FL0 & (1 << 10)) {
                // send a stop
                send_stop();

                // return an error
                return false;
            }

            return true;
        }

        constexpr static void send_stop() {
            // send a stop
            I2c::port->MASTER_CTRL |= (0x1 << 2);

            // wait stop transaction
            while (!(I2c::port->INT_FL0 & (0x1 << 6)));
        }

    public:
        template <speed Speed = speed::fast>
        constexpr static void init() {
            // enable the clocks on the i2c peripheral
            clocks::enable<I2c>();

            // configure the gpio pins
            detail::pins::set_peripheral<typename I2c::sda::pin, typename I2c::sda::periph>();
            detail::pins::set_peripheral<typename I2c::scl::pin, typename I2c::scl::periph>();

            // enable the peripheral
            I2c::port->CTRL = 0x1;

            // clear the fifo's
            I2c::port->TX_CTRL0 |= (1 << 7);
            I2c::port->RX_CTRL0 |= (1 << 7);

            // set the thresholds
            I2c::port->RX_CTRL0 &= ~(0xf << 8);
            I2c::port->RX_CTRL0 |= (1 << 8);
            I2c::port->TX_CTRL0 &= ~(0xf << 8);
            I2c::port->TX_CTRL0 |= (1 << 8);

            // set the timeout
            I2c::port->TIMEOUT = (8 * 50);

            // calculate the speed of the i2c clock
            clock_info sp = calculate_clock<Speed>();

            // set the speed
            I2c::port->CLK_LO = sp.low_clk;
            I2c::port->CLK_HI = sp.high_clk;

            // set the high speed settings
            I2c::port->HS_CLK = (sp.hs_high_clk << 8) | sp.hs_low_clk;

            // configure the i2c peripheral (set master mode, set the high 
            // speed bit and disable clock stretching)
            I2c::port->CTRL |= (0x1 << 1) | (sp.hs_bit << 15) | (1 << 12); 
        }  

        /**
         * @brief Read from a device on the i2c bus
         * 
         * @tparam SendStop 
         * @tparam RepeatedStart 
         * @param address 
         * @param data 
         * @param size 
         * @return state if read was successfull
         */
        template <bool SendStop = true, bool RepeatedStart = false>
        constexpr static bool read(const uint8_t address, uint8_t* const data, const uint8_t size) {
            // clear the interrupt status register
            I2c::port->INT_FL0 = 0xffff;
            I2c::port->INT_FL1 = 0x3;

            // write the amount of bytes we want to receive in the RX_CTRL1 register
            I2c::port->RX_CTRL1 = size;

            // try to send the slave address
            if (!send_slave_address<RepeatedStart>(address, true)) {
                return false;
            }

            bool failed = false;
            uint32_t index = 0;

            // read the data
            while (index < size) {
                // wait until we have data in the fifo or an error
                while (!(I2c::port->INT_FL0 & ((0x1 << 4) | (0x1) | error_mask))) {
                    // do nothing
                }

                // check if we have data 
                if (I2c::port->INT_FL0 & ((0x1 << 4) | (0x1))) {
                    // read the data in the fifo
                    while (index < size && !(I2c::port->STATUS & (0x1 << 1))) {
                        // read the data from the fifo
                        data[index] = I2c::port->FIFO;

                        // increment the index
                        index++;
                    }

                    // check if done is signaled
                    if (I2c::port->INT_FL0 & 0x1) {
                        // check if we got all the data
                        if (index < size) {
                            // notify we have failed if we dont have enough
                            failed = true;
                        }

                        // clear the done flag
                        I2c::port->INT_FL0 = 0x1;

                        break;
                    }
                    else {
                        // clear the flag we have data
                        I2c::port->INT_FL0 = (0x1 << 4);
                    }
                }
                // check if we had a error
                else {
                    // send a stop when we have a error
                    send_stop();

                    // stop the loop. Notify we have failed
                    return false;
                }
            }

            if constexpr (SendStop) {
                // send a stop
                send_stop();
            }
            
            // return the status
            return !failed;
        }

        /**
         * @brief Write to a device on the i2c bus
         * 
         * @tparam SendStop 
         * @tparam RepeatedStart 
         * @param address 
         * @param data 
         * @param size 
         * @return state if read was successfull
         */
        template <bool SendStop = true, bool RepeatedStart = false>
        constexpr static bool write(const uint8_t address, const uint8_t* const data, const uint32_t size) {
            // clear the interrupt status register
            I2c::port->INT_FL0 = 0xffff;
            I2c::port->INT_FL1 = 0x3;

            // try to send the slave address
            if (!send_slave_address<RepeatedStart>(address, false)) {
                return false;
            }

            uint32_t index = 0;

            // try to write as many bytes until the fifo is full
            while (index < size) {
                // wait until we have a error or if we have space in the fifo
                while (!(I2c::port->INT_FL0 & (0x1 << 5)) && !(I2c::port->INT_FL0 & error_mask)) {
                    // do nothing
                }

                // check for errors
                if (I2c::port->INT_FL0 & error_mask) {
                    // send a stop
                    send_stop();

                    // return an error
                    return false;
                }
                else {
                    // write the data to the fifo
                    I2c::port->FIFO = data[index];

                    index++;
                }
            }

            if constexpr (SendStop) {
                // send a stop
                send_stop();
            }

            return true;
        }

        /**
         * @brief Stop a transmission that has been started did 
         * not send a stop
         * 
         */
        constexpr static void stop() {
            // stop the current transmission
            send_stop();   
        }
    };
}

#endif