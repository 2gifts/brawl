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

