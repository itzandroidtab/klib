#ifndef KLIB_USB_CAMERA_HPP
#define KLIB_USB_CAMERA_HPP

#include <algorithm>
#include <span>

#include <klib/string.hpp>
#include <klib/usb/usb/device.hpp>
#include <klib/usb/usb/descriptor.hpp>
#include <klib/usb/usb/video/descriptor.hpp>

namespace klib::usb::device {
    /**
     * @brief MJPEG video class used for the camera device
     * 
     * @tparam Width width of the data in pixels
     * @tparam Height height of the data in pixels
     * @tparam Fps 
     * @tparam InterruptEndpoint
     * @tparam IsoEndpoint 
     * @tparam MaxIsoEndpointSize size for the isochronous endpoint. Default should be fine for most devices.
     */
    template <uint32_t Width, uint32_t Height, uint32_t Fps = 10, uint32_t InterruptEndpoint = 6, uint32_t IsoEndpoint = 8, uint32_t MaxIsoEndpointSize = 1023>
    class video_mjpeg {
    public:
        // the max size of the isochronous endpoint
        constexpr static inline uint32_t max_iso_endpoint_size = MaxIsoEndpointSize;

        // the endpoints for the USB device
        constexpr static inline uint32_t interrupt_endpoint = InterruptEndpoint;
        constexpr static inline uint32_t iso_endpoint = IsoEndpoint;

        /**
         * @brief Enum with the string descriptor indexes
         *
         */
        enum class string_index {
            language = 0,
            manufacturer = 1,
            product = 2,
            serial = 3
        };

        /**
         * @brief Enum with the ids for all units
         *
         */
        enum class unit_index {
            management_unit = 0x00,
            camera_unit = 0x01,
            composite_unit = 0x02,
            output_unit = 0x03,
            selector_unit = 0x04,
            processing_unit = 0x05
        };

        // Push the current pack to the stack and set the pack to 1
        // as all these structs have specific sizes
        #pragma pack(push, 1)

        /**
         * @brief Config descriptor for the usb camera with mjpeg compression
         *
         * @details packed so we can write this whole descriptor
         * to the usb hardware in one go.
         *
         */
        struct config_descriptor {
            // configuration descriptor
            descriptor::configuration configuration;

            // interface association
            descriptor::interface_association association;

            // standard vs interface descriptor
            descriptor::interface interface0;

            // video control interface
            video::cs_video_control_interface vc_interface;

            // video input terminal
            video::camera_input_terminal camera_input_terminal;

            // composite input terminal
            video::input_terminal input_terminal;

            // video output terminal that represents the
            // usb pipe to the host
            video::output_terminal output_terminal;

            // selector unit
            video::selector_unit<2> selector_unit;

            // video processing unit
            video::processing_unit processing_unit;

            // endpoint descriptor
            descriptor::endpoint endpoint0;

            // class specific endpoint descriptor
            descriptor::cs_endpoint cs_endpoint;

            // standard vs interface descriptor
            descriptor::interface interface1;

            // video streaming header
            video::video_streaming_header header;

            // the video format
            video::video_mjpeg_format format;

            // the video frame
            video::video_mjpeg_frame<video::continuous_frame_intervals> frame;

            // standard vs interface descriptor
            descriptor::interface interface2;

            // endpoint descriptor
            descriptor::endpoint endpoint1;
        };

        // release the old pack so the rest of the structs are not
        // affected by the pack(1)
        #pragma pack(pop)

