// mtx: matrix routines (the paired single versions are hand written asm in the original, same here) plus a
// handful of plain C ones for the light/camera matrices.
#include "revolution/mtx.h"
extern double sin(double);
extern double cos(double);
extern double tan(double);

// small float constants: declared up here, defined at the bottom so the c functions load them from memory
// like the asm ones do and the constant pool ends up in retail order
extern const f32 mtx_one;
extern const f32 mtx_zero;
extern const f32 mtx_half;
extern const f32 mtx_three;
extern const f32 mtx_two;
extern const f32 mtx_neg_one;
extern const f32 mtx_deg2rad;

static f32 Unit01[2] = {0.0f, 1.0f};

void PSMTXRotTrig(Mtx m, char axis, f32 sinA, f32 cosA);
static void __PSMTXRotAxisRadInternal(Mtx m, const Vec *axis, f32 sinA, f32 cosA);

asm void PSMTXIdentity(register Mtx m) {
    nofralloc
    lfs f0, mtx_zero
    lfs f1, mtx_one
    psq_st f0, 0x8(r3), 0, 0
    ps_merge10 f2, f1, f0
    ps_merge01 f1, f0, f1
    psq_st f0, 0x18(r3), 0, 0
    psq_st f0, 0x20(r3), 0, 0
    psq_st f1, 0x10(r3), 0, 0
    psq_st f2, 0(r3), 0, 0
    psq_st f2, 0x28(r3), 0, 0
    blr
}

asm void PSMTXCopy(register const Mtx src, register Mtx dst) {
    nofralloc
    psq_l f0, 0(r3), 0, 0
    psq_st f0, 0(r4), 0, 0
    psq_l f1, 0x8(r3), 0, 0
    psq_st f1, 0x8(r4), 0, 0
    psq_l f2, 0x10(r3), 0, 0
    psq_st f2, 0x10(r4), 0, 0
    psq_l f3, 0x18(r3), 0, 0
    psq_st f3, 0x18(r4), 0, 0
    psq_l f4, 0x20(r3), 0, 0
    psq_st f4, 0x20(r4), 0, 0
    psq_l f5, 0x28(r3), 0, 0
    psq_st f5, 0x28(r4), 0, 0
    blr
}

asm void PSMTXConcat(register const Mtx a, register const Mtx b, register Mtx ab) {
    nofralloc
    stwu r1, -0x40(r1)
    psq_l f0, 0(r3), 0, 0
    stfd f14, 0x8(r1)
    psq_l f6, 0(r4), 0, 0
    lis r6, Unit01@ha
    psq_l f7, 0x8(r4), 0, 0
    stfd f15, 0x10(r1)
    addi r6, r6, Unit01@l
    stfd f31, 0x28(r1)
    psq_l f8, 0x10(r4), 0, 0
    ps_muls0 f12, f6, f0
    psq_l f2, 0x10(r3), 0, 0
    ps_muls0 f13, f7, f0
    psq_l f31, 0(r6), 0, 0
    ps_muls0 f14, f6, f2
    psq_l f9, 0x18(r4), 0, 0
    ps_muls0 f15, f7, f2
    psq_l f1, 0x8(r3), 0, 0
    ps_madds1 f12, f8, f0, f12
    psq_l f3, 0x18(r3), 0, 0
    ps_madds1 f14, f8, f2, f14
    psq_l f10, 0x20(r4), 0, 0
    ps_madds1 f13, f9, f0, f13
    psq_l f11, 0x28(r4), 0, 0
    ps_madds1 f15, f9, f2, f15
    psq_l f4, 0x20(r3), 0, 0
    psq_l f5, 0x28(r3), 0, 0
    ps_madds0 f12, f10, f1, f12
    ps_madds0 f13, f11, f1, f13
    ps_madds0 f14, f10, f3, f14
    ps_madds0 f15, f11, f3, f15
    psq_st f12, 0(r5), 0, 0
    ps_muls0 f2, f6, f4
    ps_madds1 f13, f31, f1, f13
    ps_muls0 f0, f7, f4
    psq_st f14, 0x10(r5), 0, 0
    ps_madds1 f15, f31, f3, f15
    psq_st f13, 0x8(r5), 0, 0
    ps_madds1 f2, f8, f4, f2
    ps_madds1 f0, f9, f4, f0
    ps_madds0 f2, f10, f5, f2
    lfd f14, 0x8(r1)
    psq_st f15, 0x18(r5), 0, 0
    ps_madds0 f0, f11, f5, f0
    psq_st f2, 0x20(r5), 0, 0
    ps_madds1 f0, f31, f5, f0
    lfd f15, 0x10(r1)
    psq_st f0, 0x28(r5), 0, 0
    lfd f31, 0x28(r1)
    addi r1, r1, 0x40
    blr
}

