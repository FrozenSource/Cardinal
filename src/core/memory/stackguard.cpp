#include <std/types.h>
#include <std/io.h>

//TODO: Make this value randomized
#define STACK_CHK_GUARD 0x595e9fbd94fda766

uint64_t __stack_chk_guard = STACK_CHK_GUARD;

/**
 * Basic stack smashing protector implementation
 * Based on https://wiki.osdev.org/Stack_Smashing_Protector
 */
C_FUNCTION void __stack_chk_fail(void) {
    printf("Stack Smashing Detected", 0);
}