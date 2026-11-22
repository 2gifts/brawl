#include <types.h>
// Havok translation unit hkSolverExport.o (main.dol 0x8029F334-0x802A0B88).
// Not yet decompiled. Functions in address order:
//   0x8029F334  2564  hkExportImpulsesAndRhs
//   0x8029FD38   848  integrateRhsPair
//   0x802A0088   404  integrateRhs
//   0x802A021C   188  integrateAngularRhs
//   0x802A02D8   532  integrateRhs2Lin2Ang
//   0x802A04EC  1692  hkSolveGetToiViolatingConstraintVelocity

// TODO: clean up, give variables real names
// TODO: clean up, give variables real names (shape of integrateRhs2Lin2Ang below)
// TODO: clean up, give variables real names (shape of integrateRhs2Lin2Ang below)
namespace jj_802A0088 {
#pragma fp_contract on

struct V { float x, y, z, w; } __attribute__((aligned(16)));
struct B { char pad[0x40]; V linear, angular; };
struct J { V linear, angularA, angularB; };
struct L { V angularB, linear, angularA; };

extern "C" float fn_802A0088(const B* a, const B* b, const J* j, float fa, float fb) {
    L l;
    l.angularB.x = b->angular.x * j->angularB.x;
    l.angularB.y = b->angular.y * j->angularB.y;
    l.angularB.z = b->angular.z * j->angularB.z;
    l.angularB.w = b->angular.w * j->angularB.w;
    l.linear.x = a->linear.x - b->linear.x;
    l.linear.y = a->linear.y - b->linear.y;
    l.linear.z = a->linear.z - b->linear.z;
    l.linear.w = a->linear.w - b->linear.w;
    l.angularA.x = a->angular.x * j->angularA.x;
    l.angularA.y = a->angular.y * j->angularA.y;
    l.angularA.z = a->angular.z * j->angularA.z;
    l.angularA.w = a->angular.w * j->angularA.w;
    l.linear.x *= j->linear.x;
    l.linear.y *= j->linear.y;
    l.linear.z *= j->linear.z;
    l.linear.w *= j->linear.w;
    l.angularA.x += l.angularB.x;
    l.angularA.y += l.angularB.y;
    l.angularA.z += l.angularB.z;
    l.angularA.w += l.angularB.w;
    l.angularA.x += l.linear.x;
    l.angularA.y += l.linear.y;
    l.angularA.z += l.linear.z;
    l.angularA.w += l.linear.w;
    float d = l.angularA.z + (l.angularA.x + l.angularA.y);
    float f = d * fb;
    float s = fa * j->linear.w;
    return s - f;
}
}

namespace jj_802A021C {
#pragma fp_contract on

struct V { float x, y, z, w; } __attribute__((aligned(16)));
struct B { char pad[0x50]; V angular; };
struct J { V angularA, angularB; };

extern "C" float fn_802A021C(const B* a, const B* b, const J* j, float fa, float fb) {
    V sum;
    V tb;
    sum.x = a->angular.x * j->angularA.x;
    sum.y = a->angular.y * j->angularA.y;
    sum.z = a->angular.z * j->angularA.z;
    sum.w = a->angular.w * j->angularA.w;
    tb.x = b->angular.x * j->angularB.x;
    tb.y = b->angular.y * j->angularB.y;
    tb.z = b->angular.z * j->angularB.z;
    tb.w = b->angular.w * j->angularB.w;
    sum.x += tb.x;
    sum.y += tb.y;
    sum.z += tb.z;
    sum.w += tb.w;
    float d = sum.z + (sum.x + sum.y);
    float f = d * fb;
    return fa * j->angularB.w - f;
}
}

namespace jj_802A02D8 {
#pragma fp_contract on

struct V { float x, y, z, w; } __attribute__((aligned(16)));
struct B { char pad[0x40]; V linear, angular; };
struct C { V linearA, linearB, angularA, angularB; };
struct L { V linearB, linearA, angularB, angularA; };

extern "C" float fn_802A02D8(const B* a, const B* b, const C* c, float fa, float fb) {
    L l;
    l.angularA.x = a->angular.x * c->angularA.x;
    l.angularA.y = a->angular.y * c->angularA.y;
    l.angularA.z = a->angular.z * c->angularA.z;
    l.angularA.w = a->angular.w * c->angularA.w;
    l.angularB.x = b->angular.x * c->angularB.x;
    l.angularB.y = b->angular.y * c->angularB.y;
    l.angularB.z = b->angular.z * c->angularB.z;
    l.angularB.w = b->angular.w * c->angularB.w;
    l.linearA.x = a->linear.x * c->linearA.x;
    l.linearA.y = a->linear.y * c->linearA.y;
    l.linearA.z = a->linear.z * c->linearA.z;
    l.linearA.w = a->linear.w * c->linearA.w;
    l.linearB.x = b->linear.x * c->linearB.x;
    l.linearB.y = b->linear.y * c->linearB.y;
    l.linearB.z = b->linear.z * c->linearB.z;
    l.linearB.w = b->linear.w * c->linearB.w;
    l.angularA.x += l.angularB.x;
    l.angularA.y += l.angularB.y;
    l.angularA.z += l.angularB.z;
    l.angularA.w += l.angularB.w;
    l.linearA.x -= l.linearB.x;
    l.linearA.y -= l.linearB.y;
    l.linearA.z -= l.linearB.z;
    l.linearA.w -= l.linearB.w;
    l.angularA.x += l.linearA.x;
    l.angularA.y += l.linearA.y;
    l.angularA.z += l.linearA.z;
    l.angularA.w += l.linearA.w;
    float d = l.angularA.z + (l.angularA.x + l.angularA.y);
    float f = d * fb;
    float s = fa * c->linearA.w;
    return s - f;
}
}

