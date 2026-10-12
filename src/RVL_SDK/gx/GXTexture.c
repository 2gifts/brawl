// gx textures: tex object setup/queries, tlut objects, the tmem cache regions and the callbacks that pick them, plus
// the su (scale/offset) register update that has to follow any texture used by a tev stage or indirect stage.
#include "revolution/gx.h"
#include "revolution/gx/GXRegs.h"

#define SET_REG_FIELD(reg, size, shift, val) FAST_FLAG_SET(reg, val, shift, size)

extern void* memset(void*, int, unsigned long);
extern void __GXFlushTextureState(void);

// HYPOTHESIS: the in-memory layout of a GXTexObj, from the field offsets the code uses
typedef struct {
    u32 mode0;
    u32 mode1;
    u32 image0;
    u32 image3;
    void* userData;
    GXTexFmt fmt;
    u32 tlutName;
    u16 loadCnt;
    u8 loadFmt;
    u8 flags;
} __GXTexObjInt;

typedef struct {
    u32 tlut;
    u32 loadTlut0;
    u16 numEntries;
} __GXTlutObjInt;

typedef struct {
    u32 image1;
    u32 image2;
    u32 dummy;
    u8 is32bMipmap;
    u8 isCached;
} __GXTexRegionInt;

typedef struct {
    u32 loadTlut1;
    __GXTlutObjInt tlutObj;
} __GXTlutRegionInt;

static u8 GXTexMode0Ids[8] = {0x80, 0x81, 0x82, 0x83, 0xA0, 0xA1, 0xA2, 0xA3};
static u8 GXTexMode1Ids[8] = {0x84, 0x85, 0x86, 0x87, 0xA4, 0xA5, 0xA6, 0xA7};
static u8 GXTexImage0Ids[8] = {0x88, 0x89, 0x8A, 0x8B, 0xA8, 0xA9, 0xAA, 0xAB};
static u8 GXTexImage1Ids[8] = {0x8C, 0x8D, 0x8E, 0x8F, 0xAC, 0xAD, 0xAE, 0xAF};
static u8 GXTexImage2Ids[8] = {0x90, 0x91, 0x92, 0x93, 0xB0, 0xB1, 0xB2, 0xB3};
static u8 GXTexImage3Ids[8] = {0x94, 0x95, 0x96, 0x97, 0xB4, 0xB5, 0xB6, 0xB7};
static u8 GXTexTlutIds[8] = {0x98, 0x99, 0x9A, 0x9B, 0xB8, 0xB9, 0xBA, 0xBB};
static u8 GX2HWFiltConv[8] = {0, 4, 1, 5, 2, 6, 0, 0};
static u8 HW2GXFiltConv[8] = {0, 2, 4, 0, 1, 3, 5, 0};

u32 GXGetTexBufferSize(u16 width, u16 height, u32 format, GXBool mipmap, u8 max_lod) {
    u32 tileShiftX;
    u32 tileShiftY;
    u32 tileBytes;
    u32 bufferSize;
    u32 nx;
    u32 ny;
    u32 level;

    switch (format) {
    case GX_TF_I4:
    case GX_TF_C4:
    case GX_TF_CMPR:
    case GX_CTF_R4:
    case GX_CTF_Z4:
        tileShiftX = 3;
        tileShiftY = 3;
        break;
    case GX_TF_I8:
    case GX_TF_IA4:
    case GX_TF_C8:
    case GX_TF_Z8:
    case GX_CTF_RA4:
    case GX_CTF_A8:
    case GX_CTF_R8:
    case GX_CTF_G8:
    case GX_CTF_B8:
    case GX_CTF_Z8M:
    case GX_CTF_Z8L:
        tileShiftX = 3;
        tileShiftY = 2;
        break;
    case GX_TF_IA8:
    case GX_TF_RGB565:
    case GX_TF_RGB5A3:
    case GX_TF_RGBA8:
    case GX_TF_C14X2:
    case GX_TF_Z16:
    case GX_TF_Z24X8:
    case GX_CTF_RA8:
    case GX_CTF_RG8:
    case GX_CTF_GB8:
    case GX_CTF_Z16L:
        tileShiftX = 2;
        tileShiftY = 2;
        break;
    default:
        tileShiftY = 0;
        tileShiftX = 0;
        break;
    }

    if (format == GX_TF_RGBA8 || format == GX_TF_Z24X8) {
        tileBytes = 64;
    } else {
        tileBytes = 32;
    }

    if (mipmap == GX_TRUE) {
        bufferSize = 0;
        for (level = 0; level < max_lod; level++) {
            nx = (width + (1 << tileShiftX) - 1) >> tileShiftX;
            ny = (height + (1 << tileShiftY) - 1) >> tileShiftY;
            bufferSize += nx * ny * tileBytes;

            if (width == 1 && height == 1) {
                break;
            }

            width = (width > 1) ? (width >> 1) : 1;
            height = (height > 1) ? (height >> 1) : 1;
        }
    } else {
        nx = (width + (1 << tileShiftX) - 1) >> tileShiftX;
        ny = (height + (1 << tileShiftY) - 1) >> tileShiftY;
        bufferSize = nx * ny * tileBytes;
    }

    return bufferSize;
}

