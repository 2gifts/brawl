// gx frame buffer side: render mode tables, the efb->xfb / efb->texture copy setup (src/dst/clear/filter), and the
// copy commands. the copy regs are shadowed in gx->cpDisp* / gx->cpTex*, and written out when a copy is kicked.
#include "revolution/gx.h"
#include "revolution/gx/GXRegs.h"

#define SET_REG_FIELD(reg, size, shift, val) FAST_FLAG_SET(reg, val, shift, size)

extern void __GetImageTileCount(GXTexFmt fmt, u16 wd, u16 ht, u32* rowTiles, u32* colTiles, u32* cmpTiles);

// the standard render modes. only the entries with names in the sdk header are named here, the rest keep their
// address names (they are the matching non-deflicker / progressive variants as far as i can tell)
GXRenderModeObj GXNtsc480IntDf = {
    0x0, 640, 480, 480, 40, 0, 640, 480, 1, 0, 0,
    {{6, 6}, {6, 6}, {6, 6}, {6, 6}, {6, 6}, {6, 6}, {6, 6}, {6, 6}, {6, 6}, {6, 6}, {6, 6}, {6, 6}},
    {8, 8, 10, 12, 10, 8, 8},
};

GXRenderModeObj GXNtsc480Int = {
    0x0, 640, 480, 480, 40, 0, 640, 480, 1, 0, 0,
    {{6, 6}, {6, 6}, {6, 6}, {6, 6}, {6, 6}, {6, 6}, {6, 6}, {6, 6}, {6, 6}, {6, 6}, {6, 6}, {6, 6}},
    {0, 0, 21, 22, 21, 0, 0},
};

GXRenderModeObj lbl_80475FA0 = {
    0x2, 640, 480, 480, 40, 0, 640, 480, 0, 0, 0,
    {{6, 6}, {6, 6}, {6, 6}, {6, 6}, {6, 6}, {6, 6}, {6, 6}, {6, 6}, {6, 6}, {6, 6}, {6, 6}, {6, 6}},
    {0, 0, 21, 22, 21, 0, 0},
};

GXRenderModeObj lbl_80475FDC = {
    0x2, 640, 480, 480, 40, 0, 640, 480, 0, 0, 0,
    {{6, 6}, {6, 6}, {6, 6}, {6, 6}, {6, 6}, {6, 6}, {6, 6}, {6, 6}, {6, 6}, {6, 6}, {6, 6}, {6, 6}},
    {8, 8, 10, 12, 10, 8, 8},
};

GXRenderModeObj GXMpal480IntDf = {
    0x8, 640, 480, 480, 40, 0, 640, 480, 1, 0, 0,
    {{6, 6}, {6, 6}, {6, 6}, {6, 6}, {6, 6}, {6, 6}, {6, 6}, {6, 6}, {6, 6}, {6, 6}, {6, 6}, {6, 6}},
    {8, 8, 10, 12, 10, 8, 8},
};

GXRenderModeObj lbl_80476054 = {
    0x8, 640, 480, 480, 40, 0, 640, 480, 1, 0, 0,
    {{6, 6}, {6, 6}, {6, 6}, {6, 6}, {6, 6}, {6, 6}, {6, 6}, {6, 6}, {6, 6}, {6, 6}, {6, 6}, {6, 6}},
    {0, 0, 21, 22, 21, 0, 0},
};

GXRenderModeObj lbl_80476090 = {
    0xA, 640, 480, 480, 40, 0, 640, 480, 0, 0, 0,
    {{6, 6}, {6, 6}, {6, 6}, {6, 6}, {6, 6}, {6, 6}, {6, 6}, {6, 6}, {6, 6}, {6, 6}, {6, 6}, {6, 6}},
    {0, 0, 21, 22, 21, 0, 0},
};

GXRenderModeObj lbl_804760CC = {
    0xA, 640, 480, 480, 40, 0, 640, 480, 0, 0, 0,
    {{6, 6}, {6, 6}, {6, 6}, {6, 6}, {6, 6}, {6, 6}, {6, 6}, {6, 6}, {6, 6}, {6, 6}, {6, 6}, {6, 6}},
    {8, 8, 10, 12, 10, 8, 8},
};

