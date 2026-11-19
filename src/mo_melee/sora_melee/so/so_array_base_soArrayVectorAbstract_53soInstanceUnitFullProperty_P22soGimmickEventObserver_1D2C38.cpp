#pragma force_active off
#include <so/so_array.h>
#include <so/templates/so_instance_unit.h>
class soGimmickEventObserver;
#include <new>

typedef soArrayVector<soInstanceUnitFullProperty<soGimmickEventObserver*>, 8> Vec;

// MATCH-ONLY: constructing the vector forces the Abstract substitution adjustor thunk into this unit.
void vela_use(void* p) { new (p) Vec(); }
