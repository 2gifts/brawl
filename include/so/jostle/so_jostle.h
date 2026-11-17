#pragma once

#include <StaticAssert.h>
#include <types.h>

// HYPOTHESIS: layout read off the constructor/reset/log accessors in sora_melee 0xC1A30..0xC20D0.
class soJostle {
public:
    s32 m_0x0;
    s32 m_0x4;
    float m_0x8;
    float m_0xC;
    float m_0x10;
    s32 m_0x14;
    u8 m_priority;
    s8 m_0x19;
    u8 m_0x1A;
    s16 m_logNo;
    char m_0x1E[10];

    soJostle(u8 priority, s32 arg);
    ~soJostle();
    void reset();
    void initLog();
    bool setLogNo(s32 logNo);
    void setPriority(u8 priority);
};
static_assert(sizeof(soJostle) == 0x28, "Class is wrong size!");
