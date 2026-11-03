#pragma once

#include <types.h>

// HYPOTHESIS: 4-float quaternion/vector class name from sora.map ([Quat]/(mt_quaternion.o)).
class Quat {
public:
    float m_x;
    float m_y;
    float m_z;
    float m_w;

    Quat() { }
    Quat(const Quat& orig) {
        m_x = orig.m_x;
        m_y = orig.m_y;
        m_z = orig.m_z;
        m_w = orig.m_w;
    }

    Quat& operator=(const Quat& orig) {
        m_x = orig.m_x;
        m_y = orig.m_y;
        m_z = orig.m_z;
        m_w = orig.m_w;
        return *this;
    }
};
