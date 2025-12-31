#include <stdint.h>

#pragma pack(push, 1)

/**
 * @brief Struct for memory segments that are loaded after the startup code is run
 * 
 */
struct data_memory_segment_t {
    const uint32_t *rom_start;
    uint32_t *start;
    uint32_t *end;
};

/**
 * @brief Struct for bss memory segments that are cleared after the startup code is run
 * 
 */
struct bss_memory_segment_t {
    uint32_t *start;
    uint32_t *end;
};

#pragma pack(pop)

extern "C" {
    // multisection data segment symbol
    extern const data_memory_segment_t __multisection_data_start;

    // multisection bss segment symbol
    extern const bss_memory_segment_t __multisection_bss_start;
}

namespace klib::entry {
    void secondary_memory_loader() {
        // loop over all data segments
        for (const data_memory_segment_t *segment = &__multisection_data_start; ; segment++) {
            // check if we have reached the end of the segments
            if (!segment->start && !segment->end && !segment->rom_start) {
                // invalid segment
                break;
            }

            // get the length of the segment in elements
            const uint32_t length = (segment->end - segment->start);

            // check if we have any length to copy
            if (!length) {
                continue;
            }

            // copy rom to ram
            for (uint32_t i = 0; i < length; i++) {
                ((volatile uint32_t*)segment->start)[i] = segment->rom_start[i];
            }
        }

        // loop over all bss segments
        for (const bss_memory_segment_t *segment = &__multisection_bss_start; ; segment++) {
            // check if we have reached the end of the segments
            if (!segment->start && !segment->end) {
                // invalid segment
                break;
            }

            // get the length of the segment in elements
            const uint32_t length = (segment->end - segment->start);

            // check if we have any length to copy
            if (!length) {
                continue;
            }

            // clear the bss segment
            for (uint32_t i = 0; i < length; i++) {
                ((volatile uint32_t*)segment->start)[i] = 0x00;
            }
        }
    }
}