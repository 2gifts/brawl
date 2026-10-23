#pragma once

// Local BrawlHeaders shadow: name the leading float of soCameraRange read by Lucario's Final Smash camera.

#include <StaticAssert.h>
#include <types.h>
#include <mt/mt_vector.h>
#include <so/so_enable.h>
#include <cm/cm_subject.h>

struct soCameraRange {
    union {
        char _[0x4];
        // HYPOTHESIS: vertical offset of the range centre (Lucario's Aura Storm camera adds it to the kirifuda height).
        float unk0;
    };
    Rect2D m_range;
};
static_assert(sizeof(soCameraRange) == 0x14, "Class is wrong size!");

struct soCameraClipSphere {
    char _[0x14];
};
static_assert(sizeof(soCameraClipSphere) == 0x14, "Class is wrong size!");

struct soCameraZoom {
    u32 m_nodeIndex;
    char _[0x10];
};
static_assert(sizeof(soCameraZoom) == 0x14, "Class is wrong size!");

class soCameraSubject : public soEnable {
    char _4[0x3C];

    virtual ~soCameraSubject();
};
static_assert(sizeof(soCameraSubject) == 0x44, "Class is wrong size!");
