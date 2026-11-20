#pragma once

#include <types.h>

// Reference-counted loader of the shared menu fonts. Every user (a MuMsg, the HUD, the save point menu, ...) takes a
// slot with the font flags it needs; a font is read when the first slot asks for it and removed when the last slot that
// asked for it is released. Defined in mu_msg.cpp, whose static initializer constructs the global instance.
// HYPOTHESIS: member names (the class and method names are the map's); the flags are FontData's 1 = normal,
// 2 = hiragana, 4 = normal 12, 8 = melee, from the FontData calls.
class MuMsgFontMngr {
public:
    u8 m_slots[16];   // 0x00 font flags held by each slot (0 = free)
    u8 m_locked : 1;  // 0x10 set: loadFont/unloadFont do nothing
    char _11[3];

    MuMsgFontMngr() {
        for (int i = 0; i < 16; i++) {
            m_slots[i] = 0;
        }
    }
    ~MuMsgFontMngr() { }

    // Takes a free slot for the fonts in flags and loads the ones no slot holds yet; heapType 0xFF picks the default
    // heap of each font, watch picks the watched normal font. Returns the slot.
    u8 loadFont(u32 flags, int heapType, int watch);
    // Frees the slot and removes the fonts no other slot holds.
    void unloadFont(int slot);
};

// HYPOTHESIS: the name of the global instance.
extern MuMsgFontMngr g_MuMsgFontMngr;
