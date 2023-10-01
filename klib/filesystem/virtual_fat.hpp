#ifndef KLIB_VIRTUAL_FILESYSTEM_HPP
#define KLIB_VIRTUAL_FILESYSTEM_HPP

#include <cstdint>
#include <type_traits>
#include <algorithm>

namespace klib::filesystem {
    /**
     * @brief fat file attributes
     * 
     */
    class attributes {
    public:
        enum : uint8_t {
            read_only = 0x01,
            hidden = 0x02,
            system = 0x04,
            volume_id = 0x08,
            directory = 0x10,
            archive = 0x20,
            long_name = (read_only | hidden | system | volume_id)
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
        uint32_t total_sectors32;

        // Extended BIOS parameter block 26 bytes 

        // extended section. Casted to correct type when FAT type
        // is known at runtime
        uint8_t extended_section[54];
    };
 
    /**
    * @brief Fat directory structure as described in the fat specification
     * 
     */
    struct fat_directory {
        // short filename limited to 11 characters (8.3 format)
        uint8_t name[11];

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

        // generate the default comparison operator
        bool operator==(const fat_directory&) const = default;
    };

    static_assert(sizeof(fat_directory) == 32, "Invalid fat directory structure size");

    // release the old pack so the rest of the structs are not 
    // affected by the pack(1)
    #pragma pack(pop)
}

namespace klib::filesystem::detail {
    template <uint32_t ClusterCount, bool fat12, bool fat32>
    class cluster {};

    // fat12 fat entry (should be used on ClusterCount <= 4084)
    template <uint32_t ClusterCount>
    class cluster<ClusterCount, true, false> {
    public:
        // the type to cast to for the value of set_cluster
        using type = uint16_t;

        // amount of bits in the cluster
        constexpr static uint32_t bits = 12;

        enum : uint16_t {
            free = 0x000,
            allocated = 0x002,
            reverved = ClusterCount + 1,
            bad_sector = 0xff7,
            reserved1 = 0xff8,
            final_cluster = 0xffff            
        };

        /**
         * @brief register a cluster in the fat
         * 
         * @param fat
         * @param index 
         * @param value 
         */
        static void set_cluster(uint8_t *const fat, const uint32_t index, const type value) {
            const uint32_t offset = index + (index / 2);

            // set the fat entry
            if (index & 0x1) {
                fat[offset] = ((value & 0xf) << 4) | (fat[offset] & 0xf);
                fat[offset + 1] = ((value >> 4) & 0xff);
            }
            else {
                fat[offset] = value & 0xff;
                fat[offset + 1] = ((value >> 8) & 0xf) | (fat[offset + 1] & 0xf0);                
            }
        }
    };

    // fat16 fat entry (ClusterCount >= 4085 && ClusterCount < 65524)
    template <uint32_t ClusterCount>
    class cluster<ClusterCount, false, false> {
    public:
        // the type to cast to for the value of set_cluster
        using type = uint16_t;

        // amount of bits in the cluster
        constexpr static uint32_t bits = 16;

        enum : uint16_t {
            free = 0x0000,
            allocated = 0x0002,
            reverved = ClusterCount + 1,
            bad_sector = 0xfff7,
            reserved1 = 0xfff8,
            final_cluster = 0xffff            
        };

        /**
         * @brief register a cluster in the fat
         * 
         * @param fat
         * @param index 
         * @param value 
         */
        static void set_cluster(uint8_t *const fat, const uint32_t index, const type value) {
            const uint32_t offset = index * 2;

            // set the fat entry
            fat[offset] = value & 0xff;
            fat[offset + 1] = value >> 8;
        }
    };

    // fat32 fat entry (should be used on ClusterCount > 65524)
    template <uint32_t ClusterCount>
    class cluster<ClusterCount, false, true> {
    public:
        // the type to cast to for the value of set_cluster
        using type = uint32_t;

        // amount of bits in the cluster
        constexpr static uint32_t bits = 32;

