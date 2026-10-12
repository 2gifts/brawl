// gx bring-up: grabbing the hardware register windows, fifo + shutdown setup, the default texture region
// callbacks and then __GXInitGX which pushes every piece of gx state to its default through the normal api calls.
#include "revolution/gx.h"
#include "revolution/gx/GXRegs.h"
#include "revolution/os.h"
#include "revolution/vi.h"
#include "revolution/mtx.h"
#include "revolution/base/PPCArch.h"

#define SET_REG_FIELD(reg, size, shift, val) FAST_FLAG_SET(reg, val, shift, size)

extern void GXSetTevKAlphaSel(GXTevStageID, GXTevKAlphaSel);
extern void __GXFlushTextureState(void);
extern void __GXInitRevisionBits(void);
extern void __GXSetIndirectMask(u32);
extern void __GXSetGenMode(void);
extern void __GXSetTmemConfig(u32);
extern void __GXFifoInit(void);
extern void __GXPEInit(void);
extern void fn_801F05C0(void);

// the version string the sdk stamps in. it lives in the same .data blob as the tables below
const char* __GXVersion = "<< RVL_SDK - GX \\trelease build: May  8 2007 12:59:16 (0x4199_60831) >>";

volatile void* __piReg;
volatile void* __cpReg;
volatile void* __peReg;
volatile void* __memReg;

// declared backwards: the c compiler lays statics out in reverse order
static BOOL shutdownStarted;
static OSTime shutdownTime;
static u32 shutdownCount;

static __GXFifoObj FifoObj;

static BOOL fn_801ED66C(BOOL final, u32 event);

// (initialized data comes out in declaration order)
static u8 DefaultTexData[32] __attribute__((aligned(32))) = {
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
};

static GXVtxAttrFmtList GXDefaultVATList[13] = {
    {GX_VA_POS, GX_POS_XYZ, GX_F32, 0},
    {GX_VA_NRM, GX_NRM_XYZ, GX_F32, 0},
    {GX_VA_CLR0, GX_CLR_RGBA, GX_RGBA8, 0},
    {GX_VA_CLR1, GX_CLR_RGBA, GX_RGBA8, 0},
    {GX_VA_TEX0, GX_TEX_ST, GX_F32, 0},
    {GX_VA_TEX1, GX_TEX_ST, GX_F32, 0},
    {GX_VA_TEX2, GX_TEX_ST, GX_F32, 0},
    {GX_VA_TEX3, GX_TEX_ST, GX_F32, 0},
    {GX_VA_TEX4, GX_TEX_ST, GX_F32, 0},
    {GX_VA_TEX5, GX_TEX_ST, GX_F32, 0},
    {GX_VA_TEX6, GX_TEX_ST, GX_F32, 0},
    {GX_VA_TEX7, GX_TEX_ST, GX_F32, 0},
    {GX_VA_NULL, GX_POS_XY, GX_U8, 0},
};

static f32 GXDefaultProjData[8] = {1.0f, 0.0f, 1.0f, 0.0f, -1.0f, -2.0f, 0.0f, 0.0f};

static u32 GXTexRegionAddrTable[48] = {
    0x00000, 0x10000, 0x20000, 0x30000, 0x40000, 0x50000, 0x60000, 0x70000,
    0x08000, 0x18000, 0x28000, 0x38000, 0x48000, 0x58000, 0x68000, 0x78000,
    0x00000, 0x90000, 0x20000, 0xB0000, 0x40000, 0x98000, 0x60000, 0xB8000,
    0x80000, 0x10000, 0xA0000, 0x30000, 0x88000, 0x50000, 0xA8000, 0x70000,
    0x00000, 0x90000, 0x20000, 0xB0000, 0x40000, 0x90000, 0x60000, 0xB0000,
    0x80000, 0x10000, 0xA0000, 0x30000, 0x80000, 0x50000, 0xA0000, 0x70000,
};

