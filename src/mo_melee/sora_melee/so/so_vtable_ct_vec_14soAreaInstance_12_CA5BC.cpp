#pragma force_active off
#include <so/so_array.h>
#include <so/templates/so_area_instance.h>
#include <new>

typedef soAreaInstance VelaElm;
typedef soArrayVector<VelaElm, 12> VelaVec;

// MATCH-ONLY: constructing the vector forces its vtable and special members into this unit.
#pragma dont_inline on
void vela_b(void* p) { new (p) VelaVec(1, 0); }
#pragma dont_inline reset