        enum : uint32_t{
            free = 0x0000000,
            allocated = 0x0000002,
            reverved0 = ClusterCount + 1,
            bad_sector = 0xffffff7,
            reserved1 = 0xffffff8,
            final_cluster = 0xffffffff
        };

        /**
         * @brief register a cluster in the fat
         * 
         * @param fat
         * @param index 
         * @param value 
         */
        static void set_cluster(uint8_t *const fat, const uint32_t index, const type value) {
            const uint32_t offset = index * 4;

            // set the fat entry
            fat[offset] = value & 0xff;
            fat[offset + 1] = (value >> 8) & 0xff;
            fat[offset + 2] = (value >> 16) & 0xff;
            fat[offset + 3] = (fat[offset + 3] & 0xf0) | ((value >> 24) & 0x0f);
        }
    };
}

namespace klib::filesystem {
    /**
     * @brief Virtual FAT12/FAT16 filesystem. Switches automaticly between the 
     * 2 based on the amount of clusters
     * 
     * @tparam MaxFiles Max amount of files that can be stored
     * @tparam TotalSize Total disk size
     * @tparam ClusterSize Amount of sectors per cluster
     * @tparam FatSizeLimit Allows to limit the FAT in ram. Can be used 
     * @tparam NumFats Number of fats
     * to simulate a big filesystem with low amounts of ram
     */
    template <
        typename Handler,
        uint32_t MaxFiles = 32, 
        uint32_t TotalSize = (1 * 1024 * 1024), 
        uint32_t ClusterSize = 64, 
        uint32_t FatSizeLimit = 0xffffffff,
        uint8_t NumFats = 0x01
    >
    class virtual_fat {
    public:
        // sector size of this storage type
        constexpr static uint32_t sector_size = 512;

    protected:
        constexpr static uint32_t root_entry_count = MaxFiles;

        // make sure the maxfiles template is correct
        static_assert((MaxFiles & 0xf) == 0, "Maxfiles needs to be a modulo of 16");

        // amount of fats (recommended value is 2)
        constexpr static uint8_t number_of_fats = NumFats; 

        // amount of sectors per cluster
        constexpr static uint8_t sectors_per_cluster = ClusterSize;

        // number of reserved sectors in the reserved region of the 
        // volume starting at the first sector of the volume
        constexpr static uint16_t reserved_sector_count = 0x0001; 

        // get the total amount of sectors
        constexpr static uint32_t sector_count = TotalSize / sector_size;

        // get the root directory sector count
        constexpr static uint32_t root_directory_sector_count = (
            ((root_entry_count * sizeof(fat_directory)) + (sector_size - 1)) / sector_size
        );

        // calculate the fat size or use the max fat size (calculation 
        // is not perfect. Look at FAT specification for more info)
        constexpr static uint32_t fat_size = (
            ((sector_count - (reserved_sector_count + root_directory_sector_count)) + 
            (((256 * sectors_per_cluster) + number_of_fats) - 1)) / ((256 * sectors_per_cluster) + number_of_fats)
        );

        // mbr boot sector
        constexpr static inline fat_boot_sector mbr = {
            .bootjmp = {0xeb, 0x3c, 0x90},
            .oem_name = {'M','S','D','O','S','5','.','0'},  

            // 512 bytes per sector
            .bytes_per_sector = sector_size,

            // 32k per cluster
            .sectors_per_cluster = sectors_per_cluster,
            .reserved_sector_count = reserved_sector_count,
            .num_fats = number_of_fats,
            .root_entry_count = root_entry_count,

            // set the total amount of sectors if it is less than 0xffff
            .total_sectors16 = (sector_count > 0xffff) ? 
                0x0000 : sector_count,
            
            // removable medium
            .media_type = 0xf8,

            .fat_size16 = fat_size,
            .sectors_per_track = 0x0001,
            .head_count = 0x0001,
            .hidden_sector_count = 0x00,

            // set the total amount of sectors if it is bigger than 0xffff
            .total_sectors32 = (sector_count > 0xffff) ? 
                sector_count : 0x0000,

            .extended_section = {},
        };