asm void PSMTXConcatArray(register const Mtx a, register const Mtx b, register Mtx ab, register u32 count) {
    nofralloc
    stwu r1, -0x60(r1)
    stfd f31, 0x50(r1)
    psq_st f31, 0x58(r1), 0, 0
    stfd f30, 0x40(r1)
    psq_st f30, 0x48(r1), 0, 0
    stfd f29, 0x30(r1)
    psq_st f29, 0x38(r1), 0, 0
    stfd f28, 0x20(r1)
    psq_st f28, 0x28(r1), 0, 0
    stfd f27, 0x10(r1)
    psq_st f27, 0x18(r1), 0, 0
    subi r0, r6, 0x1
    psq_l f0, 0(r3), 0, 0
    psq_l f1, 0x8(r3), 0, 0
    la r6, Unit01(r0)
    psq_l f2, 0x10(r3), 0, 0
    psq_l f3, 0x18(r3), 0, 0
    psq_l f4, 0x20(r3), 0, 0
    psq_l f5, 0x28(r3), 0, 0
    mtctr r0
    psq_l f6, 0(r4), 0, 0
    psq_l f7, 0x8(r4), 0, 0
    ps_muls0 f12, f6, f0
    psq_l f8, 0x10(r4), 0, 0
    ps_muls0 f31, f6, f2
    psq_l f10, 0x20(r4), 0, 0
    ps_muls0 f29, f6, f4
    psq_l f9, 0x18(r4), 0, 0
    ps_madds1 f12, f8, f0, f12
    psq_l f27, 0(r6), 0, 0
    ps_madds1 f31, f8, f2, f31
    psq_l f11, 0x28(r4), 0, 0
    ps_madds1 f29, f8, f4, f29
    ps_muls0 f13, f7, f0
    ps_muls0 f30, f7, f2
    ps_muls0 f28, f7, f4
    ps_madds0 f12, f10, f1, f12
    ps_madds0 f31, f10, f3, f31
    ps_madds0 f29, f10, f5, f29
    ps_madds1 f13, f9, f0, f13
    psq_st f12, 0(r5), 0, 0
    ps_madds1 f30, f9, f2, f30
    ps_madds1 f28, f9, f4, f28
    psq_st f31, 0x10(r5), 0, 0
L_801EC334:
    ps_madds0 f13, f11, f1, f13
    psq_l f6, 0x30(r4), 0, 0
    ps_madds0 f30, f11, f3, f30
    psq_st f29, 0x20(r5), 0, 0
    ps_madds0 f28, f11, f5, f28
    psq_l f8, 0x40(r4), 0, 0
    ps_madd f13, f27, f1, f13
    psq_l f10, 0x50(r4), 0, 0
    ps_muls0 f12, f6, f0
    psq_l f7, 0x38(r4), 0, 0
    ps_muls0 f31, f6, f2
    psq_l f9, 0x48(r4), 0, 0
    psq_st f13, 0x8(r5), 0, 0
    ps_madd f30, f27, f3, f30
    ps_madds1 f12, f8, f0, f12
    psq_l f11, 0x58(r4), 0, 0
    ps_muls0 f29, f6, f4
    addi r4, r4, 0x30
    psq_st f30, 0x18(r5), 0, 0
    ps_madd f28, f27, f5, f28
    ps_madds1 f31, f8, f2, f31
    ps_madds1 f29, f8, f4, f29
    psq_st f28, 0x28(r5), 0, 0
    ps_madds0 f12, f10, f1, f12
    ps_muls0 f13, f7, f0
    ps_muls0 f30, f7, f2
    ps_muls0 f28, f7, f4
    psq_st f12, 0x30(r5), 0, 0
    ps_madds0 f31, f10, f3, f31
    ps_madds0 f29, f10, f5, f29
    ps_madds1 f13, f9, f0, f13
    psq_st f31, 0x40(r5), 0, 0
    ps_madds1 f30, f9, f2, f30
    ps_madds1 f28, f9, f4, f28
    addi r5, r5, 0x30
    bdnz L_801EC334
    ps_madds0 f13, f11, f1, f13
    psq_st f29, 0x20(r5), 0, 0
    ps_madds0 f30, f11, f3, f30
    ps_madds0 f28, f11, f5, f28
    ps_madd f13, f27, f1, f13
    ps_madd f30, f27, f3, f30
    ps_madd f28, f27, f5, f28
    psq_st f13, 0x8(r5), 0, 0
    psq_st f30, 0x18(r5), 0, 0
    psq_st f28, 0x28(r5), 0, 0
    psq_l f31, 0x58(r1), 0, 0
    lfd f31, 0x50(r1)
    psq_l f30, 0x48(r1), 0, 0
    lfd f30, 0x40(r1)
    psq_l f29, 0x38(r1), 0, 0
    lfd f29, 0x30(r1)
    psq_l f28, 0x28(r1), 0, 0
    lfd f28, 0x20(r1)
    psq_l f27, 0x18(r1), 0, 0
    lfd f27, 0x10(r1)
    addi r1, r1, 0x60
    blr
}

