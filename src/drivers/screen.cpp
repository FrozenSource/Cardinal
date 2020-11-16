#include <drivers/screen.h>
#include <std/string.h>
#include <core/io/ports.h>

#define VGA_WIDTH 80
#define VGA_HEIGHT 25

#define FB_COMMAND_PORT 0x3D4
#define FB_DATA_PORT 0x3D5
#define FB_HIGH_BYTE_COMMAND 14
#define FB_LOW_BYTE_COMMAND 15

void EnableCursor() {
    port_byte_out(FB_COMMAND_PORT, 0x09);   // set maximum scan line register to 15
    port_byte_out(FB_DATA_PORT, 0x0F);
    port_byte_out(FB_COMMAND_PORT, 0x0B);   // set the cursor end line to 15
    port_byte_out(FB_DATA_PORT, 0x0F);
    port_byte_out(FB_COMMAND_PORT, 0x0A);   // set the cursor start line to 14 and enable cursor visibility
    port_byte_out(FB_DATA_PORT, 0x0E);
}

bool cStaticTerminalDriver::Init()
{
    if (IsInitialized())
        return true;

    EnableCursor();

    this->ppFrameBuffer = (byte *)C_MEMORY_VGA_VIDEO_ADDRESS;
    this->peColorScheme = TerminalFromFGBG(eTerminalColor::LIGHT_GREY, eTerminalColor::BLACK);
    this->puiCurrentColumn = 0;
    this->puiCurrentRow = 0;

    this->pbInitialized = true;
    return true;
}

bool cStaticTerminalDriver::Destroy()
{
    this->pbInitialized = false;
    return true;
}

void cStaticTerminalDriver::Print(cstring sStr)
{
    for (memsize_t uiIndex = 0; uiIndex < strlen(sStr); uiIndex++)
    {
        Print(sStr[uiIndex]);
    }
}

uint16_t cStaticTerminalDriver::OffsetFromCoords(uint8_t uiX, uint8_t uiY)
{
    return 2 * (uiY * VGA_WIDTH + uiX);
}

eTerminalColor cStaticTerminalDriver::TerminalFromFGBG(eTerminalColor eForeground, eTerminalColor eBackground)
{
    return (eTerminalColor)(eForeground | eBackground << 4);
}

void cStaticTerminalDriver::Print(char cCharacter)
{
    Init();

    if (cCharacter == '\n') {
        this->puiCurrentColumn = 0;
        if (++this->puiCurrentRow >= VGA_HEIGHT) {
            Clear();
        }
    } else if (cCharacter == '\t') {
        this->puiCurrentColumn = this->puiCurrentColumn + 4 - (this->puiCurrentColumn % 4);
    } else if (cCharacter == '\r') {
        this->puiCurrentColumn = 0;
    } else {
        const uint16_t uiOffset = OffsetFromCoords(this->puiCurrentColumn, this->puiCurrentRow);

        this->ppFrameBuffer[uiOffset] = cCharacter;
        this->ppFrameBuffer[uiOffset + 1] = this->peColorScheme;

        if (++this->puiCurrentColumn >= VGA_WIDTH) {
            this->puiCurrentColumn = 0;
            this->puiCurrentRow++;
        }

        if (this->puiCurrentRow >= VGA_HEIGHT) {
            Clear();
        }
    }
    SetCursorPosition(this->puiCurrentColumn, this->puiCurrentRow);
}

void cStaticTerminalDriver::SetColoring(eTerminalColor eColor)
{
    this->peColorScheme = eColor;
}

void cStaticTerminalDriver::SetCursorPosition(uint8_t uiX, uint8_t uiY)
{
    uint16_t uiPos = (uiY * VGA_WIDTH) + uiX;
    port_byte_out(FB_COMMAND_PORT, FB_HIGH_BYTE_COMMAND);
    port_byte_out(FB_DATA_PORT, ((uiPos >> 8) & 0x00FF));
    port_byte_out(FB_COMMAND_PORT, FB_LOW_BYTE_COMMAND);
    port_byte_out(FB_DATA_PORT, uiPos & 0x00FF);
}

void cStaticTerminalDriver::Clear()
{
    memset((char*) C_MEMORY_VGA_VIDEO_ADDRESS, 0, 2 * VGA_HEIGHT * VGA_WIDTH);
    this->puiCurrentColumn = 0;
    this->puiCurrentRow = 0;

    SetCursorPosition(this->puiCurrentColumn, this->puiCurrentRow);
}