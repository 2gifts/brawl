#include <gr/collision/gr_collision.h>
#include <gr/collision/gr_collision_manager.h>
#include <gr/collision/gr_collision_status.h>
#include <mt/mt_coll2d.h>
#include <nw4r/ut/ut_LinkList.h>
#include <types.h>

typedef nw4r::ut::LinkList<grCollStatus, 0> grCollStatusNodeList;
grCollStatusNodeList lbl_8049E594;
extern grCollisionManager* lbl_805A0360;

// The four attack point flags: the top nibble of the byte at 0x6F.
struct grCollStatusAttackBits {
    u8 m_up : 1;
    u8 m_down : 1;
    u8 m_left : 1;
    u8 m_right : 1;
    u8 : 4;
};

void grCollStatus::setStAir() {
    m_setting = Setting_Air;
    if (!(m_unk6D & 8)) {
        m_touchDown = false;
    }
}

void grCollStatus::setStCliff() {
    m_setting = Setting_Cliff;
}

void grCollStatus::setPassableCheck(bool passable) {
    if (passable) {
        m_noPassableCheck = false;
    } else {
        m_noPassableCheck = true;
    }
}

void grCollStatus::clearIgnoreLine() {
    m_collLineWorkArray[9].m_collisionLine = NULL;
    m_collLineWorkArray[9].m_collisionJoint = NULL;
    m_collLineWorkArray[9].m_collision = NULL;
    m_collLineWorkArray[9].m_unkC = 0;
}

void grCollStatus::setAttackPoint(bool up, bool down, bool left, bool right) {
    grCollStatusAttackBits* bits = (grCollStatusAttackBits*)&m_unk6F;
    bits->m_up = up;
    bits->m_down = down;
    bits->m_left = left;
    bits->m_right = right;
}

void grCollStatus::setTouchLineSpeed(Vec2f* speed) {
    float x = speed->m_x;
    float y = speed->m_y;
    m_touchLineSpeed.m_x = x;
    m_touchLineSpeed.m_y = y;
}

// The shape corners are copied as plain words.
struct grCollStatusRhombusWords {
    u32 m_words[14];
};

void grCollStatus::getShape(clRhombus2D* out) {
    *(grCollStatusRhombusWords*)out = *(grCollStatusRhombusWords*)((char*)m_currentCollShape + 4);
}

void grCollStatus::getShapePrev(clRhombus2D* out) {
    *(grCollStatusRhombusWords*)out = *(grCollStatusRhombusWords*)((char*)m_prevCollShape + 4);
}

bool grCollStatus::isTouch(TouchId touchId) {
    bool touch = false;
    if (touchId == 0xFF && (m_touchUp == true || m_touchDown == true || m_touchLeft == true || m_touchRight == true
                            || m_touchUpRight == true || m_touchDownRight == true || m_touchUpLeft == true
                            || m_touchDownLeft == true)) {
        touch = true;
    } else {
        switch (touchId) {
        case Touch_Up:
            touch = m_touchUp;
            break;
        case Touch_Down:
            touch = m_touchDown;
            break;
        case Touch_Left:
            touch = m_touchLeft;
            break;
        case Touch_Right:
            touch = m_touchRight;
            break;
        case Touch_Up_Right:
            touch = m_touchUpRight;
            break;
        case Touch_Down_Right:
            touch = m_touchDownRight;
            break;
        case Touch_Up_Left:
            touch = m_touchUpLeft;
            break;
        case Touch_Down_Left:
            touch = m_touchDownLeft;
            break;
        }
    }
    return touch;
}

u32 grCollStatus::getTouchCount() {
    u32 count = 0;
    if (isTouch(Touch_Up) == true) {
        count = 1;
    }
    if (isTouch(Touch_Left) == true) {
        count++;
    }
    if (isTouch(Touch_Right) == true) {
        count++;
    }
    if (isTouch(Touch_Down) == true) {
        count++;
    }
    if (isTouch(Touch_Up_Left) == true) {
        count++;
    }
    if (isTouch(Touch_Down_Left) == true) {
        count++;
    }
    if (isTouch(Touch_Up_Right) == true) {
        count++;
    }
    if (isTouch(Touch_Down_Right) == true) {
        count++;
    }
    return count;
}

