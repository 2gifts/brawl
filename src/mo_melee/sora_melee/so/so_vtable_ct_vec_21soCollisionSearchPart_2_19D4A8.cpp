#pragma force_active off
#define YK_STAGE_INLINE
#include <so/so_array.h>
#include <revolution/gx.h>
#include <so/collision/templates/so_collision_search_part.h>
#include <new>

typedef soCollisionSearchPart VelaElm;
typedef soArrayVector<VelaElm, 2> VelaVec;

// MATCH-ONLY: constructing the vector forces its vtable and special members into this unit.
#pragma dont_inline on
void vela_c(void* p, const VelaElm& e) { new (p) VelaVec(1, e, 0); }
void vela_del(VelaVec* p) { delete p; }
#pragma dont_inline reset
