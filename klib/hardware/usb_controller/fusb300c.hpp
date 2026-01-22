#ifndef KLIB_FUSB300C_HPP
#define KLIB_FUSB300C_HPP

#include <cstdint>

#include <klib/multispan.hpp>

namespace klib::hardware::usb_controller {
    /**
     * @brief Driver for the fusb300c
     *
     * @tparam Bus
     * @tparam IrqPin
     * @tparam Address
     */
    template <typename Bus, uint8_t Address = 0x22>
    class fusb300c {
    public:
        // using for the callbacks
        using interrupt_callback = void(*)();

    protected:
        // callbacks
        static inline interrupt_callback callback = nullptr;

        /**
         * @brief All the registers available on the fusb300c
         *
         */
        enum class reg: uint8_t {
            device_id = 0x01,
            switches = 0x02,
            measure = 0x04,
            pddetvt = 0x05,
            contorl = 0x06,
            mask = 0x0a,
            power = 0x0b,
            swreset = 0x0c,
            status0 = 0x40,
            status1 = 0x41,
            interrupt = 0x42,
        };

        /**
         * @brief Write a register with data
         *
         * @param command
         * @param data
         * @return status
         */
        static bool write_reg(const reg command, const std::span<const uint8_t> data) {
            // create a multispan to send
            const auto buffer = make_span_array(
                std::span<const uint8_t>{reinterpret_cast<const uint8_t*>(&command), sizeof(command)},
                data
            );

            // write the buffer to the fusb300c
            return Bus::write(Address, buffer);
        }

        /**
         * @brief Write one byte to a register
         *
         * @param command
         * @param data
         * @return status
         */
        static bool write_reg(const reg command, const uint8_t data) {
            // write using the other write
            return write_reg(command, {&data, sizeof(data)});
        }

        /**
         * @brief read multiple bytes from a register
         *
         * @param command
         * @param data
         * @return status
         */
        static bool read_reg(const reg command, std::span<uint8_t> data) {
            // write the register we want to read
            if (!Bus::write(Address, {reinterpret_cast<const uint8_t*>(&command), sizeof(command)})) {
                return false;
            }

            // start a read
            return Bus::read(Address, data);
        }

        /**
         * @brief Interrupt handler. Calls the specific function
         * that handles the interrupt request using the irq
         * helper
         *
         */
        static void irq_handler() {
            uint8_t status;

            // check what interrupt we are handling
            if (!read_reg(reg::interrupt, {&status, sizeof(status)})) {
                return;
            }

            // call the callback
            if (callback) {
                callback();
            }
        }

        static bool init_impl() {
            uint8_t buffer;

            // check if we have a device attached
            if (!read_reg(reg::device_id, {&buffer, sizeof(buffer)})) {
                return false;
            }

            // reset the fusb300c
            if (!write_reg(reg::swreset, 0x01)) {
                return false;
            }

            // enable measurement on the cc1/cc2 switches
            if (!write_reg(reg::switches, 0x0f)) {
                return false;
            }

            // enable vbus measurement (0.84v threshold)
            if (!write_reg(reg::measure, (0x1 << 6) | 0b000001)) {
                return false;
            }

            // enable power (needed for the CC1/CC2 measurement)
            if (!write_reg(reg::power, 0x07)) {
                return false;
            }

            return true;
        }

    public:
        /**
         * @brief Init the fusb300c
         *
         * @tparam IrqPin
         * @param callback
         * @return status
         */
        static bool init() {
            // init the fusb300c
            if (!init_impl()) {
                return false;
            }

            // enable the interrupt mask (disable all interrupts)
            if (!write_reg(reg::contorl, (0x1 << 5))) {
                return false;
            }

            return true;
        }

        /**
         * @brief Init and register our callback on the irq pin
         *
         * @tparam IrqPin
         * @param callback
         * @return true
         * @return false
         */
        template <typename IrqPin>
        static bool init(const interrupt_callback callback = nullptr) {
            // init the driver
            if (!init_impl()) {
                return false;
            }

            // disable the interrupt mask (enable all interrupts)
            if (!write_reg(reg::contorl, 0x00)) {
                return false;
            }

            // register our callback
            callback = callback;

            // setup and register our interrupt handler
            IrqPin::template init<IrqPin::edge::falling>(irq_handler);

            // return no errors
            return true;
        }

        /**
         * @brief Read if vbus is above the defined threshold
         *
         * @return true
         * @return false
         */
        static bool has_vbus() {
            uint8_t buffer;

            // read the status register
            if (!read_reg(reg::status0, {&buffer, sizeof(buffer)})) {
                return false;
            }

            // check if the vbus is high
            return buffer & (0x1 << 7);
        }
    };
}

#endif