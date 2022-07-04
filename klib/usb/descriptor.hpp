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
     * Descriptor types from the usb 2.0 specification
     * 
     */ 
    enum class descriptor_type {
        device = 1,
        configuration = 2,
        string = 3,
        interface = 4,
        endpoint = 5,
        device_qualifier,
        other_speed_config = 7,
        interface_power = 8
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
        const uint8_t bLenght = sizeof(device);

        // descriptorType
        const uint8_t bDescriptionType = static_cast<uint8_t>(descriptor_type::device);

        // usb specification number wich device compies to
        uint16_t bcdUSB;

        // class code
        uint8_t bDeviceClass;

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
        const uint8_t bLenght = sizeof(configuration);

        // descriptorType
        const uint8_t bDescriptionType = static_cast<uint8_t>(descriptor_type::configuration);

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
    template<uint32_t Size>
    struct string {
        // size of descriptor in bytes
        const uint8_t bLenght = sizeof(string);

        // descriptorType
        const uint8_t bDescriptionType = static_cast<uint8_t>(descriptor_type::string);

        // unicode encoded string
        uint16_t bString[Size];
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
        const uint8_t bLenght = sizeof(interface);

        // descriptorType
        const uint8_t bDescriptionType = static_cast<uint8_t>(descriptor_type::interface);

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
        const uint8_t bLenght = sizeof(endpoint);

        // descriptorType
        const uint8_t bDescriptionType = static_cast<uint8_t>(descriptor_type::endpoint);

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
}

// release the old pack so the rest of the structs are not 
// affected by the pack(1)
#pragma pack(pop)

#endif