        // configuration descriptor
        constexpr __attribute__((aligned(4))) static inline config_descriptor config = {
            {
                .wTotalLength = sizeof(config_descriptor),
                .bNumInterfaces = 0x02,
                .bConfigurationValue = 0x01,
                .iConfiguration = 0x00,
                .bmAttributes = 0x80,
                .bMaxPower = 0xfa,
            },
            {
                .bFirstInterface = 0x00,
                .bInterfaceCount = 0x02,
                .bFunctionClass = static_cast<uint8_t>(video::interface_type::video),
                .bFunctionSubClass = static_cast<uint8_t>(video::interface_subtype::video_interface_collection),
                .bFunctionProtocol = static_cast<uint8_t>(video::protocol_code::undefined),
                .iFunction = static_cast<uint8_t>(string_index::product),
            },
            {
                .bInterfaceNumber = 0x00,
                .bAlternateSetting = 0x00,
                .bNumEndpoints = 0x01,
                .bInterfaceClass = static_cast<uint8_t>(video::interface_type::video),
                .bInterfaceSubClass = static_cast<uint16_t>(video::interface_subtype::videocontrol),
                .bInterfaceProtocol = static_cast<uint8_t>(video::protocol_code::protocol_15),
                .iInterface = static_cast<uint8_t>(string_index::product),
            },
            {
                .bcdUVC = 0x0150,
                .wTotalLength = (
                    sizeof(video::cs_video_control_interface) +
                    sizeof(video::camera_input_terminal) +
                    sizeof(video::input_terminal) +
                    sizeof(video::output_terminal) +
                    sizeof(video::selector_unit<2>) +
                    sizeof(video::processing_unit)
                ),
                .dwClockFrequency = 0x5B8D80,
                .bInCollection = 0x01,
                .baInterfaceNr = 0x01,
            },
            {
                .bTerminalID = static_cast<uint8_t>(unit_index::camera_unit),
                .wTerminalType = static_cast<uint16_t>(video::input_terminal_type::camera),
                .bAssocTerminal = 0x00,
                .iTerminal = 0x00,
                .wObjectiveFocalLengthMin = 0x0000,
                .wObjectiveFocalLengthMax = 0x0000,
                .wOcularFocalLength = 0x0000,
                .bmControls = 0x00,
            },
            {
                .bTerminalID = static_cast<uint8_t>(unit_index::composite_unit),
                .wTerminalType = static_cast<uint16_t>(video::external_terminal_type::composite_connector),
                .bAssocTerminal = 0x00,
                .iTerminal = 0x00,
            },
            {
                .bTerminalID = static_cast<uint8_t>(unit_index::output_unit),
                .wTerminalType = static_cast<uint16_t>(video::usb_terminal_type::streaming),
                .bAssocTerminal = 0x00,
                .bSourceID = static_cast<uint8_t>(unit_index::processing_unit),
                .iTerminal = 0x00,
            },
            {
                .bUnitID = static_cast<uint8_t>(unit_index::selector_unit),
                .baSourceID = {
                    static_cast<uint8_t>(unit_index::camera_unit),
                    static_cast<uint8_t>(unit_index::composite_unit)
                },
                .iSelector = 0x00,
            },
            {
                .bUnitID = static_cast<uint8_t>(unit_index::processing_unit),
                .bSourceID = static_cast<uint8_t>(unit_index::selector_unit),
                .wMaxMultiplier = 0x0000,
                .bmControls = 0x0000,
                .iProcessing = 0x00,
                .bmVideoStandards = 0x00,
            },
            {
                .bEndpointAddress = 0x80 | InterruptEndpoint,
                .bmAttributes = static_cast<uint8_t>(klib::usb::descriptor::transfer_type::interrupt),
                .wMaxPacketSize = 0x0040,
                .bInterval = 0x20
            },
            {
                .bDescriptorSubType = static_cast<uint8_t>(video::endpoint_subtype::interrupt),
                .wMaxTransferSize = 0x0040,
            },
            {
                .bInterfaceNumber = 0x01,
                .bAlternateSetting = 0x00,
                .bNumEndpoints = 0x00,
                .bInterfaceClass = static_cast<uint8_t>(video::interface_type::video),
                .bInterfaceSubClass = static_cast<uint16_t>(video::interface_subtype::videostreaming),
                .bInterfaceProtocol = static_cast<uint8_t>(video::protocol_code::protocol_15),
                .iInterface = 0x00,
            },
            {
                .bNumFormats = 0x01,
                .wTotalLength = (
                    sizeof(video::video_streaming_header) +
                    sizeof(video::video_mjpeg_format) +
                    sizeof(video::video_mjpeg_frame<video::continuous_frame_intervals>)
                ),
                .bEndpointAddress = 0x80 | IsoEndpoint,
                .bmInfo = 0x00,
                .bTerminalLink = static_cast<uint8_t>(unit_index::output_unit),
                .bStillCaptureMethod = 0x01,
                .bTriggerSupport = 0x01,
                .bTriggerUsage = 0x00,
                .bmaControls = 0x00,
            },
            {
                .bFormatIndex = 0x01,
                .bNumFrameDescriptors = 0x01,
                .bmFlags = 0x01,
                .bDefaultFrameIndex = 0x01,
                .bAspectRatioX = 0x00,
                .bAspectRatioY = 0x00,
                .bmInterlaceFlags = 0x00,
                .bCopyProtect = 0x00,
            },
            {
                .bFrameIndex = 0x01,
                .bmCapabilities = 0x03,
                .wWidth = Width,
                .wHeight = Height,
                .dwMinBitRate = Width * Height * 16 * Fps,
                .dwMaxBitRate = Width * Height * 16 * Fps,
                .dwMaxVideoFrameBufSize = Width * Height * 16 / 8,
                .dwDefaultFrameInterval = 10000000 / Fps,
                .bFrameIntervalType = 0x00,
                .interval = {
                    .dwMinFrameInterval = 10000000 / Fps,
                    .dwMaxFrameInterval = 10000000 / Fps,
                    .dwFrameIntervalStep = 0x00000000,
                },
            },
            {
                .bInterfaceNumber = 0x01,
                .bAlternateSetting = 0x01,
                .bNumEndpoints = 0x01,
                .bInterfaceClass = static_cast<uint8_t>(video::interface_type::video),
                .bInterfaceSubClass = static_cast<uint16_t>(video::interface_subtype::videostreaming),
                .bInterfaceProtocol = static_cast<uint8_t>(video::protocol_code::protocol_15),
                .iInterface = 0x00,
            },
            {
                .bEndpointAddress = 0x80 | IsoEndpoint,
                .bmAttributes = static_cast<uint8_t>(klib::usb::descriptor::transfer_type::isochronous),
                .wMaxPacketSize = MaxIsoEndpointSize,
                .bInterval = 0x01
            }
        };
    };

