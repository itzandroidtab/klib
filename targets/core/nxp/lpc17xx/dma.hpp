#ifndef KLIB_NXP_LPC17XX_DMA_HPP
#define KLIB_NXP_LPC17XX_DMA_HPP

#include <array>
#include <span>
#include <bit>

#include <klib/klib.hpp>
#include <klib/io/dma.hpp>
#include <klib/irq_helper.hpp>

#include <io/power.hpp>

namespace klib::core::lpc17xx::io::detail::dma {
    // using to the interrupt callback in the transfer helper
    using interrupt_callback = void(*)();

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

    /**
     * @brief Helper struct for when there is a limited amount of
     * data the dma can send in one go
     *
     */
    struct transfer_helper {
        // requested size of the current endpoint
        uint32_t requested_size;

        // transmitted/received amount of data.
        uint32_t transferred_size;

        // callback when the transmit/receive is done
        interrupt_callback callback;
    };

    /**
     * @brief Get the memory burst size for a specific size
     *
     * @param size
     * @return uint8_t
     */
    constexpr uint8_t get_memory_burst_size(uint16_t size) {
        // return the most appropiate width for the input. TODO: For
        // the ssp the limit seems to be 128 bytes
        const std::array<uint16_t, 7> values = {1, 4, 8, 16, 32, 64, 128};

        for (uint32_t i = values.size(); i != 0; i--) {
            // check if the current value matches the burst size
            // from the data
            if (size >= values[i - 1]) {
                return i - 1;
            }
        }

        // return the highest value if we didnt find it
        return values.size() - 1;
    }

    /**
     * @brief Get the transfer width from a input size (sizeof(T) or peripheral byte size)
     *
     * @param size
     * @return uint8_t
     */
    constexpr uint8_t get_transfer_width(const uint32_t size) {
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
                return i - 1;
            }
        }

        // return the highest value if we didnt find it
        return values.size() - 1;
    }
}

namespace klib::core::lpc17xx::io {
    template <typename Dma>
    class dma {
    public:
        // interrupt callback type for the dma channels
        using interrupt_callback = irq_helper<Dma::max_channels>::interrupt_callback;

    protected:
        // IRQ helper for the dma channels
        static inline irq_helper<Dma::max_channels> helper;

    public:
        template <std::endian Endian = std::endian::little>
        static void init() {
            // enable the dma controller
            target::io::power_control::enable<Dma>();

            // enable the dma controller with the endianness
            Dma::port->CONFIG = 0x1 | ((Endian != std::endian::little) << 1);

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

            // register the interrupt handler
            target::irq::register_irq<Dma::interrupt_id>(irq_handler);

            // enable the interrupt
            target::enable_irq<Dma::interrupt_id>();
        }

        /**
         * @brief Register a callback
         *
         * @tparam Irq
         * @param callback
         */
        template <uint32_t Irq>
        static void register_irq(const interrupt_callback &callback) {
            helper.template register_irq<Irq>(callback);
        }

        /**
         * @brief Clear a callback
         *
         * @tparam Irq
         */
        template <uint32_t Irq>
        static void unregister_irq() {
            helper.template unregister_irq<Irq>();
        }

    public:
        static void irq_handler() {
            // get the status and the mask (INTTCSTAT is the masked result. We use
            // this as the mask for now)
            const uint32_t status = Dma::port->INTSTAT;
            const uint32_t mask = Dma::port->INTTCSTAT;

            // handle the interrupt
            helper.handle_irq(status, mask);

            // clear the interrupt flag
            Dma::port->INTTCCLEAR = status & mask;
        }
    };

    template <typename Dma, uint8_t Channel, typename Source, typename Destination>
    class dma_channel {
    protected:
        // make sure we have a valid dma channel
        static_assert(Channel < Dma::max_channels, "DMA does not support this channel");

        // helper for the dma transfers
        static inline volatile detail::dma::transfer_helper helper = {};

        /**
         * @brief DMA channel interrupt handler. Handles multi transmits when
         * we have a big size.
         *
         */
        static void irq_handler() {
            // check if we are done transmitting/receiving
            if (helper.transferred_size >= helper.requested_size) [[unlikely]] {
                // we are finished with the transfer. Call the callback 
                // when this happens if we have one
                if (helper.callback) {
                    // call the callback
                    helper.callback();

                    // clear the callback so we dont call it again
                    helper.callback = nullptr;
                }

                return;
            }

            // prepare the next interrupt
            peripheral_memory_impl<true>();
        }

