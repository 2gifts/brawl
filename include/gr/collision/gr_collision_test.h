#pragma once

// grCollisionTest: the character-vs-stage collision test of one grCollStatus (main.dol 0x801173C0-0x80125308,
// gr_collision_body*.cpp). The base class owns the work state, the touch records and the corrections;
// grCollisionTestCircle and grCollisionTestRhombus (0x330 and up) supply the shape handling through the virtuals.
//
// Layout recovered from the constructor (0x801173C0), resetWork (0x80117D44) and every access in the range.
// Naming: m_unkNNN = hex byte offset when no role is evident; "// role:" comments give the evidence for named fields.
// The vptr is at 0x32C (end of the base class), as the compiler places it for this class.

#include <StaticAssert.h>
#include <gr/collision/gr_collision_linework.h>
#include <gr/collision/gr_collision_shape.h>
#include <gr/collision/gr_collision_test_types.h>
#include <mt/mt_coll2d.h>
#include <mt/mt_vector.h>
#include <types.h>

class grCollCollection;
class grCollStatus;
class grCollisionJoint;

class grCollisionTest {
public:
    // Bits of m_unkE: direction of the move current shape -> target shape, set from the sign of m_moveVec
    // (Circle::resetWork, Rhombus::resetWork, testGround: dx > 1e-5 -> 2, dx < -1e-5 -> 1, dy > 1e-5 -> 4, dy < -1e-5 -> 8).
    static const u8 DIR_LEFT = 1;
    static const u8 DIR_RIGHT = 2;
    static const u8 DIR_UP = 4;
    static const u8 DIR_DOWN = 8;

    // Bits of m_unk2C8 / m_unk2C9 / m_unk2CA / m_unk2CB (static-correction / ajust results; which side was pushed).
    // role: ajustShapeUp sets 0x20, ajustShapeLeft 0x40 and ajustShapeRight 0x80 in m_unk2CB; initialCorrection/lastCorrection
    // turn bit 0x20 into touch record 0 (id 4, line work m_unk30C) and bit 0x10 into the ground line (m_unk224 / grCollStatus + 0x104).
    static const u8 SIDE_DOWN = 0x10;
    static const u8 SIDE_UP = 0x20;
    static const u8 SIDE_LEFT = 0x40;
    static const u8 SIDE_RIGHT = 0x80;

    // Number of grCollTouchRecord entries (one per grCollStatus::TouchId).
    static const int TOUCH_NUM = 8;
    // Capacity of m_preIgnoreLines (pushbackPreIgnoreLine refuses the 9th) and of m_ignoreJoints (testAir refuses the 33rd).
    static const int PRE_IGNORE_MAX = 8;
    static const int IGNORE_JOINT_MAX = 32;

    // 0
    float m_unk0; // ctor: 0.001f. Not read in the range.
    // 4
    float m_unk4; // ctor: 0.001f. role: calcNewShapeOnRight/Left shorten the wall segment end by this amount.
    // 8
    float m_unk8; // movement rate still to do (1.0 = the whole move). role: ctor/reset 1.0, testCollisionMain* clamp it
                  // (below 0.01 -> 0) and pass rate * 0.99 to the virtual updateCurrent, updateTouchLine records hits.