        // get the amount of data sectors
        constexpr static uint32_t data_sector_count = (
            sector_count - (mbr.reserved_sector_count * mbr.fat_size16) + root_directory_sector_count
        );

        // get the amount of clusters
        constexpr static uint32_t cluster_count = data_sector_count / mbr.sectors_per_cluster;

        // get the cluster values we need for the fat cluster
        using cluster = detail::cluster<cluster_count, (cluster_count <= 4084), (cluster_count > 65524)>;

        // make sure we do not have to many sectors
        static_assert(cluster_count <= 65524, "To many sectors for FAT12/FAT16. FAT32 is not supported");

        // amount of current fat clusters in use
        static inline uint32_t cluster_index = 0; 

        // fat file allocation table
        static inline uint8_t fat[sector_size * klib::min(mbr.fat_size16, FatSizeLimit) * number_of_fats] = {};

        // amount of active fat directory entries
        static inline uint32_t directory_index = 0;

        // fat directory entry
        static inline fat_directory directory[root_entry_count] = {};

        // using for the read and write callbacks
        using read_callback = void(*)(const uint32_t offset, uint8_t *const data, const uint32_t sectors);
        using write_callback = void(*)(const uint32_t offset, const uint8_t *const data, const uint32_t sectors);

        /**
         * @brief structure for virtual media
         * 
         */
        struct media {
            // media read callback
            read_callback read;

            // media write callback
            write_callback write;

            // amount of sectors the media has
            uint32_t sector_count;
        };

        // callbacks for when a file/media is requested by the host
        // (+ num_fats + mbr + root directory)
        static inline media virtual_media[root_entry_count + number_of_fats + 1 + 1] = {};

    protected:
        /**
         * @brief returns if a filename char array is following the 8.3 standard
         * 
         * @param filename 
         * @return true 
         * @return false 
         */
        static bool is_valid_filename(const uint8_t* filename) {
            // invalid characters for the 8.3 specification
            constexpr static char invalid_starting_chars[] = {
                0xe5, // deleted
                0x00, // deleted (and all following entries are free)
                0x20, // space not allowed as first character
            };

            // check for invalid starting characters
            for (uint32_t i = 0; i < sizeof(invalid_starting_chars); i++) {
                if (invalid_starting_chars[i] == filename[0]) {
                    return false;
                }
            }

            // make sure all the other characters are valid
            for (uint32_t i = 0; i < 11; i++) {
                if (!character_valid(filename[i])) {
                    return false;
                }
            }

            return true;
        }

        /**
         * @brief returns if a character is valid in a 8.3 filename
         * 
         * @param character 
         * @return true 
         * @return false 
         */
        static bool character_valid(const char character) {
            // all the invalid characters
            constexpr static char invalid_chars[] = {
                0x22, 0x2a, 0x2b, 0x2c, 0x2e, 0x2f, 0x3a, 0x3b, 
                0x3c, 0x3d, 0x3e, 0x3f, 0x5b, 0x5c, 0x5d, 0x7c
            };

            // lower case characters are not allowed
            if ((character >= 'a') && (character <= 'z')) {
                return false;
            }

            // values less than 0x20 are not allowed except 0x5
            if ((character < 0x20) && (character != 0x5)) {
                return false;
            }

            // check for special characters that are not allowed
            for (uint32_t i = 0; i < sizeof(invalid_chars); i++) {
                if (invalid_chars[i] == character) {
                    return false;
                }
            }

            return true;
        }

