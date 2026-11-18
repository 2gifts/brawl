#pragma once

// Local shadow of BrawlHeaders/gf/gf_copyefb.h: names the fields of one copied frame (format, buffer, size, dimensions).

#include <StaticAssert.h>
#include <memory.h>
#include <types.h>
#include <GX.h>

struct gfCopyEFB {
    GXTexFmt m_format;
    void* m_texBuffer;
    u32 m_texBufferSize;
    u16 m_width;
    u16 m_height;
    GXTexObj m_gxTexObj;
    bool m_isValid;
    char _49[3];
};
static_assert(sizeof(gfCopyEFB) == 52, "Class is wrong size!");

class gfCopyEFBMgr {
public:
    gfCopyEFB m_copyEFBs[2];
    bool m_104;
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
