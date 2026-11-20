#pragma once

#include <StaticAssert.h>
#include <so/collision/so_collision.h>
#include <so/so_array.h>
#include <types.h>

struct soCollisionShieldPartUnit;

// Layout verified by the array-copy methods and concrete-vector element stride.
struct soCollisionCatchData {
    union {
        struct {
            float m_offsetX, m_offsetY, m_offsetZ; // the offset words as the anim cmd fills them
        };
        struct {
            // MATCH-ONLY: raw words preserve the original aggregate-copy instructions.
            u32 unk0, unk4, unk8, unkC, unk10, unk14;
        };
        struct {
            // HYPOTHESIS: offset words, size and bitfield flags
            u32 m_offset0, m_offset1, m_offset2;
            float m_size;
            u32 m_nodeIndex : 9;
            u32 m_unk10_9 : 9;  // checked by the anim cmd reader (a value-index or int argument)
            u32 m_targetMask : 10; // HYPOTHESIS: target layer mask (soCollisionCatchPart::set feeds it to clTarget 1)
            u32 m_unk10_28 : 3;
            u32 m_unk10_31 : 1;
            u32 unk14_0 : 4;
            u32 unk14_4 : 1;
            u32 unk14_5 : 1;
            u32 m_isNoTeam : 1; // HYPOTHESIS: name (the anim cmd sets it from its last argument)
            u32 unk14_7 : 25;
        };
    };
};
static_assert(sizeof(soCollisionCatchData) == 0x18, "Class is wrong size!");

class soCollisionCatchPart {
public:
    s32 unk0; // nonzero while the part is set
    soCollisionCatchData m_data;
    soArrayVector<clTarget, 6> m_targets;
    s32 unk58;

public:
    void reset();
    void set(const soCollisionCatchData* data, soCollisionShieldPartUnit* unit);
};
static_assert(sizeof(soCollisionCatchPart) == 0x5C, "Class is wrong size!");