        /**
         * @brief Memory to peripheral implementation
         *
         */
        template <bool Increment>
        static void peripheral_memory_impl() {
            // get the amount of data we still have to send
            const auto transfer_size = (helper.requested_size - helper.transferred_size);

            // determine the amount we will send in the current transfer
            const auto current_transfer = klib::min(transfer_size, 0xfff);

            // get the control register for the current channel
            const uint32_t control = Dma::port->CH[Channel].CONTROL;

            // check if we need to move to the next address
            if constexpr (Increment) {
                if (control & (0x1 << 26)) {
                    // advance by the source transfer width
                    Dma::port->CH[Channel].SRCADDR = Dma::port->CH[Channel].SRCADDR + (1u << ((control >> 18) & 0x7));
                }
                if (control & (0x1 << 27)) {
                    // advance by the destination transfer width
                    Dma::port->CH[Channel].DESTADDR = Dma::port->CH[Channel].DESTADDR + (1u << ((control >> 21) & 0x7));
                }
            }

            // set the amount of data we will transfer
            Dma::port->CH[Channel].CONTROL = (control & (~0xfff)) | current_transfer;

            // add the amount we are transferring right now
            helper.transferred_size += current_transfer;

            // enable the channel
            Dma::port->CH[Channel].CONFIG |= 0x1;
        }

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
                Source::template dma_enable<true, true>();
            }

            // enable dma on the destination peripheral
            if constexpr (!std::is_same_v<Destination, klib::io::dma::memory>) {
                // enable the dma on the destination
                Destination::template dma_enable<true, false>();
            }

