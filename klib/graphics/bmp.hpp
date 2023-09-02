#ifndef KLIB_BMP_HPP
#define KLIB_BMP_HPP

#include <cstdint>

namespace klib::detail::bmp {
    // Push the current pack to the stack and set the pack to 1
    // as all these structs have specific sizes
    #pragma pack(push, 1)
    /**
     * @brief 
     * 
     */
    struct header {
        // magic identifier (should be 0x4d42);
        uint16_t type;

        // size of the file in bytes
        uint32_t size;

        // reserved fields
        uint16_t reserved1;
        uint16_t reserved2;

        // offset to the data in the image. 
        uint32_t offset;

        // DIB header size
        uint32_t dib_header_size;

        // width of the image
        int32_t width_px;

        // height of the image
        int32_t height_px;

        // the amount of planes
        uint16_t num_planes;

        // amount of bits per pixel
        uint16_t bits_per_pixel;

        // compression type
        uint32_t compression;

        // size of the image in bytes
        uint32_t image_size_bytes;

        // y pixels per meter
        int32_t x_resolution_ppm;

        // y pixels per meter
        int32_t y_resolution_ppm;

        // number of colors
        uint32_t num_colors;

        // important colors
        uint32_t important_colors;
    };

    // release the old pack so the rest of the structs are not 
    // affected by the pack(1)
    #pragma pack(pop)

    // make sure the bmp header is 54 bytes big
    static_assert(sizeof(header) == 54, "BMP header size is invalid");
}

#endif