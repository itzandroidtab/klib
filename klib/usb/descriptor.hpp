#ifndef KLIB_USB_PROTOCOL_HPP
#define KLIB_USB_PROTOCOL_HPP

#include <cstdint>

// Push the current pack to the stack and set the pack to 1
// as all these structs have specific sizes
#pragma pack(push, 1)

namespace klib::usb::descriptor {
    /**
     * More information can be found here:
     * https://www.beyondlogic.org/usbnutshell/usb5.shtml
     *
     */

    /**
     * @brief Defined class codes following the USB definitions
     * https://www.usb.org/defined-class-codes
     * 
     */
    enum class class_type: uint8_t {
        use_class_info = 0x00,
        audio = 0x01,
        communication_and_cdc = 0x02,
        hid = 0x03,
        physical = 0x04,
        image = 0x06,
        printer = 0x07,
        mass_storage = 0x08,
        hub = 0x09,
        cdc_Data = 0x0a,
        smart_card = 0x0b,
        content_security = 0x0d,
        video = 0x0e,
        personal_healthcare = 0x0f,
        audio_video_devices = 0x10,
        billboard_device = 0x11,
        type_c_bridge = 0x12,
        bulk_display = 0x13,
        mctp_over_usb = 0x14,
        i3c = 0x3c,
        diagnostic_device = 0xdc,
        wireless_controller = 0xe0,
        miscellaneous = 0xef,
        application_specific = 0xfe,
        vendor_specific = 0xff,
    };

    /**
     * Descriptor types from the usb 2.0 specification
     *
     */
    enum class descriptor_type: uint8_t {
        device = 1,
        configuration = 2,
        string = 3,
        interface = 4,
        endpoint = 5,
        device_qualifier = 6,
        other_speed_config = 7,
        interface_power = 8,
        otg = 9,
        debug = 10,
        interface_association = 11,
        bos = 15,
        device_capability = 16,
        cs_interface = 36,
        cs_endpoint = 37,
        superspeed_usb_endpoint_compainion = 48,
    };

    /**
     * Transfer types
     * (bmAttributes)
     *
     */
    enum class transfer_type {
        control = 0,
        isochronous = 1,
        bulk = 2,
        interrupt = 3
    };

    /**
     * Synchronisation types
     * (bmAttributes)
     *
     */
    enum class synchronisation_type {
        no_synchronisation = 0,
        asynchronous = 1,
        adaptive = 2,
        synchronous = 3
    };

    /**
     * Usage types
     * (bmAttributes)
     *
     */
    enum class usage_type {
        data_endpoint = 0,
        feedback_endpoint = 1,
        explicit_feedback_data_endpoint = 2,
        reserved = 3
    };

    /**
     * The device descriptor of a USB device represents the entire device.
     * As a result a USB device can only have one device descriptor. It
     * specifies some basic, yet important information about the device
     * such as the supported USB version, maximum packet size, vendor and
     * product ids and the number of possible configurations the device can
     * have.
     *
     */
    struct device {
        // size of descriptor in bytes
        const uint8_t bLength = sizeof(device);

        // descriptorType
        const descriptor_type bDescriptionType = descriptor_type::device;

        // usb specification number which device compies to
        uint16_t bcdUSB;

        // class code
        class_type bDeviceClass;

        // subclass code
        uint8_t bDeviceSubClass;

        // protocol code
        uint8_t bDeviceProtocol;

        // maximum packet size for zero endpoint(control).
        // valid sizes are (8, 16, 32, 64)
        uint8_t bMaxPacketSize;

        // vendor id
        uint16_t idVendor;

        // product id
        uint16_t idProduct;

        // device release number
        uint16_t bcdDevice;

        // index of manufacturer string descriptor
        uint8_t iManufacturer;

        // index of product string descriptor
        uint8_t iProduct;

        // index of serial number string descriptor
        uint8_t iSerialNumber;

        // number of possible configurations
        uint8_t bNumConfigurations;
    };

    static_assert(sizeof(device) == 18, "Device descriptor is not 18 bytes in length");

