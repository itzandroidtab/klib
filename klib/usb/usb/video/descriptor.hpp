#ifndef KLIB_USB_VIDEO_DESCRIPTOR_HPP
#define KLIB_USB_VIDEO_DESCRIPTOR_HPP

#include <cstdint>

#include <klib/usb/usb/descriptor.hpp>

// Push the current pack to the stack and set the pack to 1
// as all these structs have specific sizes
#pragma pack(push, 1)

namespace klib::usb::video {
    /**
     * @brief Video interface class code
     *
     */
    enum class interface_type {
        video = 0x0e,
    };

    /**
     * @brief Video interface subclass codes
     *
     */
    enum class interface_subtype {
        undefined = 0x00,
        videocontrol = 0x01,
        videostreaming = 0x02,
        video_interface_collection = 0x03
    };

    /**
     * @brief Class specific protocol codes
     *
     */
    enum class protocol_code: uint8_t {
        undefined = 0x00,
        protocol_15 = 0x01,
    };

    /**
     * @brief Class specific descriptors
     *
     */
    enum class class_type: uint8_t {
        undefined = 0x20,
        cs_device = 0x21,
        cs_configuration = 0x22,
        cs_string = 0x23,
        cs_interface = 0x24,
        cs_endpoint = 0x25,
    };

    /**
     * @brief Class specific requests for the handle class packet function
     *
     */
    enum class class_request: uint8_t {
        undefined = 0x00,
        set_cur = 0x01,
        set_cur_all = 0x11,
        get_cur = 0x81,
        get_min = 0x82,
        get_max = 0x83,
        get_res = 0x84,
        get_len = 0x85,
        get_info = 0x86,
        get_def = 0x87,
        get_cur_all = 0x91,
        get_min_all = 0x92,
        get_max_all = 0x93,
        get_res_all = 0x94,
        get_def_all = 0x97,
    };

    /**
     * @brief Class specific USB terminal types
     *
     */
    enum class usb_terminal_type: uint16_t {
        vendor_specific = 0x0100,
        streaming = 0x0101,
    };

    /**
     * @brief Class specific input terminal types
     *
     */
    enum class input_terminal_type: uint16_t {
        vendor_specific = 0x0200,
        camera = 0x0201,
        media_transport_input = 0x0202,
    };

    /**
     * @brief Class specific output terminal types
     *
     */
    enum class output_terminal_type: uint16_t {
        vendor_specific = 0x0300,
        display = 0x0301,
        media_transport_output = 0x0302,
    };

    /**
     * @brief Class specific external terminal types
     *
     */
    enum class external_terminal_type: uint16_t {
        vendor_specific = 0x0400,
        composite_connector = 0x0401,
        svideo_connector = 0x0402,
        component_connector = 0x0403,
    };

    /**
     * @brief Video endpoint specific subtype (VE)
     *
     */
    enum class endpoint_subtype: uint8_t {
        undefined = 0x00,
        general = 0x01,
        endpoint = 0x02,
        interrupt = 0x03,
    };

    /**
     * @brief Class specific control selectors
     *
     */
    enum class vc_interface_control_selectors {
        undefined = 0x00,
        power_mode = 0x01,
        request_error_code = 0x02,
    };
}

namespace klib::usb::video::detail {
    /**
     * @brief Video constrol specific subtype (VC)
     *
     */
    enum class control_subtype: uint8_t {
        undefined = 0x00,
        header = 0x01,
        input_terminal = 0x02,
        output_terminal = 0x03,
        selector_unit = 0x04,
        processing_unit = 0x05,
        extension_unit = 0x06,
        encoding_unit = 0x07,
    };

    /**
     * @brief Video streaming specific subtype (VS)
     *
     */
    enum class streaming_subtype: uint8_t {
        undefined = 0x00,
        input_header = 0x01,
        output_header = 0x02,
        still_image_frame = 0x03,
        format_uncompressed = 0x04,
        frame_uncompressed = 0x05,
        format_mjpeg = 0x06,
        frame_mjpeg = 0x07,
        format_mpeg2ts = 0x0a,
        format_dv = 0x0c,
        colorformat = 0x0d,
        format_frame_based = 0x10,
        frame_frame_based = 0x11,
        format_stream_based = 0x12,
        format_h264 = 0x13,
        frame_h264 = 0x14,
        format_vp8 = 0x16,
        frame_vp8 = 0x17,
        format_vp8_simulcast = 0x18,
    };

