// Havok translation unit hkSolverTimeDate.o (main.dol 0x802A0B88-0x802A0BCC).
// Functions in address order (method names from the Havok TU map):
//   0x802A0B88    68  hkSolverGetSystemTime   [map: hkSolverTimeDate__hkSolverGetSystemTime]
#include <havok/hkBase.h>

typedef long long OSTime;

extern "C" OSTime OSGetTime(void);
// HYPOTHESIS: 64-bit by 64-bit division helper (__div2i in the image).
extern "C" OSTime __div2i(OSTime num, OSTime den);
// Runtime conversion helpers (runtime.c): __cvt_sll_flt and __cvt_dbl_ull.
extern "C" float __cvt_sll_flt(OSTime value);
extern "C" float __cvt_dbl_ull(float value);

// Elapsed system time in seconds-like units: OS time divided by the bus clock quarter.
float hkSolverGetSystemTime() {
    OSTime now = OSGetTime();
    OSTime tick = *(u32*)0x800000F8 >> 2;
    float t = __cvt_sll_flt(__div2i(now, tick));
    return __cvt_dbl_ull(t + 946728000.0f);
}
