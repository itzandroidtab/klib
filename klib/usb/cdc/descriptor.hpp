#ifndef KLIB_USB_CDC_DESCRIPTOR_HPP
#define KLIB_USB_CDC_DESCRIPTOR_HPP

#include <cstdint>

#include <klib/usb/descriptor.hpp>

// Push the current pack to the stack and set the pack to 1
// as all these structs have specific sizes
#pragma pack(push, 1)

namespace klib::usb::cdc::detail {
    /**
     * @brief Functional descriptor subtype
     *
     */
    enum class subtype {
        header = 0x00,
        call_management = 0x01,
        abstract_control_management = 0x02,
        direct_line_management = 0x03,
        telephone_ringer = 0x04,
        telephone_call_and_line_state_reporting_capabilities = 0x05,
        union_cdc = 0x06,
        country_selection = 0x07,
        telephone_operational_modes = 0x08,
        usb_terminal = 0x09,
        network_channel = 0x0a,
        protocol_unit = 0x0b,
        extension_unit = 0x0c,
        multi_channel_management = 0x0d,
        capi_control_management = 0x0e,
        ethernet_networking = 0x0f,
        atm_networking = 0x10,
        wireless_handset_control_model = 0x11,
        mobile_direct_line_model = 0x12,
        mdlm_detail = 0x13,
        device_management_model = 0x14,
        obex = 0x15,
        command_set = 0x16,
        command_set_detail = 0x17,
        telephone_control_model = 0x18,
        obex_service_identifier = 0x19,
        ncm = 0x1a,
    };

    /**
     * @brief Base for all the functional descriptors
     *
     */
    template <typename Base, subtype SubType>
    struct functional: public descriptor::cs_interface<Base, static_cast<uint8_t>(SubType)> {};
}

namespace klib::usb::cdc {
    /**
     * @brief Header descriptor
     *
     */
    struct header {
        // header of the descriptor
        const detail::functional<header, detail::subtype::header> header;

        // usb specification number which device compies to
        uint16_t bcdCDC;
    };

    static_assert(sizeof(header) == 0x05, "Cdc functional header descriptor size is wrong");

    /**
     * @brief Call management descriptor
     *
     */
    struct call_management {
        // header of the descriptor
        const detail::functional<call_management, detail::subtype::call_management> header;

        // b[0] = device call management is handled by device
        // b[1] = device sends/receives call management
        // b[2..7] = reserved
        uint8_t bmCapabilities;

        // interface number of the data class interface. (optionally
        // used for call management)
        uint8_t bDataInterface;
    };

    static_assert(sizeof(call_management) == 0x05, "Cdc call management descriptor size is wrong");

    /**
     * @brief Abstract control management descriptor
     *
     */
    struct abstract_control_management {
        // header of the descriptor
        const detail::functional<abstract_control_management, detail::subtype::abstract_control_management> header;

        // b[0] = device suppports the request combination of
        // set_comm_feature, clear_comm_feature and get_comm_feature
        // b[1] = device supports the request combination of
        // set_line_coding, set_control_line_state, get_line_coding,
        // and the notification serial_state
        // b[2] = device supports the request send_break
        // b[3] = device supports the notification network_connection
        // b[4..7] = reserved
        uint8_t bmCapabilities;
    };

    static_assert(sizeof(abstract_control_management) == 0x04, "Cdc abstract control management descriptor size is wrong");

    struct direct_line_management {
        // header of the descriptor
        const detail::functional<direct_line_management, detail::subtype::direct_line_management> header;

        // b[0] = device suppports the request combination of
        // pulse_setup, send_pulse, and set_pulse_time
        // b[1] = device supports the request combination of
        // set_aux_line_state, ring_aux_jack, and
        // notification aux_jack_hook_state
        // b[2] = device requires extra pulse_setup request
        // during pulse dialing sequence to disengage holding
        // circuit
        // b[3..7] = reserved
        uint8_t bmCapabilities;
    };

    static_assert(sizeof(direct_line_management) == 0x04, "Cdc direct line management descriptor size is wrong");

    struct telephone_ringer {
        // header of the descriptor
        const detail::functional<telephone_ringer, detail::subtype::telephone_ringer> header;

        // number of discrete steps in volume supported by the
        // ringer
        // 0 = 256 discrete volume steps
        // 1 = 1 discrete volume step (fixed volume)
        // 2 = 2 discrete volume steps
        // ....
        // 254 = 254 discrete volume steps
        uint8_t bRingerVolSteps;

        // number of ringer patterns supported, 0 is reserved for
        // future use
        uint8_t bNumRingerPatterns;
    };

    static_assert(sizeof(telephone_ringer) == 0x05, "Cdc telephone ringer descriptor size is wrong");

    struct telephone_call_and_line_state_reporting_capabilities {
        // header of the descriptor
        const detail::functional<telephone_call_and_line_state_reporting_capabilities, detail::subtype::telephone_call_and_line_state_reporting_capabilities> header;

        // b[0] = differentiate between normal and interrupted dialtone
        // b[1] = reports ringback, busy and fast busy states
        // b[2] = reports caller id
        // b[3] = reports incoming distinctive ringing patterns
        // b[4] = supports DTMF digits input remotely over the
        // telephone line
        // b[5] = supports line state change notifications
        // b[6..31] = reserved
        uint32_t bmCapabilities;
    };

