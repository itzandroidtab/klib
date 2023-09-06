#ifndef KLIB_VIRTUAL_FILESYSTEM_HPP
#define KLIB_VIRTUAL_FILESYSTEM_HPP

#include <cstdint>

namespace klib::filesystem {
    /**
     * @brief fat file attributes
     * 
     */
    class attributes {
    public:
        enum {
            ATTR_READ_ONLY = 0x01,
            ATTR_HIDDEN = 0x02,
            ATTR_SYSTEM = 0x04,
            ATTR_VOLUME_ID = 0x08,
            ATTR_DIRECTORY = 0x10,
            ATTR_ARCHIVE = 0x20,
            ATTR_LONG_NAME = (ATTR_READ_ONLY | ATTR_HIDDEN | ATTR_SYSTEM | ATTR_VOLUME_ID)
        };
    };

    // Push the current pack to the stack and set the pack to 1
    // as all these structs have specific sizes
    #pragma pack(push, 1)

    /**
     * @brief Fat boot sector as described in the fat specification
     * 
     */
    struct fat_boot_sector {
        // jump instruction to boot code
        uint8_t bootjmp[3];

        // OEM name identifier
        uint8_t oem_name[8];

        // DOS 2.0 BPB (bios parameter block) 11 bytes

        // count of bytes per sector. This value may take on 
        // only the following values: 512, 1024, 2048, 4096
        uint16_t bytes_per_sector;

        // number of sectors per allocation unit. This value
        // must be a power of 2 that is greater than 0. The
        // legal values are 1, 2, 4, 8, 16, 32, 64, 128
        uint8_t sectors_per_cluster;

        // number of reserved sectors in the reserved region 
        // of the volume starting at the first sector of the
        // volume
        uint16_t reserved_sector_count;

        // the count of the file allocation tables (FATs) on
        // the volume. A value of 2 is recommended
        uint8_t num_fats;

        // root entry count (only for FAT12 and FAT16) for 
        // FAT32 this field must be 0
        uint16_t root_entry_count;

        // old 16-bit total count of the sectors on the volume.
        // This count includes the count of all the sectors in 
        // all four regions of the volume. This field can be 0
        // but then the total_sectors32 must be non zero. For
        // FAT32 this field must be 0. For FAT12 and FAT16 this 
        // field contains the sector count if it fits and if it
        // does not fit it should be 0
        uint16_t total_sectors16;

        // media type. legal values are 0xf0, 0xf8, 0xf9, 0xfa, 
        // 0xfb, 0xfc, 0xfd, 0xfe, and 0xff. 
        // 0xf8 is the standard value foor "fixed" (non-removable)
        // media. For removable media 0xf0 is frequently used
        uint8_t media_type;

        // on FAT12 and FAT16 this field should have a count of 
        // sectors occupied by one FAT. On FAT32 volumes this 
        // field must be 0 and fat_size32 contains the FAT size
        // count
        uint16_t fat_size16;

        // DOS 3.31 BPB (bios parameter block) 12 bytes

        // amount of sectors per track. Is only relevant for media
        // that have a geometry (broken down into tracks by 
        // multiple heads and cylinders)
        uint16_t sectors_per_track;

        // number of heads for interrupt 0x13
        uint16_t head_count;

        // Count of hidden sectors preceding the partition that 
        // contains this FAT volume. (must always be 0 for media
        // that is not partitioned)
        uint32_t hidden_sector_count;

        // 32-bit field for the total count of sectors on the
        // volume. This count includes the count of all the sectors
        // in all four regions of the volume
        uint16_t total_sectors32;

        // Extended BIOS parameter block 26 bytes 

        // extended section. Casted to correct type when FAT type
        // is known at runtime
        uint8_t extended_section[54];
    };
 
    /**
    * @brief Fat directory entry as described in the fat specification
     * 
     */
    struct fat_directory_entry {
        // short filename limited to 11 characters (8.3 format)
        uint8_t directory[11];

        // file attribute.
        uint8_t attributes;

        // reserved field. Must be 0
        uint8_t reserved;

        // component of the file creation time. Count of tenths of
        // a second (range 0 <= field <= 199)
        uint8_t creation_time_ms;

        // creation time. Granularity is 2 seconds
        uint16_t creation_time;

        // creation date
        uint16_t creation_date;
        
        // last access date. Last access is defined as a read or
        // write operation performed on the file/directory
        uint16_t accessed_date;

        // high word of first data cluster number for file/
        // directory. Only valid for FAT32 on FAT12 and FAT16 this
        // field must be 0
        uint16_t first_cluster_high_16;

        // last modification (write) time 
        uint16_t modification_time;

        // last modification (write) date
        uint16_t modification_date;

        // low word of first data cluster number for the file/
        // directory
        uint16_t first_cluster_low_16;

        // quantity containing the size in bytes for the file/
        // directory
        uint32_t filesize;        
    };

    // release the old pack so the rest of the structs are not 
    // affected by the pack(1)
    #pragma pack(pop)

}

#endif