void __GetImageTileCount(GXTexFmt fmt, u16 wd, u16 ht, u32* rowTiles, u32* colTiles, u32* cmpTiles) {
    u32 xShift;
    u32 yShift;

    switch (fmt) {
    case GX_TF_I4:
    case GX_TF_C4:
    case GX_TF_CMPR:
    case GX_CTF_R4:
    case GX_CTF_Z4:
        xShift = 3;
        yShift = 3;
        break;
    case GX_TF_I8:
    case GX_TF_IA4:
    case GX_TF_C8:
    case GX_TF_Z8:
    case GX_CTF_RA4:
    case GX_CTF_A8:
    case GX_CTF_R8:
    case GX_CTF_G8:
    case GX_CTF_B8:
    case GX_CTF_Z8M:
    case GX_CTF_Z8L:
        xShift = 3;
        yShift = 2;
        break;
    case GX_TF_IA8:
    case GX_TF_RGB565:
    case GX_TF_RGB5A3:
    case GX_TF_RGBA8:
    case GX_TF_C14X2:
    case GX_TF_Z16:
    case GX_TF_Z24X8:
    case GX_CTF_RA8:
    case GX_CTF_RG8:
    case GX_CTF_GB8:
    case GX_CTF_Z16L:
        xShift = 2;
        yShift = 2;
        break;
    default:
        yShift = 0;
        xShift = 0;
        break;
    }

    if (wd == 0) {
        wd = 1;
    }
    if (ht == 0) {
        ht = 1;
    }

    *rowTiles = (wd + (1 << xShift) - 1) >> xShift;
    *colTiles = (ht + (1 << yShift) - 1) >> yShift;
    *cmpTiles = (fmt == GX_TF_RGBA8 || fmt == GX_TF_Z24X8) ? 2 : 1;
}

