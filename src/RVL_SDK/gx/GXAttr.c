// gx vertex attribute setup: which attributes the vertex stream carries (vcd), how each is encoded (vat), the
// array bases/strides, and the texgen setup. almost everything is just bit-twiddling the shadowed cp/xf regs
// in gx->..., and then marking them dirty so GXBegin flushes them.
#include "revolution/gx.h"
#include "revolution/gx/GXRegs.h"
#include "revolution/gd/GDGeometry.h"

#define SET_REG_FIELD(reg, size, shift, val) FAST_FLAG_SET(reg, val, shift, size)
#define GETFIELD(reg, shift, size) (((reg) >> (shift)) & ((1 << (size)) - 1))

// HYPOTHESIS: the vcd normal field gets written from either the nrm or the nbt flag, never both
static inline void SetVCDNrm_i(void) {
    if (gx->hasNrms || gx->hasBiNrms) {
        SC_CP_VCD_REG_LO_SET_NRM(gx->vcdLo, gx->nrmType);
    } else {
        SC_CP_VCD_REG_LO_SET_NRM(gx->vcdLo, GX_NONE);
    }
}

static inline void SetVtxDesc_i(GXAttr attr, GXAttrType type) {
    switch (attr) {
    case GX_VA_PNMTXIDX:
        SC_CP_VCD_REG_LO_SET_PMIDX(gx->vcdLo, type);
        break;
    case GX_VA_TEX0MTXIDX:
        SC_CP_VCD_REG_LO_SET_T0MIDX(gx->vcdLo, type);
        break;
    case GX_VA_TEX1MTXIDX:
        SC_CP_VCD_REG_LO_SET_T1MIDX(gx->vcdLo, type);
        break;
    case GX_VA_TEX2MTXIDX:
        SC_CP_VCD_REG_LO_SET_T2MIDX(gx->vcdLo, type);
        break;
    case GX_VA_TEX3MTXIDX:
        SC_CP_VCD_REG_LO_SET_T3MIDX(gx->vcdLo, type);
        break;
    case GX_VA_TEX4MTXIDX:
        SC_CP_VCD_REG_LO_SET_T4MIDX(gx->vcdLo, type);
        break;
    case GX_VA_TEX5MTXIDX:
        SC_CP_VCD_REG_LO_SET_T5MIDX(gx->vcdLo, type);
        break;
    case GX_VA_TEX6MTXIDX:
        SC_CP_VCD_REG_LO_SET_T6MIDX(gx->vcdLo, type);
        break;
    case GX_VA_TEX7MTXIDX:
        SC_CP_VCD_REG_LO_SET_T7MIDX(gx->vcdLo, type);
        break;
    case GX_VA_POS:
        SC_CP_VCD_REG_LO_SET_POS(gx->vcdLo, type);
        break;
    case GX_VA_NRM:
        if (type != GX_NONE) {
            gx->hasNrms = GX_TRUE;
            gx->hasBiNrms = GX_FALSE;
            gx->nrmType = type;
        } else {
            gx->hasNrms = GX_FALSE;
        }
        break;
    case GX_VA_NBT:
        if (type != GX_NONE) {
            gx->hasBiNrms = GX_TRUE;
            gx->hasNrms = GX_FALSE;
            gx->nrmType = type;
        } else {
            gx->hasBiNrms = GX_FALSE;
        }
        break;
    case GX_VA_CLR0:
        SC_CP_VCD_REG_LO_SET_COL0(gx->vcdLo, type);
        break;
    case GX_VA_CLR1:
        SC_CP_VCD_REG_LO_SET_COL1(gx->vcdLo, type);
        break;
    case GX_VA_TEX0:
        SC_CP_VCD_REG_HI_SET_TEX0(gx->vcdHi, type);
        break;
    case GX_VA_TEX1:
        SC_CP_VCD_REG_HI_SET_TEX1(gx->vcdHi, type);
        break;
    case GX_VA_TEX2:
        SC_CP_VCD_REG_HI_SET_TEX2(gx->vcdHi, type);
        break;
    case GX_VA_TEX3:
        SC_CP_VCD_REG_HI_SET_TEX3(gx->vcdHi, type);
        break;
    case GX_VA_TEX4:
        SC_CP_VCD_REG_HI_SET_TEX4(gx->vcdHi, type);
        break;
    case GX_VA_TEX5:
        SC_CP_VCD_REG_HI_SET_TEX5(gx->vcdHi, type);
        break;
    case GX_VA_TEX6:
        SC_CP_VCD_REG_HI_SET_TEX6(gx->vcdHi, type);
        break;
    case GX_VA_TEX7:
        SC_CP_VCD_REG_HI_SET_TEX7(gx->vcdHi, type);
        break;
    default:
        break;
    }
}

