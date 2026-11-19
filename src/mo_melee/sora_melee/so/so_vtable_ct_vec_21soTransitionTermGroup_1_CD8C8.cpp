#pragma force_active off
#define SO_ARRAY_SPLIT_VECTOR_CTOR
#define FT_MODULE_BUILDER
#include <so/so_array.h>
#include <so/transition/so_transition_module_impl.h>
#include <new>

typedef soTransitionTermGroup VelaElm;
typedef soArrayVector<VelaElm, 1> VelaVec;

// The size argument is ignored: the vector always starts empty and its groups are default constructed in place.
template <>
soArrayVector<VelaElm, 1>::soArrayVector(s32)
    : m_topIndex(0), m_lastIndex(0), m_size(0), m_isFull(false) { }

// TODO: clean up -- MATCH-ONLY helpers that force the vector members out of line into this unit; they should disappear once
// the owner of the vector (the transition module) is decompiled.
#pragma dont_inline on
void vela_a(void* p) { new (p) VelaVec(0); }
void vela_del(VelaVec* p) { delete p; }
#pragma dont_inline reset
