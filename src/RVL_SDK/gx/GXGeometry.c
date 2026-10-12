// gx geometry state: pushing dirty state out to the xf/bp units before a draw, GXBegin, and the line/point/cull
// settings. most of the dirty-state work is just register writes keyed off the bits in gx->dirtyState.
#include "revolution/gx.h"
#include "revolution/gx/GXRegs.h"

#define SET_REG_FIELD(reg, size, shift, val) FAST_FLAG_SET(reg, val, shift, size)

// the neighbours in gx land this calls (names are my best guess from the standard gx api)
extern void __GXSetSUTexRegs(void);
extern void __GXUpdateBPMask(void);
extern void __GXSetVCD(void);
extern void __GXSetVAT(void);
extern void __GXCalculateVLim(void);

// HYPOTHESIS: these three were inline in the original dirty-state code (the register writes show up in-line there)
static inline void __GXSetAmbMat(u32 dirtyFlags) {
    if (dirtyFlags & 0x100) {
        GX_WRITE_XF_REG(0x100a, gx->ambColor[0], 0);
    }

    if (dirtyFlags & 0x200) {
        GX_WRITE_XF_REG(0x100b, gx->ambColor[1], 0);
    }

    if (dirtyFlags & 0x400) {
        GX_WRITE_XF_REG(0x100c, gx->matColor[0], 0);
    }

    if (dirtyFlags & 0x800) {
        GX_WRITE_XF_REG(0x100d, gx->matColor[1], 0);
    }
}

static inline void __GXSetLightChan(volatile s32 dirtyFlags) {
    int i;
    u32 bits;
    u32 addr = 0x100e;

    if (dirtyFlags & 0x1000000) {
        u32 colors = (gx->genMode >> 4) & 7;
        GX_WRITE_XF_REG(0x1009, colors, 0);
    }

    bits = dirtyFlags;
    bits = (bits >> 12) & 0xF;
    i = 0;
    for (; bits != 0; i++, addr++, bits >>= 1) {
        if (bits & 1) {
            GX_WRITE_XF_REG(addr, gx->chanCtrl[i], 0);
        }
    }
}

static inline void __GXSetTexGen(volatile s32 dirtyFlags) {
    int i;
    u32 bits;
    u32 taddr = 0x1040;
    u32 dtaddr;

    if (dirtyFlags & 0x2000000) {
        u32 gens = gx->genMode & 0xF;
        GX_WRITE_XF_REG(0x103f, gens, 0);
    }

    bits = dirtyFlags;
    bits = (bits >> 16) & 0xFF;
    i = 0;
    for (; bits != 0; taddr++, i++, bits >>= 1) {
        dtaddr = taddr + 0x10;
        if (bits & 1) {
            GX_WRITE_XF_REG(taddr, gx->texGenCtrl[i], 0);
            GX_WRITE_XF_REG(dtaddr, gx->dualTexGenCtrl[i], 0);
        }
    }
}

static inline void GenMode_i(void) {
    GX_WRITE_RA_REG(gx->genMode);
    gx->bpSentNot = GX_FALSE;
}

void __GXSetDirtyState(void) {
    u32 tempFlags;
    u32 dirtyFlags = gx->dirtyState;

    if (dirtyFlags & 1) {
        __GXSetSUTexRegs();
    }

    if (dirtyFlags & 2) {
        __GXUpdateBPMask();
    }

    if (dirtyFlags & 4) {
        GenMode_i();
    }

    if (dirtyFlags & 8) {
        __GXSetVCD();
    }

    if (dirtyFlags & 0x10) {
        __GXSetVAT();
    }

    if (dirtyFlags & 0x18) {
        __GXCalculateVLim();
    }

    dirtyFlags &= ~0xFF;
    if (dirtyFlags) {
        tempFlags = dirtyFlags & 0xF00;
        if (tempFlags != 0) {
            __GXSetAmbMat(tempFlags);
        }

        tempFlags = dirtyFlags & 0x100F000;
        if (tempFlags != 0) {
            __GXSetLightChan(tempFlags);
        }

        tempFlags = dirtyFlags & 0x2FF0000;
        if (tempFlags != 0) {
            __GXSetTexGen(tempFlags);
        }

        tempFlags = dirtyFlags & 0x4000000;
        if (tempFlags != 0) {
            __GXSetMatrixIndex(GX_VA_PNMTXIDX);
            __GXSetMatrixIndex(GX_VA_TEX4MTXIDX);
        }

        tempFlags = dirtyFlags & 0x10000000;
        if (tempFlags != 0) {
            __GXSetViewport();
        }

        tempFlags = dirtyFlags & 0x8000000;
        if (tempFlags != 0) {
            __GXSetProjection();
        }

        gx->bpSentNot = GX_TRUE;
    }

    gx->dirtyState = 0;
}