void grCollStatus::clearTouchLine(TouchId touchId) {
    grCollisionLineWork* work = &m_collLineWorkArray[touchId];
    work->m_collisionLine = NULL;
    work->m_collisionJoint = NULL;
    work->m_collision = NULL;
    work->m_unkC = 0;
    switch (touchId) {
    case Touch_Up:
        m_touchUp = false;
        break;
    case Touch_Left:
        m_touchLeft = false;
        break;
    case Touch_Right:
        m_touchRight = false;
        break;
    case Touch_Down:
        m_touchDown = false;
        break;
    case Touch_Up_Right:
        m_touchUpRight = false;
        break;
    case Touch_Down_Right:
        m_touchDownRight = false;
        break;
    case Touch_Up_Left:
        m_touchUpLeft = false;
        break;
    case Touch_Down_Left:
        m_touchDownLeft = false;
        break;
    }
}

grCollisionLineWork* grCollStatus::getTouchLineWork(TouchId touchId) {
    return &m_collLineWorkArray[touchId];
}

union grCollStatusLineId {
    u32 m_word;
    struct {
        u16 m_collisionIndex;
        u16 m_lineIndex;
    } m_ids;
};

static inline u32 grCollStatusMakeLineId(grCollision* collision, grCollisionLine* line) {
    grCollStatusLineId id;
    id.m_word = 0xFFFF;
    id.m_ids.m_collisionIndex = collision->m_index;
    id.m_ids.m_lineIndex = collision->getLineIndex(line);
    return id.m_word;
}

static inline u32 grCollStatusMakeNoLineId() {
    grCollStatusLineId id;
    id.m_word = 0xFFFF;
    id.m_ids.m_collisionIndex = 0;
    id.m_ids.m_lineIndex = 0;
    return id.m_word;
}

u32 grCollStatus::getTouchLineId(TouchId touchId) {
    grCollisionLineWork* work = &m_collLineWorkArray[touchId];
    if (work->isValid()) {
        return grCollStatusMakeLineId(work->m_collision, work->m_collisionLine);
    }
    return grCollStatusMakeNoLineId();
}

int grCollStatus::getTouchLineMaterialType(TouchId touchId) {
    grCollisionLineWork* work = &m_collLineWorkArray[touchId];
    if (work->isValid() && isTouch(touchId) == true) {
        return work->m_collisionLine->m_materialType;
    }
    return 0;
}

void grCollStatus::setCliffRange(Vec2f* a, Vec2f* b, float len) {
    float by = b->m_y;
    float ax = a->m_x;
    float ay = a->m_y;
    float bx = b->m_x;
    m_cliffRangeA.m_x = ax;
    m_cliffRangeA.m_y = ay;
    m_cliffRangeB.m_x = bx;
    m_cliffRangeB.m_y = by;
    m_cliffRangeLen = len;
    m_cliffRangeEndY = by;
}

void grCollStatus::regist() {
    if (!m_isRegistered) {
        lbl_8049E594.Insert(lbl_8049E594.GetEndIter(), this);
        m_isRegistered = true;
    }
}

void grCollStatus::unRegist() {
    if (m_isRegistered == true) {
        lbl_8049E594.Erase(this);
        m_isRegistered = false;
        clearLineWorks();
    }
}

grCollStatus::~grCollStatus() {
    if (m_isRegistered == true) {
        lbl_8049E594.Erase(this);
        m_isRegistered = false;
        clearLineWorks();
    }
}

void grCollStatus::leaveCliffHang() {
    lbl_805A0360->removeCliffHangInfo(this);
    m_isCliffHang = false;
}

