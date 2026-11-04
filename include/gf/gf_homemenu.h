#pragma once

#include <StaticAssert.h>
#include <types.h>

// Local shadow of BrawlHeaders' gf/gf_homemenu.h: adds loadResource and releaseResource (names from the sora map), the home menu lifecycle and status declarations and layout.
class gfHomeMenu {
public:
    bool setBan(u32 p1, u32 p2);
    void kill();
    void start();
    // HYPOTHESIS: loads the files of the HOME Menu again after releaseResource.
    void loadResource();
    // HYPOTHESIS: frees the memory of the HOME Menu (the Wi-Fi interface needs it); false while it cannot.
    bool releaseResource();
    void userProc();
    bool returnStatus();

    // HYPOTHESIS: remaining layout and bitfield meanings are unclassified.
    u8 unk0[0xAC];
    u32 unkAC; // HYPOTHESIS: a gfFileIOHandle (only its destructor uses it)
    union {
        struct {
            s32 unkB0Hi : 4;
            s32 unkB0Mode : 4;
            u32 unkB0Lo : 24;
        } unkB0;
        struct {
            u8 unkB0Byte;
            u8 unkB1Hi : 2;
            u8 unkB1_20 : 1;
            u8 unkB1_10 : 1;
            u8 unkB1Lo : 4;
            u8 unkB2[2];
        };
    };
    u8 unkB4[0x10];
};
// TODO size assertion

extern gfHomeMenu* g_gfHomeMenu;
