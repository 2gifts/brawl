#include <types.h>
// Havok translation unit hkPoweredChainSolverUtil.o (main.dol 0x80290594-0x80293CBC).
// Not yet decompiled. Functions in address order:
//   0x80290594  1016  hkPoweredChainBuildJacobian
//   0x8029098C   308  hkPoweredChain_BuildConstraintMatrixAndLuDecomposition
//   0x80290AC0     4  __ct
//   0x80290AC4   288  setZero
//   0x80290BE4  1676  hkPoweredChain_ComputeConstraintMatrix_DiagonalAtRow
//   0x80291270    20  __cl
//   0x80291284   824  hkPoweredChain_ComputeConstraintMatrix_NextOffDiagonalAtR
//   0x802915BC     8  __ct
//   0x802915C4   888  hkPoweredChain_UpdateLuDecomposition
//   0x8029193C   820  __as
//   0x80291C70   360  hkPoweredChain_ComputeConstraintMatrixLuDecomposition_For
//   0x80291DD8    16  max2<f>
//   0x80291DE8   820  __ct
//   0x8029211C   148  hkPoweredChain_DisableMotorInMatrixRow_ThisConstraint
//   0x802921B0   172  hkPoweredChain_DisableMotorInMatrixRow_NextConstraint
//   0x8029225C  2248  hkPoweredChain_CalculateVelocities
//   0x80292B24    84  hkPoweredChain_OverwriteVelocityWithExplicitImpulse
//   0x80292B78   964  hkPoweredChain_RestoreVelocityValue
//   0x80292F3C   332  hkPoweredChain_SolveConstraintMatrix_ComputeVectorG
//   0x80293088   636  hkPoweredChain_ScanAndDisableMotors
//   0x80293304  2488  hkPoweredChain_ScanAndEnableMotors

// Function bodies below are taken from https://github.com/jj-link/brawl.

namespace hkMath {
float max2f(float a, float b) {
    if (a > b) {
        return a;
    }
    return b;
}
}

// Shape from https://github.com/jj-link/brawl src/havok/fn_80292B24.cpp.
// hkPoweredChain_OverwriteVelocityWithExplicitImpulse
struct hkPoweredChainMotorEntry_80292B24 {
    float x, a, b, c, d, e;
};
struct hkPoweredChainMotorInput_80292B24 {
    unsigned char flags;
    unsigned char pad[3];
    hkPoweredChainMotorEntry_80292B24 entries[3];
};
struct hkPoweredChainMotorOutput_80292B24 {
    unsigned char pad[16];
    float values[4];
};
// TODO: cleanup (shape from jj-link/brawl fn_80292B24)
extern "C" void fn_80292B24(int index, int channel, hkPoweredChainMotorInput_80292B24* input,
                            hkPoweredChainMotorOutput_80292B24* output) {
    float value;
    hkPoweredChainMotorInput_80292B24* in = &input[index];
    hkPoweredChainMotorOutput_80292B24* out = &output[index];
    if ((((int)in->flags >> (channel * 2)) & 3) == 1)
        value = in->entries[channel].a;
    else
        value = in->entries[channel].b;
    out->values[channel] = value;
}

// fn_802921B0: body from https://github.com/jj-link/brawl src/havok/fn_802921B0.cpp (namespaced; labels as in our symbols).
#include <types.h>
namespace jj_fn_802921B0 {
extern "C" {
extern "C" float lbl_805A3E00;

// TODO: cleanup (shape from jj-link/brawl fn_802921B0)
void fn_802921B0(unsigned char* arg0, float* arg1) {
    int flags = *arg0;
    if (flags != 0) {
        float v = lbl_805A3E00;
        for (int i = 0; i < 3; ++i) {
            if ((flags >> (i << 1)) & 3) {
                arg1[12 + i * 4 + 3] = v;
                arg1[12 + i * 4 + 2] = v;
                arg1[12 + i * 4 + 1] = v;
                arg1[12 + i * 4] = v;
                arg1[36 + i * 4 + 3] = v;
                arg1[36 + i * 4 + 2] = v;
                arg1[36 + i * 4 + 1] = v;
                arg1[36 + i * 4] = v;
            }
            flags = *arg0;
        }
    }
}
}
} // namespace jj_fn_802921B0