static OSShutdownFunctionInfo GXShutdownFuncInfo = {fn_801ED66C, 127, NULL, NULL};

// texture region the default callback hands back for a given map
GXTexRegion* GXDefaultTexRegionCallback(const GXTexObj* t_obj, GXTexMapID id) {
    GXTexFmt fmt;
    GXBool mipmap;

    fmt = GXGetTexObjFmt(t_obj);
    mipmap = GXGetTexObjMipMap(t_obj);
    id = (GXTexMapID)(id % 8);

    switch (fmt) {
    case GX_TF_RGBA8:
        if (mipmap) {
            return &gx->TexRegions2[id];
        } else {
            return &gx->TexRegions1[id];
        }
    case GX_TF_C4:
    case GX_TF_C8:
    case GX_TF_C14X2:
        return &gx->TexRegions0[id];
    default:
        if (mipmap) {
            return &gx->TexRegions1[id];
        } else {
            return &gx->TexRegions0[id];
        }
    }
}

GXTlutRegion* GXDefaultTlutRegionCallback(u32 tlut) {
    if (tlut >= 20) {
        return NULL;
    }

    return &gx->TlutRegions[tlut];
}

// shutdown hook: on the dry run it just watches the memory counter for activity, on the real thing it parks the gp
static BOOL fn_801ED66C(BOOL final, u32 event) {
    u32 reg;
    OSTime time;

    if (!final) {
        if (!shutdownStarted) {
            shutdownCount = GX_MEM_COUNTER_READ_U32(MEM_PE_REQCOUNT);
            shutdownTime = OSGetTime();
            shutdownStarted = TRUE;
            return FALSE;
        } else {
            time = OSGetTime();
            reg = GX_MEM_COUNTER_READ_U32(MEM_PE_REQCOUNT);
            if (time - shutdownTime < 10) {
                return FALSE;
            }
            if (reg != shutdownCount) {
                shutdownTime = time;
                shutdownCount = reg;
                return FALSE;
            }
        }
    } else {
        GXSetBreakPtCallback(NULL);
        GXSetDrawSyncCallback(NULL);
        GXSetDrawDoneCallback(NULL);
        GX_WRITE_U32(0);
        GX_WRITE_U32(0);
        GX_WRITE_U32(0);
        GX_WRITE_U32(0);
        GX_WRITE_U32(0);
        GX_WRITE_U32(0);
        GX_WRITE_U32(0);
        GX_WRITE_U32(0);
        PPCSync();
        GX_CP_REG_WRITE_U16(1, 0);
        GX_CP_REG_WRITE_U16(2, 3);
        gx->abtWaitPECopy = GX_TRUE;
        fn_801F05C0();
    }

    return TRUE;
}

void __GXInitRevisionBits(void) {
    int i;
    u32 reg0;
    u32 reg1;
    u32 reg2;

    // flag the two high bits on the vat entries (revision check) and push them
    for (i = 0; i < 8; i++) {
        SC_CP_VAT_REG_A_SET_BYTEDEQUANT(gx->vatA[i], 1);
        SC_CP_VAT_REG_B_SET_VCACHE_ENHANCE(gx->vatB[i], 1);
        GX_WRITE_U8(8);
        GX_WRITE_U8(0x80 | i);
        GX_WRITE_U32(gx->vatB[i]);
    }

    reg0 = 0;
    reg1 = 0;
    reg2 = 0;
    SET_REG_FIELD(reg0, 6, 0, 0x3F);
    SET_REG_FIELD(reg1, 1, 0, 1);
    SET_REG_FIELD(reg2, 4, 0, 0xF);
    SET_REG_FIELD(reg2, 8, 24, 0x58);
    GX_WRITE_XF_REG(0x1000, reg0, 0);
    GX_WRITE_XF_REG(0x1012, reg1, 0);
    GX_WRITE_RA_REG(reg2);
}

