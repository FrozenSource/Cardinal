#pragma once

#define C_FUNCTION extern "C"

extern "C" inline void __cxa_pure_virtual() {
    // Do nothing or print an error message.
}