    static_assert(sizeof(telephone_call_and_line_state_reporting_capabilities) == 0x07, "Cdc telephone call and line state reporting capabilities descriptor size is wrong");

    template <uint32_t Entries>
    struct union_cdc {
        // header of the descriptor
        const detail::functional<union_cdc, detail::subtype::union_cdc> header;

        // the interface number of the communications or data class
        // interface, designated as the controlling interface for
        // the union
        const uint8_t bControlInterface;

        // interface number of first subordinate interface in the
        // union
        uint8_t bSubordinateInterface[Entries];
    };

    static_assert(sizeof(union_cdc<2>) == 0x6, "Cdc union  descriptor size is wrong");

    struct country_selection {
        // header of the descriptor
        const detail::functional<country_selection, detail::subtype::country_selection> header;

        // index of a string giving the release date for the implemented
        // ISO 3166 country codes
        //
        // date shall be presented as ddmmyyyy
        uint8_t iCountryCodeRelDate;
    };

    static_assert(sizeof(country_selection) == 0x04, "Cdc country selection descriptor size is wrong");

    struct telephone_operational_modes {
        // header of the descriptor
        const detail::functional<telephone_operational_modes, detail::subtype::telephone_operational_modes> header;

        // b[0] = supports simple mode
        // b[1] = supports standalone mode
        // b[2] = supports computer centric mode
        // b[3..7] = reserved
        uint8_t bmCapabilities;
    };

    static_assert(sizeof(telephone_operational_modes) == 0x04, "Cdc telephone operational modes descriptor size is wrong");

    // // TODO: add all these subtypes
    // struct usb_terminal {
    //     // header of the descriptor
    //     const detail::functional<usb_terminal, detail::subtype::usb_terminal> header;

    //     // TODO: add the data of the subtype
    // };

    // struct network_channel {
    //     // header of the descriptor
    //     const detail::functional<network_channel, detail::subtype::network_channel> header;

    //     // TODO: add the data of the subtype
    // };

    // struct protocol_unit {
    //     // header of the descriptor
    //     const detail::functional<protocol_unit, detail::subtype::protocol_unit> header;

    //     // TODO: add the data of the subtype
    // };

    // struct extension_unit {
    //     // header of the descriptor
    //     const detail::functional<extension_unit, detail::subtype::extension_unit> header;

    //     // TODO: add the data of the subtype
    // };

    // struct multi_channel_management {
    //     // header of the descriptor
    //     const detail::functional<multi_channel_management, detail::subtype::multi_channel_management> header;

    //     // TODO: add the data of the subtype
    // };

    // struct capi_control_management {
    //     // header of the descriptor
    //     const detail::functional<capi_control_management, detail::subtype::capi_control_management> header;

    //     // TODO: add the data of the subtype
    // };

    // struct ethernet_networking {
    //     // header of the descriptor
    //     const detail::functional<ethernet_networking, detail::subtype::ethernet_networking> header;

    //     // TODO: add the data of the subtype
    // };

    // struct atm_networking {
    //     // header of the descriptor
    //     const detail::functional<atm_networking, detail::subtype::atm_networking> header;

    //     // TODO: add the data of the subtype
    // };

    // struct wireless_handset_control_model {
    //     // header of the descriptor
    //     const detail::functional<wireless_handset_control_model, detail::subtype::wireless_handset_control_model> header;

    //     // TODO: add the data of the subtype
    // };

    // struct mobile_direct_line_model {
    //     // header of the descriptor
    //     const detail::functional<mobile_direct_line_model, detail::subtype::mobile_direct_line_model> header;

    //     // TODO: add the data of the subtype
    // };

    // struct mdlm_detail {
    //     // header of the descriptor
    //     const detail::functional<mdlm_detail, detail::subtype::mdlm_detail> header;

    //     // TODO: add the data of the subtype
    // };

    // struct device_management_model {
    //     // header of the descriptor
    //     const detail::functional<device_management_model, detail::subtype::device_management_model> header;

    //     // TODO: add the data of the subtype
    // };

    // struct obex {
    //     // header of the descriptor
    //     const detail::functional<obex, detail::subtype::obex> header;

    //     // TODO: add the data of the subtype
    // };

    // struct command_set {
    //     // header of the descriptor
    //     const detail::functional<command_set, detail::subtype::command_set> header;

    //     // TODO: add the data of the subtype
    // };

    // struct command_set_detail {
    //     // header of the descriptor
    //     const detail::functional<command_set_detail, detail::subtype::command_set_detail> header;

    //     // TODO: add the data of the subtype
    // };

    // struct telephone_control_model {
    //     // header of the descriptor
    //     const detail::functional<telephone_control_model, detail::subtype::telephone_control_model> header;

    //     // TODO: add the data of the subtype
    // };

    // struct obex_service_identifier {
    //     // header of the descriptor
    //     const detail::functional<obex_service_identifier, detail::subtype::obex_service_identifier> header;

    //     // TODO: add the data of the subtype
    // };

    // struct ncm {
    //     // header of the descriptor
    //     const detail::functional<ncm, detail::subtype::ncm> header;

    //     // TODO: add the data of the subtype
    // };
}


// release the old pack so the rest of the structs are not
// affected by the pack(1)
#pragma pack(pop)

#endif