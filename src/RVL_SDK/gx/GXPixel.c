// gx pixel engine state: fog, blending, z compare, pixel format and the other bits of the back end that
// only need a register write.
#include "revolution/gx.h"
#include "revolution/gx/GXRegs.h"
#include "revolution/mtx.h"

// the sdk uses doubles in a few spots here (the 1.0l and 0.5l compares), and sqrt/the float->int helper
extern double sqrt(double x);

#define SET_REG_FIELD(reg, size, shift, val) FAST_FLAG_SET(reg, val, shift, size)

void GXSetFog(GXFogType type, f32 startz, f32 endz, f32 nearz, f32 farz, GXColor color) {
    u32 fogColorReg = 0;
    u32 fogParamReg0 = 0;
    u32 fogParamReg1 = 0;
    u32 fogParamReg2 = 0;
    u32 fogParamReg3 = 0;
    u32 fsel = type & 7;
    u32 proj = (type >> 3) & 1;
    f32 a, c;
    u32 a_hex, c_hex;

    if (proj) {
        if (farz == nearz || endz == startz) {
            a = 0.0f;
            c = 0.0f;
        } else {
            f32 tmp = 1.0f / (endz - startz);

            a = tmp * (farz - nearz);
            c = tmp * (startz - nearz);
        }
    } else {
        f32 a0, b0, c0;
        u32 expB;
        u32 magB;
        u32 shiftB;

        if (farz == nearz || endz == startz) {
            a0 = 0.0f;
            b0 = 0.5f;
            c0 = 0.0f;
        } else {
            a0 = (farz * nearz) / ((farz - nearz) * (endz - startz));
            b0 = farz / (farz - nearz);
            c0 = startz / (endz - startz);
        }

        expB = 0;
        while (b0 > 1.0) {
            b0 *= 0.5f;
            expB++;
        }

        while (b0 > 0.0f && b0 < 0.5) {
            b0 *= 2.0f;
            expB--;
        }

        a = a0 / (1 << (expB + 1));
        magB = 8388638.0f * b0;
        shiftB = expB + 1;
        c = c0;

        SET_REG_FIELD(fogParamReg1, 24, 0, magB);
        SET_REG_FIELD(fogParamReg1, 8, 24, 0xEF);
        SET_REG_FIELD(fogParamReg2, 5, 0, shiftB);
        SET_REG_FIELD(fogParamReg2, 8, 24, 0xF0);
    }

    a_hex = *(u32 *)&a;
    c_hex = *(u32 *)&c;

    fogParamReg0 = __rlwimi(fogParamReg0, a_hex, 20, 21, 31);
    fogParamReg0 = __rlwimi(fogParamReg0, a_hex, 20, 13, 20);
    fogParamReg0 = __rlwimi(fogParamReg0, a_hex, 20, 12, 12);
    SET_REG_FIELD(fogParamReg0, 8, 24, 0xEE);

    fogParamReg3 = __rlwimi(fogParamReg3, c_hex, 20, 21, 31);
    fogParamReg3 = __rlwimi(fogParamReg3, c_hex, 20, 13, 20);
    fogParamReg3 = __rlwimi(fogParamReg3, c_hex, 20, 12, 12);
    SET_REG_FIELD(fogParamReg3, 1, 20, proj);
    SET_REG_FIELD(fogParamReg3, 3, 21, fsel);
    SET_REG_FIELD(fogParamReg3, 8, 24, 0xF1);

    fogColorReg = __rlwimi(fogColorReg, *(u32 *)&color, 24, 8, 31);
    SET_REG_FIELD(fogColorReg, 8, 24, 0xF2);

    GX_WRITE_RA_REG(fogParamReg0);
    GX_WRITE_RA_REG(fogParamReg1);
    GX_WRITE_RA_REG(fogParamReg2);
    GX_WRITE_RA_REG(fogParamReg3);
    GX_WRITE_RA_REG(fogColorReg);
    gx->bpSentNot = GX_FALSE;
}

void GXInitFogAdjTable(GXFogAdjTable *table, u16 width, const Mtx44 proj) {
    f32 radius, pixel, sidec, x, v;
    u32 i;

    if (proj[3][3] == 0.0) {
        radius = proj[2][3] / (proj[2][2] - 1.0f);
        sidec = radius / proj[0][0];
    } else {
        sidec = 1.0f / proj[0][0];
        radius = 1.7320508f * sidec;
    }

    pixel = 2.0f / width;

    for (i = 0; i < 10; i++) {
        x = (i + 1) * 32;
        x *= pixel;
        x *= sidec;
        v = sqrt(1.0f + (x * x) / (radius * radius));
        table->r[i] = (u32)(256.0f * v) & 0xFFF;
    }
}