void GXSetVtxDesc(GXAttr attr, GXAttrType type) {
    SetVtxDesc_i(attr, type);
    SetVCDNrm_i();
    gx->dirtyState |= 8;
}

void GXSetVtxDescv(const GXVtxDescList* list) {
    for (; list->attr != GX_VA_NULL; list++) {
        SetVtxDesc_i(list->attr, list->type);
    }

    SetVCDNrm_i();
    gx->dirtyState |= 8;
}

// the xf side wants the vertex format summarised too, this was a separate helper that got inlined
static inline void XfVtxSpecs_i(void) {
    u32 nrm;
    u32 clr;
    u32 txc;
    u32 cmd;

    nrm = gx->hasBiNrms ? 2 : (gx->hasNrms ? 1 : 0);

    // both color fields, then all eight texcoord fields, as "highest used index + 1"
    clr = 32 - __cntlzw(GETFIELD(gx->vcdLo, 13, 4)) + 1;
    txc = 32 - __cntlzw(GETFIELD(gx->vcdHi, 0, 16)) + 1;

    GX_WRITE_U8(0x10);
    GX_WRITE_U32(0x1008);
    cmd = 0;
    cmd = cmd | (clr >> 1);
    cmd = ((txc << 3) & ~0xF) | cmd;
    GX_WRITE_U32(cmd | (nrm << 2));

    gx->bpSentNot = GX_TRUE;
}

void __GXSetVCD(void) {
    GX_WRITE_U8(8);
    GX_WRITE_U8(0x50);
    GX_WRITE_U32(gx->vcdLo);
    GX_WRITE_U8(8);
    GX_WRITE_U8(0x60);
    GX_WRITE_U32(gx->vcdHi);

    XfVtxSpecs_i();
}

void __GXCalculateVLim(void) {
    static u8 tbl1[] = {0, 4, 1, 2};
    static u8 tbl2[] = {0, 8, 1, 2};
    static u8 tbl3[] = {0, 12, 1, 2};

    u32 vlim;
    u32 vcdLo;
    u32 vcdHi;
    s32 nrmCnt;

    if (gx->vNum == 0) {
        return;
    }

    vcdLo = gx->vcdLo;
    vcdHi = gx->vcdHi;

    // normal component count bit out of vat a (set when it's the 9 element nbt style)
    nrmCnt = gx->vatA[0];
    nrmCnt = (nrmCnt & 0x200) >> 9;

    vlim = GETFIELD(vcdLo, 0, 1);
    vlim += GETFIELD(vcdLo, 1, 1);
    vlim += GETFIELD(vcdLo, 2, 1);
    vlim += GETFIELD(vcdLo, 3, 1);
    vlim += GETFIELD(vcdLo, 4, 1);
    vlim += GETFIELD(vcdLo, 5, 1);
    vlim += GETFIELD(vcdLo, 6, 1);
    vlim += GETFIELD(vcdLo, 7, 1);
    vlim += GETFIELD(vcdLo, 8, 1);

    vlim += tbl3[GETFIELD(vcdLo, 9, 2)];
    vlim += tbl3[GETFIELD(vcdLo, 11, 2)] * (nrmCnt == 1 ? 3 : 1);
    vlim += tbl1[GETFIELD(vcdLo, 13, 2)];
    vlim += tbl1[GETFIELD(vcdLo, 15, 2)];

    vlim += tbl2[GETFIELD(vcdHi, 0, 2)];
    vlim += tbl2[GETFIELD(vcdHi, 2, 2)];
    vlim += tbl2[GETFIELD(vcdHi, 4, 2)];
    vlim += tbl2[GETFIELD(vcdHi, 6, 2)];
    vlim += tbl2[GETFIELD(vcdHi, 8, 2)];
    vlim += tbl2[GETFIELD(vcdHi, 10, 2)];
    vlim += tbl2[GETFIELD(vcdHi, 12, 2)];
    vlim += tbl2[GETFIELD(vcdHi, 14, 2)];

    gx->vLim = vlim;
}

