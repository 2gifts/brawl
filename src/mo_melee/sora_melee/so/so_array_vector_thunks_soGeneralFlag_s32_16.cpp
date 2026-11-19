#pragma force_active off
#define IT_GENERAL_FLAG_OOL // BYTES: the element constructors stay out of line
#define IT_GENERAL_FLAG_DEFAULT
#define IT_GENERAL_FLAG_DTOR
#define WN_MARIO_FIREBALL_UNIT // BYTES: the fill constructor counts with a signed index and initializes the members in its initializer list
#include <so/so_array.h>
#include <new>

// The element constructor and destructor are out of line (the array code constructs through __construct_array).
template <typename T>
soGeneralFlag<T>::soGeneralFlag() : flag(0) { }
template <typename T>
soGeneralFlag<T>::soGeneralFlag(T init) : flag(init) { }
template <typename T>
soGeneralFlag<T>::~soGeneralFlag() { }

typedef soGeneralFlag<s32> VelaElm;
typedef soArrayVector<VelaElm, 16> VelaVec;

// MATCH-ONLY: constructing the vector forces its vtable and thunks into this unit.
#pragma dont_inline on
void vela_c(void* p, const VelaElm& e) { new (p) VelaVec(1, e, 0); }
void vela_del(VelaVec* p) { delete p; }
#pragma dont_inline reset