void grCollStatus::getPos(Vec2f* pos) {
    if (m_currentCollShape->getType() == 2) {
        *pos = m_currentCollShape->getDownPos();
    }
}

void grCollStatus::setIgnoreLine(u32* lineId) {
    u32 id = *lineId;
    grCollision::getLineWork(&id, &m_collLineWorkArray[9]);
}

extern "C" bool fn_80112DBC(grCollisionManager* manager, grCollStatus* status, const u32& lineId);

bool grCollStatus::checkCliffEntryOverlap() {
    grCollisionLineWork* work = &m_collLineWorkArray[8];
    grCollisionManager* manager = lbl_805A0360;
    u32 lineId = grCollStatusMakeLineId(work->m_collision, work->m_collisionLine);
    return fn_80112DBC(manager, this, lineId);
}

void grCollStatus::entryCliffHang() {
    grCollisionLineWork* work = &m_collLineWorkArray[8];
    grCollisionManager* manager = lbl_805A0360;
    u32 lineId = grCollStatusMakeLineId(work->m_collision, work->m_collisionLine);
    manager->addCliffHangInfo(this, lineId);
    m_isCliffHang = true;
}

// Drops every line the status remembers (the touch lines, the ledge line, the ignored line and the work lines).
void grCollStatus::clearLineWorks() {
    m_collLineWorkArray[0].m_collisionLine = NULL;
    m_collLineWorkArray[0].m_collisionJoint = NULL;
    m_collLineWorkArray[0].m_collision = NULL;
    m_collLineWorkArray[0].m_unkC = 0;
    m_collLineWorkArray[1].m_collisionLine = NULL;
    m_collLineWorkArray[1].m_collisionJoint = NULL;
    m_collLineWorkArray[1].m_collision = NULL;
    m_collLineWorkArray[1].m_unkC = 0;
    m_collLineWorkArray[2].m_collisionLine = NULL;
    m_collLineWorkArray[2].m_collisionJoint = NULL;
    m_collLineWorkArray[2].m_collision = NULL;
    m_collLineWorkArray[2].m_unkC = 0;
    m_collLineWorkArray[3].m_collisionLine = NULL;
    m_collLineWorkArray[3].m_collisionJoint = NULL;
    m_collLineWorkArray[3].m_collision = NULL;
    m_collLineWorkArray[3].m_unkC = 0;
    m_collLineWorkArray[4].m_collisionLine = NULL;
    m_collLineWorkArray[4].m_collisionJoint = NULL;
    m_collLineWorkArray[4].m_collision = NULL;
    m_collLineWorkArray[4].m_unkC = 0;
    m_collLineWorkArray[5].m_collisionLine = NULL;
    m_collLineWorkArray[5].m_collisionJoint = NULL;
    m_collLineWorkArray[5].m_collision = NULL;
    m_collLineWorkArray[5].m_unkC = 0;
    m_collLineWorkArray[6].m_collisionLine = NULL;
    m_collLineWorkArray[6].m_collisionJoint = NULL;
    m_collLineWorkArray[6].m_collision = NULL;
    m_collLineWorkArray[6].m_unkC = 0;
    m_collLineWorkArray[7].m_collisionLine = NULL;
    m_collLineWorkArray[7].m_collisionJoint = NULL;
    m_collLineWorkArray[7].m_collision = NULL;
    m_collLineWorkArray[7].m_unkC = 0;
    ((u8*)this)[0x75] = 0; // m_touchBits as a plain byte
    m_collLineWorkArray[8].m_collisionLine = NULL;
    m_collLineWorkArray[8].m_collisionJoint = NULL;
    m_collLineWorkArray[8].m_collision = NULL;
    m_collLineWorkArray[8].m_unkC = 0;
    m_collLineWorkArray[9].m_collisionLine = NULL;
    m_collLineWorkArray[9].m_collisionJoint = NULL;
    m_collLineWorkArray[9].m_collision = NULL;
    m_collLineWorkArray[9].m_unkC = 0;
    m_collLineWorkArray[18].m_collisionLine = NULL;
    m_collLineWorkArray[18].m_collisionJoint = NULL;
    m_collLineWorkArray[18].m_collision = NULL;
    m_collLineWorkArray[18].m_unkC = 0;
    ((u8*)this)[0x7D] = 0;
    m_collLineWorkArray[10].m_collisionLine = NULL;
    m_collLineWorkArray[10].m_collisionJoint = NULL;
    m_collLineWorkArray[10].m_collision = NULL;
    m_collLineWorkArray[10].m_unkC = 0;
    m_collLineWorkArray[11].m_collisionLine = NULL;
    m_collLineWorkArray[11].m_collisionJoint = NULL;
    m_collLineWorkArray[11].m_collision = NULL;
    m_collLineWorkArray[11].m_unkC = 0;
    m_collLineWorkArray[12].m_collisionLine = NULL;
    m_collLineWorkArray[12].m_collisionJoint = NULL;
    m_collLineWorkArray[12].m_collision = NULL;
    m_collLineWorkArray[12].m_unkC = 0;
    m_collLineWorkArray[13].m_collisionLine = NULL;
    m_collLineWorkArray[13].m_collisionJoint = NULL;
    m_collLineWorkArray[13].m_collision = NULL;
    m_collLineWorkArray[13].m_unkC = 0;
    m_collLineWorkArray[14].m_collisionLine = NULL;
    m_collLineWorkArray[14].m_collisionJoint = NULL;
    m_collLineWorkArray[14].m_collision = NULL;
    m_collLineWorkArray[14].m_unkC = 0;
    m_collLineWorkArray[15].m_collisionLine = NULL;
    m_collLineWorkArray[15].m_collisionJoint = NULL;
    m_collLineWorkArray[15].m_collision = NULL;
    m_collLineWorkArray[15].m_unkC = 0;
    m_collLineWorkArray[16].m_collisionLine = NULL;
    m_collLineWorkArray[16].m_collisionJoint = NULL;
    m_collLineWorkArray[16].m_collision = NULL;
    m_collLineWorkArray[16].m_unkC = 0;
    m_collLineWorkArray[17].m_collisionLine = NULL;
    m_collLineWorkArray[17].m_collisionJoint = NULL;
    m_collLineWorkArray[17].m_collision = NULL;
    m_collLineWorkArray[17].m_unkC = 0;
}