    // 12 (flags; MSB first, as the ctor merges the clears/sets: bits 7,6 cleared, 5 set, 4-1 cleared, 0 untouched)
    bool m_isFinished : 1;  // bit 7 (0x80). role: test() loops (max 11 passes) until it is set; every sub-test sets it when done.
    bool m_unkC_6 : 1;      // bit 6 (0x40). HYPOTHESIS: "in lastCorrection": initialCorrection clears it, lastCorrection sets it.
    bool m_needCollect5 : 1; // bit 5 (0x20). HYPOTHESIS: forces updateCollection to re-collect; set by the ctor, cleared by updateCollection.
    bool m_needCollect4 : 1; // bit 4 (0x10). HYPOTHESIS: as bit 5; testAir sets it after a cloud pass-through / breakable hit.
    bool m_unkC_3 : 1;      // bit 3 (0x08). updateTouchLine sets it when touchId >= 4 (corner touch); testCollisionMainAir clears/tests it.
    bool m_unkC_2 : 1;      // bit 2 (0x04). initialCorrection sets it after re-fitting the shape vertically; testAir/testGround stop on it.
    bool m_unkC_1 : 1;      // bit 1 (0x02). initialCorrection sets it after re-fitting the shape horizontally; testAir/testGround stop on it.
    bool m_unkC_0 : 1;      // bit 0 (0x01). Not touched anywhere in the range (the ctor preserves it).
    // 13
    u8 m_touchMask; // bit n set = touch record n was written this pass. role: updateTouchLine ORs 1 << touchId; testGround
                    // ORs 8, testRightWallMain 4, testLeftWallMain 2, testTopMain 1; the "!= 0" tail copies records into grCollStatus.
    // 14
    u8 m_unkE; // direction bits of the move (DIR_*), see above. role: getNext*Line/checkMoveLenXDown/calcNewShapeOn* branch on it.
    // 15
    u8 m_unkF; // touch-id mask (bit n = grCollStatus::TouchId n) of the sides tested by testCollisionMain*.
               // 0xFF after resetWork; testGround 0xFE, testRightWallMain/testLeftWallMain 0xF9, testTopMain 0xF6, each |= 6 / 9 at the end.
               // role: testCollisionMain*/AirSub remap bit0->2, bit3->1, bit1->4, bit2->8 and AND the result with grCollisionLine::m_unkE.
    // 16
    u8 m_unk10; // touch-id mask for the static corrections, 0xFF after resetWork.
                // role: createCheckMaskStatic remaps it (same remap as above); initial/lastCorrection LR/UD, ajustShape* use it.
    // 17
    u8 m_unk11; // pass counter of test()'s loop (0..10).
    // 18
    char m_unk12[2];
    // 20
    u16 m_exclusiveLineAttr; // role: resetWork copies grCollStatus::m_exclusiveLineAttr (status + 0xE); testAir ORs 0x800;
                             // passed to grCollCollection::collect and ANDed with grCollisionLine::m_unkE by the wall tests.
    // 22
    char m_unk16[2];
    // 24
    grCollStatus* m_collStatus;
    // 28
    grCollCollection* m_collection; // the lines of the swept area. role: test()/initPos() point it at a stack grCollCollection;
                                    // updateCollection resets/collects; reads of +0x1A (count) and +0x1C (line works).
    // 32
    int m_unk20; // 1 when grCollStatus + 0xB == 1 (resetWork); the facing/side used by stopEdgeCorrect, testCliff, Rhombus::resetWork.
    // 36
    char m_unk24[0x10]; // not accessed anywhere in the range
    // 52 (0x34)
    grCollTouchRecord m_touch[TOUCH_NUM]; // indexed by grCollStatus::TouchId. role: updateTouchLine; updateCollStatusTouchInfo copies
                                          // m_touch[id].m_lineWork into grCollStatus::m_collLineWorkArray[id].
    // 244 (0xF4)
    grCollTouchRecord m_hit; // the nearest hit so far. role: updateTouchLine copies the touch record with the smallest rate here
                             // (when rate <= m_hit.m_rate); testAir/updateCollStatusTouchInfo switch on m_hit.m_id.
    // 268 (0x10C)
    float m_unk10C; // remaining move length on the ground (testGround sets it from |dx| or from the slope; checkMoveLenXDown reads it).
    // 272 (0x110)
    char m_unk110[4]; // not accessed anywhere in the range
    // 276 (0x114)
    Vec2f m_moveVec; // target centre - current centre (testGround, Rhombus::resetWork). role: m_unkE is derived from its signs.
    // 284 (0x11C)
    u16 m_preIgnoreNum; // role: pushbackPreIgnoreLine appends to m_preIgnoreLines while < 8; testCollisionMainAir flushes
                        // them with grCollStatus::addIgnoreLine and zeroes it.
    // 286 (0x11E)
    char m_unk11E[2];
    // 288 (0x120)
    grCollisionLineWork m_preIgnoreLines[PRE_IGNORE_MAX];
    // 416 (0x1A0)
    u16 m_ignoreJointNum; // role: testAir appends m_hit.m_lineWork.m_collisionJoint (offset 0x100) here while < 0x20; isIgnoreLine scans it.
    // 418 (0x1A2)
    char m_unk1A2[2];
    // 420 (0x1A4)
    grCollisionJoint* m_ignoreJoints[IGNORE_JOINT_MAX]; // role: isIgnoreLine compares the line work's joint (offset 4) with each entry.
    // 548 (0x224)
    grCollisionLineWork m_unk224; // the ground line the shape stands on (testGround stores the current line, testCollisionMainGround
                                  // and ajustShapeUp compare against it; initialCorrection copies m_touch[3].m_lineWork into it,
                                  // lastCorrection copies m_unk31C).
    // 564 (0x234)
    grCollisionLineWork m_unk234[8]; // constructed with the ctor but not accessed in the range
    // 692 (0x2B4)
    const char* m_name; // ctor: the "no name" literal (lbl_8059E6A8). HYPOTHESIS: a debug name.
    // 696 (0x2B8) The four work shapes. The subclass constructor points them at its own shape members.
    grCollShape* m_unk2b8; // role: previous shape (Circle::resetWork sets it from grCollStatus::m_prevCollShape)
    grCollShape* m_unk2bc; // role: the shape being moved (current -> result); copied to grCollStatus::m_currentCollShape at the end
    grCollShape* m_unk2c0; // role: scratch shape the calcNewShapeOn* functions fill before testing it
    grCollShape* m_unk2c4; // role: the target shape (getAABBox sweeps m_unk2bc..m_unk2c4)
    // 712 (0x2C8) result flags of the static corrections (SIDE_*)
    // role (cluster D): the tests of the bits compile as bit extractions, so m_unk2C8 / m_unk2C9 also have a bitfield view.
    union {
        u8 m_unk2C8; // initialCorrection: summary of the pass (bit 7 right, 6 left, 5 up, 4 down)
        grCollSideBits m_sides2C8;
    };
    union {
        u8 m_unk2C9; // lastCorrection: summary of the pass (copied from m_unk2CB first)
        grCollSideBits m_sides2C9;
    };
    u8 m_unk2CA; // scratch: filled by the virtual testStaticCorrection during one *Correction{LR,UD}, ORed into the summary
    u8 m_unk2CB; // ajustShape{Up,Left,Right} results
    // 716 (0x2CC)
    u32 m_unk2CC : 8; // ctor: 2
    u32 m_unk2CD : 8; // ctor: 2. role: test() sets 3 before testCliff, 2 before testAir, 1 before testGround.
    u32 : 16;
    // 720 (0x2D0)
    int m_unk2D0; // HYPOTHESIS: which test is running: 0 testAir, 1 testTopMain, 2 testLeftWallMain, 3 testRightWallMain, 4 testGround.
    // 724 (0x2D4)
    float m_unk2D4; // ctor: 0, resetWork: 0. role: checkMoveLenXDown stores the new X on the ground; calcNewShapeOnGround reads it.
    // 728 (0x2D8)
    float m_unk2D8; // ctor: 0, resetWork: 1.0. role: checkMoveLenXDown stores dx / segment length (the slope's cosine).
    // 732 (0x2DC)
    float m_unk2DC; // ctor: 0. role: initialCorrection/lastCorrection store right.x - left.x (the shape width before the pass).
    // 736 (0x2E0)
    float m_unk2E0; // ctor: 0. role: initialCorrection/lastCorrection store top.y - down.y (the shape height before the pass).
    // 740 (0x2E4)
    Vec2f m_unk2E4; // the shape centre (written from getCenterPos by Rhombus::resetWork, from (left+right)/2 and (top+down)/2 by
                    // initialCorrection); ajustShapeLeft raises x, ajustShapeRight lowers x, ajustShapeUp lowers y, ajustShapeDown raises y.
    // 748 (0x2EC) the corner positions of m_unk2bc before a correction pass (getTopPos 0x38, getDownPos 0x2C, getLeftPos 0x30, getRightPos 0x34)
    Vec2f m_unk2EC; // top
    // 756 (0x2F4)
    Vec2f m_unk2F4; // down
    // 764 (0x2FC)
    Vec2f m_unk2FC; // left
    // 772 (0x304)
    Vec2f m_unk304; // right
    // 780 (0x30C)
    grCollisionLineWork m_unk30C; // role: ceiling line found by the static correction; initialCorrection/lastCorrection copy it into m_touch[0] (id 4)
    // 796 (0x31C)
    grCollisionLineWork m_unk31C; // role: ground line found by the static correction; testGround adopts it after initialCorrection,
                                  // lastCorrection copies it into m_unk224 and grCollStatus + 0x104
    // 812 (0x32C): vptr

