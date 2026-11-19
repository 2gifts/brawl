#pragma force_active off
#include <so/so_array.h>
#include <so/collision/so_collision_attack_part.h>
#include <new>

typedef soArrayVector<soCollisionAttackAbsolute, 1> VelaVec;

// TODO: clean up -- MATCH-ONLY helpers that force the constructor and destructor out of line into this unit; they should
// disappear once the owner of the vector is decompiled.
#pragma dont_inline on
void vela_a(void* p, s32 size) { new (p) VelaVec(size, 0); }
void vela_del(VelaVec* p) { delete p; }
#pragma dont_inline reset
