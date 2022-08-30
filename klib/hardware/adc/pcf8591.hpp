#ifndef KLIB_PCF8591_HPP
#define KLIB_PCF8591_HPP

namespace klib::hardware::adc {
    template <typename Bus>
    class pcf8591 {
    protected:
        // address of the device
        const uint8_t address;

        // flag to enable or disable the output channel
        bool output_enabled;

    public:
        pcf8591(const uint8_t address = 0x48, const bool output_enabled = true):
            address(address), output_enabled(output_enabled)
        {}

        uint8_t get(const uint8_t channel) const {
            // create the data format for the pcf8591 (for writing and receiving)
            uint8_t buffer[] = {static_cast<uint8_t>((output_enabled << 6) | (channel & 0x3)), 0x00};

            // write the channel we want to read
            (void)Bus::write(address, buffer, 1);

            // read the result from the bus (2 bytes as the first byte is the control byte)
            (void)Bus::read(address, buffer, sizeof(buffer));

            // return the result
            return buffer[1];
        }

        void set(const uint8_t value, const bool enabled = true) {
            // change the output enable flag
            output_enabled = enabled;

            // create a array to write to the bus
            const uint8_t buffer[] = {static_cast<uint8_t>(output_enabled << 6), value};

            // write the full data only if the output is enabled
            if (output_enabled) {
                // write to the bus
                (void)Bus::write(address, buffer, sizeof(buffer));
            }
            else {
                // write to the bus (only the first byte to disable the output)
                (void)Bus::write(address, buffer, 1);
            }
        }
    };
};

#endif