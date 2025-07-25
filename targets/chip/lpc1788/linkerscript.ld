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
    rom (rx) : org = 0x00000000, len = 512k
    ram (rwx) : org = 0x10000000, len = 64k
    ram1 (rwx) : org = 0x20000000, len = 32k
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
    .vectors :
    {
        . = ALIGN(4);
        /* vector table */
        KEEP(*(.vectors .vectors.*));
        . = ALIGN(4);
    } > rom

    .crp 0x000002fc :
    {
        /* keep this even if it is not used. This prevents the CRP 
           from being enabled 
           
           Warning: setting this value to any of the following will
           disable parts/everything of the debug interface

           CRP1 = 0x12345678
           CRP2 = 0x87654321
           CRP3 = 0x43218765
         */
        LONG(0x00)
    } > rom

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
    .rodata : 
    {
        . = ALIGN(4);

        /* Constands, strings, etc */
        *(.rodata .rodata.* .gnu.linkonce.r.*);

        . = ALIGN(4);
    } > rom

    /* Support C constructors, and C destructors in both user code
       and the C library. This also provides support for C++ code. */    
    .preinit_array :
    {
        . = ALIGN(4);
        PROVIDE(__preinit_array_start = .);

        KEEP(*(SORT(.preinit_array.*)))
        KEEP(*(SORT(.preinit_array)))

        PROVIDE(__preinit_array_end = .);
    } > rom

    .init_array :
    {
        . = ALIGN(4);
        PROVIDE(__init_array_start = .);

        KEEP(*(SORT(.init_array.*)))
        KEEP(*(SORT(.init_array)))

        PROVIDE(__init_array_end = .);
    } > rom

    .fini_array :
    {
        . = ALIGN(4);
        PROVIDE(__fini_array_start = .);

        KEEP(*(SORT(.fini_array.*)))
        KEEP(*(SORT(.fini_array)))
        
        PROVIDE(__fini_array_end = .);
    } > rom

    /* Stack segment */
    .stack (NOLOAD) :
    {
        . = ALIGN(4);
        PROVIDE(__stack_start = .);

        . = . + STACK_SIZE;
        . = ALIGN(4);
        
        PROVIDE(__stack_end = .);
    } > ram

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
    } > ram AT > rom

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
    } > ram

    .periph :
    {
        . = ALIGN(4);
        PROVIDE(__periph_start = .);

        *(SORT_BY_ALIGNMENT(.periph))
        *(SORT_BY_ALIGNMENT(.periph.*))
        . = ALIGN(4);

        PROVIDE(__periph_end = .);
    } > ram1

    /* Heap segment */
    .heap (NOLOAD) :
    {
        . = ALIGN(4);
        PROVIDE(__heap_start = .);
        PROVIDE(__heap_end = (ORIGIN(ram) + LENGTH(ram)));
    } > ram

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
