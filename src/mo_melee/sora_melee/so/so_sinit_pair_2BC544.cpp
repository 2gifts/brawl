#include <types.h>

// TODO: clean up - placeholder pair type and names; replace with the real static object type (and real
// symbol names) once the owner of these two bss statics is identified.
struct SinitPair {
    s32 m_a;
    s32 m_b;
    SinitPair(s32 a, s32 b) : m_a(a), m_b(b) {}
};

SinitPair lbl_27_bss_7168(0xFF, 0);
SinitPair lbl_27_bss_7170(0xFF, 1);