    /**
     * Descriptor for a configuration.
     *
     */
    struct configuration {
        // size of descriptor in bytes
        const uint8_t bLength = sizeof(configuration);

        // descriptorType
        const descriptor_type bDescriptionType = descriptor_type::configuration;

        // total length in bytes of data returned
        uint16_t wTotalLength;

        // number of interfaces
        uint8_t bNumInterfaces;

        // value to use as an argument to select this configuration
        uint8_t bConfigurationValue;

        // index of string descriptor describing this configuration
        uint8_t iConfiguration;

        // b[0..4] = reserved, set to 0
        // b[5] = remote wakeup
        // b[6] = self powered
        // b[7] = reserved, set to 1 (USB 1.0 Bus powered)
        uint8_t bmAttributes;

        // maximum power consumption in 2mA units
        uint8_t bMaxPower;
    };

    static_assert(sizeof(configuration) == 9, "Configuration descriptor is not 9 bytes in length");

    /**
     * Descriptor for a string. This provides human readable information
     * and is optional. If they are not used, any string index fields of
     * descriptors must be set to zero indicating there is no string
     * descriptor available.
     *
     * All strings are encoded in the unicode format and products can be
     * made to support multiple languages.
     *
     * Language descriptor uses this descriptor to send the languages
     * are supported by the device. List with id's can be found here:
     *
     * http://www.usb.org/developers/data/USB_LANGIDs.pdf
     *
     * e.g. for English - United States code 0x0409 is used.
     *
     * @tparam Size the size of the string
     *
     */
    template <uint32_t Size>
    struct string {
        // size of descriptor in bytes
        const uint8_t bLength = sizeof(string<Size>);

        // descriptorType
        const descriptor_type bDescriptionType = descriptor_type::string;

        // unicode encoded string
        char16_t bString[Size];
    };

    static_assert(sizeof(string<1>) == 4, "String descriptor with 1 entry is not 4 bytes in length");

    /**
     * A interface descriptor can be seen as a header or grouping of the
     * endpoints into a functional group performing a single feature of the
     * device.
     *
     */
    struct interface {
        // size of descriptor in bytes
        const uint8_t bLength = sizeof(interface);

        // descriptorType
        const descriptor_type bDescriptionType = descriptor_type::interface;

        // number of interface
        uint8_t bInterfaceNumber;

        // value used to select alternative setting
        uint8_t bAlternateSetting;

        // number of endpoints used for this interface
        uint8_t bNumEndpoints;

        // class code
        uint8_t bInterfaceClass;

        // subclass code
        uint8_t bInterfaceSubClass;

        // protocol code
        uint8_t bInterfaceProtocol;

        // index of string descirptor describing this interface
        uint8_t iInterface;
    };

    static_assert(sizeof(interface) == 9, "Interface descriptor is not 9 bytes in length");

    /**
     * Descriptor for an endpoint. Endpoint zero is always assumed to be a
     * control endpoint and is configured before any descriptors are even
     * requested. The host will use the information returned from these
     * descriptors to determine the bandwidth requirements of the bus.
     *
     */
    struct endpoint {
        // size of descriptor in bytes
        const uint8_t bLength = sizeof(endpoint);

        // descriptorType
        const descriptor_type bDescriptionType = descriptor_type::endpoint;

        // endpoint address
        // b[0..3] = endpoint number
        // b[4..6] = reserved, set to 0
        // b[7] = direction (ignored for control endpoints)
        // 0x0 = out
        // 0x1 = in
        uint8_t bEndpointAddress;

        // b[0..1] = Transfer type
        // 0x0 = Control
        // 0x1 = Isochronous
        // 0x2 = Bulk
        // 0x3 = Interrupt
        // b[2..3] = Synchronisation type
        // 0x0 = no synchronisation
        // 0x1 = asynchronous
        // 0x2 = adaptive
        // 0x3 = synchronous
        // b[4..5] = Usage type
        // 0x0 = data endpoint
        // 0x1 = feedback endpoint
        // 0x2 = explicit feedback data type
        // 0x3 = reserved
        // b[2..7] = reserved if Isochronous endpoint
        uint8_t bmAttributes;

