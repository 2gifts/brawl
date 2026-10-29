#pragma once

// Element types of grCollisionTest (gr_collision_test.h). Layouts come from the unrolled array
// initialisation of grCollisionTest::grCollisionTest (main.dol 0x801173C0) and from the accesses in
// grCollisionTest::resetWork / updateTouchLine / updateCollStatusTouchInfo.

#include <StaticAssert.h>
#include <gr/collision/gr_collision_linework.h>
#include <mt/mt_coll2d.h>
#include <mt/mt_vector.h>
#include <types.h>

// One recorded touch of the moving shape against a line (0x18 bytes). grCollisionTest keeps one per
// grCollStatus::TouchId (8 of them) and a ninth "nearest hit" copy.
// Default state (ctor/resetWork): id 0xFF, rate 1.0, empty line work.
struct grCollTouchRecord {
    // 0
    u8 m_id; // touch id of the side that was hit (0 up, 1 left, 2 right, 3 down, 4-7 corners); 0xFF: no hit.
             // role: updateTouchLine stores its touch id argument here; the best record is copied to
             // grCollisionTest::m_hit, whose id testAir/updateCollStatusTouchInfo switch on.
    // 4
    float m_rate; // fraction of the move done at the hit; 1.0 = no hit. role: updateTouchLine keeps the smallest rate.
    // 8
    grCollisionLineWork m_lineWork; // the touched line (0x10 bytes)

    // The grCollisionTest constructor (0x801173C0) builds m_touch[8] and m_hit with this: per element the original stores
    // {line work flag RMW, rate, id, line work words}; this body gives {line work, rate, id}. Tried: init list
    // (id, rate) 83%, body (id, rate) 91%, body (rate, id) 91%, ": m_rate(1.0f) { m_id = 0xFF; }" 93% of the constructor.
    // With a POD record (the line work spelled as raw members) and the body order {flag = 0, rate, id, line, joint, collision, unkC}
    // the constructor reaches 99.6%: the scheduler then keeps the order of the original. Only a duplicated store of m_hit's first
    // line pointer is missing. The line work member (a class with a constructor) cannot be kept that way, hence not applied.
    grCollTouchRecord() : m_rate(1.0f) {
        m_id = 0xFF;
    }
};
static_assert(sizeof(grCollTouchRecord) == 0x18, "Class is wrong size!");

// The side flags of grCollisionTest::m_unk2C8 / m_unk2C9 (which sides the static correction pushed against), MSB first.
struct grCollSideBits {
    bool m_right : 1; // 0x80 (touch id 2)
    bool m_left : 1;  // 0x40 (touch id 1)
    bool m_up : 1;    // 0x20 (touch id 0)
    bool m_down : 1;  // 0x10 (touch id 3)
    bool : 1;
    bool : 1;
    bool : 1;
    bool : 1;
};

// Candidate cliff found by grCollisionTest::testCliffCheck (a stack local there, 0x10 bytes).
// Layout from updateCliffInfo (main.dol 0x80120500) and checkCliffRayReach / testCliffCheck.
struct grCollCliffInfo {
    // 0 (the first word is read-modify-written as a whole by updateCliffInfo)
    u8 m_valid : 8; // 0 until updateCliffInfo fills it
    // 1
    int m_side : 8; // HYPOTHESIS: 0 = left edge, 1 = right edge of the line (the side argument of updateCliffInfo);
                    // testCliffCheck copies it to grCollStatus + 0x54 (m_cliffDir)
    // 2
    u32 m_unk2 : 16;
    // 4
    grCollisionLineWork* m_lineWork; // the line the cliff belongs to (points into the collected line works)
    // 8
    Vec2f m_pos; // the edge point
};
static_assert(sizeof(grCollCliffInfo) == 0x10, "Class is wrong size!");

// Out parameter of the four-argument grCollision::getSegment overload (main.dol 0x80111968): the segment of a line followed by
// the fourth argument. HYPOTHESIS: the word is a flag passed through (every caller in the range passes 0).
struct grCollSegmentEx {
    // 0
    clSegment2D m_segment;
    // 16
    int m_unk10;
};
static_assert(sizeof(grCollSegmentEx) == 0x14, "Class is wrong size!");
