// vec: vector math. add/normalize/magnitude/dot/cross are paired single asm, the half angle helper is C.
#include "revolution/mtx.h"

// sqrt iteration constants for the asm below, globals so they land in the pool before the C literals
const f32 vec_half = 0.5f;
const f32 vec_three = 3.0f;

asm void PSVECAdd(register const Vec *a, register const Vec *b, register Vec *ab) {
    nofralloc
    psq_l f2, 0(r3), 0, 0
    psq_l f4, 0(r4), 0, 0
    ps_add f6, f2, f4
    psq_st f6, 0(r5), 0, 0
    psq_l f3, 0x8(r3), 1, 0
    psq_l f5, 0x8(r4), 1, 0
    ps_add f7, f3, f5
    psq_st f7, 0x8(r5), 1, 0
    blr
}

asm void PSVECNormalize(register const Vec *src, register Vec *dst) {
    nofralloc
    psq_l f2, 0(r3), 0, 0
    psq_l f3, 0x8(r3), 1, 0
    ps_mul f5, f2, f2
    lfs f0, vec_half
    lfs f1, vec_three
    ps_madd f4, f3, f3, f5
    ps_sum0 f4, f4, f3, f5
    frsqrte f5, f4
    fmuls f6, f5, f5
    fmuls f0, f5, f0
    fnmsubs f6, f6, f4, f1
    fmuls f5, f6, f0
    ps_muls0 f2, f2, f5
    ps_muls0 f3, f3, f5
    psq_st f2, 0(r4), 0, 0
    psq_st f3, 0x8(r4), 1, 0
    blr
}

asm f32 PSVECMag(register const Vec *v) {
    nofralloc
    psq_l f0, 0(r3), 0, 0
    lfs f4, vec_half
    ps_mul f0, f0, f0
    lfs f1, 0x8(r3)
    fsubs f2, f4, f4
    ps_madd f1, f1, f1, f0
    ps_sum0 f1, f1, f0, f0
    fcmpu cr0, f1, f2
    beqlr
    frsqrte f0, f1
    lfs f3, vec_three
    fmuls f2, f0, f0
    fmuls f0, f0, f4
    fnmsubs f2, f2, f1, f3
    fmuls f0, f2, f0
    fmuls f1, f1, f0
    blr
}

asm f32 PSVECDotProduct(register const Vec *a, register const Vec *b) {
    nofralloc
    psq_l f2, 0x4(r3), 0, 0
    psq_l f3, 0x4(r4), 0, 0
    ps_mul f2, f2, f3
    psq_l f5, 0(r3), 0, 0
    psq_l f4, 0(r4), 0, 0
    ps_madd f3, f5, f4, f2
    ps_sum0 f1, f3, f2, f2
    blr
}

asm void PSVECCrossProduct(register const Vec *a, register const Vec *b, register Vec *axb) {
    nofralloc
    psq_l f1, 0(r4), 0, 0
    lfs f2, 0x8(r3)
    psq_l f0, 0(r3), 0, 0
    ps_merge10 f6, f1, f1
    lfs f3, 0x8(r4)
    ps_mul f4, f1, f2
    ps_muls0 f7, f1, f0
    ps_msub f5, f0, f3, f4
    ps_msub f8, f0, f6, f7
    ps_merge11 f9, f5, f5
    ps_merge01 f10, f5, f8
    psq_st f9, 0(r5), 1, 0
    ps_neg f10, f10
    psq_st f10, 0x4(r5), 0, 0
    blr
}

void C_VECHalfAngle(const Vec *a, const Vec *b, Vec *half) {
    Vec na, nb, ns;

    na.x = -a->x;
    na.y = -a->y;
    na.z = -a->z;

    nb.x = -b->x;
    nb.y = -b->y;
    nb.z = -b->z;

    PSVECNormalize(&na, &na);
    PSVECNormalize(&nb, &nb);
    PSVECAdd(&na, &nb, &ns);

    if (PSVECDotProduct(&ns, &ns) > 0.0f) {
        PSVECNormalize(&ns, half);
    } else {
        *half = ns;
    }
}

asm f32 PSVECSquareDistance(register const Vec *a, register const Vec *b) {
    nofralloc
    psq_l f0, 0x4(r3), 0, 0
    psq_l f1, 0x4(r4), 0, 0
    psq_l f2, 0(r3), 0, 0
    ps_sub f3, f0, f1
    psq_l f0, 0(r4), 0, 0
    ps_sub f0, f2, f0
    ps_mul f3, f3, f3
    ps_madd f1, f0, f0, f3
    ps_sum0 f1, f1, f3, f3
    blr
}