void GXGetVtxDesc(GXAttr attr, GXAttrType* type) {
    u32 result;

    switch (attr) {
    case GX_VA_PNMTXIDX:
        result = GETFIELD(gx->vcdLo, 0, 1);
        break;
    case GX_VA_TEX0MTXIDX:
        result = GETFIELD(gx->vcdLo, 1, 1);
        break;
    case GX_VA_TEX1MTXIDX:
        result = GETFIELD(gx->vcdLo, 2, 1);
        break;
    case GX_VA_TEX2MTXIDX:
        result = GETFIELD(gx->vcdLo, 3, 1);
        break;
    case GX_VA_TEX3MTXIDX:
        result = GETFIELD(gx->vcdLo, 4, 1);
        break;
    case GX_VA_TEX4MTXIDX:
        result = GETFIELD(gx->vcdLo, 5, 1);
        break;
    case GX_VA_TEX5MTXIDX:
        result = GETFIELD(gx->vcdLo, 6, 1);
        break;
    case GX_VA_TEX6MTXIDX:
        result = GETFIELD(gx->vcdLo, 7, 1);
        break;
    case GX_VA_TEX7MTXIDX:
        result = GETFIELD(gx->vcdLo, 8, 1);
        break;
    case GX_VA_POS:
        result = GETFIELD(gx->vcdLo, 9, 2);
        break;
    case GX_VA_NRM:
        result = gx->hasNrms ? GETFIELD(gx->vcdLo, 11, 2) : GX_NONE;
        break;
    case GX_VA_NBT:
        result = gx->hasBiNrms ? GETFIELD(gx->vcdLo, 11, 2) : GX_NONE;
        break;
    case GX_VA_CLR0:
        result = GETFIELD(gx->vcdLo, 13, 2);
        break;
    case GX_VA_CLR1:
        result = GETFIELD(gx->vcdLo, 15, 2);
        break;
    case GX_VA_TEX0:
        result = GETFIELD(gx->vcdHi, 0, 2);
        break;
    case GX_VA_TEX1:
        result = GETFIELD(gx->vcdHi, 2, 2);
        break;
    case GX_VA_TEX2:
        result = GETFIELD(gx->vcdHi, 4, 2);
        break;
    case GX_VA_TEX3:
        result = GETFIELD(gx->vcdHi, 6, 2);
        break;
    case GX_VA_TEX4:
        result = GETFIELD(gx->vcdHi, 8, 2);
        break;
    case GX_VA_TEX5:
        result = GETFIELD(gx->vcdHi, 10, 2);
        break;
    case GX_VA_TEX6:
        result = GETFIELD(gx->vcdHi, 12, 2);
        break;
    case GX_VA_TEX7:
        result = GETFIELD(gx->vcdHi, 14, 2);
        break;
    default:
        result = GX_NONE;
        break;
    }

    *type = result;
}

void GXGetVtxDescv(GXVtxDescList* list) {
    GXAttr attr;

    for (attr = GX_VA_PNMTXIDX; attr <= GX_VA_TEX7; attr++) {
        list[attr].attr = attr;
        GXGetVtxDesc(attr, &list[attr].type);
    }

    list[attr].attr = GX_VA_NBT;
    GXGetVtxDesc(GX_VA_NBT, &list[attr].type);
    attr++;
    list[attr].attr = GX_VA_NULL;
}

