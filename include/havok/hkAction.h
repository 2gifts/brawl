#pragma once

#include <havok/hkBase.h>

struct hkWorld;
struct hkSimulationIsland;

// Base of the world actions (0x18 bytes). Layout and names from main.dol's hkActionClass; the world and
// island pointers are runtime-only (not serialized). The action vtable past hkReferencedObject is not
// recovered yet.
struct hkAction : hkReferencedObject {
    hkWorld* m_world;              // 0x08 HYPOTHESIS: type (the reflection only says pointer)
    hkSimulationIsland* m_island;  // 0x0C HYPOTHESIS: type; the island sets it in addAction
    u32 m_userData;                // 0x10
    const char* m_name;            // 0x14
};