    /**
     * @brief Base for all the video control descriptors
     *
     */
    template <typename Base, control_subtype SubType>
    struct vc_header: public descriptor::cs_interface<Base, static_cast<uint8_t>(SubType)> {};

    /**
     * @brief Base for all the video control descriptors
     *
     */
    template <typename Base, streaming_subtype SubType>
    struct vs_header: public descriptor::cs_interface<Base, static_cast<uint8_t>(SubType)> {};
}

namespace klib::usb::video {
    /**
     * @brief class sepcific video control interface descriptor
     *
     */
    struct cs_video_control_interface {
        // header of the descriptor
        const detail::vc_header<cs_video_control_interface, detail::control_subtype::header> header;

        // revision of the class specification
        uint16_t bcdUVC;

        // total size of the class specific descriptors
        uint16_t wTotalLength;

        // device will provide timestamps based on this
        // value (is deprecated)
        uint32_t dwClockFrequency;

        // number of streaming interfaces
        uint8_t bInCollection;

        // Video streaming interface that belongs to
        // this video control interface
        uint8_t baInterfaceNr;
    };

    static_assert(sizeof(cs_video_control_interface) == 0x0d, "Video control interface descriptor size is wrong");

    /**
     * @brief input terminal descriptor
     *
     */
    struct input_terminal {
        // header of the descriptor
        const detail::vc_header<input_terminal, detail::control_subtype::input_terminal> header;

        // a non-zero constant that uniquely identifies
        // the terminal within the video function. This
        // value is uqed in all requests to address this
        // terminal
        uint8_t bTerminalID;

        // terminal type
        uint16_t wTerminalType;

        // id of the output terminal that is associated
        // with this terminal
        uint8_t bAssocTerminal;

        // index of a string descriptor that describes
        // this terminal
        uint8_t iTerminal;
    };

    static_assert(sizeof(input_terminal) == 0x08, "Input terminal descriptor size is wrong");

    /**
     * @brief camera input terminal descriptor
     *
     */
    struct camera_input_terminal {
        // header of the descriptor
        const detail::vc_header<camera_input_terminal, detail::control_subtype::input_terminal> header;

        // a non-zero constant that uniquely identifies
        // the terminal within the video function. This
        // value is uqed in all requests to address this
        // terminal
        uint8_t bTerminalID;

        // terminal type
        uint16_t wTerminalType;

        // id of the output terminal that is associated
        // with this terminal
        uint8_t bAssocTerminal;

        // index of a string descriptor that describes
        // this terminal
        uint8_t iTerminal;

        // the value of Lmin if optical zoom is
        // supported. If not supported it should
        // be zero
        uint16_t wObjectiveFocalLengthMin;

        // the value of Lmax if optical zoom is
        // supported. If not supported it should
        // be zero
        uint16_t wObjectiveFocalLengthMax;

        // the value of Locular if optical zoom
        // is supported. If not supported it
        // should be zero
        uint16_t wOcularFocalLength;

        // the bmControls size
        const uint8_t bControlSize = 3;

        // bitfield with supported controls (1 = enabled)
        // b[0] = scanning mode
        // b[1] = auto-exposure mode
        // b[2] = auto-exposure priority
        // b[3] = exposure time (absolute)
        // b[4] = exposure time (relative)
        // b[5] = focus (absolute)
        // b[6] = focus (relative)
        // b[7] = iris (absolute)
        // b[8] = iris (relative)
        // b[9] = zoom (absolute)
        // b[10] = zoom (relative)
        // b[11] = pantilt (absolute)
        // b[12] = pantilt (relative)
        // b[13] = roll (absolute)
        // b[14] = roll (relative)
        // b[15] = reserved
        // b[16] = reserved
        // b[17] = focus, auto
        // b[18] = privacy
        // b[19] = focus, simple
        // b[20] = window
        // b[21] = region of interest
        // b[22..24] = reserved
        uint32_t bmControls: 24;
    };

