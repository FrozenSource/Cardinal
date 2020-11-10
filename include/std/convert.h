#pragma once

#include <std/types.h>

cstring itoa(int iValue);

template <typename T>
cstring dtoa(T value, uint8_t uiDecimals) {
    static char dtoaBuffer[258]; // max uint8_t + '.' + \0

    cstring sBuffer = dtoaBuffer;
    cstring sMajor = itoa((int)value);

    if (value < 0) value *= -1;

    while (*sMajor != 0) {
        *sBuffer++ = *sMajor++;
    }

    if (uiDecimals > 0) {
        *sBuffer++ = '.';
        value -= (int)value;

        for (uint8_t uiIndex = 0; uiIndex < uiDecimals; uiIndex++) {
            value *= 10;
            *sBuffer++ = (int) value + 48;
            value -= (int) value;
        }
    }
    *sBuffer = 0;

    return dtoaBuffer;
}