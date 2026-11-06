#pragma once

#include <havok/hkBase.h>

struct hkWorld;
struct hkStepInfo;

// Per-step world housekeeping, held by the world at 0x4C and called by hkSimulation::integrate before the
// integration. Slots from hkDefaultWorldMaintenanceMgr's vtable in the image (lbl_80488600: 0x08 dtor fn_802F0AE4,
// 0x0C hkReferencedObject::calcStatistics, 0x10 fn_802F073C, 0x14 fn_802F0758), names from the map.
// HYPOTHESIS: the parameter types (integrate passes the world and its stack step info).
struct hkWorldMaintenanceMgr : hkReferencedObject {
    virtual ~hkWorldMaintenanceMgr();                                       // 0x08
    virtual void init(hkWorld* world);                                      // 0x10
    virtual void performMaintenance(hkWorld* world, hkStepInfo& stepInfo); // 0x14
};
