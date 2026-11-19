#pragma force_active off
#define SO_ARRAY_INLINE_ELEMENT_CTOR
#include <so/so_array.h>
#include <so/templates/so_array_value_wnemProcFncObj.h>
#include <new>

typedef soArrayVector<wnemProcFncObj, 70> VelaVec;

// TODO: clean up -- MATCH-ONLY helper that forces the vector constructor out of line into this unit; it should disappear once
// the owner of the vector (the enemy process module) is decompiled.
#pragma dont_inline on
void vela_a(void* p, s32 size) { new (p) VelaVec(size, 0); }
#pragma dont_inline reset