void GXClearVtxDesc(void) {
    gx->vcdLo = 0;
    SC_CP_VCD_REG_LO_SET_POS(gx->vcdLo, GX_DIRECT);
    gx->vcdHi = 0;
    gx->hasNrms = GX_FALSE;
    gx->hasBiNrms = GX_FALSE;
    gx->dirtyState |= 8;
}

static inline void SetVAT_i(u32* vatA, u32* vatB, u32* vatC, GXAttr attr, GXCompCnt cnt, GXCompType type, u8 shift) {
    switch (attr) {
    case GX_VA_POS:
        SC_CP_VAT_REG_A_SET_POSCNT(*vatA, cnt);
        SC_CP_VAT_REG_A_SET_POSFMT(*vatA, type);
        SC_CP_VAT_REG_A_SET_POSSHFT(*vatA, shift);
        break;
    case GX_VA_NRM:
    case GX_VA_NBT:
        SC_CP_VAT_REG_A_SET_NRMFMT(*vatA, type);
        if (cnt == GX_NRM_NBT3) {
            // HYPOTHESIS: count is a single bit, so nbt3 is stored as nbt plus the normalindex3 flag
            SC_CP_VAT_REG_A_SET_NRMCNT(*vatA, GX_NRM_NBT);
            SC_CP_VAT_REG_A_SET_NORMALINDEX3(*vatA, 1);
        } else {
            SC_CP_VAT_REG_A_SET_NRMCNT(*vatA, cnt);
            SC_CP_VAT_REG_A_SET_NORMALINDEX3(*vatA, 0);
        }
        break;
    case GX_VA_CLR0:
        SC_CP_VAT_REG_A_SET_COL0CNT(*vatA, cnt);
        SC_CP_VAT_REG_A_SET_COL0FMT(*vatA, type);
        break;
    case GX_VA_CLR1:
        SC_CP_VAT_REG_A_SET_COL1CNT(*vatA, cnt);
        SC_CP_VAT_REG_A_SET_COL1FMT(*vatA, type);
        break;
    case GX_VA_TEX0:
        SC_CP_VAT_REG_A_SET_TEX0CNT(*vatA, cnt);
        SC_CP_VAT_REG_A_SET_TEX0FMT(*vatA, type);
        SC_CP_VAT_REG_A_SET_TEX0SHFT(*vatA, shift);
        break;
    case GX_VA_TEX1:
        SC_CP_VAT_REG_B_SET_TEX1CNT(*vatB, cnt);
        SC_CP_VAT_REG_B_SET_TEX1FMT(*vatB, type);
        SC_CP_VAT_REG_B_SET_TEX1SHFT(*vatB, shift);
        break;
    case GX_VA_TEX2:
        SC_CP_VAT_REG_B_SET_TEX2CNT(*vatB, cnt);
        SC_CP_VAT_REG_B_SET_TEX2FMT(*vatB, type);
        SC_CP_VAT_REG_B_SET_TEX2SHFT(*vatB, shift);
        break;
    case GX_VA_TEX3:
        SC_CP_VAT_REG_B_SET_TEX3CNT(*vatB, cnt);
        SC_CP_VAT_REG_B_SET_TEX3FMT(*vatB, type);
        SC_CP_VAT_REG_B_SET_TEX3SHFT(*vatB, shift);
        break;
    case GX_VA_TEX4:
        SC_CP_VAT_REG_B_SET_TEX4CNT(*vatB, cnt);
        SC_CP_VAT_REG_B_SET_TEX4FMT(*vatB, type);
        SC_CP_VAT_REG_C_SET_TEX4SHFT(*vatC, shift);
        break;
    case GX_VA_TEX5:
        SC_CP_VAT_REG_C_SET_TEX5CNT(*vatC, cnt);
        SC_CP_VAT_REG_C_SET_TEX5FMT(*vatC, type);
        SC_CP_VAT_REG_C_SET_TEX5SHFT(*vatC, shift);
        break;
    case GX_VA_TEX6:
        SC_CP_VAT_REG_C_SET_TEX6CNT(*vatC, cnt);
        SC_CP_VAT_REG_C_SET_TEX6FMT(*vatC, type);
        SC_CP_VAT_REG_C_SET_TEX6SHFT(*vatC, shift);
        break;
    case GX_VA_TEX7:
        SC_CP_VAT_REG_C_SET_TEX7CNT(*vatC, cnt);
        SC_CP_VAT_REG_C_SET_TEX7FMT(*vatC, type);
        SC_CP_VAT_REG_C_SET_TEX7SHFT(*vatC, shift);
        break;
    default:
        break;
    }
}

