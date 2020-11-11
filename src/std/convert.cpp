#include <std/convert.h>
#include <std/string.h>

char* itoa(int32_t iValue, int32_t iBase) {
    static char itoaBuffer[256];

    int32_t i = 0;
    int32_t sign;

    if (iValue == 0)
    {
        itoaBuffer[i++] = '0';
        itoaBuffer[i] = '\0';
        return &itoaBuffer[0];
    }

    if ((sign = iValue) < 0 && iBase == 10) {
        iValue = -iValue;
    }

    do {
        int32_t iMod = iValue % iBase;
        itoaBuffer[i++] = (iMod < 10) ? iMod + '0' : iMod + 'a' - 10;
    } while ((iValue /= iBase) > 0);

    if (sign < 0) {
        itoaBuffer[i++] = '-';
    }

    itoaBuffer[i] = '\0';
    reverse(itoaBuffer);
    return &itoaBuffer[0];
}