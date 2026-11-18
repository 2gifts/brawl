#include <types.h>

struct cmUnkParam {
    int m_mode;
    float m_pitch;
    float m_zero;
    float m_dist;
};

extern "C" void fn_8009E3D0(cmUnkParam* param) {
    param->m_mode = 0;
    param->m_pitch = -0.08726647f;
    param->m_zero = 0.0f;
    param->m_dist = 80.0f;
}
