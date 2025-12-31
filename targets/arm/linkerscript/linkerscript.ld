/* helpers to combine strings with a parameter */
#define COMBINE_STRX(a,b) a##b
#define COMBINE_STR2(a,b) COMBINE_STRX(a,b)
#define COMBINE_STR3(a,b,c) COMBINE_STR2(COMBINE_STR2(a,b),c)

/* macro to define the vectors section. Should be pointed to the correct memory region */
#define VECTORS() \
    .vectors (READONLY) : \
    { \
        . = ALIGN(4); \
        /* vector table */ \
        KEEP(*(.vectors .vectors.*)); \
        . = ALIGN(4); \
    }

/* macro to define the rodata section */
#define RODATA() \
    .rodata (READONLY) : \
    { \
        . = ALIGN(4); \
        /* Constands, strings, etc */ \
        *(.rodata .rodata.* .gnu.linkonce.r.*); \
        . = ALIGN(4); \
    }

/* macro to define a init array. Array name is passed as an argument */
#define CONSTRUCTOR_SECTION(name) \
    .name : \
    { \
        . = ALIGN(4); \
        PROVIDE(COMBINE_STR3(__,name,_start) = .); \
        KEEP(*(SORT(.name.*))) \
        KEEP(*(SORT(.name))) \
        PROVIDE(COMBINE_STR3(__,name,_end) = .); \
    }
