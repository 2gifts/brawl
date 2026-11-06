#pragma once

#include <havok/hkBase.h>
#include <havok/hkMemory.h>
#include <havok/hkVector4.h>

struct hkWorld;
struct hkStepInfo;

// Per-step world housekeeping, held by the world at 0x4C and called by hkSimulation::integrate before the
// integration. Slots from hkDefaultWorldMaintenanceMgr's vtable in the image (0x80488600: 0x08 dtor,
// 0x0C hkReferencedObject::calcStatistics, 0x10 init, 0x14 performMaintenance), names from the map.
// HYPOTHESIS: the parameter types (integrate passes the world and its stack step info).
struct hkWorldMaintenanceMgr : hkReferencedObject {
    virtual void init(hkWorld* world) = 0;                                      // 0x10
    virtual void performMaintenance(hkWorld* world, hkStepInfo& stepInfo) = 0; // 0x14
};

// The world's default maintenance (map names; its unit is split as hkWorldMemoryWatchDog.cpp): splits the islands,
// moves the clock back once the step start passes m_resetTime, and marks islands inactive that want it.
// HYPOTHESIS: the member names (the constructor sets 33 and 63; the clock is moved back to m_restartTime once the
// step start reaches m_resetTime).
struct hkDefaultWorldMaintenanceMgr : hkWorldMaintenanceMgr {
    HK_DECLARE_REF_ALLOCATOR(0x13)

    hkReal m_restartTime; // 0x08
    hkReal m_resetTime;   // 0x0C

    hkDefaultWorldMaintenanceMgr();
    virtual ~hkDefaultWorldMaintenanceMgr() {}                              // 0x08
    virtual void init(hkWorld* world);                                      // 0x10
    virtual void performMaintenance(hkWorld* world, hkStepInfo& stepInfo); // 0x14
};