void grCollStatus::setStGround() {
    if (m_setting == Setting_Cliff) {
        grCollisionLineWork* cliff = &m_collLineWorkArray[8];
        m_collLineWorkArray[Touch_Down] = *cliff;
        if (cliff->isValid()) {
            clSegment2D segment;
            cliff->m_collision->getSegment(&segment, cliff->m_collisionLine);
            Vec2f pos;
            if (m_cliffDir == 1) {
                pos.m_x = segment.m_p0Pos.m_x + 0.01f;
                pos.m_y = segment.m_p0Pos.m_y;
            } else {
                pos.m_x = segment.m_p1Pos.m_x - 0.01f;
                pos.m_y = segment.m_p1Pos.m_y;
            }
            m_currentCollShape->setDownPos(pos);
        }
        cliff->m_collisionLine = NULL;
        cliff->m_collisionJoint = NULL;
        cliff->m_collision = NULL;
        cliff->m_unkC = 0;
    }
    m_setting = Setting_Ground;
    m_touchDown = true;
}

void grCollStatus::setStGroundSimple() {
    m_setting = Setting_Ground;
    m_touchDown = true;
}

void grCollStatus::setShapeKeepPrev(clRhombus2D* rhombus) {
    m_prevCollShape->set(*m_currentCollShape);
    *(grCollStatusRhombusWords*)((char*)m_currentCollShape + 4) = *(grCollStatusRhombusWords*)rhombus;
    m_prevDownPos = m_prevCollShape->getDownPos();
    m_unk1C = m_currentCollShape->getDownPos();
}
