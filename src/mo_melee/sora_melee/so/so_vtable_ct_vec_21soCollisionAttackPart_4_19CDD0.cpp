#pragma force_active off
#define YK_STAGE_INLINE
#include <so/so_array.h>
#include <so/collision/templates/so_collision_attack_part.h>
#include <new>

typedef soCollisionAttackPart VelaElm;
typedef soArrayVector<VelaElm, 4> VelaVec;

// MATCH-ONLY: constructing the vector forces its vtable and special members into this unit.
#pragma dont_inline on
void vela_c(void* p, const VelaElm& e) { new (p) VelaVec(1, e, 0); }
#pragma dont_inline reset