    /**
     * @brief Uncompressed video class used for the camera device (for now only rgb565 is supported)
     * 
     * @tparam Width width of the data in pixels
     * @tparam Height height of the data in pixels
     * @tparam Fps 
     * @tparam InterruptEndpoint
     * @tparam IsoEndpoint 
     * @tparam MaxIsoEndpointSize size for the isochronous endpoint. Default should be fine for most devices.
     */
    template <uint32_t Width, uint32_t Height, uint32_t Fps = 10, uint32_t InterruptEndpoint = 6, uint32_t IsoEndpoint = 8, uint32_t MaxIsoEndpointSize = 1023>
    class video_uncompressed {
    public:
        // the max size of the isochronous endpoint
        constexpr static inline uint32_t max_iso_endpoint_size = MaxIsoEndpointSize;

        // the endpoints for the USB device
        constexpr static inline uint32_t interrupt_endpoint = InterruptEndpoint;
        constexpr static inline uint32_t iso_endpoint = IsoEndpoint;
        
        /**
         * @brief Enum with the string descriptor indexes
         *
         */
        enum class string_index {
            language = 0,
            manufacturer = 1,
            product = 2,
            serial = 3
        };

        /**
         * @brief Enum with the ids for all units
         *
         */
        enum class unit_index {
            management_unit = 0x00,
            camera_unit = 0x01,
            composite_unit = 0x02,
            output_unit = 0x03,
            selector_unit = 0x04,
            processing_unit = 0x05
        };

        // Push the current pack to the stack and set the pack to 1
        // as all these structs have specific sizes
        #pragma pack(push, 1)

        /**
         * @brief Config descriptor for the usb camera wihtout compression
         *
         * @details packed so we can write this whole descriptor
         * to the usb hardware in one go.
         *
         */
        struct config_descriptor {
            // configuration descriptor
            descriptor::configuration configuration;

            // interface association
            descriptor::interface_association association;

            // standard vs interface descriptor
            descriptor::interface interface0;

            // video control interface
            video::cs_video_control_interface vc_interface;

            // video input terminal
            video::camera_input_terminal camera_input_terminal;

            // composite input terminal
            video::input_terminal input_terminal;

            // video output terminal that represents the
            // usb pipe to the host
            video::output_terminal output_terminal;

            // selector unit
            video::selector_unit<2> selector_unit;

            // video processing unit
            video::processing_unit processing_unit;

            // endpoint descriptor
            descriptor::endpoint endpoint0;

            // class specific endpoint descriptor
            descriptor::cs_endpoint cs_endpoint;

            // standard vs interface descriptor
            descriptor::interface interface1;

            // video streaming header
            video::video_streaming_header header;

            // the video format
            video::video_uncompressed_format format;

            // the video frame
            video::video_uncompressed_frame<video::continuous_frame_intervals> frame;

            // standard vs interface descriptor
            descriptor::interface interface2;

            // endpoint descriptor
            descriptor::endpoint endpoint1;
        };

        // release the old pack so the rest of the structs are not
        // affected by the pack(1)
        #pragma pack(pop)