GXRenderModeObj GXPal528IntDf = {
    0x4, 640, 528, 528, 40, 23, 640, 528, 1, 0, 0,
    {{6, 6}, {6, 6}, {6, 6}, {6, 6}, {6, 6}, {6, 6}, {6, 6}, {6, 6}, {6, 6}, {6, 6}, {6, 6}, {6, 6}},
    {8, 8, 10, 12, 10, 8, 8},
};

GXRenderModeObj lbl_80476144 = {
    0x4, 640, 528, 528, 40, 23, 640, 528, 1, 0, 0,
    {{6, 6}, {6, 6}, {6, 6}, {6, 6}, {6, 6}, {6, 6}, {6, 6}, {6, 6}, {6, 6}, {6, 6}, {6, 6}, {6, 6}},
    {0, 0, 21, 22, 21, 0, 0},
};

GXRenderModeObj GXEurgb60Hz480IntDf = {
    0x14, 640, 480, 480, 40, 0, 640, 480, 1, 0, 0,
    {{6, 6}, {6, 6}, {6, 6}, {6, 6}, {6, 6}, {6, 6}, {6, 6}, {6, 6}, {6, 6}, {6, 6}, {6, 6}, {6, 6}},
    {8, 8, 10, 12, 10, 8, 8},
};

GXRenderModeObj lbl_804761BC = {
    0x14, 640, 480, 480, 40, 0, 640, 480, 1, 0, 0,
    {{6, 6}, {6, 6}, {6, 6}, {6, 6}, {6, 6}, {6, 6}, {6, 6}, {6, 6}, {6, 6}, {6, 6}, {6, 6}, {6, 6}},
    {0, 0, 21, 22, 21, 0, 0},
};

GXRenderModeObj lbl_804761F8 = {
    0x16, 640, 480, 480, 40, 0, 640, 480, 0, 0, 0,
    {{6, 6}, {6, 6}, {6, 6}, {6, 6}, {6, 6}, {6, 6}, {6, 6}, {6, 6}, {6, 6}, {6, 6}, {6, 6}, {6, 6}},
    {0, 0, 21, 22, 21, 0, 0},
};

GXRenderModeObj lbl_80476234 = {
    0x16, 640, 480, 480, 40, 0, 640, 480, 0, 0, 0,
    {{6, 6}, {6, 6}, {6, 6}, {6, 6}, {6, 6}, {6, 6}, {6, 6}, {6, 6}, {6, 6}, {6, 6}, {6, 6}, {6, 6}},
    {8, 8, 10, 12, 10, 8, 8},
};

void GXAdjustForOverscan(const GXRenderModeObj* rmin, GXRenderModeObj* rmout, u16 hor, u16 ver) {
    u16 hor2 = hor * 2;
    u16 ver2 = ver * 2;
    u32 verf;
    u32 mode;
    u32 tvmode;

    if (rmin != rmout) {
        *rmout = *rmin;
    }

    tvmode = rmin->viTVmode;
    mode = tvmode & 3;
    rmout->fbWidth = rmin->fbWidth - hor2;
    verf = (u32)(ver2 * rmin->efbHeight) / rmin->xfbHeight;
    rmout->efbHeight = rmin->efbHeight - verf;

    if (rmin->xFBmode == VI_XFBMODE_SF && mode == 0) {
        rmout->xfbHeight = rmin->xfbHeight - (ver2 >> 1);
    } else {
        rmout->xfbHeight = rmin->xfbHeight - ver2;
    }

    rmout->viWidth = rmin->viWidth - hor2;

    if (mode == 1) {
        rmout->viHeight = rmin->viHeight - (ver2 << 1);
    } else {
        rmout->viHeight = rmin->viHeight - ver2;
    }

    rmout->viXOrigin = rmin->viXOrigin + hor;
    rmout->viYOrigin = rmin->viYOrigin + ver;
}