    grCollisionTest(grCollStatus* status);

    // ---- virtual table (main.dol 0x8045D9E0: slot 0 at +8). Pure virtuals are zero in the base table.
    virtual void resetWork();                                            // +0x08
    virtual void updateCurrent(float rate) = 0;                          // +0x0C
    virtual void updateTarget(float rate) = 0;                           // +0x10
    virtual void ajustShape();                                           // +0x14 (empty in the base class, 0x8012407C)
    virtual int testStaticCorrection(grCollisionLineWork* lineWork) = 0; // +0x18 HYPOTHESIS: returns nonzero when the shape was pushed
    virtual void testCollisionSetup() = 0;                               // +0x1C
    virtual int testCollisionNotMove(grCollisionLineWork* lineWork) = 0; // +0x20 HYPOTHESIS: returns nonzero on a hit (counted by testCollisionMain*)
    virtual int testCollisionCompound(grCollisionLineWork* lineWork) = 0;     // +0x24
    virtual int testStaticCorrectionLast(grCollisionLineWork* lineWork) = 0;  // +0x28
    virtual void testStaticCorrectionSetup() = 0;                        // +0x2C (called by initialCorrection)
    virtual void updateIgnoreLine() = 0;                                 // +0x30 (called by lastCorrection)
    virtual int testCollisionNotMoveSlatingLine(grCollisionLineWork* lineWork, void* unk) = 0; // +0x34 HYPOTHESIS: parameters