// fn_80292F3C: body from https://github.com/jj-link/brawl src/havok/fn_80292F3C.cpp (namespaced; callees rebound to our C++ names).
#include <types.h>
#include <havok/hkMatrix6.h>
namespace jj_fn_80292F3C {
struct Block {
    float v[8];
};

struct Context {
    int unk0;
    int count;
    char pad8[0x18];
    char* matrices;
    char pad24[8];
    Block* values;
};

extern "C" const float lbl_805A3E00;

// TODO: cleanup (shape from jj-link/brawl fn_80292F3C)
extern "C" void fn_80292F3C(Context* ctx, Block* out)
{
    Block tmp __attribute__((aligned(16)));
    int m, v;
    Block* base = out + 1;
    int i;

    base[-1].v[0] = base[-1].v[1] = base[-1].v[2] = base[-1].v[3] = lbl_805A3E00;
    base[-1].v[4] = base[-1].v[5] = base[-1].v[6] = base[-1].v[7] = lbl_805A3E00;

    for (i = 0, m = 0, v = 0; i < ctx->count;
         m += 0x3c0, v += 0x20, ++i) {
        const Block* p = (const Block*)((const char*)ctx->values + v);
        hkMatrix6SetMulV(*(hkVector8*)(&tmp), *(const hkMatrix6*)(ctx->matrices + m), *(const hkVector8*)(base + (i - 1)));

        const void* matrix = ctx->matrices + m + 0xc0;
        tmp.v[0] = p->v[0] - tmp.v[0];
        tmp.v[1] = p->v[1] - tmp.v[1];
        tmp.v[2] = p->v[2] - tmp.v[2];
        tmp.v[3] = p->v[3] - tmp.v[3];
        tmp.v[4] = p->v[4] - tmp.v[4];
        tmp.v[5] = p->v[5] - tmp.v[5];
        tmp.v[6] = p->v[6] - tmp.v[6];
        tmp.v[7] = p->v[7] - tmp.v[7];

        hkMatrix6SetMulV(*(hkVector8*)((Block*)((char*)base + v)), *(const hkMatrix6*)(matrix), *(const hkVector8*)(&tmp));
    }
}
} // namespace jj_fn_80292F3C

// TODO: cleanup (shape from jj-link/brawl fn_80291DE8)
// fn_80291DE8: body from https://github.com/jj-link/brawl src/havok/fn_80291DE8.cpp (namespaced; callees rebound to our C++ names).
#include <types.h>
namespace jj_fn_80291DE8 {
extern "C" void fn_80291DE8(float *dst, const float *src) {
    dst[0] = src[0];
    dst[1] = src[1];
    dst[2] = src[2];
    dst[3] = src[3];
    dst[4] = src[4];
    dst[5] = src[5];
    dst[6] = src[6];
    dst[7] = src[7];
    dst[8] = src[8];
    dst[9] = src[9];
    dst[10] = src[10];
    dst[11] = src[11];
    dst[12] = src[12];
    dst[13] = src[13];
    dst[14] = src[14];
    dst[15] = src[15];
    dst[16] = src[16];
    dst[17] = src[17];
    dst[18] = src[18];
    dst[19] = src[19];
    dst[20] = src[20];
    dst[21] = src[21];
    dst[22] = src[22];
    dst[23] = src[23];
    dst[24] = src[24];
    dst[25] = src[25];
    dst[26] = src[26];
    dst[27] = src[27];
    dst[28] = src[28];
    dst[29] = src[29];
    dst[30] = src[30];
    dst[31] = src[31];
    dst[32] = src[32];
    dst[33] = src[33];
    dst[34] = src[34];
    dst[35] = src[35];
    dst[36] = src[36];
    dst[37] = src[37];
    dst[38] = src[38];
    dst[39] = src[39];
    dst[40] = src[40];
    dst[41] = src[41];
    dst[42] = src[42];
    dst[43] = src[43];
    dst[44] = src[44];
    dst[45] = src[45];
    dst[46] = src[46];
    dst[47] = src[47];
}
} // namespace jj_fn_80291DE8

// TODO: clean up, give variables real names
extern "C" void fn_80290AC0(void) {}

extern "C" void fn_802915BC(void* self, unsigned char value) {
    *(unsigned char*)((unsigned char*)self + 0x0) = value;
}

// TODO: clean up, give variables real names
namespace jj_80291270 {
extern "C" int fn_80291270(int arg0, int arg1, int arg2) {
    return (arg2 << 4) + (arg1 << 2) + arg0;
}
}