void GXSetDispCopySrc(u16 left, u16 top, u16 wd, u16 ht) {
    gx->cpDispSrc = 0;
    SET_REG_FIELD(gx->cpDispSrc, 10, 0, left);
    SET_REG_FIELD(gx->cpDispSrc, 10, 10, top);
    SET_REG_FIELD(gx->cpDispSrc, 8, 24, 0x49);

    gx->cpDispSize = 0;
    SET_REG_FIELD(gx->cpDispSize, 10, 0, wd - 1);
    SET_REG_FIELD(gx->cpDispSize, 10, 10, ht - 1);
    SET_REG_FIELD(gx->cpDispSize, 8, 24, 0x4A);
}

void GXSetTexCopySrc(u16 left, u16 top, u16 wd, u16 ht) {
    gx->cpTexSrc = 0;
    SET_REG_FIELD(gx->cpTexSrc, 10, 0, left);
    SET_REG_FIELD(gx->cpTexSrc, 10, 10, top);
    SET_REG_FIELD(gx->cpTexSrc, 8, 24, 0x49);

    gx->cpTexSize = 0;
    SET_REG_FIELD(gx->cpTexSize, 10, 0, wd - 1);
    SET_REG_FIELD(gx->cpTexSize, 10, 10, ht - 1);
    SET_REG_FIELD(gx->cpTexSize, 8, 24, 0x4A);
}

void GXSetDispCopyDst(u16 wd, u16 ht) {
    u16 stride = (u16)(wd << 1);

    gx->cpDispStride = 0;
    SET_REG_FIELD(gx->cpDispStride, 10, 0, stride >> 5);
    SET_REG_FIELD(gx->cpDispStride, 8, 24, 0x4D);
}

void GXSetTexCopyDst(u16 wd, u16 ht, GXTexFmt fmt, GXBool mipmap) {
    u32 rowTiles;
    u32 colTiles;
    u32 cmpTiles;
    u32 peTexFmt;

    gx->cpTexZ = 0;
    peTexFmt = fmt & 0xF;
    if (fmt == GX_TF_Z16) {
        peTexFmt = 0xB;
    }

    switch (fmt) {
    case GX_TF_I4:
    case GX_TF_I8:
    case GX_TF_IA4:
    case GX_TF_IA8:
    case GX_CTF_YUVA8:
        SET_REG_FIELD(gx->cpTex, 2, 15, 3);
        break;
    default:
        SET_REG_FIELD(gx->cpTex, 2, 15, 2);
        break;
    }

    gx->cpTexZ = (fmt & 0x10) == 0x10;
    SET_REG_FIELD(gx->cpTex, 1, 3, peTexFmt >> 3);

    __GetImageTileCount(fmt, wd, ht, &rowTiles, &colTiles, &cmpTiles);

    gx->cpTexStride = 0;
    SET_REG_FIELD(gx->cpTexStride, 10, 0, rowTiles * cmpTiles);
    SET_REG_FIELD(gx->cpTexStride, 8, 24, 0x4D);
    SET_REG_FIELD(gx->cpTex, 1, 9, mipmap);
    SET_REG_FIELD(gx->cpTex, 3, 4, peTexFmt);
}

void GXSetDispCopyFrame2Field(GXCopyMode mode) {
    SET_REG_FIELD(gx->cpDisp, 2, 12, mode);
    SET_REG_FIELD(gx->cpTex, 2, 12, 0);
}

void GXSetCopyClamp(GXFBClamp clamp) {
    SET_REG_FIELD(gx->cpDisp, 1, 0, clamp);
    gx->cpDisp = __rlwimi(gx->cpDisp, clamp, 0, 30, 30);
    SET_REG_FIELD(gx->cpTex, 1, 0, clamp);
    gx->cpTex = __rlwimi(gx->cpTex, clamp, 0, 30, 30);
}

static inline u32 XfbLines_i(u32 height, u32 iScale) {
    u32 tmp;
    u32 lines;

    tmp = iScale & 0x1FF;
    lines = ((height - 1) << 8) / tmp + 1;

    if (tmp > 0x80 && tmp < 0x100) {
        u32 t = tmp;
        while ((t & 1) == 0) {
            t >>= 1;
        }
        if (height % t == 0) {
            lines++;
        }
    }

    if (lines > 0x400) {
        lines = 0x400;
    }

    return lines;
}

