#ifndef KLIB_NXP_LPC17XX_SSP_HPP
#define KLIB_NXP_LPC17XX_SSP_HPP

#include <klib/multispan.hpp>
#include <klib/io/core_clock.hpp>
#include <klib/io/bus/spi.hpp>

#include <io/port.hpp>
#include <io/clocks.hpp>
#include <io/power.hpp>

namespace klib::core::lpc17xx::io {
    template <typename Ssp>
    class ssp {
    protected:
        template <klib::io::spi::bits Bits>
        constexpr static uint8_t convert_bits() {
            // make sure we have an amount of bits we support
            static_assert(static_cast<uint32_t>(Bits) >= 8, "below 8 bits per transfer is not supported");
            static_assert(static_cast<uint32_t>(Bits) <= 16, "above 16 bits per transfer is not supported");

            // return the raw value
            return static_cast<uint8_t>(Bits) - 1;
        }

        /**
         * @brief Write size amount of data to the fifo buffer
         * 
         * @tparam T 
         * @param data 
         * @param size 
         * @param transmitted 
         * @return uint16_t 
         */
        template <typename T>
        static uint16_t write_fifo(const T& data, const uint16_t size, const uint16_t transmitted) {
            uint16_t t = 0;

            // write as many bytes until the fifo is full
            while ((Ssp::port->SR & (0x1 << 1)) && ((transmitted + t) < size)) {
                // write dummy data if we dont have data or if we need 
                // to send more data we have in the buffer
                if (data.empty() || ((transmitted + t) >= data.size())) {
                    Ssp::port->DR = 0x00;
                }
                else {
                    Ssp::port->DR = data[transmitted + t];
                }

                // increment the amount of bytes transmitted
                t++;
            }

            return t;
        }

        /**
         * @brief Read size amount of data from the fifo buffer
         * 
         * @tparam T 
         * @param data 
         * @param size 
         * @param received 
         * @return uint16_t 
         */
        template <typename T>
        static uint16_t read_fifo(const T& data, const uint16_t size, const uint16_t received) {
            uint16_t r = 0;

            // read as much data as we can
            while (Ssp::port->SR & (0x1 << 2) && ((received + r) < size)) {
                // read the data.
                const uint16_t d = Ssp::port->DR;

                // store the value if we do not have a nullptr
                if ((!data.empty()) && ((received + r) < data.size())) {
                    data[received + r] = d;
                }

                // increment the amount of bytes read
                r++;
            }

            return r;
        }

    public:
        template <
            klib::io::spi::mode Mode = klib::io::spi::mode::mode0,
            uint32_t Frequency = 1'000'000,
            klib::io::spi::bits Bits = klib::io::spi::bits::bit_8,
            bool ExternalCs = false
        >
        static void init() {
            // enable the power for the ssp
            target::io::power_control::enable<Ssp>();

            // check if we need to setup the clock
            constexpr bool need_clock_setup = requires() {
                target::io::clocks::set<Ssp, target::io::clocks::divider::div_1>();
            };

            // setup the clock
            if constexpr (need_clock_setup) {
                target::io::clocks::set<Ssp, target::io::clocks::divider::div_1>();
            }

            // configure the gpio pins
            target::io::detail::pins::set_peripheral<typename Ssp::sck::pin, typename Ssp::sck::periph>();
            target::io::detail::pins::set_peripheral<typename Ssp::mosi::pin, typename Ssp::mosi::periph>();
            target::io::detail::pins::set_peripheral<typename Ssp::miso::pin, typename Ssp::miso::periph>();

            // only configure the chipselect when not using a different pin
            if constexpr (!ExternalCs) {
                target::io::detail::pins::set_peripheral<typename Ssp::cs0::pin, typename Ssp::cs0::periph>();
            }
            
            // setup the clock for the ssp. Value should be between 2 - 254. 
            // Note: to get lower frequencies. the SCR in CR0 can be used to
            // decrease the clock even further. (PCLK / (CPSDVSR * [SCR + 1]))
            // As most of the time higher speeds is better, SCR is not used at 
            // the moment (last bit ofthe CPSR does not stick. e.g 0x1 -> 0x0)
            Ssp::port->CPSR = static_cast<uint8_t>(
                klib::io::clock::get() / static_cast<uint32_t>(Frequency)
            );
            
            // set all the settings in the CR0 register
            Ssp::port->CR0 = (
                convert_bits<Bits>() | 
                (klib::io::spi::get_cpha<Mode>() << 6) |
                (klib::io::spi::get_cpol<Mode>() << 7)
            );

            // enable the ssp
            Ssp::port->CR1 = 0x1 << 1;
        }