        /**
         * @brief Read the mbr data
         * 
         * @param offset 
         * @param data 
         * @param sectors 
         */
        static void read_mbr(const uint32_t offset, uint8_t *const data, const uint32_t sectors) {
            // the mbr is only 1 sector
            if (offset != 0) {
                // do nothing with the other sectors
                return;
            }

            // boot sector, clear any data in the buffer
            std::fill_n(data + sizeof(mbr), sector_size - sizeof(mbr), 0x00);

            // copy the mbr to the buffer
            std::copy_n(reinterpret_cast<const uint8_t*>(&mbr), sizeof(mbr), data);

            // set the signature to 0xaa55 to maintain compatibilty (i.e. with android)
            data[510] = 0x55;
            data[511] = 0xaa;
        }

        /**
         * @brief Read the fat data
         * 
         * @param offset 
         * @param data 
         * @param sectors 
         */
        template <uint32_t Fat>
        static void read_fat(const uint32_t offset, uint8_t *const data, const uint32_t sectors) {
            // read all the fat sectors
            for (uint32_t i = 0; i < sectors; i++) {
                // get the fat offset
                const uint32_t fat_offset = i + offset + (Fat * klib::min(mbr.fat_size16, FatSizeLimit));

                // check if we should read the actual fat or the "simulated fat"
                if (fat_offset >= FatSizeLimit) {
                    // fill the "simulated fat" with all zero to mark as free
                    std::fill_n(data, sector_size, 0x00);
                }
                else {
                    // read the actual fat
                    std::copy_n(
                        reinterpret_cast<const uint8_t*>(&fat) + (fat_offset * sector_size), 
                        sector_size * sectors, data
                    );
                }
            }
        }

        /**
         * @brief Called when the host is trying to read the directory 
         * structure of the filesystem
         * 
         * @param offset 
         * @param data 
         * @param sectors 
         */
        static void read_directory_structure(const uint32_t offset, uint8_t *const data, const uint32_t sectors) {
            // check if we have something to do
            if (!sectors) {
                return;
            }

            // get the amount of bytes used by the fat array
            const uint32_t size = (directory_index * sizeof(fat_directory));

            // read all the fat sectors
            for (uint32_t i = 0; i < sectors; i++) {
                // get the fat offset
                const uint32_t dir_offset = i + offset;

                // get the directory entries
                const uint32_t byte_count = klib::min(
                    ((dir_offset * sector_size) > size) ? 0 : (size - (dir_offset * sector_size)), sector_size
                );

                // check if we need to clear any bytes
                if (byte_count < sector_size) {
                    // clear the data we are not using
                    std::fill_n(data + byte_count, (sectors * sector_size) - byte_count, 0x00);
                }

                // copy the entry count of directory structures
                std::copy_n(
                    &reinterpret_cast<const uint8_t*>(&directory)[dir_offset * sector_size], 
                    byte_count, &data[i * sector_size]
                );
            }
        }

        /**
         * @brief Called when the host is trying to write to the root 
         * directory of the filesystem
         * 
         * @param offset 
         * @param data 
         * @param sectors 
         */
        static void write_directory_structure(const uint32_t offset, const uint8_t *const data, const uint32_t sectors) {
            // check if it is trying to write outside of the fat directory array
            if ((offset + sectors * sector_size) > sizeof(directory)) {
                // it is trying to write outside the directory structure exit
                return;
            }

            // get a the offset into the fat directory array
            const uint32_t fat_offset = offset * sector_size / sizeof(fat_directory);

            // get the max amount of items in the current offset
            const uint32_t entries = klib::max(
                root_entry_count - (offset * (sector_size / sizeof(fat_directory))), 0
            );

            // check if a file entry has changed (when we are at offset 0 we skip the
            // directory entry. On the other offsets we start at index 0)
            for (uint32_t i = (offset ? 1 : 0); i < entries; i++) {
                // get a reference to the received structure
                auto &n = (reinterpret_cast<const fat_directory*>(data))[i];
                auto &old = directory[fat_offset + i];

                // check if the entry has changed
                if (n == old) {
                    // the data is the same. Skip this entry
                    continue;
                }

                // check what happend with the file
                if (n.name[0] == 0xe5) {
                    // the file has been deleted
                    Handler::on_delete(old);
                }
                else if (n.name != old.name && is_valid_filename(n.name)) {
                    // a new file has been created
                    Handler::on_create(n);
                }
                else {
                    // something else changed. Let the handler check what 
                    // it is it for us
                    Handler::on_change(old, n);
                }

                // store the new data in the directory structure
                old = n;
            }
        }             

