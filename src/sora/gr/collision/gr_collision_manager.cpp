#include <gr/collision/gr_collision_line.h>
#include <mt/mt_coll2d.h>
#include <mt/mt_vector.h>
#include <mt/mt_common.h>
#include <nw4r/ut/ut_algorithm.h>
#include <OS/OSTime.h>
#include <gm/gm_global.h>
#include <gr/collision/gr_collision.h>
#include <gr/collision/gr_collision_joint.h>
#include <gr/collision/gr_collision_manager.h>
#include <gr/collision/gr_collision_status.h>
#include <nw4r/ut/ut_LinkList.h>
#include <types.h>
#include <mt/mt_ps_minmax.h>

extern "C" void fn_80110A40(grCollision* collision);
extern "C" void fn_80110F7C(grCollision* collision);
extern "C" void fn_8011135C(grCollision* collision, grCollision* target);

// HYPOTHESIS: g_grCollisionList links grCollision through m_next (0x30); g_grCollStatusList at 0x8049E594.
typedef nw4r::ut::LinkList<grCollision, 0x30> grCollisionNodeList;
typedef nw4r::ut::LinkList<grCollStatus, 0> grCollStatusNodeList;
extern grCollStatusNodeList lbl_8049E594;
extern grCollisionManager* lbl_805A0360;
extern bool lbl_805A0358;

// The original passes the unextended 32-bit loop counters to these two (every caller in the dol does), so their index
// parameters are u32; these stand-ins name the same symbols as the members.
extern "C" grCollisionJoint* getJoint__11grCollisionFUl(grCollision* collision, u32 index);
extern "C" grCollisionLine* getLine__16grCollisionJointFUl(grCollisionJoint* joint, u32 index);

grCollisionManager::grCollisionManager() :
    gfTask("GroundCollision", Category_GrCollision, 4, 13, true), unk41(0), m_nextIndex(0), unk44(0) {
    m_indexBits.initilize(0x100);
    lbl_805A0360 = this;
    for (int i = 0; i < 256; i++) {
        m_collisions[i] = NULL;
    }
    for (int i = 0; i < 8; i++) {
        m_cliffHangInfos[i].m_status = NULL;
        m_cliffHangInfos[i].m_lineId = 0xFFFF;
        m_cliffHangInfos[i].m_team = 0;
    }
    unk4F0 = 0;
    unk4F4 = 0;
}

grCollisionManager::~grCollisionManager() {
    lbl_805A0360 = NULL;
}

void grCollisionManager::collisionUpdate(bool dynamic) {
    grCollisionNodeList& list = (grCollisionNodeList&)g_grCollisionList;
    clAABBox2D range;
    if (m_flags.m_useClipRange) {
        // MATCH-ONLY: the bottom and top edges are read first, as the original loads them.
        float down = m_clipRange.m_down;
        range.m_unk1 = m_clipRange.m_left;
        range.m_unk2 = down;
        float up = m_clipRange.m_up;
        range.m_unk3 = m_clipRange.m_right;
        range.m_unk4 = up;
    }
    for (grCollisionNodeList::Iterator it = list.GetBeginIter(); it != list.GetEndIter(); it++) {
        grCollision* collision = &*it;
        if (collision->m_isEnabled && dynamic == collision->m_isDynamic) {
            collision->update();
            if (m_flags.m_useClipRange) {
                collision->updateClippingRange(&range);
            }
            fn_80110F7C(collision);
        }
    }
    for (grCollisionNodeList::Iterator it = list.GetBeginIter(); it != list.GetEndIter(); it++) {
        grCollision* collision = &*it;
        u8 flags = collision->_spacer[0];
        if (((flags >> 6) & 1) && dynamic == collision->m_isDynamic && ((flags >> 5) & 1)) {
            grCollisionNodeList::Iterator jt = it;
            for (jt++; jt != list.GetEndIter(); jt++) {
                grCollision* target = &*jt;
                if (target->m_isEnabled && dynamic == collision->m_isDynamic && (((u8)collision->_spacer[0] >> 6) & 1) &&
                    (((u8)target->_spacer[0] >> 5) & 1))
                {
                    fn_8011135C(collision, target);
                }
            }
        }
    }
}

