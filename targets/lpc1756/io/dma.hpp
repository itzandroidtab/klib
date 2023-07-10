#ifndef KLIB_LPC1756_DMA_HPP
#define KLIB_LPC1756_DMA_HPP

#include <array>

#include <lpc1756.hpp>

#include <klib/io/dma.hpp>

#include "power.hpp"
#include "clocks.hpp"

namespace klib::lpc1756::io::periph {
    struct dma0 {
        // peripheral id (e.g dma0, dma1)
        constexpr static uint32_t id = 0;

        // interrupt id (including the arm vector table)
        constexpr static uint32_t irq_id = 42;

        // peripheral clock bit position
        constexpr static uint32_t clock_id = 29;

        // port to the GPDMA hardware
        static inline GPDMA_Type *const port = GPDMA;

        // available channels in the dma
        constexpr static uint32_t max_channels = 8;
    };
}

namespace klib::lpc1756::io {
    template <typename Dma>
    struct dma {
    public:
        template <bool LittleEndian = true>
        static void init() {
            // enable the dma controller
            power_control::enable<Dma>();

            // enable the dma controller with the endianness
            Dma::port->CONFIG = 0x1 | (LittleEndian << 1);

            // wait until the dma controller is enabled
            while (!(Dma::port->CONFIG & 0x1)) {
                // do nothing
            }

            // reset all the dma channels
            for (uint8_t i = 0; i < Dma::max_channels; i++) {
                // clear the channel config to stop the channel
                Dma::port->CH[i].CONFIG = 0x00;
            }

            // clear all interrupt flags
            Dma::port->INTERRCLR = 0xff;
            Dma::port->INTTCCLEAR = 0xff;            
        }
    };

    template <typename Dma, uint8_t Channel, typename Source, typename Destination>
    struct dma_channel {
    protected:
        // make sure we have a valid dma channel
        static_assert(Channel < Dma::max_channels, "DMA does not support this channel");

        /**
         * @brief Get the memory burst size for a specific size
         * 
         * @param size 
         * @return uint8_t 
         */
        static uint8_t get_memory_burst_size(uint16_t size) {
            // TODO: optimize this based on the size. Current selection is 4 bytes
            return 0b01;
        }

        /**
         * @brief Get the transfer width from a input size (sizeof(T) or peripheral byte size)
         * 
         * @param size 
         * @return uint8_t 
         */
        static uint8_t get_transfer_width(const uint32_t size) {
            // return the most appropriate width for the input. (note size == 3 
            // will fall back to single byte width) 
            switch (size) {
                case 4:
                    return 0b10;
                case 2:
                    return 0b01;
                default:
                    return 0b00;
            }
        }

        /**
         * @brief Convert the peripheral burst size to the parameter the dma needs
         * 
         * @param size 
         * @return uint8_t 
         */
        consteval static uint8_t get_peripheral_burst_size(uint16_t size) {
            // return the most appropiate width for the input.
            const std::array<uint16_t, 8> values = {1, 4, 8, 16, 32, 64, 128, 256};

            for (uint32_t i = values.size(); i != 0; i--) {
                // check if the current value matches the burst size 
                // from the data
                if (size >= values[i - 1]) {
                    return i;
                }
            }

            // return the highest value if we didnt find it
            return values[values.size() - 1];
        }

        /**
         * @brief Available transfer types
         * 
         */
        enum class transfer_type {
            memory_to_memory = 0x0,
            memory_to_peripheral = 0x1,
            peripheral_to_memory = 0x2,
            peripheral_to_peripheral = 0x3,
        };

    public:
        /**
         * @brief Init the dma channel (also enables the peripheral dma)
         * 
         */
        static void init() {
            // disable the current channel
            Dma::port->CH[Channel].CONFIG = 0;

            // disable linked lists
            Dma::port->CH[Channel].LLI = 0;

            // enable dma on the source peripheral
            if constexpr (!std::is_same_v<Source, klib::io::dma::memory>) {
                // enable the dma on the source
                Source::template dma_enable<true, 1>();
            }

            // enable dma on the destination peripheral
            if constexpr (!std::is_same_v<Destination, klib::io::dma::memory>) {
                // enable the dma on the destination
                Destination::template dma_enable<true, 0>();
            }
        }