void GXInitTexObj(GXTexObj* obj, void* image_ptr, u16 width, u16 height, GXTexFmt format, GXTexWrapMode wrap_s, GXTexWrapMode wrap_t,
                  GXBool mipmap) {
    __GXTexObjInt* t = (__GXTexObjInt*)obj;
    u32 imageBase;
    u32 maxLOD;
    u16 rowT, colT;

    memset(t, 0, sizeof(GXTexObj));

    SET_REG_FIELD(t->mode0, 2, 0, wrap_s);
    SET_REG_FIELD(t->mode0, 2, 2, wrap_t);
    SET_REG_FIELD(t->mode0, 1, 4, 1);

    if (mipmap) {
        u8 lmax;

        t->flags |= 1;

        if (format == GX_TF_C4 || format == GX_TF_C8 || format == GX_TF_C14X2) {
            SET_REG_FIELD(t->mode0, 3, 5, 5);
        } else {
            SET_REG_FIELD(t->mode0, 3, 5, 6);
        }

        if (width > height) {
            maxLOD = 31 - __cntlzw(width);
        } else {
            maxLOD = 31 - __cntlzw(height);
        }
        lmax = (u8)(16.0f * maxLOD);
        SET_REG_FIELD(t->mode1, 8, 8, lmax);
    } else {
        SET_REG_FIELD(t->mode0, 3, 5, 4);
    }

    SET_REG_FIELD(t->image0, 10, 0, width - 1);
    SET_REG_FIELD(t->image0, 10, 10, height - 1);
    SET_REG_FIELD(t->image3, 24, 0, (u32)image_ptr >> 5);
    t->fmt = format;
    SET_REG_FIELD(t->image0, 4, 20, format);

    switch (format & 0xF) {
    case GX_TF_I4:
    case GX_TF_C4:
        t->loadFmt = 1;
        rowT = 3;
        colT = 3;
        break;
    case GX_TF_I8:
    case GX_TF_IA4:
    case GX_TF_C8:
        t->loadFmt = 2;
        rowT = 3;
        colT = 2;
        break;
    case GX_TF_IA8:
    case GX_TF_RGB565:
    case GX_TF_RGB5A3:
    case GX_TF_C14X2:
        t->loadFmt = 2;
        rowT = 2;
        colT = 2;
        break;
    case GX_TF_RGBA8:
        t->loadFmt = 3;
        rowT = 2;
        colT = 2;
        break;
    case GX_TF_CMPR:
        t->loadFmt = 0;
        rowT = 3;
        colT = 3;
        break;
    default:
        t->loadFmt = 2;
        rowT = 2;
        colT = 2;
        break;
    }

    t->flags |= 2;
    imageBase = (((width + (1 << rowT) - 1) >> rowT) * ((height + (1 << colT) - 1) >> colT));
    t->loadCnt = imageBase & 0x7FFF;
}

void GXInitTexObjCI(GXTexObj* obj, void* image_ptr, u16 width, u16 height, GXCITexFmt format, GXTexWrapMode wrap_s, GXTexWrapMode wrap_t,
                    GXBool mipmap, u32 tlut_name) {
    __GXTexObjInt* t = (__GXTexObjInt*)obj;

    GXInitTexObj(obj, image_ptr, width, height, (GXTexFmt)format, wrap_s, wrap_t, mipmap);
    t->tlutName = tlut_name;
    t->flags &= ~2;
}

void GXInitTexObjLOD(GXTexObj* obj, GXTexFilter min_filt, GXTexFilter mag_filt, f32 min_lod, f32 max_lod, f32 lod_bias, GXBool bias_clamp,
                     GXBool do_edge_lod, GXAnisotropy max_aniso) {
    __GXTexObjInt* t = (__GXTexObjInt*)obj;
    u8 lbias;
    u8 lmin;
    u8 lmax;

    if (lod_bias < -4.0f) {
        lod_bias = -4.0f;
    } else if (lod_bias >= 4.0f) {
        lod_bias = 3.99f;
    }

    lbias = (u8)(32.0f * lod_bias);
    SET_REG_FIELD(t->mode0, 8, 9, lbias);
    SET_REG_FIELD(t->mode0, 1, 4, (mag_filt == GX_LINEAR));
    SET_REG_FIELD(t->mode0, 3, 5, GX2HWFiltConv[min_filt]);
    SET_REG_FIELD(t->mode0, 1, 8, (do_edge_lod == GX_FALSE));
    SET_REG_FIELD(t->mode0, 2, 17, 0);
    SET_REG_FIELD(t->mode0, 2, 19, max_aniso);
    SET_REG_FIELD(t->mode0, 1, 21, bias_clamp);

    if (min_lod < 0.0f) {
        min_lod = 0.0f;
    } else if (min_lod > 10.0f) {
        min_lod = 10.0f;
    }
    lmin = (u8)(16.0f * min_lod);

    if (max_lod < 0.0f) {
        max_lod = 0.0f;
    } else if (max_lod > 10.0f) {
        max_lod = 10.0f;
    }
    lmax = (u8)(16.0f * max_lod);

    SET_REG_FIELD(t->mode1, 8, 0, lmin);
    SET_REG_FIELD(t->mode1, 8, 8, lmax);
}

void GXInitTexObjData(GXTexObj* obj, void* image_ptr) {
    __GXTexObjInt* t = (__GXTexObjInt*)obj;
    SET_REG_FIELD(t->image3, 24, 0, (u32)image_ptr >> 5);
}