// Fits m_floorRange around the floor lines of the enabled static collisions (the lines that are longer than a minimum).
void grCollisionManager::calcFloorRange() {
    float minY = 0.0f;
    float maxY = minY;
    float minX = minY;
    float maxX = minY;
    bool first = true;
    grCollisionNodeList& list = (grCollisionNodeList&)g_grCollisionList;
    for (grCollisionNodeList::Iterator it = list.GetBeginIter(); it != list.GetEndIter(); it++) {
        grCollision* collision = &*it;
        if (collision->m_isEnabled && collision->m_isDynamic != 1 && collision->m_isEnabledLatch) {
            u32 jointLen = collision->m_jointLen;
            for (u32 i = 0; i < jointLen; i++) {
                grCollisionJoint* joint = getJoint__11grCollisionFUl(collision, i);
                if (joint->m_0x54_6) {
                    u32 lineLen = joint->m_lineLen;
                    for (u32 j = 0; j < lineLen; j++) {
                        grCollisionLine* line = getLine__16grCollisionJointFUl(joint, j);
                        if (line->m_isUnder) {
                            clSegment2D segment;
                            collision->getSegment(&segment, line);
                            float length = (segment.m_p1Pos - segment.m_p0Pos).length();
                            if (length > 0.01f) {
                                float lineMinY = psMin(segment.m_p0Pos.m_y, segment.m_p1Pos.m_y);
                                float lineMaxY = psMax(segment.m_p0Pos.m_y, segment.m_p1Pos.m_y);
                                float lineMinX = psMin(segment.m_p0Pos.m_x, segment.m_p1Pos.m_x);
                                float lineMaxX = psMax(segment.m_p0Pos.m_x, segment.m_p1Pos.m_x);
                                if (first) {
                                    minY = lineMinY;
                                    first = false;
                                    minX = lineMinX;
                                    maxY = lineMaxY;
                                    maxX = lineMaxX;
                                } else {
                                    float newMinY = psMin(minY, lineMinY);
                                    float newMinX = psMin(minX, lineMinX);
                                    float newMaxY = psMax(maxY, lineMaxY);
                                    float newMaxX = psMax(maxX, lineMaxX);
                                    minY = newMinY;
                                    minX = newMinX;
                                    maxY = newMaxY;
                                    maxX = newMaxX;
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    m_floorRange.m_left = minX;
    m_floorRange.m_right = maxX;
    m_floorRange.m_down = minY;
    m_floorRange.m_up = maxY;
}

void grCollisionManager::processUpdate() {
    m_flags.m_isActive = true;
    m_flags.m_isGameFrame = false;
    if (g_GameGlobal->isPrevJustGameFrame()) {
        m_flags.m_isGameFrame = true;
    }
    if (m_flags.m_isActive) {
        lbl_805A0358 = isUnkB5();
        OSGetTime();
        collisionUpdate(false);
        calcFloorRange();
        OSGetTime();
    }
}

void grCollisionManager::processPreMapCorrection() {
    if (m_flags.m_isActive && unk41 == 0) {
        for (grCollStatusNodeList::Iterator it = lbl_8049E594.GetBeginIter(); it != lbl_8049E594.GetEndIter(); it++) {
            grCollStatus* status = &*it;
            if (status->m_enabled && status->m_unk124 == 0) {
                status->test();
                grCollision* collision = status->m_unk104;
                if (collision != NULL) {
                    collision->update();
                    fn_80110F7C(collision);
                }
            }
        }
    }
}

void grCollisionManager::processMapCorrection() {
    if (m_flags.m_isActive && unk41 == 0) {
        collisionUpdate(true);
        for (grCollStatusNodeList::Iterator it = lbl_8049E594.GetBeginIter(); it != lbl_8049E594.GetEndIter(); it++) {
            grCollStatus* status = &*it;
            if (status->m_enabled && status->m_unk124 == 1) {
                status->test();
            }
        }
    }
}

void grCollisionManager::processFixPosition() {
    if (m_flags.m_isGameFrame && unk41 == 0) {
        grCollisionNodeList& list = (grCollisionNodeList&)g_grCollisionList;
        for (grCollisionNodeList::Iterator it = list.GetBeginIter(); it != list.GetEndIter(); it++) {
            if (it->m_disablePending) {
                fn_80110A40(&*it);
                it->m_disablePending = false;
            }
        }
    }
}

void grCollisionManager::processDebug() { }

void grCollisionManager::renderDebug() { }

void grCollisionManager::regist(grCollision* collision) {
    u8 next = m_nextIndex;
    u16 index = next;
    u32 mask = 1 << (index & 31);
    if (!(m_indexBits.m_bits[(int)index >> 5] & mask)) {
        m_indexBits.m_bits[next >> 5] |= mask;
        m_nextIndex++;
    } else {
        index = m_indexBits.getUnsetBit();
        m_indexBits.m_bits[index >> 5] |= 1 << (index & 31);
        m_nextIndex = index + 1;
    }
    collision->m_index = (u8)index;
    m_collisions[index] = collision;
}

void grCollisionManager::release(grCollision* collision) {
    u16 index = collision->m_index;
    m_collisions[index] = NULL;
    m_indexBits.m_bits[(int)index >> 5] &= ~(1 << (index & 31));
    collision->m_index = 0xFFFF;
}

grCollision* grCollisionManager::getCollision(int index) {
    return m_collisions[index];
}

bool grCollisionManager::checkCliffOverlap(grCollStatus* status, const u32& lineId) {
    CliffHangInfo* info = m_cliffHangInfos;
    u8 team = status->m_cliffTeam;
    bool result = false;
    for (int i = 0; i < 8; i++, info++) {
        if (info->m_status != NULL && lineId == info->m_lineId) {
            if (info->m_team == team) {
                result = false;
            } else {
                result = true;
            }
            break;
        }
    }
    return result;
}

void grCollisionManager::addCliffHangInfo(grCollStatus* status, const u32& lineId) {
    CliffHangInfo* info = m_cliffHangInfos;
    u8 team = status->m_cliffTeam;
    for (int i = 0; i < 8; i++, info++) {
        if (info->m_status == NULL) {
            info->m_status = status;
            info->m_lineId = lineId;
            info->m_team = team;
            return;
        }
    }
}

void grCollisionManager::removeCliffHangInfo(grCollStatus* status) {
    for (int i = 0; i < 8; i++) {
        CliffHangInfo* info = &m_cliffHangInfos[i];
        if (info->m_status == status) {
            info->m_status = NULL;
            info->m_lineId = 0xFFFF;
            info->m_team = 0;
            return;
        }
    }
}

// TODO: clean up, give variables real names
namespace jj_801127D0 {

extern const float lbl_805A2928;
extern const float lbl_805A292C;
extern const float lbl_805A2930;

extern "C" grCollisionJoint* getJoint__11grCollisionFUs(grCollision*, u32);
extern "C" grCollisionLine* getLine__16grCollisionJointFUs(grCollisionJoint*, u32);

static inline float vectorLengthSq(const Vec2f& a, const Vec2f& b) {
    return (a - b).lengthSq();
}

extern "C" void fn_801127D0(void* self) {
    grCollisionJoint* joint;
    grCollision* coll;
    u8* base = (u8*)self;
    float minY = lbl_805A2928;
    float maxY = lbl_805A2928;
    float minX = lbl_805A2928;
    float maxX = lbl_805A2928;
    bool first = true;

    nw4r::ut::LinkList<u8, 48>& list =
        *(nw4r::ut::LinkList<u8, 48>*)&g_grCollisionList;
    nw4r::ut::LinkList<u8, 48>::Iterator it = list.GetBeginIter();

    for (; it != list.GetEndIter(); ++it) {
        u8* obj = &*it;
        u8 flags = obj[0x2C];

        if ((flags >> 7 & 1) && ((u32)(flags >> 1 & 1) != 1) &&
            (obj[0x2D] >> 6 & 1)) {
            coll = (grCollision*)obj;
            u16 jointLen = *(u16*)(obj + 0x06);

            for (u32 j = 0; j < jointLen; j++) {
                joint = getJoint__11grCollisionFUs(coll, j);

                if (joint->m_0x54_6) {
                    u16 lineLen = *(u16*)((u8*)joint + 0x02);

                    for (u32 k = 0; k < lineLen; k++) {
                        grCollisionLine* line = getLine__16grCollisionJointFUs(joint, k);

                        if (*((u8*)line + 0x0F) & 1) {
                            clSegment2D seg;
                            coll->getSegment(&seg, line);

                            float lengthSquared =
                                vectorLengthSq(seg.m_p1Pos, seg.m_p0Pos);
                            float length =
                                (float)(double)nw4r::ut::Abs(lengthSquared) <=
                                        lbl_805A292C
                                    ? lbl_805A2928
                                    : lengthSquared * rsqrtf(lengthSquared);

                            if (length > lbl_805A2930) {
                                register float y0, y1, a, b, x0, x1, c, d;
                                y1 = seg.m_p1Pos.m_y;
                                y0 = seg.m_p0Pos.m_y;
                                x1 = seg.m_p1Pos.m_x;
                                x0 = seg.m_p0Pos.m_x;

                                asm { ps_sub a, y0, y1 }
                                asm { ps_sub b, y0, y1 }
                                asm { ps_sub c, x0, x1 }
                                asm { ps_sub d, x0, x1 }
                                asm { ps_sel a, a, y1, y0 }
                                asm { ps_sel b, b, y0, y1 }
                                asm { ps_sel c, c, x1, x0 }
                                asm { ps_sel d, d, x0, x1 }

                                if (first) {
                                    minY = a;
                                    minX = c;
                                    maxY = b;
                                    maxX = d;
                                    first = false;
                                } else {
                                    register float e, f, g, h;
                                    register float my = minY;
                                    register float mx = minX;
                                    register float My = maxY;
                                    register float Mx = maxX;

                                    asm { ps_sub e, my, a }
                                    asm { ps_sub f, mx, c }
                                    asm { ps_sub g, My, b }
                                    asm { ps_sub h, Mx, d }
                                    asm { ps_sel e, e, a, my }
                                    asm { ps_sel f, f, c, mx }
                                    asm { ps_sel g, g, My, b }
                                    asm { ps_sel h, h, Mx, d }

                                    minY = e;
                                    minX = f;
                                    maxY = g;
                                    maxX = h;
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    *(float*)(base + 0x58) = minX;
    *(float*)(base + 0x5C) = maxX;
    *(float*)(base + 0x64) = minY;
    *(float*)(base + 0x60) = maxY;
}
}

