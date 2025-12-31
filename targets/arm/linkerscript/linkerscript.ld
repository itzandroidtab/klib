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
        . = ALIGN(align); \
    )

/* helper macro to create a alligned readonly section */
#define ALLIGNED_READONLY_SECTION(name, align, data) \
    SECTION_IMPL_ATTR( \
        name, \
        READONLY, \
        . = ALIGN(align); \
        data \
        . = ALIGN(align); \
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

/* macro to define a data section. Note: this section is not automaticly loaded. 
   The secondary loaded needs to be enabled to load this at startup after the 
   startup code */
#define DATA_SECTION(name, region, storage) \
    COMBINE_STR2(.name,_data) : \
    { \
        . = ALIGN(4); \
        PROVIDE(COMBINE_STR3(__,name,_data_rom_start) = LOADADDR(COMBINE_STR2(.name,_data))); \
        PROVIDE(COMBINE_STR3(__,name,_data_start) = .); \
        *(SORT_BY_ALIGNMENT(COMBINE_STR2(.name,_data))); \
        *(SORT_BY_ALIGNMENT(COMBINE_STR2(.name,_data.*))); \
        . = ALIGN(4); \
        PROVIDE(COMBINE_STR3(__,name,_data_end) = .); \
    } > region AT > storage

/* data section entry for the multi section table */
#define DATA_SECTION_ENTRY(name) \
    LONG(COMBINE_STR3(__,name,_data_rom_start)); \
    LONG(COMBINE_STR3(__,name,_data_start)); \
    LONG(COMBINE_STR3(__,name,_data_end));

/* end marker for the multi section table */
#define DATA_SECTION_ENTRY_END() \
    LONG(0); \
    LONG(0); \
    LONG(0);

/* multisection table to automaticly load the data into the section location */
#define DATA_MULTISECTION_TABLE(entries) \
    ALLIGNED_READONLY_SECTION( \
        multisection_data, 4, \
        PROVIDE(__multisection_data_start = .); \
        entries \
        PROVIDE(__multisection_data_end = .); \
    )

#define BSS_SECTION(name, region) \
    COMBINE_STR2(.name,_bss) (NOLOAD) : \
    { \
        . = ALIGN(4); \
        PROVIDE(COMBINE_STR3(__,name,_bss_start) = .); \
        *(SORT_BY_ALIGNMENT(COMBINE_STR2(.name,_bss))); \
        *(SORT_BY_ALIGNMENT(COMBINE_STR2(.name,_bss.*))); \
        . = ALIGN(4); \
        PROVIDE(COMBINE_STR3(__,name,_bss_end) = .); \
    } > region

/* bss section entry for the multi section table */
#define BSS_SECTION_ENTRY(name) \
    LONG(COMBINE_STR3(__,name,_bss_start)); \
    LONG(COMBINE_STR3(__,name,_bss_end));

/* end marker for the multi section table */
#define BSS_SECTION_ENTRY_END() \
    LONG(0); \
    LONG(0);

/* multisection table to automaticly load all the bss sections to zero */
#define BSS_MULTISECTION_TABLE(entries) \
    ALLIGNED_READONLY_SECTION( \
        multisection_bss, 4, \
        PROVIDE(__multisection_bss_start = .); \
        entries \
        PROVIDE(__multisection_bss_end = .); \
    )
