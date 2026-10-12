// gx transform unit: projection / position / normal / texture matrices, viewport and scissor. the matrices
// are copied into the xf memory through the fifo with paired singles.
#include "revolution/gx.h"
#include "revolution/gx/GXRegs.h"

#define XF_MTX_POS_BASE 0x0000
#define XF_MTX_NRM_BASE 0x0400
#define XF_MTX_DUALTEX_BASE 0x0500

typedef struct {
    f32 x, y, z;
} Vec3;

// the matrix copy loops, all of them just move paired singles from memory into the fifo
static inline void LoadProjPS_i(register f32 *dst) {
    register f32 ps_0, ps_1, ps_2;
    register GXData *src;

    asm volatile {
        lwz src, __GXData
        psq_l ps_0, 0 + GXData.projMtx(src), 0, 0
        psq_l ps_1, 8 + GXData.projMtx(src), 0, 0
        psq_l ps_2, 16 + GXData.projMtx(src), 0, 0
        psq_st ps_0, 0(dst), 0, 0
        psq_st ps_1, 8(dst), 0, 0
        psq_st ps_2, 16(dst), 0, 0
    }
}

static inline void WriteProjPS_i(register volatile void *dst, register const f32 *src) {
    register f32 ps_0, ps_1, ps_2;

    asm volatile {
        psq_l ps_0, 0(src), 0, 0
        psq_l ps_1, 8(src), 0, 0
        psq_l ps_2, 16(src), 0, 0
        psq_st ps_0, 0(dst), 0, 0
        psq_st ps_1, 0(dst), 0, 0
        psq_st ps_2, 0(dst), 0, 0
    }
}

static inline void Copy6Floats_i(register f32 *dst, register const f32 *src) {
    register f32 ps_0, ps_1, ps_2;

    asm volatile {
        psq_l ps_0, 0(src), 0, 0
        psq_l ps_1, 8(src), 0, 0
        psq_l ps_2, 16(src), 0, 0
        psq_st ps_0, 0(dst), 0, 0
        psq_st ps_1, 8(dst), 0, 0
        psq_st ps_2, 16(dst), 0, 0
    }
}

static inline void WriteMTXPS4x3_i(register volatile void *dst, register const f32 *src) {
    register f32 ps_0, ps_1, ps_2, ps_3, ps_4, ps_5;

    asm volatile {
        psq_l ps_0, 0(src), 0, 0
        psq_l ps_1, 8(src), 0, 0
        psq_l ps_2, 16(src), 0, 0
        psq_l ps_3, 24(src), 0, 0
        psq_l ps_4, 32(src), 0, 0
        psq_l ps_5, 40(src), 0, 0

        psq_st ps_0, 0(dst), 0, 0
        psq_st ps_1, 0(dst), 0, 0
        psq_st ps_2, 0(dst), 0, 0
        psq_st ps_3, 0(dst), 0, 0
        psq_st ps_4, 0(dst), 0, 0
        psq_st ps_5, 0(dst), 0, 0
    }
}

static inline void WriteMTXPS3x3_i(register volatile void *dst, register const f32 *src) {
    register f32 ps_0, ps_1, ps_2, ps_3, ps_4, ps_5;

    asm volatile {
        psq_l ps_0, 0(src), 0, 0
        lfs ps_1, 8(src)
        psq_l ps_2, 16(src), 0, 0
        lfs ps_3, 24(src)
        psq_l ps_4, 32(src), 0, 0
        lfs ps_5, 40(src)

        psq_st ps_0, 0(dst), 0, 0
        stfs ps_1, 0(dst)
        psq_st ps_2, 0(dst), 0, 0
        stfs ps_3, 0(dst)
        psq_st ps_4, 0(dst), 0, 0
        stfs ps_5, 0(dst)
    }
}

static inline void WriteMTXPS4x2_i(register volatile void *dst, register const f32 *src) {
    register f32 ps_0, ps_1, ps_2, ps_3;

    asm volatile {
        psq_l ps_0, 0(src), 0, 0
        psq_l ps_1, 8(src), 0, 0
        psq_l ps_2, 16(src), 0, 0
        psq_l ps_3, 24(src), 0, 0

        psq_st ps_0, 0(dst), 0, 0
        psq_st ps_1, 0(dst), 0, 0
        psq_st ps_2, 0(dst), 0, 0
        psq_st ps_3, 0(dst), 0, 0
    }
}

