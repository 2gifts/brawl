#pragma force_active off
#include <so/so_array.h>
#include <so/templates/so_instance_unit.h>
class soTurnEventObserver;
#include <new>

typedef soArrayVector<soInstanceUnitFullProperty<soTurnEventObserver*>, 8> Vec;

// MATCH-ONLY: constructing the vector forces the Abstract substitution adjustor thunk into this unit.
void vela_use(void* p) { new (p) Vec(); }
