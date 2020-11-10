#include <drivers/screen.h>
#include <std/string.h>
#include <core/io/ports.h>

#define VGA_WIDTH       80
#define VGA_HEIGHT      24

bool cStaticTerminalDriver::Init() {
    if (IsInitialized()) return true;
    
    this->ppFrameBuffer = (byte*) C_MEMORY_VGA_VIDEO_ADDRESS;
    this->peColorScheme = TerminalFromFGBG(eTerminalColor::LIGHT_GREY, eTerminalColor::BLACK);
    this->puiCurrentColumn = 0;
    this->puiCurrentRow = 0;

    this->pbInitialized = true;
    return true;
}

bool cStaticTerminalDriver::Destroy() {
    this->pbInitialized = false;
    return true;
}

void cStaticTerminalDriver::Print(cstring sStr) {
    for (memsize_t uiIndex = 0; uiIndex < strlen(sStr); uiIndex++) {
        Print(sStr[uiIndex]);
    }
}

uint32_t cStaticTerminalDriver::OffsetFromCoords(uint32_t uiX, uint32_t uiY) {
    return 2 * (uiY * VGA_WIDTH + uiX);
}

eTerminalColor cStaticTerminalDriver::TerminalFromFGBG(eTerminalColor eForeground, eTerminalColor eBackground) {
    return (eTerminalColor) (eForeground | eBackground << 4);
}

void cStaticTerminalDriver::Print(char cCharacter) {
    Init();

    if (cCharacter == '\n') {
        this->puiCurrentColumn = 0;
        if (++this->puiCurrentRow >= VGA_HEIGHT) this->puiCurrentRow = 0;
        return;
    } else if (cCharacter == '\t') {
        this->puiCurrentColumn = this->puiCurrentColumn + 4 - (this->puiCurrentColumn % 4);
        return;
    } else if (cCharacter == '\r') {
        this->puiCurrentColumn = 0;
        return;
    }

    const uint32_t uiOffset = OffsetFromCoords(this->puiCurrentColumn, this->puiCurrentRow);

    this->ppFrameBuffer[uiOffset] = cCharacter;
    this->ppFrameBuffer[uiOffset + 1] = this->peColorScheme;

    // scrolling
    if (uiOffset > VGA_HEIGHT * VGA_WIDTH * 2) {
        for (memsize_t uiIndex = 1; uiIndex < VGA_HEIGHT; uiIndex++) {
            memcpy(
                    (void *) (C_MEMORY_VGA_VIDEO_ADDRESS + (2 * uiIndex * VGA_WIDTH)),
                    (const void *) (C_MEMORY_VGA_VIDEO_ADDRESS + (2 * (uiIndex - 1) * VGA_WIDTH)),
                    2 * VGA_WIDTH
                  );
        }

        cstring sLastLine = (cstring) (2 * (VGA_HEIGHT) * VGA_WIDTH + C_MEMORY_VGA_VIDEO_ADDRESS);
        for (uint32_t uiIndex = 0; uiIndex < VGA_WIDTH * 2; uiIndex++) {
            sLastLine[uiIndex] = 0;
        }

        this->puiCurrentRow--;
    }

    this->puiCurrentColumn++;
    if (this->puiCurrentColumn >= VGA_WIDTH) {
        this->puiCurrentColumn = 0;
        this->puiCurrentRow++;
    }
}

void cStaticTerminalDriver::SetColoring(eTerminalColor eColor) {
    this->peColorScheme = eColor;
}

void cStaticTerminalDriver::Clear() {
    for (uint32_t y = 0; y < VGA_HEIGHT; y++) {
        for (uint32_t x = 0; x < VGA_WIDTH; x++) {
            Print(' ');
        }
    }
    this->puiCurrentColumn = 0;
    this->puiCurrentRow = 0;
}