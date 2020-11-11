#include <std/convert.h>
#include <core/memory/memory.h>

cstring formatBytes(uint64_t ulSize, uint8_t uiDecimals, eByteSize eMaxFormat) {
	static char buffer[256];

    const char* suffixes[7];
    suffixes[0] = "B";
    suffixes[1] = "KB";
    suffixes[2] = "MB";
    suffixes[3] = "GB";
    suffixes[4] = "TB";
    suffixes[5] = "PB";
    suffixes[6] = "EB";
    uint64_t s = 0; // which suffix to use
    double count = ulSize;
    while (count >= 1024 && s < eMaxFormat)
    {
        s++;
        count /= 1024;
    }

	cstring sCount = dtoa(count, uiDecimals);
	memsize_t uiSize = strlen(sCount);
	memcpy(buffer, sCount, uiSize);
	memsize_t uiSuffixSize = strlen((cstring) suffixes[s]);
	memcpy(buffer + uiSize, suffixes[s], uiSuffixSize);
	buffer[uiSize + uiSuffixSize] = 0;
	return (&buffer[0]);
}