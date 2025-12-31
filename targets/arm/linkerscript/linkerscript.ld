/* helpers to combine strings with a parameter */
#define COMBINE_STRX(a,b) a##b
#define COMBINE_STR2(a,b) COMBINE_STRX(a,b)
#define COMBINE_STR3(a,b,c) COMBINE_STR2(COMBINE_STR2(a,b),c)

/* helper macro to create a section */
#define SECTION_IMPL(name, data) \
    .name : \
    { \
        data \
    }

/* helper macro to create a section with a attribute */
#define SECTION_IMPL_ATTR(name, attr, data) \
    .name (attr) : \
    { \
        data \
    }

/* helper macro to create a alligned section */
#define ALLIGNED_SECTION(name, align, data) \
    SECTION_IMPL( \
        name, \
        . = ALIGN(align); \
        data \
    )

/* helper macro to create a alligned readonly section */
#define ALLIGNED_READONLY_SECTION(name, align, data) \
    SECTION_IMPL_ATTR( \
        name, \
        READONLY, \
        . = ALIGN(align); \
        data \
    )

/* macro to define the vectors section. Should be pointed to the correct memory region */
#define VECTORS() \
    ALLIGNED_READONLY_SECTION( \
        vectors, 4, \
        KEEP(*(.vectors .vectors.*)); \
    )

/* macro to define the rodata section */
#define RODATA() \
    ALLIGNED_READONLY_SECTION( \
        rodata, 4, \
        *(.rodata .rodata.* .gnu.linkonce.r.*); \
    )

/* macro to define the stack with a size in bytes */
#define STACK(size) \
    .stack (NOLOAD) : \
    { \
        . = ALIGN(4); \
        /* provide the __stack_start */ \
        PROVIDE(__stack_start = .); \
        /* add the size in bytes to the current address */ \
        . = . + size; \
        /* make sure the size is rounded off to 4 bytes  */ \
        . = ALIGN(4); \
        /* provide the __stack_end */ \
        PROVIDE(__stack_end = .); \
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