u16 GXGetNumXfbLines(u16 height, f32 yscale) {
    return (u16)XfbLines_i(height, (u32)(256.0f / yscale));
}

f32 GXGetYScaleFactor(u16 efbHeight, u16 xfbHeight) {
    u32 tgtHt;
    u32 newHt;
    f32 fScale;
    f32 scale;

    tgtHt = xfbHeight;
    scale = (f32)xfbHeight / (f32)efbHeight;
    newHt = XfbLines_i(efbHeight, (u32)(256.0f / scale));

    while (newHt > xfbHeight) {
        tgtHt--;
        scale = (f32)tgtHt / (f32)efbHeight;
        newHt = XfbLines_i(efbHeight, (u32)(256.0f / scale));
    }

    fScale = scale;

    while (newHt < xfbHeight) {
        tgtHt++;
        fScale = scale;
        scale = (f32)tgtHt / (f32)efbHeight;
        newHt = XfbLines_i(efbHeight, (u32)(256.0f / scale));
    }

    return fScale;
}

u32 GXSetDispCopyYScale(f32 vscale) {
    u32 scale;
    u32 reg;
    GXBool check;
    u32 ht;

    scale = (u32)(256.0f / vscale) & 0x1FF;

    reg = 0;
    SET_REG_FIELD(reg, 9, 0, scale);
    SET_REG_FIELD(reg, 8, 24, 0x4E);
    GX_WRITE_RA_REG(reg);
    gx->bpSentNot = GX_FALSE;

    check = (scale != 256);
    SET_REG_FIELD(gx->cpDisp, 1, 10, check);

    ht = ((gx->cpDispSize >> 10) & 0x3FF) + 1;
    return XfbLines_i(ht, scale);
}

void GXSetCopyClear(GXColor clear_clr, u32 clear_z) {
    u32 reg;

    reg = 0;
    SET_REG_FIELD(reg, 8, 0, clear_clr.r);
    SET_REG_FIELD(reg, 8, 8, clear_clr.a);
    SET_REG_FIELD(reg, 8, 24, 0x4F);
    GX_WRITE_RA_REG(reg);

    reg = 0;
    SET_REG_FIELD(reg, 8, 0, clear_clr.b);
    SET_REG_FIELD(reg, 8, 8, clear_clr.g);
    SET_REG_FIELD(reg, 8, 24, 0x50);
    GX_WRITE_RA_REG(reg);

    reg = 0;
    SET_REG_FIELD(reg, 24, 0, clear_z);
    SET_REG_FIELD(reg, 8, 24, 0x51);
    GX_WRITE_RA_REG(reg);

    gx->bpSentNot = GX_FALSE;
}