void GXSetVtxAttrFmt(GXVtxFmt fmt, GXAttr attr, GXCompCnt cnt, GXCompType type, u8 shift) {
    u32* vatA = &gx->vatA[fmt];
    u32* vatB = &gx->vatB[fmt];
    u32* vatC = &gx->vatC[fmt];

    SetVAT_i(vatA, vatB, vatC, attr, cnt, type, shift);
    gx->dirtyState |= 0x10;
    gx->dirtyVAT |= (u8)(1 << (u8)fmt);
}

void GXSetVtxAttrFmtv(GXVtxFmt fmt, const GXVtxAttrFmtList* list) {
    u32* vatA = &gx->vatA[fmt];
    u32* vatB = &gx->vatB[fmt];
    u32* vatC = &gx->vatC[fmt];

    for (; list->attr != GX_VA_NULL; list++) {
        SetVAT_i(vatA, vatB, vatC, list->attr, list->cnt, list->type, list->frac);
    }

    gx->dirtyState |= 0x10;
    gx->dirtyVAT |= (u8)(1 << (u8)fmt);
}

void __GXSetVAT(void) {
    int i = 0;
    u32 flags = gx->dirtyVAT;

    do {
        if (flags & 1) {
            GX_WRITE_U8(8);
            GX_WRITE_U8(0x70 | i);
            GX_WRITE_U32(gx->vatA[i]);
            GX_WRITE_U8(8);
            GX_WRITE_U8(0x80 | i);
            GX_WRITE_U32(gx->vatB[i]);
            GX_WRITE_U8(8);
            GX_WRITE_U8(0x90 | i);
            GX_WRITE_U32(gx->vatC[i]);
        }

        i++;
        flags >>= 1;
    } while (flags != 0);

    GX_WRITE_U8(0);
    gx->dirtyVAT = 0;
}

static inline u8 GetFracForNrm_i(GXCompType type) {
    switch (type) {
    case GX_S8:
        return 6;
    case GX_S16:
        return 14;
    case GX_U16:
    default:
        return 0;
    }
}