// GXBegin has this body folded in, so keep a copy as an inline helper
static inline void SendFlushPrim_i(void) {
    u32 i;
    u32 sz = gx->vNum * gx->vLim;

    GX_WRITE_U8(0x98);
    GX_WRITE_U16(gx->vNum);

    for (i = 0; i < sz; i += 4) {
        GX_WRITE_U32(0);
    }

    gx->bpSentNot = GX_TRUE;
}

void GXBegin(GXPrimitive prim, GXVtxFmt fmt, u16 verts) {
    if (gx->dirtyState != 0) {
        __GXSetDirtyState();
    }

    if (*(u32*)gx == 0) {
        SendFlushPrim_i();
    }

    GX_WRITE_U8(fmt | prim);
    GX_WRITE_U16(verts);
}

void __GXSendFlushPrim(void) {
    u32 i;
    u32 sz = gx->vNum * gx->vLim;

    GX_WRITE_U8(0x98);
    GX_WRITE_U16(gx->vNum);

    for (i = 0; i < sz; i += 4) {
        GX_WRITE_U32(0);
    }

    gx->bpSentNot = GX_TRUE;
}

void GXSetLineWidth(u8 width, GXTexOffset offset) {
    SET_REG_FIELD(gx->lpSize, 8, 0, width);
    SET_REG_FIELD(gx->lpSize, 3, 16, offset);
    GX_WRITE_RA_REG(gx->lpSize);
    gx->bpSentNot = GX_FALSE;
}

void GXGetLineWidth(u8* width, GXTexOffset* offset) {
    *width = gx->lpSize;
    *offset = (gx->lpSize >> 16) & 7;
}

void GXSetPointSize(u8 size, GXTexOffset offset) {
    SET_REG_FIELD(gx->lpSize, 8, 8, size);
    SET_REG_FIELD(gx->lpSize, 3, 19, offset);
    GX_WRITE_RA_REG(gx->lpSize);
    gx->bpSentNot = GX_FALSE;
}

void GXEnableTexOffsets(GXTexCoordID coord, GXBool lineOfs, GXBool pointOfs) {
    SET_REG_FIELD(gx->suTs0[coord], 1, 18, lineOfs);
    SET_REG_FIELD(gx->suTs0[coord], 1, 19, pointOfs);
    GX_WRITE_RA_REG(gx->suTs0[coord]);
    gx->bpSentNot = GX_FALSE;
}

void GXSetCullMode(GXCullMode mode) {
    // swap the two bits to get the hardware order
    GXCullMode bits = (GXCullMode)(((mode << 1) & 2) | ((mode >> 1) & 1));
    SET_REG_FIELD(gx->genMode, 2, 14, bits);
    gx->dirtyState |= 4;
}

void GXSetCoPlanar(GXBool enable) {
    u32 reg;

    SET_REG_FIELD(gx->genMode, 1, 19, enable);

    reg = 0xFE080000;
    GX_WRITE_RA_REG(reg);
    GX_WRITE_RA_REG(gx->genMode);
}

void __GXSetGenMode(void) {
    GX_WRITE_RA_REG(gx->genMode);
    gx->bpSentNot = GX_FALSE;
}