    static_assert(sizeof(camera_input_terminal) == 0x12, "Camera input terminal descriptor size is wrong");

    /**
     * @brief output terminal descriptor
     *
     */
    struct output_terminal {
        // header of the descriptor
        const detail::vc_header<output_terminal, detail::control_subtype::output_terminal> header;

        // a non-zero constant that uniquely identifies
        // the terminal within the video function. This
        // value is uqed in all requests to address this
        // terminal
        uint8_t bTerminalID;

        // terminal type
        uint16_t wTerminalType;

        // id of the output terminal that is associated
        // with this terminal
        uint8_t bAssocTerminal;

        // index of a string descriptor that describes
        // this terminal
        uint8_t bSourceID;

        // index of a string descriptor that describes
        // this terminal
        uint8_t iTerminal;
    };

    static_assert(sizeof(output_terminal) == 0x09, "Input terminal descriptor size is wrong");

    /**
     * @brief selector unit descriptor
     *
     */
    template <uint8_t NrInPins>
    struct selector_unit {
        // header of the descriptor
        const detail::vc_header<selector_unit<NrInPins>, detail::control_subtype::selector_unit> header;

        // a non-zero constant that uniquely identifies
        // the terminal within the video function. This
        // value is uqed in all requests to address this
        // terminal
        uint8_t bUnitID;

        // number of input pins
        const uint8_t bNrInPins = NrInPins;

        // arary with ids where this selector
        // unit is connected to
        uint8_t baSourceID[NrInPins];

        // index of a string descriptor that describes
        // this selector unit
        uint8_t iSelector;
    };

    static_assert(sizeof(selector_unit<2>) == 0x08, "Selector unit descriptor size is wrong");

    /**
     * @brief Processing unit descriptor
     *
     */
    struct processing_unit {
        // header of the descriptor
        const detail::vc_header<processing_unit, detail::control_subtype::processing_unit> header;

        // a non-zero constant that uniquely identifies
        // the terminal within the video function. This
        // value is uqed in all requests to address this
        // terminal
        uint8_t bUnitID;

        // id of the unit or terminal this processing
        // unit is connected to
        uint8_t bSourceID;

        // if digital mulitplier control is supported this
        // field indicates the maximum digital magnification
        // multiplied by 100. If not supported this needs
        // to be set to zero
        uint16_t wMaxMultiplier;

        // size of the bmControls field
        const uint8_t bControlSize = 3;

        // bitfield with all control that is supported by
        // the video stream (1 = enabled)
        // b[0] = brightness
        // b[1] = contrast
        // b[2] = hue
        // b[3] = saturation
        // b[4] = sharpness
        // b[5] = gamma
        // b[6] = white balance temperature
        // b[7] = white balance component
        // b[8] = backlight compensation
        // b[9] = gain
        // b[10] = power line frequency
        // b[11] = hue, auto
        // b[12] = white balance temperature, auto
        // b[13] = white balance component, auto
        // b[14] = digital multiplier
        // b[15] = digital multiplier limit
        // b[16] = analog video standard
        // b[17] = analog video lock status
        // b[18] = contrast, auto
        // b[19..23] = reserved
        uint32_t bmControls: 24;

        // index of a string descriptor that describes
        // this processing unit
        uint8_t iProcessing;

        // bitmap with all analog standards supported
        // by this processing unit
        // b[0] = none
        // b[1] = ntsc - 525/60
        // b[2] = pal - 625/50
        // b[3] = secam - 625/50
        // b[4] = ntsc - 625/50
        // b[5] = pal - 525/60
        // b[6..7] = reserved
        uint8_t bmVideoStandards;
    };

    static_assert(sizeof(processing_unit) == 0x0d, "Processing unit descriptor size is wrong");

    /**
     * @brief Video streaming header descriptor
     *
     */
    struct video_streaming_header {
        // header of the descriptor
        const detail::vs_header<video_streaming_header, detail::streaming_subtype::input_header> header;

