/* macro to define the vectors section. Should be pointed to the correct memory region */
#define VECTORS() \
    .vectors (READONLY) : \
    { \
        . = ALIGN(4); \
        /* vector table */ \
        KEEP(*(.vectors .vectors.*)); \
        . = ALIGN(4); \
    }