void GXGetVtxAttrFmt(GXVtxFmt fmt, GXAttr attr, GXCompCnt* cnt, GXCompType* type, u8* shift) {
    u32* vatA = &gx->vatA[fmt];
    u32* vatB = &gx->vatB[fmt];
    u32* vatC = &gx->vatC[fmt];

    switch (attr) {
    case GX_VA_POS:
        *cnt = GETFIELD(*vatA, 0, 1);
        *type = GETFIELD(*vatA, 1, 3);
        *shift = GETFIELD(*vatA, 4, 5);
        break;
    case GX_VA_NRM:
    case GX_VA_NBT:
        *cnt = GETFIELD(*vatA, 9, 1);
        if (*cnt == GX_NRM_NBT && GETFIELD(*vatA, 31, 1)) {
            *cnt = GX_NRM_NBT3;
        }
        *type = GETFIELD(*vatA, 10, 3);
        *shift = GetFracForNrm_i(*type);
        break;
    case GX_VA_CLR0:
        *cnt = GETFIELD(*vatA, 13, 1);
        *type = GETFIELD(*vatA, 14, 3);
        *shift = 0;
        break;
    case GX_VA_CLR1:
        *cnt = GETFIELD(*vatA, 17, 1);
        *type = GETFIELD(*vatA, 18, 3);
        *shift = 0;
        break;
    case GX_VA_TEX0:
        *cnt = GETFIELD(*vatA, 21, 1);
        *type = GETFIELD(*vatA, 22, 3);
        *shift = GETFIELD(*vatA, 25, 5);
        break;
    case GX_VA_TEX1:
        *cnt = GETFIELD(*vatB, 0, 1);
        *type = GETFIELD(*vatB, 1, 3);
        *shift = GETFIELD(*vatB, 4, 5);
        break;
    case GX_VA_TEX2:
        *cnt = GETFIELD(*vatB, 9, 1);
        *type = GETFIELD(*vatB, 10, 3);
        *shift = GETFIELD(*vatB, 13, 5);
        break;
    case GX_VA_TEX3:
        *cnt = GETFIELD(*vatB, 18, 1);
        *type = GETFIELD(*vatB, 19, 3);
        *shift = GETFIELD(*vatB, 22, 5);
        break;
    case GX_VA_TEX4:
        *cnt = GETFIELD(*vatB, 27, 1);
        *type = GETFIELD(*vatB, 28, 3);
        *shift = GETFIELD(*vatC, 0, 5);
        break;
    case GX_VA_TEX5:
        *cnt = GETFIELD(*vatC, 5, 1);
        *type = GETFIELD(*vatC, 6, 3);
        *shift = GETFIELD(*vatC, 9, 5);
        break;
    case GX_VA_TEX6:
        *cnt = GETFIELD(*vatC, 14, 1);
        *type = GETFIELD(*vatC, 15, 3);
        *shift = GETFIELD(*vatC, 18, 5);
        break;
    case GX_VA_TEX7:
        *cnt = GETFIELD(*vatC, 23, 1);
        *type = GETFIELD(*vatC, 24, 3);
        *shift = GETFIELD(*vatC, 27, 5);
        break;
    default:
        *cnt = GX_POS_XYZ;
        *type = GX_U8;
        *shift = 0;
        break;
    }
}

void GXGetVtxAttrFmtv(GXVtxFmt fmt, GXVtxAttrFmtList* list) {
    int i;

    for (i = GX_VA_POS; i <= GX_VA_TEX7; i++, list++) {
        list->attr = (GXAttr)i;
        GXGetVtxAttrFmt(fmt, (GXAttr)i, &list->cnt, &list->type, &list->frac);
    }

    list->attr = GX_VA_NULL;
}

void GXSetArray(GXAttr attr, const void* base, u8 stride) {
    u32 idx;

    if (attr == GX_VA_NBT) {
        attr = GX_VA_NRM;
    }

    idx = attr - GX_VA_POS;

    GX_WRITE_U8(8);
    GX_WRITE_U8(0xA0 | idx);
    GX_WRITE_U32((u32)base & 0x3FFFFFFF);
    GX_WRITE_U8(8);
    GX_WRITE_U8(0xB0 | idx);
    GX_WRITE_U32(stride);
}

void GXInvalidateVtxCache(void) {
    GX_WRITE_U8(0x48);
}