        // configuration descriptor
        constexpr __attribute__((aligned(4))) static inline config_descriptor config = {
            {
                .wTotalLength = sizeof(config_descriptor),
                .bNumInterfaces = 0x02,
                .bConfigurationValue = 0x01,
                .iConfiguration = 0x00,
                .bmAttributes = 0x80,
                .bMaxPower = 0xfa,
            },
            {
                .bFirstInterface = 0x00,
                .bInterfaceCount = 0x02,
                .bFunctionClass = static_cast<uint8_t>(video::interface_type::video),
                .bFunctionSubClass = static_cast<uint8_t>(video::interface_subtype::video_interface_collection),
                .bFunctionProtocol = static_cast<uint8_t>(video::protocol_code::undefined),
                .iFunction = static_cast<uint8_t>(string_index::product),
            },
            {
                .bInterfaceNumber = 0x00,
                .bAlternateSetting = 0x00,
                .bNumEndpoints = 0x01,
                .bInterfaceClass = static_cast<uint8_t>(video::interface_type::video),
                .bInterfaceSubClass = static_cast<uint16_t>(video::interface_subtype::videocontrol),
                .bInterfaceProtocol = static_cast<uint8_t>(video::protocol_code::protocol_15),
                .iInterface = static_cast<uint8_t>(string_index::product),
            },
            {
                .bcdUVC = 0x0150,
                .wTotalLength = (
                    sizeof(video::cs_video_control_interface) +
                    sizeof(video::camera_input_terminal) +
                    sizeof(video::input_terminal) +
                    sizeof(video::output_terminal) +
                    sizeof(video::selector_unit<2>) +
                    sizeof(video::processing_unit)
                ),
                .dwClockFrequency = 0x5B8D80,
                .bInCollection = 0x01,
                .baInterfaceNr = 0x01,
            },
            {
                .bTerminalID = static_cast<uint8_t>(unit_index::camera_unit),
                .wTerminalType = static_cast<uint16_t>(video::input_terminal_type::camera),
                .bAssocTerminal = 0x00,
                .iTerminal = 0x00,
                .wObjectiveFocalLengthMin = 0x0000,
                .wObjectiveFocalLengthMax = 0x0000,
                .wOcularFocalLength = 0x0000,
                .bmControls = 0x00,
            },
            {
                .bTerminalID = static_cast<uint8_t>(unit_index::composite_unit),
                .wTerminalType = static_cast<uint16_t>(video::external_terminal_type::composite_connector),
                .bAssocTerminal = 0x00,
                .iTerminal = 0x00,
            },
            {
                .bTerminalID = static_cast<uint8_t>(unit_index::output_unit),
                .wTerminalType = static_cast<uint16_t>(video::usb_terminal_type::streaming),
                .bAssocTerminal = 0x00,
                .bSourceID = static_cast<uint8_t>(unit_index::processing_unit),
                .iTerminal = 0x00,
            },
            {
                .bUnitID = static_cast<uint8_t>(unit_index::selector_unit),
                .baSourceID = {
                    static_cast<uint8_t>(unit_index::camera_unit),
                    static_cast<uint8_t>(unit_index::composite_unit)
                },
                .iSelector = 0x00,
            },
            {
                .bUnitID = static_cast<uint8_t>(unit_index::processing_unit),
                .bSourceID = static_cast<uint8_t>(unit_index::selector_unit),
                .wMaxMultiplier = 0x0000,
                .bmControls = 0x0000,
                .iProcessing = 0x00,
                .bmVideoStandards = 0x00,
            },
            {
                .bEndpointAddress = 0x80 | InterruptEndpoint,
                .bmAttributes = static_cast<uint8_t>(klib::usb::descriptor::transfer_type::interrupt),
                .wMaxPacketSize = 0x0040,
                .bInterval = 0x20
            },
            {
                .bDescriptorSubType = static_cast<uint8_t>(video::endpoint_subtype::interrupt),
                .wMaxTransferSize = 0x0040,
            },
            {
                .bInterfaceNumber = 0x01,
                .bAlternateSetting = 0x00,
                .bNumEndpoints = 0x00,
                .bInterfaceClass = static_cast<uint8_t>(video::interface_type::video),
                .bInterfaceSubClass = static_cast<uint16_t>(video::interface_subtype::videostreaming),
                .bInterfaceProtocol = static_cast<uint8_t>(video::protocol_code::protocol_15),
                .iInterface = 0x00,
            },
            {
                .bNumFormats = 0x01,
                .wTotalLength = (
                    sizeof(video::video_streaming_header) +
                    sizeof(video::video_mjpeg_format) +
                    sizeof(video::video_mjpeg_frame<video::continuous_frame_intervals>)
                ),
                .bEndpointAddress = 0x80 | IsoEndpoint,
                .bmInfo = 0x00,
                .bTerminalLink = static_cast<uint8_t>(unit_index::output_unit),
                .bStillCaptureMethod = 0x01,
                .bTriggerSupport = 0x01,
                .bTriggerUsage = 0x00,
                .bmaControls = 0x00,
            },
            {
                .bFormatIndex = 0x01,
                .bNumFrameDescriptors = 0x01,
                // RGB565 GUID as a byte array (for UVC descriptors) "{E436EB7B-524F-11CE-9F53-0020AF0BA770}"
                .guidFormat = { 0x7B, 0xEB, 0x36, 0xE4, 0x4F, 0x52, 0xCE, 0x11, 0x9F, 0x53, 0x00, 0x20, 0xAF, 0x0B, 0xA7, 0x70 },
                .bBitsPerPixel = 16,
                .bDefaultFrameIndex = 0x01,
                .bAspectRatioX = 0x00,
                .bAspectRatioY = 0x00,
                .bmInterlaceFlags = 0x00,
                .bCopyProtect = 0x00,
            },
            {
                .bFrameIndex = 0x01,
                .bmCapabilities = 0x03,
                .wWidth = Width,
                .wHeight = Height,
                .dwMinBitRate = Width * Height * 16 * Fps,
                .dwMaxBitRate = Width * Height * 16 * Fps,
                .dwMaxVideoFrameBufSize = Width * Height * 16 / 8,
                .dwDefaultFrameInterval = 10000000 / Fps,
                .bFrameIntervalType = 0x00,
                .interval = {
                    .dwMinFrameInterval = 10000000 / Fps,
                    .dwMaxFrameInterval = 10000000 / Fps,
                    .dwFrameIntervalStep = 0x00000000,
                },
            },
            {
                .bInterfaceNumber = 0x01,
                .bAlternateSetting = 0x01,
                .bNumEndpoints = 0x01,
                .bInterfaceClass = static_cast<uint8_t>(video::interface_type::video),
                .bInterfaceSubClass = static_cast<uint16_t>(video::interface_subtype::videostreaming),
                .bInterfaceProtocol = static_cast<uint8_t>(video::protocol_code::protocol_15),
                .iInterface = 0x00,
            },
            {
                .bEndpointAddress = 0x80 | IsoEndpoint,
                .bmAttributes = static_cast<uint8_t>(klib::usb::descriptor::transfer_type::isochronous),
                .wMaxPacketSize = MaxIsoEndpointSize,
                .bInterval = 0x01
            }
        };
    };

