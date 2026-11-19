#pragma force_active off
#define SO_ARRAY_ELEMENT_CTOR
#include <so/so_array.h>
#include <so/templates/so_collision_hit_group.h>
#include <new>

typedef soCollisionHitGroup VelaElm;
typedef soArrayVector<VelaElm, 3> VelaVec;

// MATCH-ONLY: constructing the vector forces its constructor and destructor into this unit.
#pragma dont_inline on
void vela_a(void* p, s32 size) { new (p) VelaVec(size, 0); }
void vela_del(VelaVec* p) { delete p; }
#pragma dont_inline reset