void GXSetFogRangeAdj(GXBool enable, u16 center, const GXFogAdjTable *table) {
    u32 fogRangeReg;
    u32 fogRangeRegK;
    int i;

    if (enable) {
        for (i = 0; i < 10; i += 2) {
            fogRangeRegK = 0;
            SET_REG_FIELD(fogRangeRegK, 12, 0, table->r[i]);
            SET_REG_FIELD(fogRangeRegK, 12, 12, table->r[i + 1]);
            SET_REG_FIELD(fogRangeRegK, 8, 24, 0xE9 + (i / 2));
            GX_WRITE_RA_REG(fogRangeRegK);
        }
    }

    fogRangeReg = 0;
    SET_REG_FIELD(fogRangeReg, 10, 0, center + 342);
    SET_REG_FIELD(fogRangeReg, 1, 10, enable);
    SET_REG_FIELD(fogRangeReg, 8, 24, 0xE8);
    GX_WRITE_RA_REG(fogRangeReg);
    gx->bpSentNot = GX_FALSE;
}

void GXSetBlendMode(GXBlendMode type, GXBlendFactor src_factor, GXBlendFactor dst_factor, GXLogicOp op) {
    u32 reg = gx->cmode0;

    SET_REG_FIELD(reg, 1, 11, type == GX_BM_SUBTRACT);
    SET_REG_FIELD(reg, 1, 0, type);
    SET_REG_FIELD(reg, 1, 1, type == GX_BM_LOGIC);
    SET_REG_FIELD(reg, 4, 12, op);
    SET_REG_FIELD(reg, 3, 8, src_factor);
    SET_REG_FIELD(reg, 3, 5, dst_factor);

    GX_WRITE_RA_REG(reg);
    gx->cmode0 = reg;
    gx->bpSentNot = GX_FALSE;
}

void GXSetColorUpdate(GXBool update_enable) {
    u32 reg = gx->cmode0;

    SET_REG_FIELD(reg, 1, 3, update_enable);
    GX_WRITE_RA_REG(reg);
    gx->cmode0 = reg;
    gx->bpSentNot = GX_FALSE;
}

void GXSetAlphaUpdate(GXBool update_enable) {
    u32 reg = gx->cmode0;

    SET_REG_FIELD(reg, 1, 4, update_enable);
    GX_WRITE_RA_REG(reg);
    gx->cmode0 = reg;
    gx->bpSentNot = GX_FALSE;
}

void GXSetZMode(GXBool compare_enable, GXCompare func, GXBool update_enable) {
    u32 reg = gx->zmode;

    SET_REG_FIELD(reg, 1, 0, compare_enable);
    SET_REG_FIELD(reg, 3, 1, func);
    SET_REG_FIELD(reg, 1, 4, update_enable);

    GX_WRITE_RA_REG(reg);
    gx->zmode = reg;
    gx->bpSentNot = GX_FALSE;
}

void GXSetZCompLoc(GXBool before_tex) {
    SET_REG_FIELD(gx->peCtrl, 1, 6, before_tex);
    GX_WRITE_RA_REG(gx->peCtrl);
    gx->bpSentNot = GX_FALSE;
}

void GXSetPixelFmt(GXPixelFmt pix_fmt, GXZFmt16 z_fmt) {
    u32 oldPeCtrl = gx->peCtrl;
    static u32 p2f[8] = {0, 1, 2, 3, 4, 4, 4, 5};

    SET_REG_FIELD(gx->peCtrl, 3, 0, p2f[pix_fmt]);
    SET_REG_FIELD(gx->peCtrl, 3, 3, z_fmt);

    if (oldPeCtrl != gx->peCtrl) {
        GX_WRITE_RA_REG(gx->peCtrl);
        SET_REG_FIELD(gx->genMode, 1, 9, pix_fmt == GX_PF_RGB565_Z16);
        gx->dirtyState |= 4;
    }

    if (p2f[pix_fmt] == 4) {
        SET_REG_FIELD(gx->cmode1, 2, 9, pix_fmt - 4);
        SET_REG_FIELD(gx->cmode1, 8, 24, 0x42);
        GX_WRITE_RA_REG(gx->cmode1);
    }

    gx->bpSentNot = GX_FALSE;
}

void GXSetDither(GXBool dither) {
    u32 reg = gx->cmode0;

    SET_REG_FIELD(reg, 1, 2, dither);
    GX_WRITE_RA_REG(reg);
    gx->cmode0 = reg;
    gx->bpSentNot = GX_FALSE;
}

void GXSetDstAlpha(GXBool enable, u8 alpha) {
    u32 reg = gx->cmode1;

    SET_REG_FIELD(reg, 8, 0, alpha);
    SET_REG_FIELD(reg, 1, 8, enable);
    GX_WRITE_RA_REG(reg);
    gx->cmode1 = reg;
    gx->bpSentNot = GX_FALSE;
}

void GXSetFieldMask(GXBool odd_mask, GXBool even_mask) {
    u32 reg = 0;

    SET_REG_FIELD(reg, 1, 0, even_mask);
    SET_REG_FIELD(reg, 1, 1, odd_mask);
    SET_REG_FIELD(reg, 8, 24, 0x44);
    GX_WRITE_RA_REG(reg);
    gx->bpSentNot = GX_FALSE;
}

void GXSetFieldMode(GXBool field_mode, GXBool half_aspect_ratio) {
    SET_REG_FIELD(gx->lpSize, 1, 22, half_aspect_ratio);
    GX_WRITE_RA_REG(gx->lpSize);
    __GXFlushTextureState();
    GX_WRITE_RA_REG((0x68 << 24) | field_mode);
    __GXFlushTextureState();
}
