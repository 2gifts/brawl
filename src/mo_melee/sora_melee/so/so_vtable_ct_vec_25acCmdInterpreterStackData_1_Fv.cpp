#pragma force_active off
#include <so/so_array.h>
#include <ac/ac_cmd_interpreter.h>
#include <new>

typedef soArrayVector<acCmdInterpreterStackData, 1> VelaVec;

// TODO: clean up -- MATCH-ONLY helper that forces the default constructor out of line into this unit; the real unit gets it
// from the code that constructs the vector, so this should disappear once that owner is decompiled.
#pragma dont_inline on
void vela_a(void* p) { new (p) VelaVec(); }
#pragma dont_inline reset
