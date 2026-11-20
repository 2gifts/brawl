#pragma force_active off
#define YK_STAGE_INLINE // MATCH-ONLY: the fill constructor element loop
#include <em/em_ai_module_impl.h>
#include <new>

typedef emAIInfo VelaElm;
typedef soArrayVector<VelaElm, 2> VelaVec;

// MATCH-ONLY: constructing an element and the vector forces their constructors and destructors into this unit.
#pragma dont_inline on
void vela_e(void* p) { new (p) VelaElm(); }
void vela_edel(VelaElm* p) { delete p; }
void vela_c(void* p, const VelaElm& e) { new (p) VelaVec(1, e, 0); }
void vela_del(VelaVec* p) { delete p; }
#pragma dont_inline reset
