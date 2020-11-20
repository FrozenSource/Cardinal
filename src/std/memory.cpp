#include <std/memory.h>

void memset(void* pAddress, byte bValue, memsize_t uiSize) {
    byte* pbAddress = (byte*) pAddress;
    while(uiSize--) {
        *pbAddress++ = bValue;
    }
}

void memcpy(void* pDestination, const void* pSource, memsize_t uiSize) {
    memsize_t uiIndex;

    /*
     * memcpy does not support overlapping buffers, so always do it
     * forwards. (Don't change this without adjusting memmove.)
     *
     * For speedy copying, optimize the common case where both pointers
     * and the length are word-aligned, and copy word-at-a-time instead
     * of byte-at-a-time. Otherwise, copy by bytes.
     *
     * The alignment logic below should be portable. We rely on
     * the compiler to be reasonably intelligent about optimizing
     * the divides and modulos out. Fortunately, it is.
     */

    if (uiSize % sizeof(long) == 0) {
        for (uiIndex = 0; uiIndex < uiSize/sizeof(long); uiIndex++) {
            ((long*)pDestination)[uiIndex] = ((long*)pSource)[uiIndex];
        }
    } else {
        for (uiIndex = 0; uiIndex < uiSize; uiIndex++) {
            ((byte*)pDestination)[uiIndex] = ((byte*)pSource)[uiIndex];
        }
    }
}

C_FUNCTION void memmove(void* pAddress, const void* pSource, memsize_t uiSize) {
    byte aBuffer[uiSize];

    memcpy((void*) pSource, aBuffer, uiSize);

    return memcpy(pAddress, (const void*) &aBuffer[0], uiSize);
}
