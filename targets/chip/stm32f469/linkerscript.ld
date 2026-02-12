SEARCH_DIR(.);

/*
Format configurations
*/
OUTPUT_FORMAT("elf32-littlearm", "elf32-bigarm", "elf32-littlearm");
OUTPUT_ARCH(arm);


/* 
The stack size used by the application. NOTE: you need to adjust according to your application. 
*/
STACK_SIZE = 0x2000;

/*
Memories definitions
*/
MEMORY
{
    rom (rx) : org = 0x00000000, len = 2m
    ccm (rwx) : org = 0x10000000, len = 64k
    /* ram0, ram1 and ram2 in one continuous block */
    ram0 (rwx) : org = 0x20000000, len = 320k
    /* ram1 (rwx) : org = 0x20028000, len = 32k */
    /* ram2 (rwx) : org = 0x20030000, len = 128k */

    /* fmc memory regions, len is the maximum. Not the actual size */
    fmc0 (rwx) : org = 0x60000000, len = 512m
    fmc1 (rwx) : org = 0x80000000, len = 512m
    fmc2 (rwx) : org = 0xa0000000, len = 512m
    fmc3 (rwx) : org = 0xd0000000, len = 512m
}

/*
Entry point
*/
ENTRY( __reset_handler );

/*
Sections
*/
SECTIONS
{
    /* Vector table. Has the initial stack pointer and the initial 
       structure for the arm interrupts */
    VECTORS() > rom

    /* Text segment, stores all user code */
    .text (READONLY) :
    {
        . = ALIGN(4);
        /* text segment */
        *(.text .text.* .gnu.linkonce.t.*);
        /* glue arm to thumb code, glue thumb to arm code*/
        *(.glue_7t .glue_7);
        /* c++ exceptions */
        *(.eh_frame .eh_frame_hdr)
        /* exception unwinding information */
        
        . = ALIGN(4);
        *(.ARM.extab* .gnu.linkonce.armextab.*);
        *(.ARM.exidx*)

        . = ALIGN(4);
        KEEP(*(.init));
        . = ALIGN(4);
        KEEP(*(.fini));

        . = ALIGN(4);
    } > rom

    /* Read only data */
    RODATA() > rom

    /* Support C constructors, and C destructors in both user code
       and the C library. This also provides support for C++ code. */    
    CONSTRUCTOR_SECTION(preinit_array) > rom
    CONSTRUCTOR_SECTION(init_array) > rom
    CONSTRUCTOR_SECTION(fini_array) > rom

    /* Stack segment */
    STACK(STACK_SIZE) > ram0

    /* Data that needs to be initialized to a value different than 0 */
    .data :
    {
        . = ALIGN(4);
        PROVIDE(__data_init_start = LOADADDR(.data));
        PROVIDE(__data_start = .);

        . = ALIGN(4);
        *(SORT_BY_ALIGNMENT(.data))
        *(SORT_BY_ALIGNMENT(.data.*))
        *(SORT_BY_ALIGNMENT(.gnu.linkonce.d.*))
        . = ALIGN(4);
        
        PROVIDE(__data_end = .);
        PROVIDE(__data_init_end = LOADADDR(.data));
    } > ram0 AT > rom

    /* Data that needs to be initialized to 0 */
    .bss (NOLOAD) :
    {
        . = ALIGN(4);
        PROVIDE(__bss_start = .);

        *(SORT_BY_ALIGNMENT(.bss))
        *(SORT_BY_ALIGNMENT(.bss.*))
        *(SORT_BY_ALIGNMENT(.gnu.linkonce.b.*))
        *(COMMON);
        . = ALIGN(4);

        PROVIDE(__bss_end = .);
    } > ram0

    /* fmc memory */
    DATA_SECTION(fmc0, fmc0, rom)
    DATA_SECTION(fmc1, fmc1, rom)
    DATA_SECTION(fmc2, fmc2, rom)
    DATA_SECTION(fmc3, fmc3, rom)
    BSS_SECTION(fmc0, fmc0)
    BSS_SECTION(fmc1, fmc1)
    BSS_SECTION(fmc2, fmc2)
    BSS_SECTION(fmc3, fmc3)

    /* create a table to initialize the data sections on a secondary memory */
    DATA_MULTISECTION_TABLE(
        DATA_SECTION_ENTRY(fmc0)
        DATA_SECTION_ENTRY(fmc1)
        DATA_SECTION_ENTRY(fmc2)
        DATA_SECTION_ENTRY(fmc3)
        DATA_SECTION_ENTRY_END()
    ) > rom

    /* create a table to initialize the bss sections on a secondary memory */
    BSS_MULTISECTION_TABLE(
        BSS_SECTION_ENTRY(fmc0)
        BSS_SECTION_ENTRY(fmc1)
        BSS_SECTION_ENTRY(fmc2)
        BSS_SECTION_ENTRY(fmc3)
        BSS_SECTION_ENTRY_END()
    ) > rom

    /* Heap segment */
    .heap (NOLOAD) :
    {
        . = ALIGN(4);
        PROVIDE(__heap_start = .);
        PROVIDE(__heap_end = (ORIGIN(ram0) + LENGTH(ram0)));
    } > ram0

    /* Remove information from the standard libraries */
    /DISCARD/ :
    {
        libc.a ( * )
        libm.a ( * )
        libgcc.a ( * )
        *(.note.GNU-stack)
    }

    .ARM.attributes 0 : { KEEP(*(.ARM.attributes)) }    
}
