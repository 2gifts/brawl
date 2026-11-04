#pragma once

#include <havok/hkBase.h>

// Memory limit watchdog referenced by hkWorldCinfo (0x0C bytes). Layout and names from main.dol's
// hkWorldMemoryWatchDogClass. Its vtable is not recovered yet.
struct hkWorldMemoryWatchDog : hkReferencedObject {
    s32 m_memoryLimit; // 0x08
};