void GXSetCopyFilter(GXBool aa, const u8 sample_pattern[12][2], GXBool vf, const u8 vfilter[7]) {
    u32 msLoc[4];
    u32 coeff0;
    u32 coeff1;

    if (aa) {
        msLoc[0] = 0;
        SET_REG_FIELD(msLoc[0], 4, 0, sample_pattern[0][0]);
        SET_REG_FIELD(msLoc[0], 4, 4, sample_pattern[0][1]);
        SET_REG_FIELD(msLoc[0], 4, 8, sample_pattern[1][0]);
        SET_REG_FIELD(msLoc[0], 4, 12, sample_pattern[1][1]);
        SET_REG_FIELD(msLoc[0], 4, 16, sample_pattern[2][0]);
        SET_REG_FIELD(msLoc[0], 4, 20, sample_pattern[2][1]);
        SET_REG_FIELD(msLoc[0], 8, 24, 1);

        msLoc[1] = 0;
        SET_REG_FIELD(msLoc[1], 4, 0, sample_pattern[3][0]);
        SET_REG_FIELD(msLoc[1], 4, 4, sample_pattern[3][1]);
        SET_REG_FIELD(msLoc[1], 4, 8, sample_pattern[4][0]);
        SET_REG_FIELD(msLoc[1], 4, 12, sample_pattern[4][1]);
        SET_REG_FIELD(msLoc[1], 4, 16, sample_pattern[5][0]);
        SET_REG_FIELD(msLoc[1], 4, 20, sample_pattern[5][1]);
        SET_REG_FIELD(msLoc[1], 8, 24, 2);

        msLoc[2] = 0;
        SET_REG_FIELD(msLoc[2], 4, 0, sample_pattern[6][0]);
        SET_REG_FIELD(msLoc[2], 4, 4, sample_pattern[6][1]);
        SET_REG_FIELD(msLoc[2], 4, 8, sample_pattern[7][0]);
        SET_REG_FIELD(msLoc[2], 4, 12, sample_pattern[7][1]);
        SET_REG_FIELD(msLoc[2], 4, 16, sample_pattern[8][0]);
        SET_REG_FIELD(msLoc[2], 4, 20, sample_pattern[8][1]);
        SET_REG_FIELD(msLoc[2], 8, 24, 3);

        msLoc[3] = 0;
        SET_REG_FIELD(msLoc[3], 4, 0, sample_pattern[9][0]);
        SET_REG_FIELD(msLoc[3], 4, 4, sample_pattern[9][1]);
        SET_REG_FIELD(msLoc[3], 4, 8, sample_pattern[10][0]);
        SET_REG_FIELD(msLoc[3], 4, 12, sample_pattern[10][1]);
        SET_REG_FIELD(msLoc[3], 4, 16, sample_pattern[11][0]);
        SET_REG_FIELD(msLoc[3], 4, 20, sample_pattern[11][1]);
        SET_REG_FIELD(msLoc[3], 8, 24, 4);
    } else {
        msLoc[0] = 0x01666666;
        msLoc[1] = 0x02666666;
        msLoc[2] = 0x03666666;
        msLoc[3] = 0x04666666;
    }

    GX_WRITE_RA_REG(msLoc[0]);
    GX_WRITE_RA_REG(msLoc[1]);
    GX_WRITE_RA_REG(msLoc[2]);
    GX_WRITE_RA_REG(msLoc[3]);

    coeff0 = 0;
    SET_REG_FIELD(coeff0, 8, 24, 0x53);
    coeff1 = 0;
    SET_REG_FIELD(coeff1, 8, 24, 0x54);

    if (vf) {
        SET_REG_FIELD(coeff0, 6, 0, vfilter[0]);
        SET_REG_FIELD(coeff0, 6, 6, vfilter[1]);
        SET_REG_FIELD(coeff0, 6, 12, vfilter[2]);
        SET_REG_FIELD(coeff0, 6, 18, vfilter[3]);
        SET_REG_FIELD(coeff1, 6, 0, vfilter[4]);
        SET_REG_FIELD(coeff1, 6, 6, vfilter[5]);
        SET_REG_FIELD(coeff1, 6, 12, vfilter[6]);
    } else {
        SET_REG_FIELD(coeff0, 6, 0, 0);
        SET_REG_FIELD(coeff0, 6, 6, 0);
        SET_REG_FIELD(coeff0, 6, 12, 21);
        SET_REG_FIELD(coeff0, 6, 18, 22);
        SET_REG_FIELD(coeff1, 6, 0, 21);
        SET_REG_FIELD(coeff1, 6, 6, 0);
        SET_REG_FIELD(coeff1, 6, 12, 0);
    }

    GX_WRITE_RA_REG(coeff0);
    GX_WRITE_RA_REG(coeff1);
    gx->bpSentNot = GX_FALSE;
}

void GXSetDispCopyGamma(GXGamma gamma) {
    SET_REG_FIELD(gx->cpDisp, 2, 7, gamma);
}