        /**
         * @brief Set the virtual media with the provided number of fats
         * 
         * @tparam NumFats 
         * @tparam Index 
         * @param offset 
         */
        template <uint32_t Index = 0>
        static void set_fat_read(uint32_t offset) {
            virtual_media[offset + Index] = {
                .read = read_fat<Index>,
                .write = nullptr,
                .sector_count = mbr.fat_size16
            };

            if constexpr (Index + 1 < NumFats) {
                return set_fat_read<Index + 1>(offset);
            }
        }

        /**
         * @brief Read write implementation that calls the corresponding
         * media callback for reading or writing
         * 
         * @tparam Read 
         * @tparam T 
         * @param sector 
         * @param data 
         * @param sectors 
         */
        template <bool Read, typename T>
        static void read_write_impl(uint32_t sector, T data, uint32_t sectors) {
            uint32_t current_sector = 0;
            uint32_t data_offset = 0;

            // check what media should handle the read
            for (uint32_t i = 0; i < directory_index; i++) {
                // get the end of the current media
                const uint32_t current_end = current_sector + virtual_media[i].sector_count;
                
                // check if the request is for the current media
                if ((sector >= current_sector) && (sector < current_end)) {
                    // get the offset in the current media
                    const uint32_t media_offset = sector - current_sector;

                    // get the maximum amount we should read in the current media
                    const uint32_t count = klib::min(current_end - sector, sectors);

                    // check if we should read or write
                    if constexpr (Read) {
                        // check if we have a read function registered
                        if (virtual_media[i].read) {
                            virtual_media[i].read(media_offset, &data[data_offset], count);
                        }
                    }
                    else {
                        // check if we have a read function registered
                        if (virtual_media[i].write) {
                            virtual_media[i].write(media_offset, &data[data_offset], count);
                        }
                    }

                    // update the sector and the amount of sectors we want to process
                    sectors -= count;
                    sector += count;

                    // move the offset we are in the ptr
                    data_offset += (count * sector_size);
                }

                // check if we have data left to process
                if (!sectors) {
                    break;
                }

                // move past the current media to go to the next one
                current_sector += virtual_media[i].sector_count;
            }             
        }

    public:
        /**
         * @brief Init the virtual filesystem. Filename should conform 
         * the 8.3 filename specification
         * 
         * @param drive_name 
         */
        static void init(const char* drive_name) {
            // set the first reserved entry to the media type. (filling the other bits to 1)
            cluster::set_cluster(fat, 0, mbr.media_type | static_cast<cluster::type>(0xfffff << 8));

            // set the second entry to the end of cluster
            cluster::set_cluster(fat, 1, cluster::final_cluster);

            // clear every other cluster entry
            for (uint32_t i = 2; i < cluster_count && i < ((sizeof(fat) * 8) / cluster::bits); i++) {
                // clear the cluster
                cluster::set_cluster(fat, i, cluster::free);
            }

            // mark we have the 2 cluster entries
            cluster_index = 2;

            // initialize the root directory
            directory[0] = {
                {}, attributes::volume_id | attributes::archive
            };

            // set the drive name in the directory
            for (uint32_t i = 0; i < sizeof(directory[0].name) && i < klib::string::strlen(drive_name); i++) {
                directory[0].name[i] = drive_name[i];
            }

            uint32_t current = 0;

            // setup the mbr read/write
            virtual_media[current++] = {
                .read = read_mbr,
                .write = nullptr,
                .sector_count = (sizeof(fat_boot_sector) + (sector_count - 1)) / sector_count
            };

            // set all the fat read functions in the virtual media
            set_fat_read(current);

            // increment the amount of media used by the fats
            current += number_of_fats;

            // setup the directory read/write
            virtual_media[current++] = {
                .read = read_directory_structure,
                .write = write_directory_structure,
                .sector_count = ((sizeof(fat_directory) * root_entry_count) + (sector_size - 1)) / sector_size
            };

            // set the amount of directory entries that are used
            directory_index = current;
        }