        // number of video payload format descriptors follow
        // the current descriptor
        uint8_t bNumFormats;

        // total length in bytes of data returned
        uint16_t wTotalLength;

        // endpoint address
        // b[0..3] = endpoint number
        // b[4..6] = reserved, set to 0
        // b[7] = direction
        // 0x0 = out
        // 0x1 = in
        uint8_t bEndpointAddress;

        // indicates the capabilities of this videostreaming
        // interface
        // b[0] = dynamic format change supported
        // b[1..7] = reserved
        uint8_t bmInfo;

        // the terminal id of the output terminal where
        // the video endpoint is connected to
        uint8_t bTerminalLink;

        // method of still image capture
        // 0x00 = none (host will not support any form
        // of still image capture)
        // 0x01 = method 1
        // 0x02 = method 2
        // 0x03 = method 3
        uint8_t bStillCaptureMethod;

        // hardware trigger support
        // b[0] = trigger support
        // 0b00 = not supported
        // 0b01 = supported
        // b[1..7] = reserved
        uint8_t bTriggerSupport;

        // specifies how the host software responds
        // to a hardware trigger interrupt
        uint8_t bTriggerUsage;

        // size of the bmControls field
        const uint8_t bControlSize = 1;

        // bitfield with controls that are supported
        // b[0] = wKeyFrameRate
        // b[1] = wPFrameRate
        // b[2] = wCompQuality
        // b[3] = wCompWindowSize
        // b[4] = Generate key frame
        // b[5] = Update frame segment
        // b[6..7] = reserved
        uint32_t bmaControls: 8;
    };

    static_assert(sizeof(video_streaming_header) == 0x0e, "Video streaming header descriptor size is wrong");

    /**
     * @brief MJPEG video format descriptor
     *
     */
    struct video_mjpeg_format {
        // header of the descriptor
        const detail::vs_header<video_mjpeg_format, detail::streaming_subtype::format_mjpeg> header;

        // index of this format descriptor
        uint8_t bFormatIndex;

        // number of frame descriptors following
        // that use this format
        uint8_t bNumFrameDescriptors;

        // Specifies characteristics of this format
        // b[0] = fixedSizeSamples
        // b[1..7] = reserved
        uint8_t bmFlags;

        // optimum frame index (used for resolution)
        // in this stream
        uint8_t bDefaultFrameIndex;

        // x dimension of the picture aspect ratio
        uint8_t bAspectRatioX;

        // y dimension of the picture aspect ratio
        uint8_t bAspectRatioY;

        // interlace information
        // b[0] = interalced stream or variable
        // b[1] = fields per frame
        // 0x0 = 2 fields
        // 0x1 = 1 field
        // b[2] = field 1 first
        // b[3] = reserved
        // b[4..5] = field pattern
        // 0b00 = field 1 only
        // 0b01 = field 2 only
        // 0b10 = regular pattern of fields 1 and 2
        // 0b11 = random pattern of fields 1 and 2
        // b[6..7] = reserved
        uint8_t bmInterlaceFlags;

        // specifies if the stream should not be
        // duplicated
        // b[0] = no restrictions
        // b[1] = restrict duplication
        // b[2..7] = reserved
        uint8_t bCopyProtect;
    };

    static_assert(sizeof(video_mjpeg_format) == 0x0b, "Video MJPEG format descriptor size is wrong");

    /**
     * @brief uncompressed video format descriptor
     *
     */
    struct video_uncompressed_format {
        // header of the descriptor
        const detail::vs_header<video_uncompressed_format, detail::streaming_subtype::format_uncompressed> header;

        // index of this format descriptor
        uint8_t bFormatIndex;

        // number of frame descriptors following
        // that use this format
        uint8_t bNumFrameDescriptors;

        // globally unique identifier used to
        // identify the stream-encoding format
        uint8_t guidFormat[16];

        // number of bits per pixel used to with
        // this format
        uint8_t bBitsPerPixel;

        // optimum frame index (used for resolution)
        // in this stream
        uint8_t bDefaultFrameIndex;

