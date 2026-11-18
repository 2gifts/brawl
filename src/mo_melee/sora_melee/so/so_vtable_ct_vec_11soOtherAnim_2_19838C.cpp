#pragma force_active off
#define SO_ARRAY_ELEMENT_CTOR
#include <so/so_array.h>
#include <so/templates/so_array_value_soOtherAnim.h>
#include <new>

typedef soOtherAnim VelaElm;
typedef soArrayVector<VelaElm, 2> VelaVec;

// MATCH-ONLY: constructing the vector forces its vtable and special members into this unit.
#pragma dont_inline on
void vela_b(void* p) { new (p) VelaVec(1, 0); }
void vela_del(VelaVec* p) { delete p; }
#pragma dont_inline reset