    // ---- cluster A: 0x801173C0 - 0x8011A944 (gr_collision_body.cpp)
    void getAABBox(clAABBox2D* out) __attribute__((never_inline));          // HYPOTHESIS: out = swept box (union of m_unk2bc and m_unk2c4 boxes, grown by 1)
    void updateCollection(const clAABBox2D* box) __attribute__((never_inline));  // HYPOTHESIS: re-collects the lines in box unless the cached box still covers it
    int initPos() __attribute__((never_inline));                            // (returns 1) snaps the shape to the nearest ground/ceiling lines and sets the status to ground or air (caller: grCollStatus::test 0x80134CA0)
    int test() __attribute__((never_inline));                               // the per-frame test (caller: grCollStatus::test 0x80134CA0, which returns its result). HYPOTHESIS: returns 1 when the ground/air state changed (started in the air: touched down; otherwise: no longer touching down)
    // mode: grCollStatus::m_setting at test() entry (1 = ground); prevGround: copy of the ground touch line at test() entry.
    void callbackExec(int mode, grCollisionLineWork* prevGround) __attribute__((never_inline)); // HYPOTHESIS: parameter types
    // The getNext*Line family: step to the connected line in the move direction; returns nonzero and fills *out when usable.
    int getNextGroundLine(grCollisionLineWork* cur, grCollisionLineWork* out) __attribute__((never_inline)); // HYPOTHESIS: return type, parameters
    int getNextRightLine(grCollisionLineWork* cur, grCollisionLineWork* out) __attribute__((never_inline));
    int getNextLeftLine(grCollisionLineWork* cur, grCollisionLineWork* out) __attribute__((never_inline));
    int getNextTopLine(grCollisionLineWork* cur, grCollisionLineWork* out) __attribute__((never_inline));
    // Returns 1 when the ground move runs past the end of the line (sets m_unk2D4 / m_unk2D8).
    int checkMoveLenXDown(const Vec2f* pos, grCollisionLineWork* lineWork) __attribute__((never_inline)); // HYPOTHESIS: parameter types
    // No `this` (callers pass the line work in r3): pos = point, outPos = pos + offset, outOffset = the line's displacement at pos.
    static void calcOffsetVector(grCollisionLineWork* lineWork, const Vec2f* pos, Vec2f* outPos, Vec2f* outOffset); // HYPOTHESIS: parameter types
    void calcNewShapeOnGround(grCollisionLineWork* lineWork, int flag) __attribute__((never_inline)); // HYPOTHESIS: flag = checkMoveLenXDown's result (end of line reached)
    void calcNewShapeOnRight(grCollisionLineWork* lineWork) __attribute__((never_inline));
    void calcNewShapeOnLeft(grCollisionLineWork* lineWork) __attribute__((never_inline));
    void calcNewShapeOnTop(grCollisionLineWork* lineWork) __attribute__((never_inline));