void GXCopyDisp(void* dest, GXBool clear) {
    u32 reg;
    u32 tempPeCtrl;
    u8 changePeCtrl;

    if (clear) {
        reg = gx->zmode;
        SET_REG_FIELD(reg, 1, 0, 1);
        SET_REG_FIELD(reg, 3, 1, 7);
        GX_WRITE_RA_REG(reg);

        reg = gx->cmode0;
        SET_REG_FIELD(reg, 1, 0, 0);
        SET_REG_FIELD(reg, 1, 1, 0);
        GX_WRITE_RA_REG(reg);
    }

    changePeCtrl = GX_FALSE;

    if (clear || (gx->peCtrl & 7) == 3) {
        if (((gx->peCtrl >> 6) & 1) == 1) {
            changePeCtrl = GX_TRUE;
            tempPeCtrl = gx->peCtrl;
            SET_REG_FIELD(tempPeCtrl, 1, 6, 0);
            GX_WRITE_RA_REG(tempPeCtrl);
        }
    }

    GX_WRITE_RA_REG(gx->cpDispSrc);
    GX_WRITE_RA_REG(gx->cpDispSize);
    GX_WRITE_RA_REG(gx->cpDispStride);

    reg = 0;
    SET_REG_FIELD(reg, 24, 0, (u32)dest >> 5);
    SET_REG_FIELD(reg, 8, 24, 0x4B);
    GX_WRITE_RA_REG(reg);

    SET_REG_FIELD(gx->cpDisp, 1, 11, clear);
    SET_REG_FIELD(gx->cpDisp, 1, 14, 1);
    SET_REG_FIELD(gx->cpDisp, 8, 24, 0x52);
    GX_WRITE_RA_REG(gx->cpDisp);

    if (clear) {
        GX_WRITE_RA_REG(gx->zmode);
        GX_WRITE_RA_REG(gx->cmode0);
    }

    if (changePeCtrl) {
        GX_WRITE_RA_REG(gx->peCtrl);
    }

    gx->bpSentNot = GX_FALSE;
}

void GXCopyTex(void* dest, GXBool clear) {
    u32 reg;
    u32 tempPeCtrl;
    u8 changePeCtrl;

    if (clear) {
        reg = gx->zmode;
        SET_REG_FIELD(reg, 1, 0, 1);
        SET_REG_FIELD(reg, 3, 1, 7);
        GX_WRITE_RA_REG(reg);

        reg = gx->cmode0;
        SET_REG_FIELD(reg, 1, 0, 0);
        SET_REG_FIELD(reg, 1, 1, 0);
        GX_WRITE_RA_REG(reg);
    }

    changePeCtrl = GX_FALSE;
    tempPeCtrl = gx->peCtrl;

    if (gx->cpTexZ && (tempPeCtrl & 7) != 3) {
        changePeCtrl = GX_TRUE;
        SET_REG_FIELD(tempPeCtrl, 3, 0, 3);
    }

    if (clear || (tempPeCtrl & 7) == 3) {
        if (((tempPeCtrl >> 6) & 1) == 1) {
            changePeCtrl = GX_TRUE;
            SET_REG_FIELD(tempPeCtrl, 1, 6, 0);
        }
    }

    if (changePeCtrl) {
        GX_WRITE_RA_REG(tempPeCtrl);
    }

    GX_WRITE_RA_REG(gx->cpTexSrc);
    GX_WRITE_RA_REG(gx->cpTexSize);
    GX_WRITE_RA_REG(gx->cpTexStride);

    reg = 0;
    SET_REG_FIELD(reg, 24, 0, (u32)dest >> 5);
    SET_REG_FIELD(reg, 8, 24, 0x4B);
    GX_WRITE_RA_REG(reg);

    SET_REG_FIELD(gx->cpTex, 1, 11, clear);
    SET_REG_FIELD(gx->cpTex, 1, 14, 0);
    SET_REG_FIELD(gx->cpTex, 8, 24, 0x52);
    GX_WRITE_RA_REG(gx->cpTex);

    if (clear) {
        GX_WRITE_RA_REG(gx->zmode);
        GX_WRITE_RA_REG(gx->cmode0);
    }

    if (changePeCtrl) {
        GX_WRITE_RA_REG(gx->peCtrl);
    }

    gx->bpSentNot = GX_FALSE;
}

void GXClearBoundingBox(void) {
    GX_WRITE_RA_REG(0x550003FF);
    GX_WRITE_RA_REG(0x560003FF);
    gx->bpSentNot = GX_FALSE;
}
