#pragma force_active on
#include <so/so_array.h>
#include <new>

typedef soArrayListUnit<u32, s8> VelaUnit;

// MATCH-ONLY: constructing/deleting the unit forces its special members into this unit.
#pragma dont_inline on
void vela_a(void* p) { new (p) VelaUnit(); }
void vela_del(VelaUnit* p) { delete p; }
#pragma dont_inline reset
