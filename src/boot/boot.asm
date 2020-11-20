; cf. https://intermezzos.github.io/book/hello-world.html

global start
global long_mode_start

; Some constants used for multiboot header
; No need to understand, they are just a bunch of flags and magic values for the bootloader to find and recognize it as a multibootkernel
FLAGS       equ  0
MAGIC       equ  0xe85250d6
; Some constants for loading higher half kernel
VM_BASE     equ 0xC0000000
PDE_INDEX   equ (VM_BASE >> 22)
PSE_BIT     equ 0x00000010
PG_BIT      equ 0x80000000

section .multiboot
header_start:
    ; `dd` means 'define double word'
    dd MAGIC	
    dd FLAGS
    dd header_end - header_start ; header length
    ; checksum
    dd 0x100000000 - (MAGIC + 0 + (header_end - header_start))
    ; required end tag
    ; `dw` means 'define word' (word = 16 bits on x86_64)
    dw 0    ; type
    dw 0    ; flags
    dd 8    ; size
header_end:

section .data
align 4096
global TEMP_PAGE_DIRECTORY
TEMP_PAGE_DIRECTORY:
    ; Map the first 4mb physical memory to first 4mb virtual memory. Otherwise, when paging is enabled, eip points to, 0x100004 for example, and MMU is not gonna know how to translate 
    ; this address into phsyical mem address, because our PDE doesn't tell MMU how to find it.
    dd 0x00000083
    times(PDE_INDEX - 1) dd 0
    dd 0x00000083
    times(1024 - PDE_INDEX - 1) dd 0 

; block started by symbol
section .bss
align 4096
p4_table:
    ; `resb` means 'reserves bytes'
    resb 4096
p3_table:
    resb 4096
p2_table:
    resb 4096

section .stack, nobits
align 4
stack_bottom:
    ; 1mb of uninitialized data(1024*1024=104856)
    resb 104856
stack_top:

section .rodata
gdt64:
    ; `dq` means 'define quad-word'
    dq 0
.code: equ $ - gdt64
    dq (1<<44) | (1<<47) | (1<<41) | (1<<43) | (1<<53)
.data: equ $ - gdt64
    dq (1<<44) | (1<<47) | (1<<41)
.pointer:
    dw .pointer - gdt64 - 1
    dq gdt64

section .text
bits 32
start:
	mov esp, stack_top
	; pass magix

	; `ebx` points to a boot information structure.
	; We move it to `esi` to pass it to our kernel.
	mov esi, ebx
	; `eax` should contain the multiboot2 magic number.
	mov edi, eax

	call check_cpuid
	call check_long_mode

	call set_up_page_tables
	call enable_paging

    lgdt [gdt64.pointer]

	jmp gdt64.code:long_mode_start

    ; should not be reached
    hlt

; cf. http://wiki.osdev.org/Setting_Up_Long_Mode#Detection_of_CPUID
check_cpuid:
    ; Check if CPUID is supported by attempting to flip the ID bit (bit 21)
    ; in the FLAGS register. If we can flip it, CPUID is available.

    ; Copy FLAGS in to EAX via stack
    pushfd
    pop eax

    ; Copy to ECX as well for comparing later on
    mov ecx, eax

    ; Flip the ID bit
    xor eax, 1 << 21

    ; Copy EAX to FLAGS via the stack
    push eax
    popfd

    ; Copy FLAGS back to EAX (with the flipped bit if CPUID is supported)
    pushfd
    pop eax

    ; Restore FLAGS from the old version stored in ECX (i.e. flipping the
    ; ID bit back if it was ever flipped).
    push ecx
    popfd

    ; Compare EAX and ECX. If they are equal then that means the bit
    ; wasn't flipped, and CPUID isn't supported.
    cmp eax, ecx
    je .no_cpuid
    ret
.no_cpuid:
    mov al, "1"
    jmp error

check_long_mode:
    ; test if extended processor info in available
    mov eax, 0x80000000    ; implicit argument for cpuid
    cpuid                  ; get highest supported argument
    cmp eax, 0x80000001    ; it needs to be at least 0x80000001
    jb .no_long_mode       ; if it's less, the CPU is too old for long mode

    ; use extended info to test if long mode is available
    mov eax, 0x80000001    ; argument for extended processor info
    cpuid                  ; returns various feature bits in ecx and edx
    test edx, 1 << 29      ; test if the LM-bit is set in the D-register
    jz .no_long_mode       ; If it's not set, there is no long mode
    ret
.no_long_mode:
    mov al, "2"
    jmp error

set_up_page_tables:	
    ; cf. http://os.phil-opp.com/modifying-page-tables.html
	; required to implement recursive mapping (paging)
	mov eax, p4_table
	or eax, 0b11 ; present + writable
	mov [p4_table + 511 * 8], eax

    ; Point the first entry of the level 4 page table to the first entry in the
    ; p3 table
    mov eax, p3_table
    or eax, 0b11
    mov dword [p4_table + 0], eax

    ; Point the first entry of the level 3 page table to the first entry in the
    ; p2 table
    mov eax, p2_table
    or eax, 0b11
    mov dword [p3_table + 0], eax

    ; point each page table level two entry to a page
    mov ecx, 0         ; counter variable
.map_p2_table:
    mov eax, 0x200000  ; 2MiB
    mul ecx
    or eax, 0b10000011
    mov [p2_table + ecx * 8], eax

    inc ecx
    cmp ecx, 512
    jne .map_p2_table

    ret

enable_paging:
    ; move page table address to cr3
    mov eax, p4_table
    mov cr3, eax

    ; enable PAE
    mov eax, cr4
    or eax, 1 << 5
    mov cr4, eax

    ; set the long mode bit
    mov ecx, 0xC0000080
    rdmsr
    or eax, 1 << 8
    wrmsr

    ; enable paging
    mov eax, cr0
    or eax, 1 << 31
    or eax, 1 << 16
    mov cr0, eax
	ret

; Prints `ERR: ` and the given error code to screen and hangs.
; parameter: error code (in ascii) in al
error:
	mov dword [0xb8000], 0x4f524f45
	mov dword [0xb8004], 0x4f3a4f52
	mov dword [0xb8008], 0x4f204f20
	mov byte  [0xb800a], al
	hlt

bits 64
activateSSE:
	mov rax, cr0
	and ax, 0b11111101
	or  ax, 0b00000001
	mov cr0, rax

	mov rax, cr4
	or ax,  0b1100000000
	mov cr4, rax
	ret

long_mode_start:
	call activateSSE

	; load 0 into all data segment registers
	mov ax, 0
	mov ss, ax
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax

    extern kmain
	call kmain

	; should not happen
	hlt