void GXInitTexObjWrapMode(GXTexObj* obj, GXTexWrapMode wrap_s, GXTexWrapMode wrap_t) {
    __GXTexObjInt* t = (__GXTexObjInt*)obj;
    SET_REG_FIELD(t->mode0, 2, 0, wrap_s);
    SET_REG_FIELD(t->mode0, 2, 2, wrap_t);
}

void GXInitTexObjTlut(GXTexObj* obj, u32 tlut_name) {
    __GXTexObjInt* t = (__GXTexObjInt*)obj;
    t->tlutName = tlut_name;
}

void GXInitTexObjFilter(GXTexObj* obj, GXTexFilter min_filt, GXTexFilter mag_filt) {
    __GXTexObjInt* t = (__GXTexObjInt*)obj;
    SET_REG_FIELD(t->mode0, 1, 4, (mag_filt == GX_LINEAR));
    SET_REG_FIELD(t->mode0, 3, 5, GX2HWFiltConv[min_filt]);
}

void GXGetTexObjAll(const GXTexObj* obj, void** image_ptr, u16* width, u16* height, GXTexFmt* format, GXTexWrapMode* wrap_s,
                    GXTexWrapMode* wrap_t, GXBool* mipmap) {
    const __GXTexObjInt* t = (const __GXTexObjInt*)obj;

    *image_ptr = (void*)((t->image3 & 0xFFFFFF) << 5);
    *width = (t->image0 & 0x3FF) + 1;
    *height = ((t->image0 >> 10) & 0x3FF) + 1;
    *format = t->fmt;
    *wrap_s = (GXTexWrapMode)(t->mode0 & 3);
    *wrap_t = (GXTexWrapMode)((t->mode0 >> 2) & 3);
    *mipmap = (GXBool)(t->flags & 1);
}

void* GXGetTexObjData(const GXTexObj* obj) {
    const __GXTexObjInt* t = (const __GXTexObjInt*)obj;
    return (void*)((t->image3 & 0xFFFFFF) << 5);
}

u16 GXGetTexObjWidth(const GXTexObj* obj) {
    const __GXTexObjInt* t = (const __GXTexObjInt*)obj;
    return (t->image0 & 0x3FF) + 1;
}

u16 GXGetTexObjHeight(const GXTexObj* obj) {
    const __GXTexObjInt* t = (const __GXTexObjInt*)obj;
    return ((t->image0 >> 10) & 0x3FF) + 1;
}

GXTexFmt GXGetTexObjFmt(const GXTexObj* obj) {
    const __GXTexObjInt* t = (const __GXTexObjInt*)obj;
    return t->fmt;
}

GXTexWrapMode GXGetTexObjWrapS(const GXTexObj* obj) {
    const __GXTexObjInt* t = (const __GXTexObjInt*)obj;
    return (GXTexWrapMode)(t->mode0 & 3);
}

GXTexWrapMode GXGetTexObjWrapT(const GXTexObj* obj) {
    const __GXTexObjInt* t = (const __GXTexObjInt*)obj;
    return (GXTexWrapMode)((t->mode0 >> 2) & 3);
}

GXBool GXGetTexObjMipMap(const GXTexObj* obj) {
    const __GXTexObjInt* t = (const __GXTexObjInt*)obj;
    return (GXBool)(t->flags & 1);
}

void GXGetTexObjLODAll(const GXTexObj* obj, GXTexFilter* min_filt, GXTexFilter* mag_filt, f32* min_lod, f32* max_lod, f32* lod_bias,
                       GXBool* bias_clamp, GXBool* do_edge_lod, GXAnisotropy* max_aniso) {
    const __GXTexObjInt* t = (const __GXTexObjInt*)obj;

    *min_filt = (GXTexFilter)HW2GXFiltConv[(t->mode0 >> 5) & 7];
    *mag_filt = (GXTexFilter)((t->mode0 >> 4) & 1);
    *min_lod = (f32)(t->mode1 & 0xFF) / 16.0f;
    *max_lod = (f32)((t->mode1 >> 8) & 0xFF) / 16.0f;
    *lod_bias = (f32)(s8)(s16)((t->mode0 >> 9) & 0xFF) / 32.0f;
    *bias_clamp = (t->mode0 >> 21) & 1;
    *do_edge_lod = !((t->mode0 >> 8) & 1);
    *max_aniso = (GXAnisotropy)((t->mode0 >> 19) & 3);
}

