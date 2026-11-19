#pragma force_active off
#define SO_ARRAY_SPLIT_VECTOR_CTOR
#include <so/so_array.h>
#include <ac/ac_cmd_interpreter.h>
#include <new>

typedef acCmdInterpreterStackData VelaElm;
typedef soArrayVector<VelaElm, 7> VelaVec;

// The size argument is ignored: the vector always starts empty and its elements are default constructed in place.
template <>
soArrayVector<VelaElm, 7>::soArrayVector(s32)
    : m_topIndex(0), m_lastIndex(0), m_size(0), m_isFull(false) { }

// TODO: clean up -- MATCH-ONLY helper that forces the constructor out of line into this unit; the real unit gets it from
// the code that constructs the vector (an interpreter owner), so this should disappear once that owner is decompiled.
#pragma dont_inline on
void vela_a(void* p) { new (p) VelaVec(0); }
#pragma dont_inline reset