// TODO: clean up, give variables real names
namespace jj_80290AC4 {
extern const float lbl_805A3E00;
extern "C" void fn_80290AC4(float *p)
{
    float arr[16] __attribute__((aligned(16)));
    float zero = lbl_805A3E00;
    p[0] = zero;
    p[1] = zero;
    p[2] = zero;
    p[3] = zero;
    p[4] = zero;
    p[5] = zero;
    p[6] = zero;
    p[7] = zero;
    p[8] = zero;
    p[9] = zero;
    p[10] = zero;
    p[11] = zero;
    p[12] = zero;
    p[13] = zero;
    p[14] = zero;
    p[15] = zero;
    p[16] = zero;
    p[17] = zero;
    p[18] = zero;
    p[19] = zero;
    p[20] = zero;
    p[21] = zero;
    p[22] = zero;
    p[23] = zero;
    p[24] = zero;
    p[25] = zero;
    p[26] = zero;
    p[27] = zero;
    p[28] = zero;
    p[29] = zero;
    p[30] = zero;
    p[31] = zero;
    p[32] = zero;
    p[33] = zero;
    p[34] = zero;
    p[35] = zero;
    p[36] = zero;
    p[37] = zero;
    p[38] = zero;
    p[39] = zero;
    p[40] = zero;
    p[41] = zero;
    p[42] = zero;
    p[43] = zero;
    p[44] = zero;
    p[45] = zero;
    p[46] = zero;
    p[47] = zero;
    arr[15] = zero;
    arr[14] = zero;
    arr[13] = zero;
    arr[12] = zero;
    arr[11] = zero;
    arr[10] = zero;
    arr[9] = zero;
    arr[8] = zero;
    arr[7] = zero;
    arr[6] = zero;
    arr[5] = zero;
    arr[4] = zero;
    arr[3] = zero;
    arr[2] = zero;
    arr[1] = zero;
    arr[0] = zero;
}
}

namespace jj_80291C70 {
typedef struct hkMath {
    f32 unk0;
} hkMath;

typedef struct Block {
    f32 v[48];
} __attribute__((aligned(16))) Block;

extern f32 lbl_805A3E04;

extern void fn_80291DE8(void *dst, const void *src);
extern void fn_8029211C(void *a, void *b, void *c, void *d);
extern void fn_802921B0(void *a, const void *b);
extern void fn_80284614(void *dst, const void *src);
extern void fn_80290AC0(void *p);
extern void fn_80284240(void *out, const void *a, const void *b);
extern void fn_8029193C(void *dst, const void *src);
extern void fn_80283EB4(void *dst, const void *src);
extern void fn_80284670(void *dst, const void *src);
extern hkMath *fn_80291270(void *base, s32 i, s32 j);
extern f32 max2f__6hkMathFff(f32 a, f32 b);

extern "C" void fn_80291C70(u8 *arg0, u8 *base, void *arg2, void *arg3, void **arg4)
{
    Block t260, t1A0, tE0, t20;
    s32 i;
    hkMath *a, *b;
    u8 flag;

    fn_80291DE8(&t260, base + 0x240);
    fn_80291DE8(&t1A0, base + 0x300);
    flag = *arg0;
    fn_8029211C(arg2, &flag, &t260, &t1A0);
    fn_802921B0(arg2, arg3);
    fn_80284614(base, arg3);
    fn_80290AC0(&tE0);
    fn_80284240(&tE0, base, *arg4);
    fn_8029193C(base + 0xC0, &t260);
    fn_80283EB4(base + 0xC0, &tE0);

    i = 0;
    do {
        a = fn_80291270(base + 0xC0, i, i);
        b = fn_80291270(base + 0x150, i, i);
        a->unk0 = max2f__6hkMathFff(a->unk0, lbl_805A3E04);
        {
            f32 threshold = lbl_805A3E04;
            b->unk0 = max2f__6hkMathFff(b->unk0, threshold);
        }
        i++;
    } while (i < 3);

    fn_80290AC0(&t20);
    fn_8029193C(&t20, base + 0xC0);
    fn_80284670(base + 0xC0, &t20);
    fn_80284240(base + 0x180, base + 0xC0, &t1A0);
    fn_8029193C(arg3, base + 0x300);
    *arg4 = base + 0x180;
}
}

