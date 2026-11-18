#pragma force_active off
#include <so/so_array.h>
class itArchive;
#include <new>

typedef itArchive* VelaElm;
typedef soArrayList<VelaElm, 128> VelaList;

// MATCH-ONLY: constructing/deleting the list forces its base-class members into this unit.
#pragma dont_inline on
void vela_a(void* p) { new (p) VelaList(); }
void vela_del(VelaList* p) { delete p; }
#pragma dont_inline reset