        /**
         * @brief Write and read from the ssp bus
         * 
         * @param tx 
         * @param rx 
         */
        template <
            bool Async = false,
            typename T = std::span<const uint8_t>,
            typename G = std::span<uint8_t>
        > 
        static void write_read(const T& tx, const G& rx) requires is_span_type_c<uint8_t, T> && is_span_type<uint8_t, G> {
            // amount of data received/transmitted
            uint32_t transmitted = 0;
            uint32_t received = 0;

            // get the amount of data to receive and transmit. The smaller 
            // between the two must still write/read the data to clear the
            // fifo
            const uint32_t size = klib::max(tx.size(), rx.size());

            // write and read until we are done
            while ((size - transmitted) > 0 || (size - received) > 0) {
                // write the tx data
                transmitted += write_fifo(tx, size, transmitted);

                // read the rx data
                received += read_fifo(rx, size, received);
            }

            // check if we should wait until completion
            if constexpr (!Async) {
                while (is_busy()) {
                    // wait until we are done
                }
            }
        }

        /**
         * @brief Write to the ssp bus
         * 
         * @warning not all data is written unless not busy anymore
         * 
         * @param data 
         */
        template <bool Async = false, typename T = std::span<const uint8_t>>
        static void write(const T& data) requires is_span_type_c<uint8_t, T> {
            write_read<Async>(data, std::span<uint8_t>());
        }

        /**
         * @brief Returns if the current ssp port is busy. (transmitting/receiving data 
         * or TX fifo is not empty)
         * 
         * @return true 
         * @return false 
         */
        static bool is_busy() {
            return Ssp::port->SR & (0x1 << 4);
        }

        /**
         * @brief Clear any data left in the rx fifo. This functions 
         * should be used before starting a dma transfer.
         * 
         */
        static void clear_rx_fifo() {
            // discard anything that is left in the fifo
            while (Ssp::port->SR & (0x1 << 2)) {
                // discard the old data to make place for 
                // the new data
                (void)Ssp::port->DR;
            }
        }

    public:
        /**
         * @brief Section for the DMA controller. Returns information for the transfer.
         * 
         * @note two dma channels are required. One for reading and one for writing (or 
         * the user can poll/write the other interface in user code)
         * 
         */

        /**
         * @brief Returns the dma channel connection id
         * 
         * @return uint16_t 
         */
        template <bool Read>
        constexpr static uint16_t dma_id() {
            // return the base id + 0 for tx and + 1 for rx
            return Ssp::dma_id + Read;
        }

        /**
         * @brief Enable/Disable the dma for the ssp read/write.
         * 
         * @tparam Enable 
         * @tparam Read 
         */
        template <bool Enable, bool Read>
        static void dma_enable() {
            // set the new state
            Ssp::port->DMACR = (Ssp::port->DMACR & ~(0x1 << Read)) | (Enable << Read);
        }

        /**
         * @brief Returns the address where the dma controller should read/write from. 
         * 
         * @return uint32_t* const 
         */
        template <bool Read>
        static volatile uint32_t *const dma_data() {
            // return the data register
            return &Ssp::port->DR;
        }

        /**
         * @brief Returns the read/write burst size of the ssp
         * 
         * @return uint32_t* const 
         */
        template <bool Read>
        constexpr static uint32_t dma_burst_size() {
            // return half the fifo size
            return 2;
        }

        /**
         * @brief Returns the read/write transfer width of the ssp
         * 
         * @return uint32_t* const 
         */
        template <bool Read>
        static uint32_t dma_width() {
            // return the width based on the current bit transfer size
            return (((Ssp::port->CR0 & 0xf) + 1) + 7) / 8;
        }

        /**
         * @brief Returns the read/write transfer width of the ssp
         * 
         * @return uint32_t* const 
         */
        template <bool Read>
        constexpr static bool dma_increment() {
            // return if the dma should increment after a read/write
            return false;
        }
    };
}

#endif