        /**
         * @brief Create a file in the virtual filesystem
         * 
         * @param file_name 
         * @param size 
         * @param read 
         * @param write 
         */
        static void create_file(const char* file_name, const uint32_t size, const read_callback read = nullptr, const write_callback write = nullptr) {
            // make sure we can create a file
            if ((directory_index + 1) >= (sizeof(virtual_media) / sizeof(virtual_media[0]))) {
                // we cannot create more files
                return;
            }

            const uint32_t clusters = (
                (size + ((sectors_per_cluster * sector_size) - 1)) / 
                (sectors_per_cluster * sector_size)
            );

            uint32_t first_cluster = 0;

            // check if we should allocate memory
            if (clusters > 0) {
                // check if we have enough clusters to allocate for the file
                if ((cluster_index + clusters) > ((sizeof(fat) * 8) / cluster::bits)) {
                    // we do not have enough clusters for the file exit
                    return;
                }

                // set the clusters in use
                first_cluster = cluster_index;

                // set all the clusters except the last one
                for (uint32_t i = 0; i < (clusters - 1); i++) {
                    const uint32_t index = cluster_index + i;

                    // update all the clusters
                    cluster::set_cluster(fat, index, index + 1);
                }

                // set the last cluster as end of the final cluster
                cluster::set_cluster(fat, cluster_index + (clusters - 1), cluster::final_cluster);

                // update the amount of clusters we just added
                cluster_index += clusters;
            }

            // get a reference to the current entry (- num_fats - mbr)
            fat_directory& entry = directory[directory_index - (number_of_fats + 1)];

            // set the file in the directory
            entry = {
                .name = {""},
                .attributes = (write == nullptr) ? 
                    static_cast<uint8_t>(attributes::read_only) : 
                    static_cast<uint8_t>(0x00),
                .reserved = 0x00,
                .creation_time_ms = 0x00,
                .creation_time = 0x0000,
                .creation_date = 0x0000,
                .accessed_date = 0x0000,
                .first_cluster_high_16 = static_cast<uint16_t>((first_cluster >> 16) & 0xffff),
                .modification_time = 0x0000,
                .modification_date = 0x0000,
                .first_cluster_low_16 = static_cast<uint16_t>(first_cluster & 0xffff),
                .filesize = size
            };

            // set the drive name in the directory
            for (uint32_t i = 0; i < sizeof(entry.name) && i < klib::string::strlen(file_name); i++) {
                entry.name[i] = file_name[i];
            }

            virtual_media[directory_index] = {
                .read = read,
                .write = write,
                .sector_count = clusters * sectors_per_cluster
            };

            directory_index++;
        }

        /**
         * @brief Returns the total size in bytes of the filesystem
         * 
         * @return constexpr uint32_t 
         */
        constexpr static uint32_t size() {
            // check what total sector field to use
            if (mbr.total_sectors16) {
                return mbr.total_sectors16 * mbr.bytes_per_sector;
            }
            else {
                return mbr.total_sectors32 * mbr.bytes_per_sector;
            }
        }

        /**
         * @brief Write handler
         * 
         * @param sector 
         * @param data 
         * @param sectors 
         */
        static void write(const uint32_t sector, const uint8_t *const data, const uint32_t sectors) {
            read_write_impl<false>(sector, data, sectors);
        }

        /**
         * @brief Read handler
         * 
         * @param sector 
         * @param data 
         * @param sectors 
         */
        static void read(uint32_t sector, uint8_t *const data, uint32_t sectors) {
            read_write_impl<true>(sector, data, sectors);
        }
    };
}

#endif