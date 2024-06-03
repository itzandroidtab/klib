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
    template <typename Payload, subtype SubType>
    struct functional {
        // size of the descriptor
        const uint8_t bFunctionLength = sizeof(functional<Payload, SubType>);

        // descriptor type (CS_INTERFACE)
        const uint8_t bDescriptorType = 0x24;

        // header functional descriptor subtype
        const uint8_t bDescriptorSubtype = static_cast<uint8_t>(SubType);

        // the payload of the functional descriptor
        Payload payload;
    };

    // make sure the size is correct
    static_assert(sizeof(functional<uint16_t, subtype::header>) == 0x05, "Cdc functional descriptor size is wrong");

    /**
     * @brief Header descriptor
     * 
     */
    struct header {
        // usb specification number which device compies to
        uint16_t bcdCDC;
    };

    /**
     * @brief Call management descriptor
     * 
     */
    struct call_management {
        // b[0] = device call management is handled by device
        // b[1] = device sends/receives call management
        // b[2..7] = reserved
        uint8_t bmCapabilities;

        // interface number of the data class interface. (optionally
        // used for call management)7
        uint8_t bDataInterface;
    };

    /**
     * @brief Abstract control management descriptor
     * 
     */
    struct abstract_control_management {
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

    struct direct_line_management {
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

    struct telephone_ringer {
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

    struct telephone_call_and_line_state_reporting_capabilities {
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

    template <uint32_t Entries>
    struct union_cdc {
        // the interface number of the communications or data class
        // interface, designated as the controlling interface for
        // the union
        const uint8_t bControlInterface;

        // interface number of first subordinate interface in the
        // union
        uint8_t bSubordinateInterface[Entries];
    };

    struct country_selection {
        // index of a string giving the release date for the implemented 
        // ISO 3166 country codes
        // 
        // date shall be presented as ddmmyyyy
        uint8_t iCountryCodeRelDate;
    };

    struct telephone_operational_modes {
        // b[0] = supports simple mode
        // b[1] = supports standalone mode
        // b[2] = supports computer centric mode
        // b[3..7] = reserved
        uint8_t bmCapabilities;
    };

    // TODO: add all these subtypes
    // struct usb_terminal {

    // };

    // struct network_channel {

    // };

    // struct protocol_unit {

    // };

    // struct extension_unit {

    // };

    // struct multi_channel_management {

    // };

    // struct capi_control_management {

    // };

    // struct ethernet_networking {

    // };

    // struct atm_networking {

    // };

    // struct wireless_handset_control_model {

    // };

    // struct mobile_direct_line_model {

    // };

    // struct mdlm_detail {

    // };

    // struct device_management_model {

    // };

    // struct obex {

    // };

    // struct command_set {

    // };

    // struct command_set_detail {

    // };

    // struct telephone_control_model {

    // };

    // struct obex_service_identifier {

    // };

    // struct ncm {

    // };
}

namespace klib::usb::cdc {
    /**
     * @brief functional header descriptor
     * 
     */
    using header = detail::functional<detail::header, detail::subtype::header>;

    static_assert(sizeof(header) == 0x05, "Cdc functional header descriptor size is wrong");

    /**
     * @brief Call management functional descriptor
     * 
     */
    using call_management = detail::functional<detail::call_management, detail::subtype::call_management>;

    static_assert(sizeof(call_management) == 0x05, "Cdc call management descriptor size is wrong");

    /**
     * @brief Call abstract control management descriptor
     * 
     */
    using abstract_control_management = detail::functional<detail::abstract_control_management, detail::subtype::abstract_control_management>;

    static_assert(sizeof(abstract_control_management) == 0x04, "Cdc abstract control management descriptor size is wrong");
    
    /**
     * @brief direct line management descriptor
     * 
     */
    using direct_line_management = detail::functional<detail::direct_line_management, detail::subtype::direct_line_management>;
    
    static_assert(sizeof(direct_line_management) == 0x04, "Cdc direct line management descriptor size is wrong");
    
    /**
     * @brief telephone ringer descriptor
     * 
     */
    using telephone_ringer = detail::functional<detail::telephone_ringer, detail::subtype::telephone_ringer>;
    
    static_assert(sizeof(telephone_ringer) == 0x05, "Cdc telephone ringer descriptor size is wrong");

    /**
     * @brief telephone ringer descriptor
     * 
     */
    using telephone_call_and_line_state_reporting_capabilities = detail::functional<detail::telephone_call_and_line_state_reporting_capabilities, detail::subtype::telephone_call_and_line_state_reporting_capabilities>;
    
    static_assert(sizeof(telephone_call_and_line_state_reporting_capabilities) == 0x07, "Cdc telephone call and line state reporting capabilities descriptor size is wrong");

    /**
     * @brief union descriptor
     * 
     * @tparam Entries 
     */
    template <uint32_t Entries>
    using union_cdc = detail::functional<detail::union_cdc<Entries>, detail::subtype::union_cdc>;
    
    static_assert(sizeof(union_cdc<2>) == 0x6, "Cdc union  descriptor size is wrong");
    
    /**
     * @brief country selection descriptor
     * 
     */
    using country_selection = detail::functional<detail::country_selection, detail::subtype::country_selection>;
    
    static_assert(sizeof(country_selection) == 0x04, "Cdc country selection descriptor size is wrong");
    
    /**
     * @brief telephone operational modes descriptor
     * 
     */
    using telephone_operational_modes = detail::functional<detail::telephone_operational_modes, detail::subtype::telephone_operational_modes>;
    
    static_assert(sizeof(telephone_operational_modes) == 0x04, "Cdc telephone operational modes descriptor size is wrong");
    
    // TODO: add all these subtypes
    // /**
    //  * @brief usb terminal descriptor
    //  * 
    //  */
    // using usb_terminal = detail::functional<detail::usb_terminal, detail::subtype::usb_terminal>;
    
    // static_assert(sizeof(usb_terminal) == 0x00, "Cdc usb terminal descriptor size is wrong");
    
    // /**
    //  * @brief network channel descriptor
    //  * 
    //  */
    // using network_channel = detail::functional<detail::network_channel, detail::subtype::network_channel>;
    
    // static_assert(sizeof(network_channel) == 0x00, "Cdc network channel descriptor size is wrong");
    
    // /**
    //  * @brief protocol unit descriptor
    //  * 
    //  */
    // using protocol_unit = detail::functional<detail::protocol_unit, detail::subtype::protocol_unit>;
    
    // static_assert(sizeof(protocol_unit) == 0x00, "Cdc protocol unit descriptor size is wrong");
    
    // /**
    //  * @brief extension unit descriptor
    //  * 
    //  */
    // using extension_unit = detail::functional<detail::extension_unit, detail::subtype::extension_unit>;
    
    // static_assert(sizeof(extension_unit) == 0x00, "Cdc extension unit descriptor size is wrong");
    
    // /**
    //  * @brief multi channel management descriptor
    //  * 
    //  */
    // using multi_channel_management = detail::functional<detail::multi_channel_management, detail::subtype::multi_channel_management>;
    
    // static_assert(sizeof(multi_channel_management) == 0x00, "Cdc multi channel management descriptor size is wrong");
    
    // /**
    //  * @brief capi control management descriptor
    //  * 
    //  */
    // using capi_control_management = detail::functional<detail::capi_control_management, detail::subtype::capi_control_management>;
    
    // static_assert(sizeof(capi_control_management) == 0x00, "Cdc capi control management descriptor size is wrong");
    
    // /**
    //  * @brief ethernet networking descriptor
    //  * 
    //  */
    // using ethernet_networking = detail::functional<detail::ethernet_networking, detail::subtype::ethernet_networking>;
    
    // static_assert(sizeof(ethernet_networking) == 0x00, "Cdc ethernet networking descriptor size is wrong");
    
    // /**
    //  * @brief atm networking descriptor
    //  * 
    //  */
    // using atm_networking = detail::functional<detail::atm_networking, detail::subtype::atm_networking>;
    
    // static_assert(sizeof(atm_networking) == 0x00, "Cdc atm networking descriptor size is wrong");
    
    // /**
    //  * @brief wireless handset control model descriptor
    //  * 
    //  */
    // using wireless_handset_control_model = detail::functional<detail::wireless_handset_control_model, detail::subtype::wireless_handset_control_model>;
    
    // static_assert(sizeof(wireless_handset_control_model) == 0x00, "Cdc wireless handset control model descriptor size is wrong");
    
    // /**
    //  * @brief mobile direct line model descriptor
    //  * 
    //  */
    // using mobile_direct_line_model = detail::functional<detail::mobile_direct_line_model, detail::subtype::mobile_direct_line_model>;
    
    // static_assert(sizeof(mobile_direct_line_model) == 0x00, "Cdc mobile direct line model descriptor size is wrong");
    
    // /**
    //  * @brief mdlm detail descriptor
    //  * 
    //  */
    // using mdlm_detail = detail::functional<detail::mdlm_detail, detail::subtype::mdlm_detail>;
    
    // static_assert(sizeof(mdlm_detail) == 0x00, "Cdc mdlm detail descriptor size is wrong");
    
    // /**
    //  * @brief device management model descriptor
    //  * 
    //  */
    // using device_management_model = detail::functional<detail::device_management_model, detail::subtype::device_management_model>;
    
    // static_assert(sizeof(device_management_model) == 0x00, "Cdc device management model descriptor size is wrong");
    
    // /**
    //  * @brief obex  descriptor
    //  * 
    //  */
    // using obex = detail::functional<detail::obex, detail::subtype::obex>;
    
    // static_assert(sizeof(obex) == 0x00, "Cdc obex  descriptor size is wrong");
    
    // /**
    //  * @brief command set descriptor
    //  * 
    //  */
    // using command_set = detail::functional<detail::command_set, detail::subtype::command_set>;
    
    // static_assert(sizeof(command_set) == 0x00, "Cdc command set descriptor size is wrong");
    
    // /**
    //  * @brief command set detail descriptor
    //  * 
    //  */
    // using command_set_detail = detail::functional<detail::command_set_detail, detail::subtype::command_set_detail>;
    
    // static_assert(sizeof(command_set_detail) == 0x00, "Cdc command set detail descriptor size is wrong");
    
    // /**
    //  * @brief telephone control model descriptor
    //  * 
    //  */
    // using telephone_control_model = detail::functional<detail::telephone_control_model, detail::subtype::telephone_control_model>;
    
    // static_assert(sizeof(telephone_control_model) == 0x00, "Cdc telephone control model descriptor size is wrong");
    
    // /**
    //  * @brief obex service identifier descriptor
    //  * 
    //  */
    // using obex_service_identifier = detail::functional<detail::obex_service_identifier, detail::subtype::obex_service_identifier>;
    
    // static_assert(sizeof(obex_service_identifier) == 0x00, "Cdc obex service identifier descriptor size is wrong");
    
    // /**
    //  * @brief ncm descriptor
    //  * 
    //  */
    // using ncm = detail::functional<detail::ncm, detail::subtype::ncm>;
    
    // static_assert(sizeof(ncm) == 0x00, "Cdc ncm descriptor size is wrong");
}

// release the old pack so the rest of the structs are not 
// affected by the pack(1)
#pragma pack(pop)

#endif