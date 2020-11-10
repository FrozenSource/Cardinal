#include <std/convert.h>
#include <std/string.h>

char* itoa(int32_t iValue) {
    static char itoaBuffer[256];

    int32_t i = 0;
    int32_t sign;

    if ((sign = iValue) < 0) {
        iValue = -iValue;
    }

    do {
        itoaBuffer[i++] = iValue % 10 + '0';
    } while ((iValue /= 10) > 0);

    if (sign < 0) {
        itoaBuffer[i++] = '-';
    }
    itoaBuffer[i] = '\0';

    reverse(itoaBuffer);
    return &itoaBuffer[0];
}