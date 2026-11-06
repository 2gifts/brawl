#pragma once

#include <havok/hkBase.h>

// Time span of one simulation step (0x10 bytes): start and end time, their difference and its inverse (0 for an
// empty step). Built on the stack by hkSimulation's integrate/collide and copied into the world and the collision
// input; the stack copies sit in a 16-byte-aligned frame.
// HYPOTHESIS: the member names (from what hkSimulation computes into them). The alignment is on the first member:
// the functions that hold one have copy propagation off (TIPS: an aligned member, not an aligned struct, does that).
struct hkStepInfo {
    hkReal m_startTime __attribute__((aligned(16))); // 0x00
    hkReal m_endTime;                                // 0x04
    hkReal m_deltaTime;                              // 0x08
    hkReal m_invDeltaTime;                           // 0x0C

    hkStepInfo() {}
    hkStepInfo(hkReal startTime, hkReal endTime) { set(startTime, endTime); }

    void set(hkReal startTime, hkReal endTime) {
        m_startTime = startTime;
        m_endTime = endTime;
        m_deltaTime = endTime - startTime;
        m_invDeltaTime = (m_deltaTime == 0.0f) ? 0.0f : 1.0f / m_deltaTime;
    }
};
