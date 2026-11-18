#pragma force_active off
#define WN_MARIO_FIREBALL_UNIT // BYTES: the fill constructor counts with a signed index and initializes the members in its initializer list
#include <so/so_array.h>
#include <revolution/gx.h>
#include <so/collision/templates/so_collision_hit_part.h>
#include <new>

typedef soCollisionHitPart VelaElm;
typedef soArrayVector<VelaElm, 3> VelaVec;

// MATCH-ONLY: constructing the vector forces its constructor into this unit.
#pragma dont_inline on
void vela_c(void* p, const VelaElm& e) { new (p) VelaVec(1, e, 0); }
#pragma dont_inline reset
