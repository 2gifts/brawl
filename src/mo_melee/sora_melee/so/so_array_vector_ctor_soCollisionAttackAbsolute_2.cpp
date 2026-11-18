#pragma force_active off
#include <so/so_array.h>
#include <so/collision/so_collision_attack_part.h>
#include <new>

typedef soArrayVector<soCollisionAttackAbsolute, 2> VelaVec;

// MATCH-ONLY: constructing the vector forces its constructor and destructor into this unit.
#pragma dont_inline on
void vela_a(void* p, s32 size) { new (p) VelaVec(size, 0); }
void vela_del(VelaVec* p) { delete p; }
#pragma dont_inline reset