void GXProject(f32 x, f32 y, f32 z, const f32 mtx[3][4], const f32 *pm, const f32 *vp, f32 *sx, f32 *sy, f32 *sz) {
    Vec3 peye;
    f32 xc, yc, zc, wc;

    peye.x = mtx[0][3] + (mtx[0][2] * z + (mtx[0][0] * x + mtx[0][1] * y));
    peye.y = mtx[1][3] + (mtx[1][2] * z + (mtx[1][0] * x + mtx[1][1] * y));
    peye.z = mtx[2][3] + (mtx[2][2] * z + (mtx[2][0] * x + mtx[2][1] * y));

    if (pm[0] == 0.0f) {
        xc = peye.x * pm[1] + peye.z * pm[2];
        yc = peye.y * pm[3] + peye.z * pm[4];
        zc = pm[6] + peye.z * pm[5];
        wc = 1.0f / -peye.z;
    } else {
        xc = pm[2] + peye.x * pm[1];
        yc = pm[4] + peye.y * pm[3];
        zc = pm[6] + peye.z * pm[5];
        wc = 1.0f;
    }

    *sx = vp[2] / 2.0f + (vp[0] + wc * (xc * vp[2] / 2.0f));
    *sy = vp[3] / 2.0f + (vp[1] + wc * (-yc * vp[3] / 2.0f));
    *sz = vp[5] + wc * (zc * (vp[5] - vp[4]));
}

void __GXSetProjection(void) {
    // needs the temp to match
    volatile void *wgpipe = &gxfifo;

    GX_WRITE_U8(0x10);
    GX_WRITE_U32(0x00061020);
    WriteProjPS_i(wgpipe, gx->projMtx);
    GX_WRITE_U32(gx->projType);
}

void GXSetProjection(const f32 mtx[4][4], GXProjectionType type) {
    gx->projType = type;

    gx->projMtx[0] = mtx[0][0];
    gx->projMtx[2] = mtx[1][1];
    gx->projMtx[4] = mtx[2][2];
    gx->projMtx[5] = mtx[2][3];

    if (type == GX_ORTHOGRAPHIC) {
        gx->projMtx[1] = mtx[0][3];
        gx->projMtx[3] = mtx[1][3];
    } else {
        gx->projMtx[1] = mtx[0][2];
        gx->projMtx[3] = mtx[1][2];
    }

    gx->dirtyState |= 0x08000000;
}

void GXSetProjectionv(const f32 *ptr) {
    gx->projType = ptr[0] == 0.0f ? GX_PERSPECTIVE : GX_ORTHOGRAPHIC;
    Copy6Floats_i(gx->projMtx, ptr + 1);
    gx->dirtyState |= 0x08000000;
}

void GXGetProjectionv(f32 *ptr) {
    ptr[0] = gx->projType != GX_PERSPECTIVE ? 1.0f : 0.0f;
    LoadProjPS_i(ptr + 1);
}

void GXLoadPosMtxImm(const f32 mtx[3][4], u32 id) {
    GX_WRITE_U8(0x10);
    GX_WRITE_U32((id * 4 + XF_MTX_POS_BASE) | ((4 * 3 - 1) << 16));
    WriteMTXPS4x3_i(&gxfifo, (const f32 *)mtx);
}

void GXLoadPosMtxIndx(u16 mtx_indx, u32 id) {
    u32 reg = 0;

    reg = __rlwimi(reg, id * 4 + XF_MTX_POS_BASE, 0, 20, 31);
    reg = __rlwimi(reg, 4 * 3 - 1, 12, 16, 19);
    reg = __rlwimi(reg, mtx_indx, 16, 0, 15);
    GX_WRITE_U8(0x20);
    GX_WRITE_U32(reg);
}

void GXLoadNrmMtxImm(const f32 mtx[3][4], u32 id) {
    GX_WRITE_U8(0x10);
    GX_WRITE_U32((id * 3 + XF_MTX_NRM_BASE) | ((3 * 3 - 1) << 16));
    WriteMTXPS3x3_i(&gxfifo, (const f32 *)mtx);
}

void GXLoadNrmMtxIndx3x3(u16 mtx_indx, u32 id) {
    u32 reg = 0;

    reg = __rlwimi(reg, id * 3 + XF_MTX_NRM_BASE, 0, 20, 31);
    reg = __rlwimi(reg, 3 * 3 - 1, 12, 16, 19);
    reg = __rlwimi(reg, mtx_indx, 16, 0, 15);
    GX_WRITE_U8(0x28);
    GX_WRITE_U32(reg);
}

void GXSetCurrentMtx(u32 id) {
    gx->matIdxA = __rlwimi(gx->matIdxA, id, 0, 26, 31);
    gx->dirtyState |= 0x04000000;
}

void GXLoadTexMtxImm(const f32 mtx[][4], u32 id, GXTexMtxType type) {
    u32 addr;
    u32 num;

    addr = id >= 0x40 ? (id - 0x40) * 4 + XF_MTX_DUALTEX_BASE : id * 4 + (u64)XF_MTX_POS_BASE;
    num = type == GX_MTX2x4 ? (u64)(2 * 4) : 3 * 4;

    GX_WRITE_U8(0x10);
    GX_WRITE_U32(addr | ((num - 1) << 16));

    if (type == GX_MTX3x4) {
        WriteMTXPS4x3_i(&gxfifo, (const f32 *)mtx);
    } else {
        WriteMTXPS4x2_i(&gxfifo, (const f32 *)mtx);
    }
}