    /**
     * @brief USB camera class that uses a video type to allow writing video frames to the host
     * 
     * @tparam VideoType 
     */
    template <typename VideoType>
    class camera {
    protected:
        // device descriptor for the hid keyboard
        const __attribute__((aligned(4))) static inline descriptor::device device = {
            .bcdUSB = static_cast<uint16_t>(setup::usb_version::usb_v1_1),
            .bDeviceClass = descriptor::class_type::application_specific,
            .bDeviceSubClass = 0x02,
            .bDeviceProtocol = 0x01,
            .bMaxPacketSize = 0x40,
            .idVendor = 0x0B6A,
            .idProduct = 0x0040,
            .bcdDevice = 0x0100,
            .iManufacturer = static_cast<uint8_t>(VideoType::string_index::manufacturer),
            .iProduct = static_cast<uint8_t>(VideoType::string_index::product),
            .iSerialNumber = static_cast<uint8_t>(VideoType::string_index::serial),
            .bNumConfigurations = 0x1
        };

        __attribute__((aligned(4))) static inline video::probe_control probe_control = {};
        __attribute__((aligned(4))) static inline video::probe_control commit_control = {};

        // language descriptor for the keyboard
        const __attribute__((aligned(4))) static inline descriptor::string<1> language = {
            .bString = {0x0409}
        };

        // manufacturer string descriptor
        const __attribute__((aligned(4))) static inline auto manufacturer = string_descriptor("KLIB");

        // product string descriptor
        const __attribute__((aligned(4))) static inline auto product = string_descriptor("KLIB Video Camera");

        // serial number string descriptor
        const __attribute__((aligned(4))) static inline auto serial = string_descriptor("00001337");

        __attribute__((aligned(4))) static inline uint8_t video_buffer[VideoType::max_iso_endpoint_size] = {0x02, 0x80};

        __attribute__((aligned(4))) static inline uint8_t end_of_frame_marker[2] = {0x02, 0x82};

        // configuration value. Value is set in the set config function
        static inline uint8_t configuration = 0x00;

        // the current alt mode
        static inline volatile uint8_t alt_mode = 0x00;

        // a non owning buffer to the data from the user
        static inline std::span<const uint8_t> buffer = {};

        template <typename Usb>
        static usb::handshake set_current_impl(const klib::usb::setup_packet &packet) {
            // check if we have a valid packet
            if (!packet.wLength) {
                return usb::handshake::ack;
            }

            // check what request we have
            // TODO: change to enum
            const uint8_t cs = packet.wValue >> 8;

            if (cs == 0x01) {
                // start a read on the endpoint
                Usb::read(usb::status_callback<Usb>, klib::usb::usb::control_endpoint,
                    usb::endpoint_mode::in, {
                        reinterpret_cast<uint8_t*>(&probe_control),
                        klib::min(packet.wLength, sizeof(probe_control))
                    }
                );

                return usb::handshake::wait;
            }
            else if (cs == 0x02) {
                // start a read on the endpoint
                Usb::read(usb::status_callback<Usb>, klib::usb::usb::control_endpoint,
                    usb::endpoint_mode::in, {
                        reinterpret_cast<uint8_t*>(&commit_control),
                        klib::min(packet.wLength, sizeof(commit_control))
                    }
                );

                return usb::handshake::wait;
            }
            else {
                // not supported
                return usb::handshake::stall;
            }
        }