        /**
         * @brief Write from memory to a peripheral
         * 
         * @tparam MemoryIncrement increment the source pointer after reading from it
         * @tparam T 
         * @param source
         * @param size 
         */
        template <bool MemoryIncrement = true, typename T>
        static void write(const T *const source, uint16_t size) {
            static_assert(std::is_same_v<Source, klib::io::dma::memory>, "Source needs to be memory for this function");
            static_assert(!std::is_same_v<Destination, klib::io::dma::memory>, "Destination needs to be a peripheral for this function");

            // set the source and destination
            Dma::port->CH[Channel].SRCADDR = reinterpret_cast<uint32_t>(source);
            Dma::port->CH[Channel].DESTADDR = reinterpret_cast<uint32_t>(Destination::template dma_data<0>());

            // setup the control register for the transfer
            Dma::port->CH[Channel].CONTROL = (
                ((size * sizeof(T)) & 0xfff) | (get_memory_burst_size(size) << 12) | 
                (get_peripheral_burst_size(Destination::template dma_burst_size<0>()) << 15) | 
                (get_transfer_width(sizeof(T)) << 18) | (MemoryIncrement << 26) | 
                (get_transfer_width(Destination::template dma_width<0>()) << 21) | 
                (Destination::template dma_increment<0>() << 27)
            );

            // setup the channel and enable it
            Dma::port->CH[Channel].CONFIG = (
                0x1 | (Destination::template dma_id<0>() << 6) | 
                (static_cast<uint32_t>(transfer_type::memory_to_peripheral) << 11)
            );
        }

        /**
         * @brief Write from a peripheral to memory
         * 
         * @tparam MemoryIncrement increment the destination pointer after writing to it
         * @tparam T 
         * @param destination 
         * @param size 
         */
        template <bool MemoryIncrement = true, typename T>
        static void read(T *const destination, uint16_t size) {
            static_assert(!std::is_same_v<Source, klib::io::dma::memory>, "Source needs to be a peripheral for this function");
            static_assert(std::is_same_v<Destination, klib::io::dma::memory>, "Destination needs to be memory for this function");

            // set the source and destination
            Dma::port->CH[Channel].SRCADDR = reinterpret_cast<uint32_t>(Source::template dma_data<1>());
            Dma::port->CH[Channel].DESTADDR = reinterpret_cast<uint32_t>(destination);

            // setup the control register for the transfer
            Dma::port->CH[Channel].CONTROL = (
                ((size * sizeof(T)) & 0xfff) | (MemoryIncrement << 27) |
                (get_peripheral_burst_size(Source::template dma_burst_size<1>()) << 12) | 
                (get_memory_burst_size(size) << 15) | (get_transfer_width(Source::template dma_width<1>()) << 18) |
                (get_transfer_width(sizeof(T)) << 21) | (Source::template dma_increment<1>() << 26)
            );

            // setup the channel and enable it
            Dma::port->CH[Channel].CONFIG = (
                0x1 | (Source::template dma_id<1>() << 6) | 
                (static_cast<uint32_t>(transfer_type::peripheral_to_memory) << 11)
            );            
        }

        /**
         * @brief Transfer memory to memory
         * 
         * @tparam MemoryIncrement increment the source pointer after reading from it
         * @param source 
         * @param destination 
         * @param size 
         */
        template <bool MemoryIncrement = true>
        static void transfer(const uint8_t *const source, uint8_t *const destination, uint16_t size) {
            static_assert(std::is_same_v<Source, klib::io::dma::memory>, "Source needs to be memory for this function");
            static_assert(std::is_same_v<Destination, klib::io::dma::memory>, "Destination needs to be memory for this function");
        }

        /**
         * @brief Transfer from a peripheral to another peripheral
         * 
         * @param size 
         */
        static void transfer(uint16_t size) {
            static_assert(!std::is_same_v<Source, klib::io::dma::memory>, "Source needs to be a peripheral for this function");
            static_assert(!std::is_same_v<Destination, klib::io::dma::memory>, "Destination needs to be a peripheral for this function");
        }

        /**
         * @brief Returns if the dma channel is busy with a operation
         * 
         * @return true 
         * @return false 
         */
        static bool is_busy() {
            const uint32_t config = Dma::port->CH[Channel].CONFIG;

            // return if the enabled or active flag is set
            return (config & 0x1) | (config & (0x1 << 17));
        }

        /**
         * @brief Stop the current dma transfer
         * 
         */
        static void stop() {
            // clear the channel enable flag to stop the transfer
            Dma::port->CH[Channel].CONFIG &= ~(0x1);
        }
    };
}

#endif