void GXSetTexCoordGen2(GXTexCoordID id, GXTexGenType type, GXTexGenSrc src, u32 texMtxIdx, GXBool normalize, u32 dualTexMtxIdx) {
    u32 reg;
    u32 inputRow;
    u32 inputForm;

    reg = 0;
    inputForm = 0;
    inputRow = 5;

    switch (src) {
    case GX_TG_POS:
        inputRow = 0;
        inputForm = 1;
        break;
    case GX_TG_NRM:
        inputRow = 1;
        inputForm = 1;
        break;
    case GX_TG_BINRM:
        inputRow = 3;
        inputForm = 1;
        break;
    case GX_TG_TANGENT:
        inputRow = 4;
        inputForm = 1;
        break;
    case GX_TG_COLOR0:
        inputRow = 2;
        break;
    case GX_TG_COLOR1:
        inputRow = 2;
        break;
    case GX_TG_TEX0:
        inputRow = 5;
        break;
    case GX_TG_TEX1:
        inputRow = 6;
        break;
    case GX_TG_TEX2:
        inputRow = 7;
        break;
    case GX_TG_TEX3:
        inputRow = 8;
        break;
    case GX_TG_TEX4:
        inputRow = 9;
        break;
    case GX_TG_TEX5:
        inputRow = 10;
        break;
    case GX_TG_TEX6:
        inputRow = 11;
        break;
    case GX_TG_TEX7:
        inputRow = 12;
        break;
    default:
        break;
    }

    switch (type) {
    case GX_TG_MTX2x4:
        reg = 0;
        SET_REG_FIELD(reg, 1, 1, 0);
        SET_REG_FIELD(reg, 1, 2, inputForm);
        SET_REG_FIELD(reg, 5, 7, inputRow);
        break;
    case GX_TG_MTX3x4:
        reg = 0;
        SET_REG_FIELD(reg, 1, 1, 1);
        SET_REG_FIELD(reg, 1, 2, inputForm);
        SET_REG_FIELD(reg, 5, 7, inputRow);
        break;
    case GX_TG_BUMP0:
    case GX_TG_BUMP1:
    case GX_TG_BUMP2:
    case GX_TG_BUMP3:
    case GX_TG_BUMP4:
    case GX_TG_BUMP5:
    case GX_TG_BUMP6:
    case GX_TG_BUMP7:
        reg = 0;
        SET_REG_FIELD(reg, 1, 2, inputForm);
        SET_REG_FIELD(reg, 3, 4, 1);
        SET_REG_FIELD(reg, 5, 7, inputRow);
        SET_REG_FIELD(reg, 3, 12, src - GX_TG_TEXCOORD0);
        SET_REG_FIELD(reg, 3, 15, type - GX_TG_BUMP0);
        break;
    case GX_TG_SRTG:
        reg = 0;
        SET_REG_FIELD(reg, 1, 2, inputForm);
        if (src == GX_TG_COLOR0) {
            SET_REG_FIELD(reg, 3, 4, 2);
        } else {
            SET_REG_FIELD(reg, 3, 4, 3);
        }
        SET_REG_FIELD(reg, 5, 7, 2);
        break;
    default:
        break;
    }

    gx->texGenCtrl[id] = reg;
    gx->dirtyState |= 0x10000 << id;

    reg = 0;
    SET_REG_FIELD(reg, 6, 0, dualTexMtxIdx - 0x40);
    SET_REG_FIELD(reg, 1, 8, normalize);
    gx->dualTexGenCtrl[id] = reg;

    switch (id) {
    case GX_TEXCOORD0:
        SET_REG_FIELD(gx->matIdxA, 6, 6, texMtxIdx);
        break;
    case GX_TEXCOORD1:
        SET_REG_FIELD(gx->matIdxA, 6, 12, texMtxIdx);
        break;
    case GX_TEXCOORD2:
        SET_REG_FIELD(gx->matIdxA, 6, 18, texMtxIdx);
        break;
    case GX_TEXCOORD3:
        SET_REG_FIELD(gx->matIdxA, 6, 24, texMtxIdx);
        break;
    case GX_TEXCOORD4:
        SET_REG_FIELD(gx->matIdxB, 6, 0, texMtxIdx);
        break;
    case GX_TEXCOORD5:
        SET_REG_FIELD(gx->matIdxB, 6, 6, texMtxIdx);
        break;
    case GX_TEXCOORD6:
        SET_REG_FIELD(gx->matIdxB, 6, 12, texMtxIdx);
        break;
    default:
        SET_REG_FIELD(gx->matIdxB, 6, 18, texMtxIdx);
        break;
    }

    gx->dirtyState |= 0x4000000;
}

void GXSetNumTexGens(u8 nTexGens) {
    gx->genMode = __rlwimi(gx->genMode, nTexGens, 0, 28, 31);
    gx->dirtyState |= 0x2000004;
}
