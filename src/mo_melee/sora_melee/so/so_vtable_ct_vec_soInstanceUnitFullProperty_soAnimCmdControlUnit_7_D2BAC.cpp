#pragma force_active off
#define SO_ARRAY_SPLIT_VECTOR_CTOR
#include <so/so_array.h>
#include <so/templates/so_instance_unit.h>
#include <so/templates/so_array_value_soAnimCmdControlUnit.h>
#include <new>

typedef soInstanceUnitFullProperty<soAnimCmdControlUnit> VelaElm;
typedef soArrayVector<VelaElm, 7> VelaVec;

// The size argument is ignored: the vector always starts empty and its units are default constructed in place.
template <>
soArrayVector<VelaElm, 7>::soArrayVector(s32)
    : m_topIndex(0), m_lastIndex(0), m_size(0), m_isFull(false) { }

// TODO: clean up -- MATCH-ONLY helpers that force the vector members out of line into this unit.
#pragma dont_inline on
void vela_a(void* p) { new (p) VelaVec(0); }
void vela_del(VelaVec* p) { delete p; }
#pragma dont_inline reset
