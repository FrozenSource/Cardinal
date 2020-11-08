#include <core/memory/memory.h>

void memset(byte* pAddress, byte bValue, memsize_t uiSize) {
    while (uiSize--)
    {
        *pAddress++ = bValue;
    }
}