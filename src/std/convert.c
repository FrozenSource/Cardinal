#include <std/convert.h>
#include <std/string.h>

void itoa(int32_t iValue, char sOut[]) {
    int32_t i = 0;
    int32_t sign;

    if ((sign = iValue) < 0) {
        iValue = -iValue;
    }

    do {
        sOut[i++] = iValue % 10 + '0';
    } while ((iValue /= 10) > 0);

    if (sign < 0) {
        sOut[i++] = '-';
    }
    sOut[i] = '\0';

    reverse(sOut);
}