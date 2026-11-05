#pragma once

#include <havok/hkCdPoint.h>

// Collector that keeps the closest contact reported so far, without the chain bookkeeping of
// hkClosestCdPointCollector. 0x08 flags that a point has been stored.
// Largest distance a collector starts with (0x7F7FFFEE, a little below FLT_MAX).
#define HK_REAL_MAX 3.40282e38f

struct hkSimpleClosestContactCollector {
    hkSimpleClosestContactCollector() {
        m_hit = false;
        m_point.m_normal.w = HK_REAL_MAX;
        m_dist = HK_REAL_MAX;
    }
    virtual ~hkSimpleClosestContactCollector() {}
    virtual void addCdPoint(hkCdPoint* point); // HYPOTHESIS: name from the map

    float m_dist;     // 0x04
    hkBool m_hit;     // 0x08
    int unk0C;        // 0x0C
    hkContactPoint m_point; // 0x10..0x2F (distance in m_normal.w)
};
