/* We are trying to load the kernel at a high virtual address 0xC0000000, which has many benefits(but grub is gonna load it to phsical address 1MB anyway).
 * So what we do, is we first let the linker link our kernel based on the assumption that the kernel is placed on 0xC0100000(which is not true), by doing ".=0xC0100000"
 * Then, we tell the linker the change the text/data/rodata/bss's LMA(load memory address) to somewhere around 0x100000, by doing .text ALIGN(4K) : AT(ADDR(.text)-0xC0000000). 
 * The grub see this LMA and do load our kernel to 0x100000.
 * OK, when our kernel finally gets to run, it immediately enable paging and map the memory around 0x100000 to 0xC0000000
 * Guess what ? Our kernel is now in higher half ! (Yeah, in the context of virtual memory, it is)
 * 
 * You may wonder why we even bother to do all this... why not just load the kernel at 3GB ??!
 * because... not all computers have 4gb of physical memory(one of the reason I can think of).
*/

ENTRY(start)

SECTIONS {
    . = 0xC0100000;

    .text ALIGN(4K) : AT(ADDR(.text)-0xC0000000)
    {
        KEEP(*(.multiboot))
        *(.text)
    }

    /* read-only data. */
    .rodata ALIGN(4K) : AT(ADDR(.rodata)-0xC0000000)
    {
        *(.rodata)
    }

    /* read-write data (uninitialized) and stack */
    .bss ALIGN(4K) : AT(ADDR(.bss)-0xC0000000)
    {
        *(.COMMON)
        *(.bss)
        *(.stack)
    }

    /* read-write data (initialized) */
    .data ALIGN(4K) : AT(ADDR(.data)-0xC0000000)
    {
        *(.data)
    }

    /* get rid of unnecessary gcc bits */
    /DISCARD/ :
    {
        *(.comment)
        *(.eh_frame)
    }

    end = .;
}