        // x dimension of the picture aspect ratio
        uint8_t bAspectRatioX;

        // y dimension of the picture aspect ratio
        uint8_t bAspectRatioY;

        // interlace information
        // b[0] = interalced stream or variable
        // b[1] = fields per frame
        // 0x0 = 2 fields
        // 0x1 = 1 field
        // b[2] = field 1 first
        // b[3] = reserved
        // b[4..5] = field pattern
        // 0b00 = field 1 only
        // 0b01 = field 2 only
        // 0b10 = regular pattern of fields 1 and 2
        // 0b11 = random pattern of fields 1 and 2
        // b[6..7] = reserved
        uint8_t bmInterlaceFlags;

        // specifies if the stream should not be
        // duplicated
        // b[0] = no restrictions
        // b[1] = restrict duplication
        // b[2..7] = reserved
        uint8_t bCopyProtect;
    };

    static_assert(sizeof(video_uncompressed_format) == 0x1b, "Video uncompressed format descriptor size is wrong");

    /**
     * @brief Continuous frame intervals
     *
     */
    struct continuous_frame_intervals {
        // shortest frame interval supported (at highest frame
        // rate) in 100ns units
        uint32_t dwMinFrameInterval;

        // longest frame interval supported (at highest frame
        // rate) in 100ns units
        uint32_t dwMaxFrameInterval;

        // indicates granularity of the frame interval range
        // in 100ns units
        uint32_t dwFrameIntervalStep;
    };

    static_assert(sizeof(continuous_frame_intervals) == 0x0c, "Continuous frame intervals size is wrong");

    /**
     * @brief Discrete frame intervals. Needs to match the
     * bFrameIntervalType parameter in the frame
     *
     * @tparam Amount
     */
    template <uint8_t Amount>
    struct discrete_frame_intervals {
        // frame interval supported in 100 ns units
        uint32_t dwFrameInterval[Amount];
    };

    static_assert(sizeof(discrete_frame_intervals<3>) == 0x0c, "Discrete frame intervals size is wrong");

    /**
     * @brief MJPEG video frame descriptor
     *
     * @tparam FrameInterval
     */
    template <typename FrameInterval>
    struct video_mjpeg_frame {
        // header of the descriptor
        const detail::vs_header<video_mjpeg_frame<FrameInterval>, detail::streaming_subtype::frame_mjpeg> header;

        // index of this format descriptor
        uint8_t bFrameIndex;

        // bitfield with the capabilities
        // b[0] = still image supported
        // b[1] = fixed frame-rate
        // b[2..7] = reserved
        uint8_t bmCapabilities;

        // width of the decoded bitmap frame in pixels
        uint16_t wWidth;

        // height of the decoded bitmap frame in pixels
        uint16_t wHeight;

        // minimum bitrate
        uint32_t dwMinBitRate;

        // maxmimum bitrate
        uint32_t dwMaxBitRate;

        // specifies the maximum number of bytes for a
        // video (or still frame) the compressor will
        // produce (is deprecated)
        uint32_t dwMaxVideoFrameBufSize;

        // specifies the frame interval the device would
        // like to use
        uint32_t dwDefaultFrameInterval;

        // indicates how the fram e interval can be
        // programmed.
        // 0 = continous frame interval
        // 1..255 = the number of discrete frame intervals
        // supported
        uint8_t bFrameIntervalType;

        // the frame interval
        FrameInterval interval;
    };

    static_assert(sizeof(video_mjpeg_frame<continuous_frame_intervals>) == 0x26, "Video mjpeg frame descriptor size is wrong");

    /**
     * @brief Video color format descriptor
     * 
     */
    struct color_format {
        // header of the descriptor
        const detail::vs_header<color_format, detail::streaming_subtype::colorformat> header;

        // color primaries
        // 0x00 = unspecified
        // 0x01 = BT.709, sRGB
        // 0x02 = BT.470-2 (M)
        // 0x03 = BT.470-2 (B, G)
        // 0x04 = BT.SMPTE 170M
        // 0x05 = BT.SMPTE 240M
        uint8_t bColorPrimaries;

