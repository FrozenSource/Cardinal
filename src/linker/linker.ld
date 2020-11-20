/* cf. https://intermezzos.github.io/book/hello-world.html */

ENTRY(start)

SECTIONS {
    /* sets the load address of the first section to 1 MiB (convention) */
    . = 1M;

    .text ALIGN(4K) :
    {
        KEEP(*(.multiboot))
        *(.text)
    }

    /* read-only data. */
    .rodata ALIGN(4K) :
    {
        *(.rodata .rodata.*)
    }

    /* read-write data (initialized) */
    .data ALIGN(4K) :
    {
        *(.data)
    }

    /* read-write data (uninitialized) and stack */
    .bss ALIGN(4K) :
    {
        *(.COMMON)
        *(.bss)
        *(.stack)
    }

    /* get rid of unnecessary gcc bits */
    /DISCARD/ :
    {
        *(.comment)
        *(.eh_frame)
    }
}