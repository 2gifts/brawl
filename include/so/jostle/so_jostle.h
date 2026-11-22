#pragma once

#include <StaticAssert.h>
#include <nw4r/ut/ut_list.h>
#include <types.h>

class gfArea;
struct Vec2f;

// HYPOTHESIS: per-frame push result of one jostle entry, filled by soJostle::move and read by soJostle::updatePosZ
// (index soJostle::m_logNo; allocated on the stack by soJostleManager::processEffect).
struct soJostleLog {
    s32 m_front;
    float m_frontDist;
    s32 m_back;
    float m_backDist;
};
static_assert(sizeof(soJostleLog) == 0x10, "Class is wrong size!");

// HYPOTHESIS: layout read off the constructor/reset/log accessors in sora_melee 0xC1A30..0xC20D0.
class soJostle {
public:
    gfArea* m_area;
    s32 m_0x4;
    float m_0x8;
    float m_0xC;
    float m_0x10;
    s32 m_0x14;
    u8 m_priority;
    s8 m_0x19;
    u8 m_0x1A;
    s8 m_0x1B;
    s16 m_logNo;
    char m_0x1E[2];
    nw4r::ut::Link m_link; // soJostleManager list link

    // HYPOTHESIS: m_0x4 points at the jostle data: push width per size kind (m_0x1B) [0..1], acceleration [2], max speed [3].
    // TODO: clean up - give m_0x4 a real data type (the constructor stores it from an s32 argument).
    const float* getData() const { return reinterpret_cast<const float*>(m_0x4); }

    soJostle(u8 priority, s32 arg);
    ~soJostle();
    void reset();
    void initLog();
    bool setLogNo(s32 logNo);
    void move(float* speed, Vec2f* pos, Vec2f* otherPos, soJostleLog* log, float otherSpeed, float otherWidth);
    void check(soJostle* other, soJostleLog* log, bool both);
    void updatePosZ(soJostleLog* log);
    void setPriority(u8 priority);
};
static_assert(sizeof(soJostle) == 0x28, "Class is wrong size!");