asm u32 PSMTXInverse(register const Mtx src, register Mtx inv) {
    nofralloc
    psq_l f0, 0(r3), 1, 0
    psq_l f1, 0x4(r3), 0, 0
    psq_l f2, 0x10(r3), 1, 0
    ps_merge10 f6, f1, f0
    psq_l f3, 0x14(r3), 0, 0
    psq_l f4, 0x20(r3), 1, 0
    ps_merge10 f7, f3, f2
    psq_l f5, 0x24(r3), 0, 0
    ps_mul f11, f3, f6
    ps_mul f13, f5, f7
    ps_merge10 f8, f5, f4
    ps_msub f11, f1, f7, f11
    ps_mul f12, f1, f8
    ps_msub f13, f3, f8, f13
    ps_mul f10, f3, f4
    ps_msub f12, f5, f6, f12
    ps_mul f9, f0, f5
    ps_mul f8, f1, f2
    ps_sub f6, f6, f6
    ps_msub f10, f2, f5, f10
    ps_mul f7, f0, f13
    ps_msub f9, f1, f4, f9
    ps_madd f7, f2, f12, f7
    ps_msub f8, f0, f3, f8
    ps_madd f7, f4, f11, f7
    ps_cmpo0 cr0, f7, f6
    bne L_801EC490
    li r3, 0x0
    blr
L_801EC490:
    fres f0, f7
    ps_add f6, f0, f0
    ps_mul f5, f0, f0
    ps_nmsub f0, f7, f5, f6
    lfs f1, 0xc(r3)
    ps_muls0 f13, f13, f0
    lfs f2, 0x1c(r3)
    ps_muls0 f12, f12, f0
    lfs f3, 0x2c(r3)
    ps_muls0 f11, f11, f0
    ps_merge00 f5, f13, f12
    ps_muls0 f10, f10, f0
    ps_merge11 f4, f13, f12
    ps_muls0 f9, f9, f0
    psq_st f5, 0(r4), 0, 0
    ps_mul f6, f13, f1
    psq_st f4, 0x10(r4), 0, 0
    ps_muls0 f8, f8, f0
    ps_madd f6, f12, f2, f6
    psq_st f10, 0x20(r4), 1, 0
    ps_nmadd f6, f11, f3, f6
    psq_st f9, 0x24(r4), 1, 0
    ps_mul f7, f10, f1
    ps_merge00 f5, f11, f6
    psq_st f8, 0x28(r4), 1, 0
    ps_merge11 f4, f11, f6
    psq_st f5, 0x8(r4), 0, 0
    ps_madd f7, f9, f2, f7
    psq_st f4, 0x18(r4), 0, 0
    ps_nmadd f7, f8, f3, f7
    li r3, 0x1
    psq_st f7, 0x2c(r4), 1, 0
    blr
}