void __GXSetViewport(void) {
    f32 a, b, c, d, e, f;
    f32 near, far;

    a = gx->vpWd / 2.0f;
    b = -gx->vpHt / 2.0f;
    d = gx->vpLeft + (gx->vpWd / 2.0f) + 342.0f;
    e = gx->vpTop + (gx->vpHt / 2.0f) + 342.0f;

    near = gx->vpNearz * gx->zScale;
    far = gx->vpFarz * gx->zScale;
    c = far - near;
    f = far + gx->zOffset;

    GX_WRITE_U8(0x10);
    GX_WRITE_U32(0x0005101A);
    GX_WRITE_F32(a);
    GX_WRITE_F32(b);
    GX_WRITE_F32(c);
    GX_WRITE_F32(d);
    GX_WRITE_F32(e);
    GX_WRITE_F32(f);
}

void GXSetViewportJitter(f32 left, f32 top, f32 wd, f32 ht, f32 nearz, f32 farz, u32 field) {
    if (field == 0) {
        top -= 0.5f;
    }

    gx->vpLeft = left;
    gx->vpTop = top;
    gx->vpWd = wd;
    gx->vpHt = ht;
    gx->vpNearz = nearz;
    gx->vpFarz = farz;
    gx->dirtyState |= 0x10000000;
}

void GXSetViewport(f32 left, f32 top, f32 wd, f32 ht, f32 nearz, f32 farz) {
    gx->vpLeft = left;
    gx->vpTop = top;
    gx->vpWd = wd;
    gx->vpHt = ht;
    gx->vpNearz = nearz;
    gx->vpFarz = farz;
    gx->dirtyState |= 0x10000000;
}

void GXGetViewportv(f32 *vp) {
    Copy6Floats_i(vp, &gx->vpLeft);
}

void GXSetZScaleOffset(f32 scale, f32 offset) {
    gx->zOffset = 16777215.0f * offset;
    gx->zScale = 1.0f + 16777215.0f * scale;
    gx->dirtyState |= 0x10000000;
}

void GXSetScissor(u32 left, u32 top, u32 wd, u32 ht) {
    u32 tp, lf, bm, rt;

    tp = top + 342;
    lf = left + 342;
    bm = tp + ht - 1;
    rt = lf + wd - 1;

    gx->suScis0 = __rlwimi(gx->suScis0, tp, 0, 21, 31);
    gx->suScis0 = __rlwimi(gx->suScis0, lf, 12, 9, 19);
    gx->suScis1 = __rlwimi(gx->suScis1, bm, 0, 21, 31);
    gx->suScis1 = __rlwimi(gx->suScis1, rt, 12, 9, 19);

    GX_WRITE_RA_REG(gx->suScis0);
    GX_WRITE_RA_REG(gx->suScis1);
    gx->bpSentNot = GX_FALSE;
}

void GXGetScissor(u32 *left, u32 *top, u32 *wd, u32 *ht) {
    u32 tp = (gx->suScis0 >> 0) & 0x7FF;
    u32 lf = (gx->suScis0 >> 12) & 0x7FF;
    u32 bm = (gx->suScis1 >> 0) & 0x7FF;
    u32 rt = (gx->suScis1 >> 12) & 0x7FF;

    *left = lf - 342;
    *top = tp - 342;
    *wd = rt - lf + 1;
    *ht = bm - tp + 1;
}

void GXSetScissorBoxOffset(s32 x_off, s32 y_off) {
    u32 reg = 0;
    u32 hx, hy;

    hx = (u32)(x_off + 342) >> 1;
    hy = (u32)(y_off + 342) >> 1;
    reg = __rlwimi(reg, hx, 0, 22, 31);
    reg = __rlwimi(reg, hy, 10, 12, 21);
    reg = __rlwimi(reg, 0x59, 24, 0, 7);
    GX_WRITE_RA_REG(reg);
    gx->bpSentNot = GX_FALSE;
}

void GXSetClipMode(GXClipMode mode) {
    GX_WRITE_U8(0x10);
    GX_WRITE_U32(0x1005);
    GX_WRITE_U32(mode);
    gx->bpSentNot = GX_TRUE;
}

void __GXSetMatrixIndex(GXAttr matIdxAttr) {
    if (matIdxAttr < GX_VA_TEX4MTXIDX) {
        GX_WRITE_U8(8);
        GX_WRITE_U8(0x30);
        GX_WRITE_U32(gx->matIdxA);
        GX_WRITE_U8(0x10);
        GX_WRITE_U32(0x1018);
        GX_WRITE_U32(gx->matIdxA);
    } else {
        GX_WRITE_U8(8);
        GX_WRITE_U8(0x40);
        GX_WRITE_U32(gx->matIdxB);
        GX_WRITE_U8(0x10);
        GX_WRITE_U32(0x1019);
        GX_WRITE_U32(gx->matIdxB);
    }

    gx->bpSentNot = GX_TRUE;
}