u32 GXGetTexObjTlut(const GXTexObj* obj) {
    const __GXTexObjInt* t = (const __GXTexObjInt*)obj;
    return t->tlutName;
}

void GXLoadTexObjPreLoaded(const GXTexObj* obj, GXTexRegion* region, GXTexMapID id) {
    __GXTexObjInt* t = (__GXTexObjInt*)obj;
    __GXTexRegionInt* r = (__GXTexRegionInt*)region;
    u32 mode0 = t->mode0;
    u32 mode1 = t->mode1;
    u32 image0 = t->image0;
    u32 image1 = r->image1;
    u32 image2 = r->image2;
    u32 image3 = t->image3;

    SET_REG_FIELD(mode0, 8, 24, GXTexMode0Ids[id]);
    SET_REG_FIELD(mode1, 8, 24, GXTexMode1Ids[id]);
    SET_REG_FIELD(image0, 8, 24, GXTexImage0Ids[id]);
    SET_REG_FIELD(image1, 8, 24, GXTexImage1Ids[id]);
    SET_REG_FIELD(image2, 8, 24, GXTexImage2Ids[id]);
    SET_REG_FIELD(image3, 8, 24, GXTexImage3Ids[id]);

    GX_WRITE_RA_REG(mode0);
    GX_WRITE_RA_REG(mode1);
    GX_WRITE_RA_REG(image0);
    GX_WRITE_RA_REG(image1);
    GX_WRITE_RA_REG(image2);

    t->mode0 = mode0;
    t->mode1 = mode1;
    t->image0 = image0;
    r->image1 = image1;
    r->image2 = image2;
    t->image3 = image3;
    GX_WRITE_RA_REG(image3);

    if (!(t->flags & 2)) {
        __GXTlutRegionInt* tlr = (__GXTlutRegionInt*)(*gx->tlutRegionCallback)(t->tlutName);

        SET_REG_FIELD(tlr->tlutObj.loadTlut0, 8, 24, GXTexTlutIds[id]);
        GX_WRITE_RA_REG(tlr->tlutObj.loadTlut0);
    }

    gx->tImage0[id] = t->image0;
    gx->tMode0[id] = t->mode0;
    gx->dirtyState |= 1;
    gx->bpSentNot = GX_FALSE;
}

void GXLoadTexObj(const GXTexObj* obj, GXTexMapID id) {
    GXTexRegion* region;

    region = gx->texRegionCallback(obj, id);
    GXLoadTexObjPreLoaded(obj, region, id);
}

void GXInitTlutObj(GXTlutObj* tlut_obj, void* lut, GXTlutFmt fmt, u16 n_entries) {
    __GXTlutObjInt* t = (__GXTlutObjInt*)tlut_obj;

    t->tlut = 0;
    SET_REG_FIELD(t->tlut, 2, 10, fmt);
    SET_REG_FIELD(t->loadTlut0, 24, 0, (u32)lut >> 5);
    SET_REG_FIELD(t->loadTlut0, 8, 24, 0x64);
    t->numEntries = n_entries;
}

void GXLoadTlut(const GXTlutObj* tlut_obj, u32 tlut_name) {
    __GXTlutObjInt* t = (__GXTlutObjInt*)tlut_obj;
    __GXTlutRegionInt* r;

    r = (__GXTlutRegionInt*)(*gx->tlutRegionCallback)(tlut_name);
    __GXFlushTextureState();
    GX_WRITE_RA_REG(t->loadTlut0);
    GX_WRITE_RA_REG(r->loadTlut1);
    __GXFlushTextureState();
    SET_REG_FIELD(t->tlut, 10, 0, r->loadTlut1);
    r->tlutObj = *t;
}

