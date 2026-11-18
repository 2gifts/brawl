#pragma force_active off
#define SO_ARRAY_SPLIT_VECTOR_CTOR
#include <so/so_array.h>
#include <so/templates/so_array_value_soAreaContactLog.h>
#include <new>

typedef soAreaContactLog VelaElm;
typedef soArrayVector<VelaElm, 16> VelaVec;

// MATCH-ONLY: constructing the vector forces its vtable and special members into this unit.
template <>
inline VelaVec::soArrayVector(s32) : m_topIndex(0), m_lastIndex(0), m_size(0), m_isFull(false) { }

#pragma dont_inline on
void vela_a(void* p) { new (p) VelaVec(0); }
#pragma dont_inline reset
