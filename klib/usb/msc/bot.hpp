#ifndef KLIB_USB_BOT_HPP
#define KLIB_USB_BOT_HPP

#include <cstdint>

// Push the current pack to the stack and set the pack to 1
// as all these structs have specific sizes
#pragma pack(push, 1)

namespace klib::usb::msc::bot {
    /**
     * @brief Command block wrapper (cbw)
     * 
     */
    struct command_block_wrapper {
        // signature that helps identify this data packet 
        // as a cbw
        const uint32_t dCBWSignature = 0x43425355;

        // command block tag send by the host
        uint32_t dCBWTag;

        // number of bytes of data that th ehost expects
        uint32_t dCBWDataTransferLength;

        // b[0..5] = reserved
        // b[6] = Obsolete
        // b[7] = direction
        // 0x0 = data-out from host to the device
        // 0x1 = data-in from the device to host
        uint8_t bmCBWFlags;

        // b[0..3] = the device logical unit number
        // b[4..7] = reserved
        uint8_t bCBWLUN;

        // b[0..4] = valid length of the cbwcb in bytes
        // b[5..7] = reserved
        uint8_t bCBWCBLength;

        // command block to be executed by the device
        uint8_t CBWCB[16];
    };

    static_assert(sizeof(command_block_wrapper) == 31, "Command block wrapper is not 31 bytes in length");

    /**
     * @brief Command status wrapper (csw)
     * 
     */
    struct command_status_wrapper {
        // signature that helps identify this data packet 
        // as a cbw
        const uint32_t dCBWSignature = 0x53425355;

        // device should set this as the data received 
        // from the host dCBWTag field
        uint32_t dCBWTag;

        // difference between the expected data and the 
        // actual transmitted amount of data
        uint32_t dCSWDataResidue;

        // status of the command
        uint8_t bCSWStatus;
    };

    static_assert(sizeof(command_status_wrapper) == 13, "Command status wrapper is not 13 bytes in length");
}

namespace klib::usb::msc::bot::status {
    /**
     * @brief bCSWStatus responses
     * 
     */
    enum class command: uint8_t {
        passed = 0x00,
        failed = 0x01,
        phase_error = 0x02,
    };
}

namespace klib::usb::msc::scsi {
    /**
     * @brief Supported scsi commands
     * 
     */
    enum class command {
        test_unit_ready = 0x00,
        request_sense = 0x03,
        inquiry = 0x12,
        mode_sense6 = 0x1a,
        start_stop_unit = 0x1b,
        receive_diagnostic_result = 0x1c,
        allow_medium_removal = 0x1e,
        read_format_capacities = 0x23,
        read_capacity10 = 0x25,
        read10 = 0x28,
        write10 = 0x2a,
        mode_sense10 = 0x5a,
        read_capacity16 = 0x9e,
    };

    /**
     * @brief Read 10 command structure
     * 
     */
    struct read10 {
        // operation code (0x28 for read10)
        uint8_t operation_code;

        // b[0..1] = obsolete
        // b[2] = rebuild assist recovery control
        // b[3] = force unit access
        // b[4] = disable page out
        // b[5..7] = read protect
        uint8_t flags;

        // logical block address
        uint32_t address;

        // group number
        uint8_t group;

        // transfer length
        uint16_t length;

        uint8_t control;
    };

    static_assert(sizeof(read10) == 10, "Read 10 command structure should be 10 bytes");

    /**
     * @brief Write 10 command structure
     * 
     */
    struct write10 {
        // operation code (0x2a for write10)
        uint8_t operation_code;

        // b[0..1] = obsolete
        // b[2] = reserved
        // b[3] = force unit access
        // b[4] = disable page out
        // b[5..7] = write protect
        uint8_t flags;

        // logical block address
        uint32_t address;

        // group number
        uint8_t group;

        // transfer length
        uint16_t length;

        uint8_t control;
    };

    static_assert(sizeof(write10) == 10, "Write 10 command structure should be 10 bytes");
}

// release the old pack so the rest of the structs are not 
// affected by the pack(1)
#pragma pack(pop)

#endif