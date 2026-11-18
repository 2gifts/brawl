#pragma force_active off
#include <so/so_array.h>
#include <so/collision/so_collision_shield_group.h>
#include <new>

typedef soCollisionShieldGroup VelaElm;
typedef soArrayVector<VelaElm, 2> VelaVec;

// MATCH-ONLY: constructing the vector forces its vtable and special members into this unit.
#pragma dont_inline on
void vela_b(void* p) { new (p) VelaVec(1, 0); }
void vela_del(VelaVec* p) { delete p; }
#pragma dont_inline reset