asm u32 PSMTXInvXpose(register const Mtx src, register Mtx xPose) {
    nofralloc
    psq_l f0, 0(r3), 1, 0
    psq_l f1, 0x4(r3), 0, 0
    psq_l f2, 0x10(r3), 1, 0
    ps_merge10 f6, f1, f0
    psq_l f3, 0x14(r3), 0, 0
    psq_l f4, 0x20(r3), 1, 0
    ps_merge10 f7, f3, f2
    psq_l f5, 0x24(r3), 0, 0
    ps_mul f11, f3, f6
    ps_merge10 f8, f5, f4
    ps_mul f13, f5, f7
    ps_msub f11, f1, f7, f11
    ps_mul f12, f1, f8
    ps_msub f13, f3, f8, f13
    ps_msub f12, f5, f6, f12
    ps_mul f10, f3, f4
    ps_mul f9, f0, f5
    ps_mul f8, f1, f2
    ps_msub f10, f2, f5, f10
    ps_msub f9, f1, f4, f9
    ps_msub f8, f0, f3, f8
    ps_mul f7, f0, f13
    ps_sub f1, f1, f1
    ps_madd f7, f2, f12, f7
    ps_madd f7, f4, f11, f7
    ps_cmpo0 cr0, f7, f1
    bne L_801EC588
    li r3, 0x0
    blr
L_801EC588:
    fres f0, f7
    psq_st f1, 0xc(r4), 1, 0
    ps_add f6, f0, f0
    ps_mul f5, f0, f0
    psq_st f1, 0x1c(r4), 1, 0
    ps_nmsub f0, f7, f5, f6
    psq_st f1, 0x2c(r4), 1, 0
    ps_muls0 f13, f13, f0
    ps_muls0 f12, f12, f0
    ps_muls0 f11, f11, f0
    psq_st f13, 0(r4), 0, 0
    psq_st f12, 0x10(r4), 0, 0
    ps_muls0 f10, f10, f0
    ps_muls0 f9, f9, f0
    psq_st f11, 0x20(r4), 0, 0
    psq_st f10, 0x8(r4), 1, 0
    ps_muls0 f8, f8, f0
    li r3, 0x1
    psq_st f9, 0x18(r4), 1, 0
    psq_st f8, 0x28(r4), 1, 0
    blr
}

void PSMTXRotRad(Mtx m, char axis, f32 rad) {
    f32 sinA = (f32)sin(rad);
    f32 cosA = (f32)cos(rad);

    PSMTXRotTrig(m, axis, sinA, cosA);
}

asm void PSMTXRotTrig(register Mtx m, register char axis, register f32 sinA, register f32 cosA) {
    nofralloc
    frsp f5, f1
    ori r0, r4, 0x20
    frsp f4, f2
    cmplwi r0, 0x78
    lfs f0, mtx_zero
    ps_neg f2, f5
    lfs f1, mtx_one
    beq L_801EC68C
    cmplwi r0, 0x79
    beq L_801EC6B4
    cmplwi r0, 0x7a
    beq L_801EC6E0
    blr
L_801EC68C:
    ps_merge00 f3, f5, f4
    psq_st f1, 0(r3), 1, 0
    ps_merge00 f1, f4, f2
    psq_st f0, 0x4(r3), 0, 0
    psq_st f0, 0xc(r3), 0, 0
    psq_st f0, 0x1c(r3), 0, 0
    psq_st f0, 0x2c(r3), 1, 0
    psq_st f3, 0x24(r3), 0, 0
    psq_st f1, 0x14(r3), 0, 0
    blr
L_801EC6B4:
    ps_merge00 f3, f4, f0
    psq_st f0, 0x18(r3), 0, 0
    ps_merge00 f1, f0, f1
    ps_merge00 f2, f2, f0
    ps_merge00 f0, f5, f0
    psq_st f3, 0(r3), 0, 0
    psq_st f3, 0x28(r3), 0, 0
    psq_st f1, 0x10(r3), 0, 0
    psq_st f0, 0x8(r3), 0, 0
    psq_st f2, 0x20(r3), 0, 0
    blr
L_801EC6E0:
    ps_merge00 f3, f5, f4
    psq_st f0, 0x8(r3), 0, 0
    ps_merge00 f2, f4, f2
    ps_merge00 f1, f1, f0
    psq_st f0, 0x18(r3), 0, 0
    psq_st f0, 0x20(r3), 0, 0
    psq_st f3, 0x10(r3), 0, 0
    psq_st f2, 0(r3), 0, 0
    psq_st f1, 0x28(r3), 0, 0
    blr
}