        template <typename Usb>
        static usb::handshake get_current_impl(const klib::usb::setup_packet &packet) {
            // get the endpoint this is requested from
            const uint8_t interface = packet.wIndex & 0xff;

            // get the unit this request is for
            const auto unit = static_cast<typename VideoType::unit_index>(packet.wIndex >> 8);

            // check what interface this request has
            if (interface == VideoType::config.interface0.bInterfaceNumber) {
                // video control interface requests

                // we only support the output terminal unit
                if (unit != VideoType::unit_index::output_unit) {
                    // not supported, stall
                    return usb::handshake::stall;
                }

                // status of the output unit
                alignas(4) const static uint8_t status = 0x06;

                // write the data to the control endpoint
                if (Usb::write(usb::status_callback<Usb>, usb::control_endpoint,
                    usb::endpoint_mode::in, {&status, sizeof(status)}))
                {
                    // no issue for now ack
                    return usb::handshake::wait;
                }
                else {
                    // something went wrong stall for now
                    return usb::handshake::stall;
                }
            }
            else {
                // video streaming interface requests
                const uint8_t cs = packet.wValue >> 8;

                // check if we have a probe request
                if (cs == 0x01) {
                    // overwrite anything the host might have set differently
                    probe_control.dwFrameInterval = VideoType::config.frame.dwDefaultFrameInterval;
                    probe_control.dwMaxVideoFrameSize = static_cast<uint32_t>(
                        (VideoType::config.frame.wHeight * VideoType::config.frame.wWidth * 16) / 2
                    );
                    probe_control.dwMaxPayloadTransferSize = VideoType::config.endpoint1.wMaxPacketSize;
                    probe_control.dwClockFrequency = VideoType::config.vc_interface.dwClockFrequency;

                    // write the data to the control endpoint
                    if (Usb::write(usb::status_callback<Usb>,
                        usb::control_endpoint, usb::endpoint_mode::in,
                        {reinterpret_cast<const uint8_t*>(&probe_control), klib::min(packet.wLength, sizeof(probe_control))}))
                    {
                        // no issue for now ack
                        return usb::handshake::wait;
                    }
                    else {
                        // something went wrong stall for now
                        return usb::handshake::stall;
                    }
                }
                else if (cs == 0x02) {
                    // write the data to the control endpoint
                    if (Usb::write(usb::status_callback<Usb>,
                        usb::control_endpoint, usb::endpoint_mode::in,
                        {reinterpret_cast<const uint8_t*>(&commit_control), klib::min(packet.wLength, sizeof(commit_control))}))
                    {
                        // no issue for now ack
                        return usb::handshake::wait;
                    }
                    else {
                        // something went wrong stall for now
                        return usb::handshake::stall;
                    }
                }

                // not supported, stall
                return usb::handshake::stall;
            }
        }

        template <typename Usb>
        static void status_callback(const uint8_t endpoint, const usb::endpoint_mode mode, const usb::error error_code, const uint32_t transferred) {
            // check if we have a nak. If we have do nothing
            if (error_code == usb::error::nak) {
                return;
            }

            // check if we have an error
            if (error_code != usb::error::no_error) {
                // something went wrong, stall the endpoint
                Usb::stall(endpoint, mode);

                // clear the buffer
                buffer = {};
                return;
            }

            // get the size we can transfer
            const uint32_t size = klib::min(sizeof(video_buffer) - 2, buffer.size());

            // check if we have any data left to send
            if (!size) {
                // no data left to send, clear the buffer
                buffer = {};

                // get the frame marker from the video buffer
                end_of_frame_marker[1] = (end_of_frame_marker[1] & (~0x1)) | (video_buffer[1] & 0x1);

                // send the last packet header
                Usb::write(nullptr, usb::get_endpoint(VideoType::config.endpoint1.bEndpointAddress),
                    usb::get_endpoint_mode(VideoType::config.endpoint1.bEndpointAddress), end_of_frame_marker
                );

                return;
            }

            // copy the new data to the buffer
            std::copy_n(buffer.begin(), size, &video_buffer[2]);

            // move the buffer with the size
            buffer = buffer.subspan(size);

            // start a new write operation
            Usb::write(status_callback<Usb>, endpoint, mode, {video_buffer, size + 2});
        }

    public:
        /**
         * @brief Returns if the device is configured
         *
         * @tparam Usb
         * @return true
         * @return false
         */
        template <typename Usb>
        static bool is_configured() {
            return static_cast<volatile uint8_t>(configuration) != 0;
        }

        /**
         * @brief Get the current alt mode
         *
         * @tparam Usb
         * @return uint8_t
         */
        template <typename Usb>
        static uint8_t get_alt_mode() {
            return alt_mode;
        }

