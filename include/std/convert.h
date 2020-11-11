#pragma once

#include <std/types.h>
#include <std/string.h>

template <typename T>
cstring itoa(T iValue, uint32_t uiBase) {
    static char itoaBuffer[256];

    uint32_t uiIndex = 0;
    T iOriginal;

    if (iValue == 0)
    {
        itoaBuffer[uiIndex++] = '0';
        itoaBuffer[uiIndex] = '\0';
        return &itoaBuffer[0];
    }

    if ((iOriginal = iValue) < 0 && uiBase == 10) {
        iValue = -iValue;
    }

    do {
        uint32_t iMod = iValue % uiBase;
        itoaBuffer[uiIndex++] = (iMod < 10) ? iMod + '0' : iMod + 'a' - 10;
    } while ((iValue /= uiBase) > 0);

    if (iOriginal < 0 && uiBase == 10) {
        itoaBuffer[uiIndex++] = '-';
    }

    itoaBuffer[uiIndex] = '\0';
    reverse(itoaBuffer);
    return &itoaBuffer[0];
}

template <typename T>
cstring dtoa(T value, uint8_t uiDecimals) {
    static char dtoaBuffer[258]; // max uint8_t + '.' + \0

    cstring sBuffer = dtoaBuffer;
    cstring sMajor = itoa((int)value, 10);

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

enum eByteSize : uint8_t {
    BYTES = 0,
    KILOBYTES = 1,
    MEGABYTES = 2,
    GIGABYTES = 3,
    TERABYTES = 4,
    PETABYTES = 5,
    ETABYTES = 6,
    AUTO = 7
};

cstring formatBytes(uint64_t ulSize, uint8_t uiDecimals = 2, eByteSize eMaxFormat = eByteSize::AUTO);