asm static void __PSMTXRotAxisRadInternal(register Mtx m, register const Vec *axis, register f32 sinA, register f32 cosA) {
    nofralloc
    psq_l f3, 0(r4), 0, 0
    frsp f11, f2
    lfs f10, mtx_half
    frsp f12, f1
    ps_mul f4, f3, f3
    lfs f2, 0x8(r4)
    fadds f8, f10, f10
    lfs f9, mtx_three
    fsubs f1, f10, f10
    ps_madd f5, f2, f2, f4
    fsubs f0, f8, f11
    ps_merge00 f11, f11, f11
    ps_sum0 f6, f5, f2, f4
    frsqrte f7, f6
    fmuls f4, f7, f7
    fmuls f5, f7, f10
    fnmsubs f4, f4, f6, f9
    fmuls f7, f4, f5
    ps_muls0 f3, f3, f7
    ps_muls0 f2, f2, f7
    ps_muls0 f6, f3, f0
    ps_muls0 f7, f2, f0
    ps_muls0 f10, f3, f12
    ps_muls1 f5, f6, f3
    ps_muls0 f4, f6, f3
    ps_muls0 f6, f6, f2
    fnmsubs f0, f2, f12, f5
    ps_neg f3, f10
    fmadds f8, f2, f12, f5
    ps_sum0 f4, f4, f0, f11
    ps_sum0 f0, f3, f1, f6
    ps_muls0 f7, f7, f2
    ps_sum0 f9, f6, f1, f10
    psq_st f4, 0(r3), 0, 0
    ps_sum0 f3, f6, f6, f3
    ps_sum1 f5, f11, f8, f5
    psq_st f0, 0x18(r3), 0, 0
    ps_sum0 f7, f7, f1, f11
    ps_sum1 f6, f10, f3, f6
    psq_st f9, 0x8(r3), 0, 0
    psq_st f5, 0x10(r3), 0, 0
    psq_st f6, 0x20(r3), 0, 0
    psq_st f7, 0x28(r3), 0, 0
    blr
}

void PSMTXRotAxisRad(Mtx m, const Vec *axis, f32 rad) {
    f32 sinA = (f32)sin(rad);
    f32 cosA = (f32)cos(rad);

    __PSMTXRotAxisRadInternal(m, axis, sinA, cosA);
}

asm void PSMTXTrans(register Mtx m, register f32 x, register f32 y, register f32 z) {
    nofralloc
    lfs f0, mtx_zero
    lfs f4, mtx_one
    stfs f1, 0xc(r3)
    stfs f2, 0x1c(r3)
    psq_st f0, 0x4(r3), 0, 0
    psq_st f0, 0x20(r3), 0, 0
    stfs f0, 0x10(r3)
    stfs f4, 0x14(r3)
    stfs f0, 0x18(r3)
    stfs f4, 0x28(r3)
    stfs f3, 0x2c(r3)
    stfs f4, 0(r3)
    blr
}

asm void PSMTXTransApply(register const Mtx src, register Mtx dst, register f32 x, register f32 y, register f32 z) {
    nofralloc
    psq_l f4, 0(r3), 0, 0
    frsp f1, f1
    psq_l f5, 0x8(r3), 0, 0
    frsp f2, f2
    psq_l f7, 0x18(r3), 0, 0
    frsp f3, f3
    psq_l f8, 0x28(r3), 0, 0
    psq_st f4, 0(r4), 0, 0
    ps_sum1 f5, f1, f5, f5
    psq_l f6, 0x10(r3), 0, 0
    psq_st f5, 0x8(r4), 0, 0
    ps_sum1 f7, f2, f7, f7
    psq_l f9, 0x20(r3), 0, 0
    psq_st f6, 0x10(r4), 0, 0
    ps_sum1 f8, f3, f8, f8
    psq_st f7, 0x18(r4), 0, 0
    psq_st f9, 0x20(r4), 0, 0
    psq_st f8, 0x28(r4), 0, 0
    blr
}

