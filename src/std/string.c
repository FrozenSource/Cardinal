#include <std/string.h>

memsize_t CStrLength(const cstring sStr) {
    memsize_t ret = 0;
    while (sStr[ret] != 0) ret++;
    return ret;
}

void CStrReverse(const cstring sStr) {
    memsize_t c, i, j;
    for (i = 0, j = CStrLength(sStr) - 1; i < j; i++, j--) {
        c = sStr[i];
        sStr[i] = sStr[j];
        sStr[j] = c;
    }
}