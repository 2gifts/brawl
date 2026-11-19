#pragma force_active off
#define YK_STAGE_INLINE
#include <revolution/gx.h>
#include <so/so_array.h>
#include <so/collision/so_collision_attack_part.h>
#include <new>

typedef soCollisionAttackPart VelaElm;
typedef soArrayVector<VelaElm, 3> VelaVec;

// TODO: clean up -- MATCH-ONLY helpers that force the vector constructor and destructor out of line into this unit; they
// should disappear once the owner of the vector (the attack module) is decompiled.
#pragma dont_inline on
void vela_c(void* p, const VelaElm& e) { new (p) VelaVec(1, e, 0); }
void vela_del(VelaVec* p) { delete p; }
#pragma dont_inline reset