asm void PSMTXScale(register Mtx m, register f32 x, register f32 y, register f32 z) {
    nofralloc
    lfs f0, mtx_zero
    stfs f1, 0(r3)
    psq_st f0, 0x4(r3), 0, 0
    psq_st f0, 0xc(r3), 0, 0
    stfs f2, 0x14(r3)
    psq_st f0, 0x18(r3), 0, 0
    psq_st f0, 0x20(r3), 0, 0
    stfs f3, 0x28(r3)
    stfs f0, 0x2c(r3)
    blr
}

asm void PSMTXScaleApply(register const Mtx src, register Mtx dst, register f32 x, register f32 y, register f32 z) {
    nofralloc
    frsp f1, f1
    psq_l f4, 0(r3), 0, 0
    frsp f2, f2
    psq_l f5, 0x8(r3), 0, 0
    frsp f3, f3
    ps_muls0 f4, f4, f1
    psq_l f6, 0x10(r3), 0, 0
    ps_muls0 f5, f5, f1
    psq_l f7, 0x18(r3), 0, 0
    ps_muls0 f6, f6, f2
    psq_l f8, 0x20(r3), 0, 0
    psq_st f4, 0(r4), 0, 0
    ps_muls0 f7, f7, f2
    psq_l f2, 0x28(r3), 0, 0
    psq_st f5, 0x8(r4), 0, 0
    ps_muls0 f8, f8, f3
    psq_st f6, 0x10(r4), 0, 0
    ps_muls0 f2, f2, f3
    psq_st f7, 0x18(r4), 0, 0
    psq_st f8, 0x20(r4), 0, 0
    psq_st f2, 0x28(r4), 0, 0
    blr
}

asm void PSMTXQuat(register Mtx m, register const Quaternion *q) {
    nofralloc
    psq_l f4, 0(r4), 0, 0
    psq_l f5, 0x8(r4), 0, 0
    ps_mul f6, f4, f4
    lfs f1, mtx_one
    ps_merge10 f9, f4, f4
    fsubs f0, f1, f1
    ps_madd f8, f5, f5, f6
    ps_muls1 f10, f5, f5
    fadds f2, f1, f1
    psq_st f0, 0xc(r3), 1, 0
    ps_sum0 f3, f8, f8, f8
    ps_mul f7, f5, f5
    psq_st f0, 0x2c(r3), 1, 0
    ps_madd f12, f4, f9, f10
    fres f13, f3
    ps_nmsub f3, f3, f13, f2
    ps_muls1 f11, f9, f5
    ps_msub f10, f4, f9, f10
    ps_mul f3, f13, f3
    ps_madds0 f9, f4, f5, f11
    ps_sum1 f8, f7, f8, f6
    fmuls f3, f3, f2
    ps_nmsub f11, f11, f2, f9
    ps_sum0 f6, f6, f6, f6
    ps_mul f9, f9, f3
    ps_mul f11, f11, f3
    ps_nmsub f8, f8, f3, f1
    ps_mul f12, f12, f3
    psq_st f9, 0x8(r3), 1, 0
    ps_mul f10, f10, f3
    ps_merge10 f7, f11, f0
    ps_merge00 f5, f12, f8
    ps_merge10 f4, f8, f10
    ps_merge01 f13, f11, f9
    psq_st f7, 0x18(r3), 0, 0
    ps_nmsub f6, f6, f3, f1
    psq_st f5, 0x10(r3), 0, 0
    psq_st f6, 0x28(r3), 1, 0
    psq_st f4, 0(r3), 0, 0
    psq_st f13, 0x20(r3), 0, 0
    blr
}