void GXInitTexCacheRegion(GXTexRegion* region, GXBool is_32b_mipmap, u32 tmem_even, GXTexCacheSize size_even, u32 tmem_odd,
                          GXTexCacheSize size_odd) {
    __GXTexRegionInt* r = (__GXTexRegionInt*)region;
    u32 sizeSel;

    switch (size_even) {
    case GX_TEXCACHE_32K:
        sizeSel = 3;
        break;
    case GX_TEXCACHE_128K:
        sizeSel = 4;
        break;
    case GX_TEXCACHE_512K:
        sizeSel = 5;
        break;
    }

    r->image1 = 0;
    SET_REG_FIELD(r->image1, 15, 0, tmem_even >> 5);
    SET_REG_FIELD(r->image1, 3, 15, sizeSel);
    SET_REG_FIELD(r->image1, 3, 18, sizeSel);

    switch (size_odd) {
    case GX_TEXCACHE_32K:
        sizeSel = 3;
        break;
    case GX_TEXCACHE_128K:
        sizeSel = 4;
        break;
    case GX_TEXCACHE_512K:
        sizeSel = 5;
        break;
    case GX_TEXCACHE_NONE:
        sizeSel = 0;
        break;
    }

    r->image2 = 0;
    SET_REG_FIELD(r->image2, 15, 0, tmem_odd >> 5);
    SET_REG_FIELD(r->image2, 3, 15, sizeSel);
    SET_REG_FIELD(r->image2, 3, 18, sizeSel);

    r->is32bMipmap = is_32b_mipmap;
    r->isCached = 1;
}

void GXInitTlutRegion(GXTlutRegion* region, u32 tmem_addr, GXTlutSize tlut_sz) {
    u32* r = (u32*)region;
    u32 reg;

    tmem_addr -= 0x80000;
    reg = 0;
    SET_REG_FIELD(reg, 10, 0, tmem_addr >> 9);
    SET_REG_FIELD(reg, 11, 10, tlut_sz);
    SET_REG_FIELD(reg, 8, 24, 0x65);
    r[0] = reg;
}

void GXInvalidateTexAll(void) {
    __GXFlushTextureState();
    GX_WRITE_RA_REG(0x66001000);
    GX_WRITE_RA_REG(0x66001100);
    __GXFlushTextureState();
}

GXTexRegionCallback GXSetTexRegionCallback(GXTexRegionCallback f) {
    GXTexRegionCallback old = gx->texRegionCallback;
    gx->texRegionCallback = f;
    return old;
}

GXTlutRegionCallback GXSetTlutRegionCallback(GXTlutRegionCallback f) {
    GXTlutRegionCallback old = gx->tlutRegionCallback;
    gx->tlutRegionCallback = f;
    return old;
}

// called for real (not inlined) from __GXSetSUTexRegs
#pragma push
#pragma auto_inline off
void __SetSURegs(u32 tmap, u32 tcoord) {
    u32 w;
    u32 h;
    BOOL s_bias;
    BOOL t_bias;

    w = gx->tImage0[tmap] & 0x3FF;
    h = (gx->tImage0[tmap] >> 10) & 0x3FF;
    SET_REG_FIELD(gx->suTs0[tcoord], 16, 0, w);
    SET_REG_FIELD(gx->suTs1[tcoord], 16, 0, h);

    s_bias = (gx->tMode0[tmap] & 3) == 1;
    t_bias = ((gx->tMode0[tmap] >> 2) & 3) == 1;
    SET_REG_FIELD(gx->suTs0[tcoord], 1, 16, s_bias);
    SET_REG_FIELD(gx->suTs1[tcoord], 1, 16, t_bias);

    GX_WRITE_RA_REG(gx->suTs0[tcoord]);
    GX_WRITE_RA_REG(gx->suTs1[tcoord]);
    gx->bpSentNot = GX_FALSE;
}
#pragma pop