    // ---- cluster B: 0x8011A944 - 0x8011DE34 (gr_collision_body_b.cpp)
    int isIgnoreLine(grCollisionLineWork* lineWork) __attribute__((never_inline)); // HYPOTHESIS: return type
    int notifyEventBreakObject(const u8* eventFlags, u32 touchMask) __attribute__((never_inline)); // HYPOTHESIS: parameter types (flags: grCollStatus + 0x6F bits)
    int testAir() __attribute__((never_inline));               // HYPOTHESIS: return type (the grCollStatus touch-flag bit 7, "touching something")
    void testAttached() __attribute__((never_inline));
    int checkCloud() __attribute__((never_inline));
    int testGround() __attribute__((never_inline));            // HYPOTHESIS: return type
    int testGroundLRWallCorrect(grCollisionLineWork* lineWork, int flag) __attribute__((never_inline)); // HYPOTHESIS: parameter types
    int stopEdgeCorrect(grCollisionLineWork* lineWork) __attribute__((never_inline));

    // ---- cluster C: 0x8011DE34 - 0x80121214 (gr_collision_body_c.cpp)
    int testRightWallMain() __attribute__((never_inline));     // HYPOTHESIS: return type
    int testLeftWallMain() __attribute__((never_inline));
    int testTopMain() __attribute__((never_inline));
    int testCliff() __attribute__((never_inline));             // HYPOTHESIS: returns the result of testAir (0 without a usable cliff line); test() ignores it
    void updateCliffInfo(grCollCliffInfo* info, grCollisionLineWork* lineWork, int side, const Vec2f* pos) __attribute__((never_inline)); // HYPOTHESIS: parameter types
    int testCliffCheck() __attribute__((never_inline));        // HYPOTHESIS: return type (1 when a cliff edge was confirmed)
    int checkCliffRayReach(const Vec2f* origin, grCollCliffInfo* info) __attribute__((never_inline)); // HYPOTHESIS: parameter types