        /**
         * @brief Write a frame to the host
         * 
         * @warning Undefined behavior if the endpoint is busy. Can take multiple ms
         * based on the size of the frame.
         *
         * @tparam Usb
         * @param data
         */
        template <typename Usb>
        static void write(std::span<const uint8_t> data) {
            // store the span into our local buffer
            buffer = data;

            // flip the bits to mark we have a new frame
            video_buffer[1] ^= 0x01;

            // use the callback to start the write operation
            status_callback<Usb>(
                usb::get_endpoint(VideoType::config.endpoint1.bEndpointAddress),
                usb::get_endpoint_mode(VideoType::config.endpoint1.bEndpointAddress),
                usb::error::no_error, 0
            );
        }

        /**
         * @brief Returns if the endpoint is busy
         * 
         * @tparam Usb 
         * @return busy
         */
        template <typename Usb>
        static bool is_busy() {
            // return if the endpoint is busy
            return (!buffer.empty()) || Usb::is_pending(
                usb::get_endpoint(VideoType::config.endpoint1.bEndpointAddress),
                usb::get_endpoint_mode(VideoType::config.endpoint1.bEndpointAddress)
            );
        }

    public:
        /**
         * @brief static functions needed for the usb stack. Should not
         * be called manually
         *
         */

        /**
         * @brief Init function. Called when the usb stack is initalized
         *
         * @tparam Usb
         */
        template <typename Usb>
        static void init() {
            // make sure the endpoint supports interrupt endpoints
            static_assert(Usb::template is_valid_endpoint<
                VideoType::interrupt_endpoint,
                klib::usb::descriptor::transfer_type::interrupt
            >(), "invalid interrupt selected");

            static_assert(Usb::template is_valid_endpoint<
                VideoType::iso_endpoint,
                klib::usb::descriptor::transfer_type::isochronous
            >(), "invalid isochronous selected");
        }

        /**
         * @brief Get the configuration of the config. Needed
         * for some hardware
         * 
         * @tparam Usb
         * @return uint16_t 
         */
        template <typename Usb>
        static uint8_t get_configuration() {
            return VideoType::config.configuration.bConfigurationValue;
        }

        /**
         * @brief Called when the host is disconnected
         *
         * @tparam Usb
         */
        template <typename Usb>
        static void disconnected() {
            // clear all the variables to default
            configuration = 0x00;
        }

        /**
         * @brief Called when a bus reset has occured
         *
         * @tparam Usb
         */
        template <typename Usb>
        static void bus_reset() {
            // clear all the variables to default
            configuration = 0x00;
        }

        /**
         * @brief Clear a feature on the device
         *
         * @warning only remote wakeup is supported
         *
         * @tparam Usb
         * @param feature
         * @param packet
         */
        template <typename Usb>
        static usb::handshake clear_feature(const klib::usb::setup::feature feature, const klib::usb::setup_packet &packet) {
            // check if we support the feature
            if (feature != klib::usb::setup::feature::remote_wake) {
                // we only support remote wakeup
                return usb::handshake::stall;
            }

            // ack the packet
            return usb::handshake::ack;
        }

        /**
         * @brief Set a feature on the device
         *
         * @warning only remote wakeup is supported
         *
         * @tparam Usb
         * @param feature
         * @param packet
         */
        template <typename Usb>
        static usb::handshake set_feature(const klib::usb::setup::feature feature, const klib::usb::setup_packet &packet) {
            // check if we support the feature
            if (feature != klib::usb::setup::feature::remote_wake) {
                // we only support remote wakeup
                return usb::handshake::stall;
            }

            // ack the packet
            return usb::handshake::ack;
        }

        /**
         * @brief Get the descriptor for the descriptor type and index
         *
         * @tparam Usb
         * @param packet
         * @param type
         * @param index
         * @return usb::description
         */
        template <typename Usb>
        static usb::description get_descriptor(const setup_packet &packet, descriptor::descriptor_type type, const uint8_t index) {
            // check if we have a default usb descriptor
            switch (type) {
                case descriptor::descriptor_type::device:
                    // return the device descriptor
                    return to_description(device, device.bLength);
                case descriptor::descriptor_type::configuration:
                    // return the whole configuration descriptor (total size is in
                    // wTotalLength of the configuration)
                    return to_description(VideoType::config, VideoType::config.configuration.wTotalLength);
                case descriptor::descriptor_type::string:
                    // check what string descriptor to send
                    switch (static_cast<VideoType::string_index>(index)) {
                        case VideoType::string_index::language:
                            return to_description(language, language.bLength);
                        case VideoType::string_index::manufacturer:
                            return to_description(manufacturer, manufacturer.bLength);
                        case VideoType::string_index::product:
                            return to_description(product, product.bLength);
                        case VideoType::string_index::serial:
                            return to_description(serial, serial.bLength);
                        default:
                            // unknown string descriptor
                            break;
                    }
                default:
                    // unkown default descriptor. Might be a class descriptor
                    break;
            }

            // unkown get descriptor call. return a nullptr and 0 size
            return {nullptr, 0};
        }

