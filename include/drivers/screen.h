#pragma once

#include <core/memory/locations.h>
#include <std/types.h>
#include <core/memory/memory.h>
#include <std/core.h>
#include <drivers/driver.h>

enum eTerminalColor : uint8_t {
    BLACK         = 0,
    BLUE          = 1,
    GREEN         = 2,
    CYAN          = 3,
    RED           = 4,
    MAGENTA       = 5,
    BROWN         = 6,
    LIGHT_GREY    = 7,
    DARK_GREY     = 8,
    LIGHT_BLUE    = 9,
    LIGHT_GREEN   = 10,
    LIGHT_CYAN    = 11,
    LIGHT_RED     = 12,
    LIGHT_MAGENTA = 13,
    LIGHT_BROWN   = 14,
    WHITE         = 15
};

class cStaticTerminalDriver : public IDriver {
public:
    bool Init();
    bool Destroy();

    void Print(cstring sStr);
    void Print(char cCharacter);
    void SetColoring(eTerminalColor eColor);
    void Clear();

    static eTerminalColor TerminalFromFGBG(eTerminalColor eForeground, eTerminalColor eBackground);
protected:
    static uint32_t OffsetFromCoords(uint32_t uiX, uint32_t uiY);
private:
    uint8_t peColorScheme;
    byte* ppFrameBuffer;
    uint32_t puiCurrentColumn;
    uint32_t puiCurrentRow;

    SINGLETON(cStaticTerminalDriver)
};