GXFifoObj* GXInit(void* base, u32 size) {
    u32 i;
    u32 reg;
    u32 freqBase;
    static BOOL shutdownFuncRegistered;
    u32 hid2;

    OSRegisterVersion(__GXVersion);

    gx->inDispList = GX_FALSE;
    gx->dlSaveContext = GX_TRUE;
    gx->abtWaitPECopy = GX_TRUE;
    gx->tcsManEnab = 0;
    gx->tevTcEnab = 0;

    GXSetMisc(GX_MT_XF_FLUSH, 0);

    __piReg = OSPhysicalToUncached(0x0C003000);
    __cpReg = OSPhysicalToUncached(0x0C000000);
    __peReg = OSPhysicalToUncached(0x0C001000);
    __memReg = OSPhysicalToUncached(0x0C004000);

    __GXFifoInit();
    GXInitFifoBase((GXFifoObj*)&FifoObj, base, size);
    GXSetCPUFifo((GXFifoObj*)&FifoObj);
    GXSetGPFifo((GXFifoObj*)&FifoObj);

    if (!shutdownFuncRegistered) {
        OSRegisterShutdownFunction(&GXShutdownFuncInfo);
        shutdownFuncRegistered = TRUE;
    }

    __GXPEInit();

    // turn the write gather pipe on
    hid2 = PPCMfhid2();
    PPCMtwpar(0x0C008000);
    hid2 |= 0x40000000;
    PPCMthid2(hid2);

    gx->genMode = 0;
    gx->bpMask = 0xFF;
    SET_REG_FIELD(gx->bpMask, 8, 24, 0xF);
    gx->lpSize = 0;
    SET_REG_FIELD(gx->lpSize, 8, 24, 0x22);

    for (i = 0; i < 16; i++) {
        gx->tevc[i] = 0;
        gx->teva[i] = 0;
        gx->tref[i / 2] = 0;
        gx->texmapId[i] = GX_TEXMAP_NULL;
        SET_REG_FIELD(gx->tevc[i], 8, 24, 0xC0 + i * 2);
        SET_REG_FIELD(gx->teva[i], 8, 24, 0xC1 + i * 2);
        SET_REG_FIELD(gx->tevKsel[i / 2], 8, 24, 0xF6 + i / 2);
        SET_REG_FIELD(gx->tref[i / 2], 8, 24, 0x28 + i / 2);
    }

    gx->iref = 0;
    SET_REG_FIELD(gx->iref, 8, 24, 0x27);

    for (i = 0; i < 8; i++) {
        gx->suTs0[i] = 0;
        gx->suTs1[i] = 0;
        SET_REG_FIELD(gx->suTs0[i], 8, 24, 0x30 + i * 2);
        SET_REG_FIELD(gx->suTs1[i], 8, 24, 0x31 + i * 2);
    }

    SET_REG_FIELD(gx->suScis0, 8, 24, 0x20);
    SET_REG_FIELD(gx->suScis1, 8, 24, 0x21);
    SET_REG_FIELD(gx->cmode0, 8, 24, 0x41);
    SET_REG_FIELD(gx->cmode1, 8, 24, 0x42);
    SET_REG_FIELD(gx->zmode, 8, 24, 0x40);
    SET_REG_FIELD(gx->peCtrl, 8, 24, 0x43);
    SET_REG_FIELD(gx->cpTex, 2, 7, 0);

    gx->zScale = 16777216.0f;
    gx->zOffset = 0.0f;
    gx->dirtyState = 0;
    gx->dirtyVAT = 0;

    freqBase = OS_BUS_CLOCK / 500;

    __GXFlushTextureState();
    reg = (freqBase >> 11) | 0x69000400;
    GX_WRITE_RA_REG(reg);

    __GXFlushTextureState();
    reg = (freqBase / 4224) | 0x46000200;
    GX_WRITE_RA_REG(reg);

    __GXInitRevisionBits();

    for (i = 0; i < 8; i++) {
        GXInitTexCacheRegion(&gx->TexRegions0[i], GX_FALSE, GXTexRegionAddrTable[i], GX_TEXCACHE_32K, GXTexRegionAddrTable[i + 8],
                             GX_TEXCACHE_32K);
        GXInitTexCacheRegion(&gx->TexRegions1[i], GX_FALSE, GXTexRegionAddrTable[i + 16], GX_TEXCACHE_32K, GXTexRegionAddrTable[i + 24],
                             GX_TEXCACHE_32K);
        GXInitTexCacheRegion(&gx->TexRegions2[i], GX_TRUE, GXTexRegionAddrTable[i + 32], GX_TEXCACHE_32K, GXTexRegionAddrTable[i + 40],
                             GX_TEXCACHE_32K);
    }

    for (i = 0; i < 16; i++) {
        GXInitTlutRegion(&gx->TlutRegions[i], 0xC0000 + i * 0x2000, GX_TLUT_256);
    }

    for (i = 0; i < 4; i++) {
        GXInitTlutRegion(&gx->TlutRegions[16 + i], 0xE0000 + i * 0x8000, GX_TLUT_1K);
    }

    GX_CP_REG_WRITE_U16(3, 0);
    SET_REG_FIELD(gx->perfSel, 4, 4, 0);
    GX_WRITE_U8(8);
    GX_WRITE_U8(0x20);
    GX_WRITE_U32(gx->perfSel);
    GX_WRITE_XF_REG(0x1006, 0, 0);
    GX_WRITE_RA_REG(0x23000000);
    GX_WRITE_RA_REG(0x24000000);
    GX_WRITE_RA_REG(0x67000000);

    __GXSetIndirectMask(0);
    __GXSetTmemConfig(2);
    __GXInitGX();

    return (GXFifoObj*)&FifoObj;
}

