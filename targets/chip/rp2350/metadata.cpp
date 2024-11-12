#include <cstdint>

#pragma pack(push, 1)

/**
 * @brief All the block items. 
 * 
 * @details bit 6 is used to make parity even
 * 
 */
enum class block_item_t: uint8_t {
    next_block_offset = 0x41,
    image_type = 0x42,
    vector_table = 0x03,
    entry_point = 0x44,
    rolling_window_delta = 0x05,
    load_map = 0x06,
    hash_def = 0x47,
    version = 0x48,
    signature = 0x09,
    partition_table = 0x0a,
    hash_value = 0x4b,
    salt = 0x0c,
    last_item = 0x7f,
};

/**
 * @brief Helper struct that combines a block item and the size flag
 * 
 */
struct item_t {
    // item type
    block_item_t type: 7; 

    // flag for if we have a 16-bit size (= 1) 
    // or 8-bit size (= 0)
    bool size: 1;
};

static_assert(sizeof(item_t) == 1, "Invalid item helper size");

/**
 * @brief Available image types in the header
 * 
 */
enum class image_t {
    invalid = 0x00,
    executable = 0x01,
    data = 0x02,
};

/**
 * @brief Available security modes in the header
 * 
 */
enum class security_t {
    unspecified = 0x00,
    non_secure = 0x01,
    secure = 0x02,
};

/**
 * @brief Available CPU's in the header
 * 
 */
enum class cpu_t {
    arm = 0x00,
    riscv = 0x01,
};

/**
 * @brief The chip the file is for
 * 
 */
enum class chip_t {
    rp2040 = 0x00,
    rp2350 = 0x01,
};

/**
 * @brief Helper struct with all the flags combined
 * 
 * @note Scaler order is changed so the bits are in the correct order
 * 
 */
struct __attribute__((scalar_storage_order("big-endian"))) image_flags_t {
    // the image type
    image_t type: 4;

    // security settings
    security_t security: 2;

    // padding
    uint8_t pad0: 2 = 0x00;

    // target cpu architecture
    cpu_t cpu: 3;

    // padding
    uint8_t pad1: 1 = 0x00;

    // target chip
    chip_t chip: 3;

    // flag if this firmware needs to "buy" 
    // within 16 seconds before commiting 
    // to firmware
    bool try_before_you_buy: 1;
};

static_assert(sizeof(image_flags_t) == 2, "Invalid image flags size");

/**
 * @brief Metadata struct required for the rp2350 to boot
 * 
 */
template <typename Items>
struct metadata_t {
    // start of the metadata header. Needs to be 0xffffded3
    uint32_t marker_start = 0xffffded3;

    // what type this metadata header is
    Items items;

    // relative pointer to the next header. 0x00 for the 
    // last header
    uint32_t next_header;

    // end of the metadata header. Needs to be 0xab123579
    uint32_t marker_end = 0xab123579;
};

/**
 * @brief Helper with the minimal data required 
 * to boot the rp2350
 * 
 */
struct minimal_metadata_t {
    // header of the first item
    item_t header0;

    // size of the first item (in words, 32-bits)
    uint8_t size0;

    // the data of the first item
    image_flags_t data0;

    // header of the second item (marking the end)
    item_t header1;

    // size of the second item (in words, 32-bits)
    uint16_t size1;

    // data of the second item
    uint8_t data1;
};

#pragma pack(pop)

// create the metadata struct that lives in memory for the bootloader
const metadata_t<minimal_metadata_t> metadata __attribute__ ((section(".metadata"), used)) = {
    .items = {
        .header0 = {
            .type = block_item_t::image_type,
            .size = false,
        },
        .size0 = 0x01,
        .data0 = {
            .type = image_t::executable,
            .security = security_t::secure,
            .cpu = cpu_t::arm,
            .chip = chip_t::rp2350,
            .try_before_you_buy = false,
        },
        .header1 = {
            .type = block_item_t::last_item,
            .size = true,
        },
        // the size of all the above items combined
        .size1 = 0x01,
        .data1 = 0x00,
    },

    // only 1 header
    .next_header = 0x00
};