#pragma once

#include <havok/hkVector4.h>
#include <havok/hkMemory.h>

// Axis-aligned bounding box (0x20 bytes). Layout from hkAabbClass.cpp.
struct hkAabb {
    hkVector4 m_min; // 0x00
    hkVector4 m_max; // 0x10

    hkAabb() {}
    hkAabb(const hkVector4& min, const hkVector4& max) : m_min(min), m_max(max) {}

    // Heap copies (hkWorldOperationQueue keeps one per queued phantom broadphase update) use memory class 0x25.
    static void* operator new(unsigned long nbytes) {
        return hkMemory::getInstance().allocateChunk(nbytes, 0x25);
    }
    static void operator delete(void* p) {
        hkMemory::getInstance().deallocateChunk(p, sizeof(hkAabb), 0x25);
    }

    // True when other lies inside this box in x, y and z (the w lanes are compared but masked out). HYPOTHESIS: the
    // name; inlined in hkBvTreeAgent::calcAabbAndQueryTree (min mask and max mask anded, then tested against 0xE).
    hkBool contains(const hkAabb& other) const {
        int mins = m_min.compareLessEqual4(other.m_min);
        int maxs = other.m_max.compareLessEqual4(m_max);
        return ((mins & maxs) & 0xE) == 0xE;
    }
};