void __GXSetSUTexRegs(void) {
    u32* pt;
    u32 nStages;
    GXData* g2;
    u32 nIndStages;
    u32 tmap;
    u32 i;
    u32 tcoord;

    if (gx->tcsManEnab != 0xFF) {
        nIndStages = (gx->genMode >> 16) & 7;
        nStages = ((gx->genMode >> 10) & 0xF) + 1;

        for (i = 0; i < nIndStages; i++) {
            switch (i) {
            case 0:
                tmap = gx->iref & 7;
                tcoord = (gx->iref >> 3) & 7;
                break;
            case 1:
                tmap = (gx->iref >> 6) & 7;
                tcoord = (gx->iref >> 9) & 7;
                break;
            case 2:
                tmap = (gx->iref >> 12) & 7;
                tcoord = (gx->iref >> 15) & 7;
                break;
            case 3:
                tmap = (gx->iref >> 18) & 7;
                tcoord = (gx->iref >> 21) & 7;
                break;
            }

            if (!(gx->tcsManEnab & (1 << tcoord))) {
                __SetSURegs(tmap, tcoord);
            }
        }

        g2 = gx;
        pt = g2->tref;
        for (i = 0; i < nStages; i++) {
            tmap = g2->texmapId[i] & ~0x100;

            if (i & 1) {
                tcoord = (pt[i / 2] >> 15) & 7;
            } else {
                tcoord = (pt[i / 2] >> 3) & 7;
            }

            if (tmap != 0xFF && !(gx->tcsManEnab & (1 << tcoord)) && (g2->tevTcEnab & (1 << i))) {
                __SetSURegs(tmap, tcoord);
            }
        }
    }
}

void __GXSetTmemConfig(u32 config) {
    switch (config) {
    case 2:
        GX_WRITE_RA_REG(0x8C0D8000);
        GX_WRITE_RA_REG(0x900DC000);
        GX_WRITE_RA_REG(0x8D0D8800);
        GX_WRITE_RA_REG(0x910DC800);
        GX_WRITE_RA_REG(0x8E0D9000);
        GX_WRITE_RA_REG(0x920DD000);
        GX_WRITE_RA_REG(0x8F0D9800);
        GX_WRITE_RA_REG(0x930DD800);
        GX_WRITE_RA_REG(0xAC0DA000);
        GX_WRITE_RA_REG(0xB00DC400);
        GX_WRITE_RA_REG(0xAD0DA800);
        GX_WRITE_RA_REG(0xB10DCC00);
        GX_WRITE_RA_REG(0xAE0DB000);
        GX_WRITE_RA_REG(0xB20DD400);
        GX_WRITE_RA_REG(0xAF0DB800);
        GX_WRITE_RA_REG(0xB30DDC00);
        break;
    case 1:
        GX_WRITE_RA_REG(0x8C0D8000);
        GX_WRITE_RA_REG(0x900DC000);
        GX_WRITE_RA_REG(0x8D0D8800);
        GX_WRITE_RA_REG(0x910DC800);
        GX_WRITE_RA_REG(0x8E0D9000);
        GX_WRITE_RA_REG(0x920DD000);
        GX_WRITE_RA_REG(0x8F0D9800);
        GX_WRITE_RA_REG(0x930DD800);
        GX_WRITE_RA_REG(0xAC0DA000);
        GX_WRITE_RA_REG(0xB00DE000);
        GX_WRITE_RA_REG(0xAD0DA800);
        GX_WRITE_RA_REG(0xB10DE800);
        GX_WRITE_RA_REG(0xAE0DB000);
        GX_WRITE_RA_REG(0xB20DF000);
        GX_WRITE_RA_REG(0xAF0DB800);
        GX_WRITE_RA_REG(0xB30DF800);
        break;
    case 0:
    default:
        GX_WRITE_RA_REG(0x8C0D8000);
        GX_WRITE_RA_REG(0x900DC000);
        GX_WRITE_RA_REG(0x8D0D8400);
        GX_WRITE_RA_REG(0x910DC400);
        GX_WRITE_RA_REG(0x8E0D8800);
        GX_WRITE_RA_REG(0x920DC800);
        GX_WRITE_RA_REG(0x8F0D8C00);
        GX_WRITE_RA_REG(0x930DCC00);
        GX_WRITE_RA_REG(0xAC0D9000);
        GX_WRITE_RA_REG(0xB00DD000);
        GX_WRITE_RA_REG(0xAD0D9400);
        GX_WRITE_RA_REG(0xB10DD400);
        GX_WRITE_RA_REG(0xAE0D9800);
        GX_WRITE_RA_REG(0xB20DD800);
        GX_WRITE_RA_REG(0xAF0D9C00);
        GX_WRITE_RA_REG(0xB30DDC00);
        break;
    }
}
