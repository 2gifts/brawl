#pragma force_active off
#include <so/so_array.h>
class itGenArchive;
#include <new>

typedef itGenArchive* VelaElm;
typedef soArrayList<VelaElm, 3> VelaList;

// MATCH-ONLY: constructing/deleting the list forces its base-class members into this unit.
#pragma dont_inline on
void vela_a(void* p) { new (p) VelaList(); }
void vela_del(VelaList* p) { delete p; }
#pragma dont_inline reset
