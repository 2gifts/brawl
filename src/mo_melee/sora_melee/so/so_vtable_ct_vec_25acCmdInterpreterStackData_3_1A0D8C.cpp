#pragma force_active off
#define SO_ARRAY_SPLIT_VECTOR_CTOR
#include <so/so_array.h>
#include <ac/ac_cmd_interpreter.h>
#include <new>

typedef acCmdInterpreterStackData VelaElm;
typedef soArrayVector<VelaElm, 3> VelaVec;

// The size argument is ignored: the vector always starts empty.
template <>
soArrayVector<VelaElm, 3>::soArrayVector(s32)
    : m_topIndex(0), m_lastIndex(0), m_size(0), m_isFull(false) { }

// MATCH-ONLY: constructing the vector forces its vtable and special members into this unit.
#pragma dont_inline on
void vela_a(void* p) { new (p) VelaVec(0); }
void vela_b(void* p) { new (p) VelaVec(1, 0); }
void vela_c(void* p, const VelaElm& e) { new (p) VelaVec(1, e, 0); }
void vela_del(VelaVec* p) { delete p; }
#pragma dont_inline reset