void __GXInitGX(void) {
    GXRenderModeObj* rmode;
    GXTexObj texObj;
    Mtx ident;
    GXColor clearColor = {64, 64, 64, 255};
    GXColor whiteColor = {255, 255, 255, 255};
    GXColor blackColor = {0, 0, 0, 0};
    u32 i;

    switch (VIGetTvFormat()) {
    case VI_NTSC:
        rmode = &GXNtsc480IntDf;
        break;
    case VI_PAL:
        rmode = &GXPal528IntDf;
        break;
    case VI_EURGB60:
        rmode = &GXEurgb60Hz480IntDf;
        break;
    case VI_MPAL:
        rmode = &GXMpal480IntDf;
        break;
    default:
        rmode = &GXNtsc480IntDf;
        break;
    }

    GXSetCopyClear(clearColor, 0xFFFFFF);

    GXSetTexCoordGen2(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_IDENTITY, GX_FALSE, 0x7D);
    GXSetTexCoordGen2(GX_TEXCOORD1, GX_TG_MTX2x4, GX_TG_TEX1, GX_IDENTITY, GX_FALSE, 0x7D);
    GXSetTexCoordGen2(GX_TEXCOORD2, GX_TG_MTX2x4, GX_TG_TEX2, GX_IDENTITY, GX_FALSE, 0x7D);
    GXSetTexCoordGen2(GX_TEXCOORD3, GX_TG_MTX2x4, GX_TG_TEX3, GX_IDENTITY, GX_FALSE, 0x7D);
    GXSetTexCoordGen2(GX_TEXCOORD4, GX_TG_MTX2x4, GX_TG_TEX4, GX_IDENTITY, GX_FALSE, 0x7D);
    GXSetTexCoordGen2(GX_TEXCOORD5, GX_TG_MTX2x4, GX_TG_TEX5, GX_IDENTITY, GX_FALSE, 0x7D);
    GXSetTexCoordGen2(GX_TEXCOORD6, GX_TG_MTX2x4, GX_TG_TEX6, GX_IDENTITY, GX_FALSE, 0x7D);
    GXSetTexCoordGen2(GX_TEXCOORD7, GX_TG_MTX2x4, GX_TG_TEX7, GX_IDENTITY, GX_FALSE, 0x7D);
    GXSetNumTexGens(1);

    GXClearVtxDesc();
    GXInvalidateVtxCache();

    for (i = GX_VA_POS; i <= 24; i++) {
        GXSetArray((GXAttr)i, gx, 0);
    }

    for (i = 0; i < 8; i++) {
        GXSetVtxAttrFmtv((GXVtxFmt)i, GXDefaultVATList);
    }

    GXSetLineWidth(6, GX_TO_ZERO);
    GXSetPointSize(6, GX_TO_ZERO);

    GXEnableTexOffsets(GX_TEXCOORD0, GX_FALSE, GX_FALSE);
    GXEnableTexOffsets(GX_TEXCOORD1, GX_FALSE, GX_FALSE);
    GXEnableTexOffsets(GX_TEXCOORD2, GX_FALSE, GX_FALSE);
    GXEnableTexOffsets(GX_TEXCOORD3, GX_FALSE, GX_FALSE);
    GXEnableTexOffsets(GX_TEXCOORD4, GX_FALSE, GX_FALSE);
    GXEnableTexOffsets(GX_TEXCOORD5, GX_FALSE, GX_FALSE);
    GXEnableTexOffsets(GX_TEXCOORD6, GX_FALSE, GX_FALSE);
    GXEnableTexOffsets(GX_TEXCOORD7, GX_FALSE, GX_FALSE);

    ident[0][0] = 1.0f;
    ident[0][1] = 0.0f;
    ident[0][2] = 0.0f;
    ident[0][3] = 0.0f;
    ident[1][0] = 0.0f;
    ident[1][1] = 1.0f;
    ident[1][2] = 0.0f;
    ident[1][3] = 0.0f;
    ident[2][0] = 0.0f;
    ident[2][1] = 0.0f;
    ident[2][2] = 1.0f;
    ident[2][3] = 0.0f;

    GXLoadPosMtxImm(ident, GX_PNMTX0);
    GXLoadNrmMtxImm(ident, GX_PNMTX0);
    GXSetCurrentMtx(GX_PNMTX0);
    GXLoadTexMtxImm(ident, GX_IDENTITY, GX_MTX3x4);
    GXLoadTexMtxImm(ident, 0x7D, GX_MTX3x4);

    GXSetViewport(0.0f, 0.0f, (f32)rmode->fbWidth, (f32)rmode->xfbHeight, 0.0f, 1.0f);
    GXSetProjectionv(GXDefaultProjData);
    __GXSetGenMode();
    GXSetCoPlanar(GX_FALSE);
    GXSetCullMode(GX_CULL_BACK);
    GXSetClipMode(GX_CLIP_ENABLE);
    GXSetScissor(0, 0, rmode->fbWidth, rmode->efbHeight);
    GXSetScissorBoxOffset(0, 0);

    GXSetNumChans(0);
    GXSetChanCtrl(GX_COLOR0A0, GX_FALSE, GX_SRC_REG, GX_SRC_VTX, GX_LIGHT_NULL, GX_DF_NONE, GX_AF_NONE);
    GXSetChanAmbColor(GX_COLOR0A0, blackColor);
    GXSetChanMatColor(GX_COLOR0A0, whiteColor);
    GXSetChanCtrl(GX_COLOR1A1, GX_FALSE, GX_SRC_REG, GX_SRC_VTX, GX_LIGHT_NULL, GX_DF_NONE, GX_AF_NONE);
    GXSetChanAmbColor(GX_COLOR1A1, blackColor);
    GXSetChanMatColor(GX_COLOR1A1, whiteColor);

    GXInvalidateTexAll();
    GXSetTexRegionCallback(GXDefaultTexRegionCallback);
    GXSetTlutRegionCallback(GXDefaultTlutRegionCallback);

    GXInitTexObj(&texObj, DefaultTexData, 4, 4, GX_TF_IA8, GX_CLAMP, GX_CLAMP, GX_FALSE);
    GXLoadTexObj(&texObj, GX_TEXMAP0);
    GXLoadTexObj(&texObj, GX_TEXMAP1);
    GXLoadTexObj(&texObj, GX_TEXMAP2);
    GXLoadTexObj(&texObj, GX_TEXMAP3);
    GXLoadTexObj(&texObj, GX_TEXMAP4);
    GXLoadTexObj(&texObj, GX_TEXMAP5);
    GXLoadTexObj(&texObj, GX_TEXMAP6);
    GXLoadTexObj(&texObj, GX_TEXMAP7);

    GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR0A0);
    GXSetTevOrder(GX_TEVSTAGE1, GX_TEXCOORD1, GX_TEXMAP1, GX_COLOR0A0);
    GXSetTevOrder(GX_TEVSTAGE2, GX_TEXCOORD2, GX_TEXMAP2, GX_COLOR0A0);
    GXSetTevOrder(GX_TEVSTAGE3, GX_TEXCOORD3, GX_TEXMAP3, GX_COLOR0A0);
    GXSetTevOrder(GX_TEVSTAGE4, GX_TEXCOORD4, GX_TEXMAP4, GX_COLOR0A0);
    GXSetTevOrder(GX_TEVSTAGE5, GX_TEXCOORD5, GX_TEXMAP5, GX_COLOR0A0);
    GXSetTevOrder(GX_TEVSTAGE6, GX_TEXCOORD6, GX_TEXMAP6, GX_COLOR0A0);
    GXSetTevOrder(GX_TEVSTAGE7, GX_TEXCOORD7, GX_TEXMAP7, GX_COLOR0A0);
    GXSetTevOrder(GX_TEVSTAGE8, GX_TEXCOORD_NULL, GX_TEXMAP_NULL, GX_COLOR_NULL);
    GXSetTevOrder(GX_TEVSTAGE9, GX_TEXCOORD_NULL, GX_TEXMAP_NULL, GX_COLOR_NULL);
    GXSetTevOrder(GX_TEVSTAGE10, GX_TEXCOORD_NULL, GX_TEXMAP_NULL, GX_COLOR_NULL);
    GXSetTevOrder(GX_TEVSTAGE11, GX_TEXCOORD_NULL, GX_TEXMAP_NULL, GX_COLOR_NULL);
    GXSetTevOrder(GX_TEVSTAGE12, GX_TEXCOORD_NULL, GX_TEXMAP_NULL, GX_COLOR_NULL);
    GXSetTevOrder(GX_TEVSTAGE13, GX_TEXCOORD_NULL, GX_TEXMAP_NULL, GX_COLOR_NULL);
    GXSetTevOrder(GX_TEVSTAGE14, GX_TEXCOORD_NULL, GX_TEXMAP_NULL, GX_COLOR_NULL);
    GXSetTevOrder(GX_TEVSTAGE15, GX_TEXCOORD_NULL, GX_TEXMAP_NULL, GX_COLOR_NULL);

    GXSetNumTevStages(1);
    GXSetTevOp(GX_TEVSTAGE0, GX_REPLACE);
    GXSetAlphaCompare(GX_ALWAYS, 0, GX_AOP_AND, GX_ALWAYS, 0);
    GXSetZTexture(GX_ZT_DISABLE, GX_TF_Z8, 0);

    for (i = 0; i < 16; i++) {
        GXSetTevKColorSel((GXTevStageID)i, GX_TEV_KCSEL_1_4);
        GXSetTevKAlphaSel((GXTevStageID)i, GX_TEV_KASEL_1);
        GXSetTevSwapMode((GXTevStageID)i, GX_TEV_SWAP0, GX_TEV_SWAP0);
    }

    GXSetTevSwapModeTable(GX_TEV_SWAP0, GX_CH_RED, GX_CH_GREEN, GX_CH_BLUE, GX_CH_ALPHA);
    GXSetTevSwapModeTable(GX_TEV_SWAP1, GX_CH_RED, GX_CH_RED, GX_CH_RED, GX_CH_ALPHA);
    GXSetTevSwapModeTable(GX_TEV_SWAP2, GX_CH_GREEN, GX_CH_GREEN, GX_CH_GREEN, GX_CH_ALPHA);
    GXSetTevSwapModeTable(GX_TEV_SWAP3, GX_CH_BLUE, GX_CH_BLUE, GX_CH_BLUE, GX_CH_ALPHA);

    for (i = 0; i < 16; i++) {
        GXSetTevDirect((GXTevStageID)i);
    }

    GXSetNumIndStages(0);
    GXSetIndTexCoordScale(GX_INDTEXSTAGE0, GX_ITS_1, GX_ITS_1);
    GXSetIndTexCoordScale(GX_INDTEXSTAGE1, GX_ITS_1, GX_ITS_1);
    GXSetIndTexCoordScale(GX_INDTEXSTAGE2, GX_ITS_1, GX_ITS_1);
    GXSetIndTexCoordScale(GX_INDTEXSTAGE3, GX_ITS_1, GX_ITS_1);

    GXSetFog(GX_FOG_NONE, 0.0f, 1.0f, 0.1f, 1.0f, blackColor);
    GXSetFogRangeAdj(GX_FALSE, 0, NULL);
    GXSetBlendMode(GX_BM_NONE, GX_BL_SRCALPHA, GX_BL_INVSRCALPHA, GX_LO_CLEAR);
    GXSetColorUpdate(GX_TRUE);
    GXSetAlphaUpdate(GX_TRUE);
    GXSetZMode(GX_TRUE, GX_LEQUAL, GX_TRUE);
    GXSetZCompLoc(GX_TRUE);
    GXSetDither(GX_TRUE);
    GXSetDstAlpha(GX_FALSE, 0);
    GXSetPixelFmt(GX_PF_RGB8_Z24, GX_ZC_LINEAR);
    GXSetFieldMask(GX_TRUE, GX_TRUE);
    GXSetFieldMode(rmode->field_rendering, (rmode->viHeight == 2 * rmode->xfbHeight) ? GX_TRUE : GX_FALSE);

    GXSetDispCopySrc(0, 0, rmode->fbWidth, rmode->efbHeight);
    GXSetDispCopyDst(rmode->fbWidth, rmode->efbHeight);
    GXSetDispCopyYScale((f32)rmode->xfbHeight / (f32)rmode->efbHeight);
    GXSetCopyClamp((GXFBClamp)(GX_CLAMP_TOP | GX_CLAMP_BOTTOM));
    GXSetCopyFilter(rmode->aa, rmode->sample_pattern, GX_TRUE, rmode->vfilter);
    GXSetDispCopyGamma(GX_GM_1_0);
    GXSetDispCopyFrame2Field(GX_COPY_PROGRESSIVE);
    GXClearBoundingBox();

    GXPokeColorUpdate(GX_TRUE);
    GXPokeAlphaUpdate(GX_TRUE);
    GXPokeDither(GX_FALSE);
    GXPokeBlendMode(GX_BM_NONE, GX_BL_ZERO, GX_BL_ONE, GX_LO_SET);
    GXPokeAlphaMode(GX_ALWAYS, 0);
    GXPokeAlphaRead(GX_READ_FF);
    GXPokeDstAlpha(GX_FALSE, 0);
    GXPokeZMode(GX_TRUE, GX_ALWAYS, GX_TRUE);
    GXSetGPMetric(GX_PERF0_NONE, GX_PERF1_NONE);
    GXClearGPMetric();
}
