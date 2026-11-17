#pragma once

// Shadows include/Brawl/Include/gf/gf_copyefb.h: m_104 is a mode byte (0, 1 or 2) that gfSceneRoot::renderXlu switches
// on, and the word at 0x6C is the color gfSceneRoot::renderXlu draws the copy with.

#include <StaticAssert.h>
#include <memory.h>
#include <types.h>
#include <GX.h>

struct gfCopyEFB {
    char _0[4];
    void* m_texBuffer;
    u32 m_texBufferSize;
    char _12[4];
    GXTexObj m_gxTexObj;
    bool m_isValid;
    char _49[3];
};
static_assert(sizeof(gfCopyEFB) == 52, "Class is wrong size!");

class gfCopyEFBMgr {
public:
    gfCopyEFB m_copyEFBs[2];
    u8 m_104; // HYPOTHESIS: 0 = off, 1 = copy after layer 6, 2 = copy after layer 3 (renderXlu); st_tengan sets it to 1
    char _105[3];
    int m_108;

    void createCopyEFB(int index, HeapType heapType);
    void getEFB(int index);
    void drawCopyEFB(int index, GXColor* color);

    bool isValid(int index);
    GXTexObj* getCopyEFBTex(int index);
    static gfCopyEFBMgr* getInstance();
};
static_assert(sizeof(gfCopyEFBMgr) == 112, "Class is wrong size!");