void C_MTXLookAt(Mtx m, const Point3d *camPos, const Vec *camUp, const Point3d *target) {
    Vec vLook;
    Vec vRight;
    Vec vUp;

    vLook.x = camPos->x - target->x;
    vLook.y = camPos->y - target->y;
    vLook.z = camPos->z - target->z;
    PSVECNormalize(&vLook, &vLook);
    PSVECCrossProduct(camUp, &vLook, &vRight);
    PSVECNormalize(&vRight, &vRight);
    PSVECCrossProduct(&vLook, &vRight, &vUp);

    m[0][0] = vRight.x;
    m[0][1] = vRight.y;
    m[0][2] = vRight.z;
    m[0][3] = -(camPos->x * vRight.x + camPos->y * vRight.y + camPos->z * vRight.z);

    m[1][0] = vUp.x;
    m[1][1] = vUp.y;
    m[1][2] = vUp.z;
    m[1][3] = -(camPos->x * vUp.x + camPos->y * vUp.y + camPos->z * vUp.z);

    m[2][0] = vLook.x;
    m[2][1] = vLook.y;
    m[2][2] = vLook.z;
    m[2][3] = -(camPos->x * vLook.x + camPos->y * vLook.y + camPos->z * vLook.z);
}

void C_MTXLightFrustum(Mtx m, f32 t, f32 b, f32 l, f32 r, f32 n, f32 scaleS, f32 scaleT, f32 transS, f32 transT) {
    f32 tmp;

    tmp = mtx_one / (r - l);
    m[0][0] = ((mtx_two * n) * tmp) * scaleS;
    m[0][1] = mtx_zero;
    m[0][2] = (((r + l) * tmp) * scaleS) - transS;
    m[0][3] = mtx_zero;

    tmp = mtx_one / (t - b);
    m[1][0] = mtx_zero;
    m[1][1] = ((mtx_two * n) * tmp) * scaleT;
    m[1][2] = (((t + b) * tmp) * scaleT) - transT;
    m[1][3] = mtx_zero;

    m[2][0] = mtx_zero;
    m[2][1] = mtx_zero;
    m[2][2] = mtx_neg_one;
    m[2][3] = mtx_zero;
}

void C_MTXLightPerspective(Mtx m, f32 fovY, f32 aspect, f32 scaleS, f32 scaleT, f32 transS, f32 transT) {
    f32 angle;
    f32 cot;

    angle = fovY * mtx_half;
    angle = angle * mtx_deg2rad;

    cot = mtx_one / (f32)tan(angle);

    m[0][0] = (cot / aspect) * scaleS;
    m[0][1] = mtx_zero;
    m[0][2] = -transS;
    m[0][3] = mtx_zero;

    m[1][0] = mtx_zero;
    m[1][1] = cot * scaleT;
    m[1][2] = -transT;
    m[1][3] = mtx_zero;

    m[2][0] = mtx_zero;
    m[2][1] = mtx_zero;
    m[2][2] = mtx_neg_one;
    m[2][3] = mtx_zero;
}

void C_MTXLightOrtho(Mtx m, f32 t, f32 b, f32 l, f32 r, f32 scaleS, f32 scaleT, f32 transS, f32 transT) {
    f32 tmp;

    tmp = mtx_one / (r - l);
    m[0][0] = (mtx_two * tmp * scaleS);
    m[0][1] = mtx_zero;
    m[0][2] = mtx_zero;
    m[0][3] = ((-(r + l) * tmp) * scaleS) + transS;

    tmp = mtx_one / (t - b);
    m[1][0] = mtx_zero;
    m[1][1] = (mtx_two * tmp) * scaleT;
    m[1][2] = mtx_zero;
    m[1][3] = ((-(t + b) * tmp) * scaleT) + transT;

    m[2][0] = mtx_zero;
    m[2][1] = mtx_zero;
    m[2][2] = mtx_zero;
    m[2][3] = mtx_one;
}

const f32 mtx_one = 1.0f;
const f32 mtx_zero = 0.0f;
const f32 mtx_half = 0.5f;
const f32 mtx_three = 3.0f;
const f32 mtx_two = 2.0f;
const f32 mtx_neg_one = -1.0f;
const f32 mtx_deg2rad = 0.017453292f;