        /**
         * @brief Get the configuration value set in the set config call
         *
         * @tparam Usb
         * @param packet
         */
        template <typename Usb>
        static usb::handshake get_config(const klib::usb::setup_packet &packet) {
            // send the configuration back to the host
            const auto result = Usb::write(
                usb::status_callback<Usb>, usb::control_endpoint,
                usb::endpoint_mode::in,
                {&configuration, sizeof(configuration)}
            );

            // check if something went wrong already
            if (!result) {
                // something went wrong stall
                return usb::handshake::stall;
            }

            // we do not ack here as the status callback
            // will handle this for us
            return usb::handshake::wait;
        }

        /**
         * @brief Set a configuration value
         *
         * @tparam Usb
         * @param packet
         */
        template <typename Usb>
        static usb::handshake set_config(const klib::usb::setup_packet &packet) {
            // check if the set is the same as the configuration we have stored
            if (packet.wValue == VideoType::config.configuration.bConfigurationValue) {
                // configure the endpoints
                Usb::configure(
                    usb::get_endpoint(VideoType::config.endpoint0.bEndpointAddress),
                    usb::get_endpoint_mode(VideoType::config.endpoint0.bEndpointAddress),
                    usb::get_transfer_type(VideoType::config.endpoint0.bmAttributes),
                    VideoType::config.endpoint0.wMaxPacketSize
                );
                Usb::configure(
                    usb::get_endpoint(VideoType::config.endpoint1.bEndpointAddress),
                    usb::get_endpoint_mode(VideoType::config.endpoint1.bEndpointAddress),
                    usb::get_transfer_type(VideoType::config.endpoint1.bmAttributes),
                    VideoType::config.endpoint1.wMaxPacketSize
                );

                // store the configuration value
                configuration = packet.wValue;

                // notify the usb driver we are configured
                Usb::configured(true);
            }
            else if (packet.wValue == 0) {
                // notify the usb driver we are not configured anymore
                Usb::configured(false);

                // reset the used endpoint if we have one
                if (configuration) {
                    // reset the endpoint
                    Usb::reset(
                        usb::get_endpoint(VideoType::config.endpoint0.bEndpointAddress),
                        usb::get_endpoint_mode(VideoType::config.endpoint0.bEndpointAddress)
                    );

                    // reset the endpoint
                    Usb::reset(
                        usb::get_endpoint(VideoType::config.endpoint1.bEndpointAddress),
                        usb::get_endpoint_mode(VideoType::config.endpoint1.bEndpointAddress)
                    );
                }

                // clear the configuration value
                configuration = 0x00;

                // ack the packet
                return usb::handshake::ack;
            }
            else {
                // not sure what to do, stall
                return usb::handshake::stall;
            }

            return usb::handshake::ack;
        }

        /**
         * @brief Get the device status. Called when the status is requested
         *
         * @tparam Usb
         * @return uint8_t
         */
        template <typename Usb>
        static uint8_t get_device_status() {
            // TODO: implement the correct get device status using the remote wakeup and self powered flags
            return 0;
        }

        /**
         * @brief Called when set interface is received
         *
         * @tparam Usb
         * @param packet
         * @return usb::handshake
         */
        template <typename Usb>
        static usb::handshake set_interface(const klib::usb::setup_packet &packet) {
            // check if the packet is following the spec
            if (packet.wIndex != 0x0001) {
                // invalid packet
                return usb::handshake::stall;
            }

            // set the alt mode
            alt_mode = packet.wValue & 0xff;

            // if not needed this function can be removed
            return usb::handshake::ack;
        }

        /**
         * @brief Called when a class specific packet is received
         *
         * @tparam Usb
         * @param packet
         */
        template <typename Usb>
        static usb::handshake handle_class_packet(const klib::usb::setup_packet &packet) {
            // convert the request to a class request
            const auto& request = static_cast<video::class_request>(packet.bRequest);

            // check what requets we got
            switch (request) {
                case video::class_request::set_cur:
                    return set_current_impl<Usb>(packet);
                case video::class_request::set_cur_all:
                    // we do not support set cur all at the moment
                    return usb::handshake::stall;
                case video::class_request::get_cur:
                case video::class_request::get_def:
                case video::class_request::get_min:
                case video::class_request::get_max:
                    return get_current_impl<Usb>(packet);
                case video::class_request::get_info:
                case video::class_request::get_res:
                case video::class_request::get_len:
                    // dont respond to these requests
                    return usb::handshake::stall;
                case video::class_request::get_cur_all:
                case video::class_request::get_min_all:
                case video::class_request::get_max_all:
                case video::class_request::get_res_all:
                case video::class_request::get_def_all:
                default:
                    // not supported. stall
                    return usb::handshake::stall;
            }
        }
    };
}


#endif