SEARCH_DIR(.);

/*
Format configurations
*/
OUTPUT_FORMAT("elf32-littlearm", "elf32-bigarm", "elf32-littlearm");
OUTPUT_ARCH(arm);


/* 
The stack size used by the application. NOTE: you need to adjust according to your application. 
*/
STACK_SIZE = 0x4000;

/*
Memories definitions
*/
MEMORY
{
    rom (rx) : org = 0x00000000, len = 256k
    /* Special ram section on the code bus*/
    ramx (rwx) : org = 0x04000000, len = 32k 
    /* ram0, ram1 and ram2 in one continuous block */
    ram0 (rwx) : org = 0x20000000, len = 64k * 3 
    /* ram1 (rwx) : org = 0x20010000, len = 64k */
    /* ram2 (rwx) : org = 0x20020000, len = 64k */
    /* the lpc55s66 does not have ram3 */
    /* ram3 (rwx) : org = 0x20030000, len = 64k */
    ram4 (rwx) : org = 0x20040000, len = 16k
    usb-ram (rwx) : org = 0x40100000, len = 16k
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
    .text :
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
