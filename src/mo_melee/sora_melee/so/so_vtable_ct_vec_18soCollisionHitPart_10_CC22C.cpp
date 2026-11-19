#pragma force_active off
#define YK_STAGE_INLINE
#include <revolution/gx.h>
#include <so/so_array.h>
#include <so/collision/templates/so_collision_hit_part.h>
#include <new>

typedef soCollisionHitPart VelaElm;
typedef soArrayVector<VelaElm, 10> VelaVec;

soCollisionHitPart::soCollisionHitPart() { }

// TODO: clean up -- MATCH-ONLY helper that forces the vector constructor out of line into this unit; it should disappear
// once the owner of the vector (the hit module) is decompiled.
#pragma dont_inline on
void vela_c(void* p, const VelaElm& e) { new (p) VelaVec(1, e, 0); }
#pragma dont_inline reset