            // register our irq handler for the current channel
            dma<Dma>::template register_irq<Channel>(irq_handler);
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
        static void write(const std::span<T>& source, const detail::dma::interrupt_callback callback = nullptr) {
            static_assert(std::is_same_v<Source, klib::io::dma::memory>, "Source needs to be memory for this function");
            static_assert(!std::is_same_v<Destination, klib::io::dma::memory>, "Destination needs to be a peripheral for this function");

            // get the amount of bytes we need to transmit
            const uint32_t size = (
                source.size_bytes() / klib::min(sizeof(T), sizeof(uint32_t))
            );

            // set the dma destination
            Dma::port->CH[Channel].DESTADDR = reinterpret_cast<uint32_t>(Destination::template dma_data<0>());
            Dma::port->CH[Channel].SRCADDR = reinterpret_cast<uint32_t>(source.data());

            // update the helper data with the information about the transfer
            // we know we are not writing to it so we can remove the const here
            // for the helper data
            helper.requested_size = size;
            helper.transferred_size = 0x00;
            helper.callback = callback;

            // setup the control register for the transfer
            Dma::port->CH[Channel].CONTROL = (
                (detail::dma::get_memory_burst_size(size) << 12) |
                (detail::dma::get_peripheral_burst_size(Destination::template dma_burst_size<0>()) << 15) |
                (detail::dma::get_transfer_width(klib::min(sizeof(T), sizeof(uint32_t))) << 18) |
                (detail::dma::get_transfer_width(Destination::template dma_width<0>()) << 21) |
                (Destination::template dma_increment<0>() << 27) |
                (MemoryIncrement << 26) | (0x1 << 31)
            );

            // check if we need a interrupt
            const bool irq = (size > 0xfff) || (callback != nullptr);

            // if we are enabling the interrupt make sure we clear the previous flags
            // to prevent a interrupt from triggering straight after enabling it
            if (irq) [[likely]] {
                Dma::port->INTTCCLEAR = 0x1 << Channel;
            }

            // setup the channel
            Dma::port->CH[Channel].CONFIG = (
                (Destination::template dma_id<0>() << 6) | (irq << 15) |
                (static_cast<uint32_t>(detail::dma::transfer_type::memory_to_peripheral) << 11)
            );

            // start the transfer
            peripheral_memory_impl<false>();
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
        static void read(const std::span<T>& destination, const detail::dma::interrupt_callback callback = nullptr) requires (!std::is_const_v<T>) {
            static_assert(!std::is_same_v<Source, klib::io::dma::memory>, "Source needs to be a peripheral for this function");
            static_assert(std::is_same_v<Destination, klib::io::dma::memory>, "Destination needs to be memory for this function");

            // get the amount of bytes we need to receive
            const uint32_t size = (
                destination.size_bytes() / klib::min(Source::template dma_width<1>(), sizeof(uint32_t))
            );

            // set the source and destination
            Dma::port->CH[Channel].SRCADDR = reinterpret_cast<uint32_t>(Source::template dma_data<1>());
            Dma::port->CH[Channel].DESTADDR = reinterpret_cast<uint32_t>(destination.data());

            // update the helper data with the information about the transfer
            helper.requested_size = size;
            helper.transferred_size = 0x00;
            helper.callback = callback;

            // setup the control register for the transfer
            Dma::port->CH[Channel].CONTROL = (
                (detail::dma::get_peripheral_burst_size(Source::template dma_burst_size<1>()) << 12) |
                (detail::dma::get_memory_burst_size(size) << 15) |
                (detail::dma::get_transfer_width(Source::template dma_width<1>()) << 18) |
                (detail::dma::get_transfer_width(klib::min(sizeof(T), sizeof(uint32_t))) << 21) |
                (Source::template dma_increment<1>() << 26) |
                (MemoryIncrement << 27) | (0x1 << 31)
            );

            // check if we need a interrupt
            const bool irq = (size > 0xfff) || (callback != nullptr);

            // if we are enabling the interrupt make sure we clear the previous flags
            // to prevent a interrupt from triggering straight after enabling it
            if (irq) [[likely]] {
                Dma::port->INTTCCLEAR = 0x1 << Channel;
            }

            // setup the channel
            Dma::port->CH[Channel].CONFIG = (
                (Source::template dma_id<1>() << 1) | (irq << 15) |
                (static_cast<uint32_t>(detail::dma::transfer_type::peripheral_to_memory) << 11)
            );

            // start the transfer
            peripheral_memory_impl<false>();
        }

        /**
         * @brief Transfer memory to memory
         *
         * @tparam MemoryIncrement increment the source pointer after reading from it
         * @param source
         * @param destination
         * @param size
         */
        template <bool SourceMemoryIncrement = true, bool DestinationMemoryIncrement = true, typename T, typename G>
        static void transfer(const std::span<T>& source, const std::span<G>& destination, const detail::dma::interrupt_callback callback = nullptr) requires (!std::is_const_v<G>) {
            static_assert(std::is_same_v<Source, klib::io::dma::memory>, "Source needs to be memory for this function");
            static_assert(std::is_same_v<Destination, klib::io::dma::memory>, "Destination needs to be memory for this function");

            // get the amount of bytes we need to move
            const uint32_t size = (
                klib::min(source.size_bytes(), destination.size_bytes()) / klib::min(sizeof(T), sizeof(uint32_t))
            );

            // set the dma destination and source
            Dma::port->CH[Channel].DESTADDR = reinterpret_cast<uint32_t>(destination.data());
            Dma::port->CH[Channel].SRCADDR = reinterpret_cast<uint32_t>(source.data());

            // update the helper data with the information about the transfer
            helper.requested_size = size;
            helper.transferred_size = 0x00;
            helper.callback = callback;

            // setup the control register for the transfer
            Dma::port->CH[Channel].CONTROL = (
                (detail::dma::get_memory_burst_size(size) << 12) |
                (detail::dma::get_memory_burst_size(size) << 15) |
                (detail::dma::get_transfer_width(klib::min(sizeof(T), sizeof(uint32_t))) << 18) |
                (detail::dma::get_transfer_width(klib::min(sizeof(T), sizeof(uint32_t))) << 21) |
                (SourceMemoryIncrement << 26) |
                (DestinationMemoryIncrement << 27) | 
                (0x1 << 31)
            );

            // check if we need a interrupt
            const bool irq = (size > 0xfff) || (callback != nullptr);

            // if we are enabling the interrupt make sure we clear the previous flags
            // to prevent a interrupt from triggering straight after enabling it
            if (irq) [[likely]] {
                Dma::port->INTTCCLEAR = 0x1 << Channel;
            }

            // setup the channel
            Dma::port->CH[Channel].CONFIG = (
                (irq << 15) |
                (static_cast<uint32_t>(detail::dma::transfer_type::memory_to_memory) << 11)
            );

            // start the transfer
            peripheral_memory_impl<false>();
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
            return (
                (config & 0x1) | (config & (0x1 << 17)) | 
                (helper.callback != nullptr) | 
                (helper.transferred_size < helper.requested_size)
            );
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