        // optoelectronic transfer characteristic of the source picture also called the gamma function
        // 0x00 = unspecified
        // 0x01 = BT.709, sRGB
        // 0x02 = BT.470-2 (M)
        // 0x03 = BT.470-2 (B, G)
        // 0x04 = BT.SMPTE 170M
        // 0x05 = BT.SMPTE 240M
        // 0x06 = Linear (V = LC)
        // 0x07 = sRGB
        uint8_t bTransferCharacteristics;

        // Matrix used to compute luma and chroma values from the color primaries.
        // 0x00 = unspecified
        // 0x01 = BT.709,
        // 0x02 = FCC
        // 0x03 = BT.470-2 (B, G)
        // 0x04 = SMPTE 170M
        // 0x05 = SMPTE 240M
        uint8_t bMatrixCoefficients;
    };

    static_assert(sizeof(color_format) == 0x06, "Color format descriptor size is wrong");
}

namespace klib::usb::video {
    /**
     * @brief Result to a get info request
     *
     */
    struct get_info {
        // bitfields what is supported
        // b[0] = supports get value requests
        // b[1] = supports set value requests
        // b[2] = disabled due to automatic mode
        // b[3] = autoupdate control
        // b[4] = asynchronous control
        // b[5] = disabled due to incompatibilty
        // with commit state
        // b[6..7] = reserved
        uint8_t info;
    };

    /**
     * @brief Probe control request response
     *
     */
    struct probe_control {
        // bitfield what is supported
        // b[0] = dwFrameInterval
        // b[1] = wKeyFrameRate
        // b[2] = wPFrameRate
        // b[3] = wCompQuality
        // b[4] = wCompWindowSize
        // b[5..15] = reserved
        uint16_t bmHint;

        // video format index
        uint8_t bFormatIndex;

        // video frame index from a frame descriptor
        uint8_t bFrameIndex;

        // frame interval in 100ns units
        uint32_t dwFrameInterval;

        // key frame rate in key-frame per video
        // frame units
        uint16_t wKeyFrameRate;

        // PFrame rate in PFrame/key frame units
        uint16_t wPFrameRate;

        // Compression quality control in abstract
        // units 1 (lowest) to 10000 (highest)
        uint16_t wCompQuality;

        // window size for average bit rate control
        uint16_t wCompWindowSize;

        // internal video streaming interface latency
        // in ms
        uint16_t wDelay;

        // maximum video frame or codec specific
        // segment size in bytes
        uint32_t dwMaxVideoFrameSize;

        // specifies the maximum number of bytes that
        // the device can transmit or receive in a
        // single payload transfer
        uint32_t dwMaxPayloadTransferSize;

        // the devcie clock frequency in hz for the
        // specified format
        uint32_t dwClockFrequency;

        // bitfield controlling the following values
        // b[0] = Frame ID is required in payload header
        // b[1] = End of frame field may be present
        // b[2] = End of slice field may be present
        // b[3..7] = reserved
        uint8_t bmFramingInfo;

        // the preferred payload format
        uint8_t bPreferedVersion;

        // the minimum payload format version supported
        // by the device for the bformatIndex
        uint8_t bMinVersion;

        // the maximum payload format version supported
        // by the device for the bformatIndex
        uint8_t bMaxVersion;

        // the current bUsage
        // 1..8 = real time modes
        // 9..16 = broadcast modes
        // 17..24 = File storage modes
        // 25..31 = MultiView modes
        // 32..255 = reserved
        uint8_t bUsage;

        // represents bit depth luma
        uint8_t bBitDepthLuma;

        // bitfield of flags that are control specific
        uint8_t bmSettings;

        // host indicates the maximum number of frames
        // stored
        uint8_t bMaxNumberOfRefFramesPlus1;

        // a field with 4x 4-bit subfields
        uint16_t bmRateControlModes;

        // a field with 4x 16-bit subfields
        uint16_t bmLayoutPerStream[4];
    };

    static_assert(sizeof(probe_control) == 0x30, "Invalid probe control type");
}

// release the old pack so the rest of the structs are not
// affected by the pack(1)
#pragma pack(pop)

#endif