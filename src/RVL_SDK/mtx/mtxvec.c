// mtxvec: matrix times vector (paired singles, hand written asm).
#include "revolution/mtx.h"

asm void PSMTXMultVec(register const Mtx m, register const Vec *src, register Vec *dst) {
    nofralloc
    psq_l f0, 0(r4), 0, 0
    psq_l f2, 0(r3), 0, 0
    psq_l f1, 0x8(r4), 1, 0
    ps_mul f4, f2, f0
    psq_l f3, 0x8(r3), 0, 0
    ps_madd f5, f3, f1, f4
    psq_l f8, 0x10(r3), 0, 0
    ps_sum0 f6, f5, f6, f5
    psq_l f9, 0x18(r3), 0, 0
    ps_mul f10, f8, f0
    psq_st f6, 0(r5), 1, 0
    ps_madd f11, f9, f1, f10
    psq_l f2, 0x20(r3), 0, 0
    ps_sum0 f12, f11, f12, f11
    psq_l f3, 0x28(r3), 0, 0
    ps_mul f4, f2, f0
    psq_st f12, 0x4(r5), 1, 0
    ps_madd f5, f3, f1, f4
    ps_sum0 f6, f5, f6, f5
    psq_st f6, 0x8(r5), 1, 0
    blr
}