    // ---- cluster D: 0x80121214 - 0x801256E4 (gr_collision_body_d.cpp)
    u16 createCheckMaskStatic(); // m_unk10 remapped to line-attribute bits (bit0->2, bit3->1, bit1->4, bit2->8)
    int initialCorrection() __attribute__((never_inline));     // HYPOTHESIS: return type (true when the shape was moved)
    int initialCorrectionLR() __attribute__((never_inline));
    int initialCorrectionUD() __attribute__((never_inline));
    int lastCorrection() __attribute__((never_inline));
    int lastCorrectionLR() __attribute__((never_inline));
    int lastCorrectionUD() __attribute__((never_inline));
    void ajustShapeDown() __attribute__((never_inline));
    void ajustShapeUp() __attribute__((never_inline));
    void ajustShapeLeft() __attribute__((never_inline));
    void ajustShapeRight() __attribute__((never_inline));
    void updateCollStatusTouchInfo() __attribute__((never_inline));  // copies m_touch[m_hit.m_id] into grCollStatus, sets its flag bit
    int testCollisionMainAir() __attribute__((never_inline));
    int testCollisionMainAirSub() __attribute__((never_inline));
    int testCollisionMain() __attribute__((never_inline));
    int testCollisionMainGround() __attribute__((never_inline));
    void pushbackPreIgnoreLine(grCollisionLineWork* lineWork) __attribute__((never_inline));
    // Records a hit of the shape against lineWork at `rate` (keeps the smallest rate per touch id and in m_hit).
    void updateTouchLine(int touchId, grCollisionLineWork* lineWork, float rate);
    // No `this` (point in r3): sweeps pt along dir against the segment seg; returns 0 on a miss, else 1 (or -1: the segment is crossed from the other side),
    // *outRate = the part of the move (0..1) done at the crossing (for a collinear segment: at its nearer end).
    static int testMovePtToSeg(Vec2f* pt, Vec2f* dir, clSegment2D* seg, float* outRate); // HYPOTHESIS: parameter types (cluster D: segment is a clSegment2D)
};
static_assert(sizeof(grCollisionTest) == 0x330, "Class is wrong size!");
static_assert(offsetof(grCollisionTest, m_unk4) == 0x4, "Class is wrong layout!");
static_assert(offsetof(grCollisionTest, m_unk8) == 0x8, "Class is wrong layout!");
static_assert(offsetof(grCollisionTest, m_touchMask) == 0xD, "Class is wrong layout!");
static_assert(offsetof(grCollisionTest, m_unkE) == 0xE, "Class is wrong layout!");
static_assert(offsetof(grCollisionTest, m_exclusiveLineAttr) == 0x14, "Class is wrong layout!");
static_assert(offsetof(grCollisionTest, m_collStatus) == 0x18, "Class is wrong layout!");
static_assert(offsetof(grCollisionTest, m_collection) == 0x1C, "Class is wrong layout!");
static_assert(offsetof(grCollisionTest, m_unk20) == 0x20, "Class is wrong layout!");
static_assert(offsetof(grCollisionTest, m_touch) == 0x34, "Class is wrong layout!");
static_assert(offsetof(grCollisionTest, m_hit) == 0xF4, "Class is wrong layout!");
static_assert(offsetof(grCollisionTest, m_unk10C) == 0x10C, "Class is wrong layout!");
static_assert(offsetof(grCollisionTest, m_moveVec) == 0x114, "Class is wrong layout!");
static_assert(offsetof(grCollisionTest, m_preIgnoreNum) == 0x11C, "Class is wrong layout!");
static_assert(offsetof(grCollisionTest, m_preIgnoreLines) == 0x120, "Class is wrong layout!");
static_assert(offsetof(grCollisionTest, m_ignoreJointNum) == 0x1A0, "Class is wrong layout!");
static_assert(offsetof(grCollisionTest, m_ignoreJoints) == 0x1A4, "Class is wrong layout!");
static_assert(offsetof(grCollisionTest, m_unk224) == 0x224, "Class is wrong layout!");
static_assert(offsetof(grCollisionTest, m_unk234) == 0x234, "Class is wrong layout!");
static_assert(offsetof(grCollisionTest, m_name) == 0x2B4, "Class is wrong layout!");
static_assert(offsetof(grCollisionTest, m_unk2b8) == 0x2B8, "Class is wrong layout!");
static_assert(offsetof(grCollisionTest, m_unk2c4) == 0x2C4, "Class is wrong layout!");
static_assert(offsetof(grCollisionTest, m_unk2C8) == 0x2C8, "Class is wrong layout!");
static_assert(offsetof(grCollisionTest, m_unk2D0) == 0x2D0, "Class is wrong layout!");
static_assert(offsetof(grCollisionTest, m_unk2D4) == 0x2D4, "Class is wrong layout!");
static_assert(offsetof(grCollisionTest, m_unk2E4) == 0x2E4, "Class is wrong layout!");
static_assert(offsetof(grCollisionTest, m_unk2EC) == 0x2EC, "Class is wrong layout!");
static_assert(offsetof(grCollisionTest, m_unk304) == 0x304, "Class is wrong layout!");
static_assert(offsetof(grCollisionTest, m_unk30C) == 0x30C, "Class is wrong layout!");
static_assert(offsetof(grCollisionTest, m_unk31C) == 0x31C, "Class is wrong layout!");