        // maximum packet size this endpoint is capable of sending or
        // receiving
        uint16_t wMaxPacketSize;

        // interval for polling endpoint data transfers. Value in frame
        // counts. Ignored for bulk and control endpoints. Isochronous
        // must equal 1 and field may range from 1 to 255 for interrupt
        // endpoints
        uint8_t bInterval;
    };

    static_assert(sizeof(endpoint) == 7, "Endpoint descriptor is not 7 bytes in length");

    /**
     * @brief Device qualifier descriptor
     *
     */
    struct qualifier {
        // size of descriptor in bytes
        const uint8_t bLength = sizeof(qualifier);

        // descriptorType
        const descriptor_type bDescriptionType = descriptor_type::device_qualifier;

        // usb specification number which device compies to
        uint16_t bcdUSB;

        // class code
        class_type bDeviceClass;

        // subclass code
        uint8_t bDeviceSubClass;

        // protocol code
        uint8_t bDeviceProtocol;

        // maximum packet size for other speed
        uint8_t bMaxPacketSize0;

        // number of possible configurations
        uint8_t bNumConfigurations;

        // reserved
        uint8_t bReserved;
    };

    static_assert(sizeof(qualifier) == 10, "Qualifier descriptor is not 10 bytes in length");

    /**
     * @brief Binary device object store (BOS)
     *
     */
    struct bos {
        // size of this descriptor
        const uint8_t bLength = sizeof(bos);

        // the BOS descriptor type
        const descriptor_type bDescriptorType = descriptor_type::bos;

        // the length of this descriptor and all of
        // its sub descriptors
        uint16_t wTotalLength;

        // the number of separate device capability
        // descriptors in the BOS
        uint8_t bNumDeviceCaps;
    };

    static_assert(sizeof(bos) == 5, "BOS descriptor is not 5 bytes in length");

    /**
     * @brief Device capability descriptor
     *
     * @tparam Size
     */
    template <typename T>
    struct capability {
        // size of this descriptor
        const uint8_t bLength = sizeof(capability<T>);

        // the device capability descriptor type
        const descriptor_type bDescriptorType = descriptor_type::device_capability;

        // device capability type (valid values are
        // listed in table 9-11)
        uint8_t bDevCapabilityType;

        // capability specific format
        T capability_dependent;
    };

    static_assert(sizeof(capability<uint8_t>) == 4, "Capability descriptor is not 4 bytes in length");

    /**
     * @brief Interface association descriptor
     *
     */
    struct interface_association {
        // size of this descriptor
        const uint8_t bLength = sizeof(interface_association);

        // the device capability descriptor type
        const descriptor_type bDescriptorType = descriptor_type::interface_association;

        // first interface number of the next
        // descriptor
        uint8_t bFirstInterface;

        // number of interfaces that follow this
        // descriptor
        uint8_t bInterfaceCount;

        // 	bInterfaceClass used for the interface
        uint8_t bFunctionClass;

        // bInterfaceSubClass used for the
        // interface
        uint8_t bFunctionSubClass;

        // bInterfaceProtocol used for the
        // interface
        uint8_t bFunctionProtocol;

        // index of string descirptor describing
        // this interface
        uint8_t iFunction;
    };

    static_assert(sizeof(interface_association) == 8, "Interface association descriptor is not 8 bytes in length");

    /**
     * @brief Base for all the class specific interface descriptors
     *
     */
    template <typename Base, uint8_t SubType>
    struct cs_interface {
        // size of the descriptor
        const uint8_t bFunctionLength = sizeof(Base);

        // the device class specific descriptor type
        const descriptor_type bDescriptorType = descriptor_type::cs_interface;

        // header functional descriptor subtype
        const uint8_t bDescriptorSubtype = static_cast<uint8_t>(SubType);
    };

    // make sure the size is correct
    static_assert(sizeof(cs_interface<uint8_t, 0x00>) == 0x03, "Class specific base descriptor size is wrong");
}

// release the old pack so the rest of the structs are not
// affected by the pack(1)
#pragma pack(pop)

#endif