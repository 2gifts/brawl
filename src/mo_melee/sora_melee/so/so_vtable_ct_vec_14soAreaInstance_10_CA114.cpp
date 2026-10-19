#pragma force_active off
#define SO_ARRAY_SPLIT_VECTOR_CTOR
#include <so/so_array.h>
#include <so/templates/so_area_instance.h>
#include <new>

typedef soAreaInstance VelaElm;
typedef soArrayVector<VelaElm, 10> VelaVec;

// HYPOTHESIS: Wario supplies a size argument, but this particular native
// constructor ignores it and always starts empty. Preserve that behavior
// explicitly rather than importing the generic size constructor.
template <>
soArrayVector<VelaElm, 10>::soArrayVector(s32)
    : m_topIndex(0), m_lastIndex(0), m_size(0), m_isFull(false) { }

// MATCH-ONLY: constructing the vector forces its vtable and special members into this unit.
#pragma dont_inline on
void vela_a(void* p) { new (p) VelaVec(0); }
void vela_b(void* p) { new (p) VelaVec(1, 0); }
void vela_c(void* p, const VelaElm& e) { new (p) VelaVec(1, e, 0); }
void vela_del(VelaVec* p) { delete p; }
#pragma dont_inline reset
