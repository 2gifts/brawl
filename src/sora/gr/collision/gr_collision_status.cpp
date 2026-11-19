#include <mt/mt_vector.h>
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

// TODO: clean up, give variables real names
namespace jj_80135B94 {
extern "C" {
extern void fn_80135950(int a, int b, int c);

// TODO: clean up, give variables real names
void fn_80135B94(int arg0, int arg1) {
    fn_80135950(arg0, arg0 + 0x1C, arg1);
}
}
}

namespace jj_80135BA0 {
extern "C" {
extern void fn_80135950(void*, void*, int);

// TODO: clean up, give variables real names
void fn_80135BA0(void* arg0, int arg1) {
    fn_80135950(arg0, (char*)arg0 + 0x14, arg1);
}
}
}

namespace jj_80137FF0 {
extern "C" {
    void fn_8015C238(void* a1, int a2);
    void __dl__FPv(void* a1);
}

// TODO: clean up, give variables real names
extern "C" void* fn_80137FF0(void* arg1, int arg2) {
    if (arg1) {
        if (arg1) {
            fn_8015C238(arg1, 0);
        }
        if (arg2 > 0) {
            __dl__FPv(arg1);
        }
    }
    return arg1;
}
}

namespace jj_80137074 {
struct Entry {
    u32 a;
    u32 b;
    u32 c;
    u8 d;
    u8 e;

    int equals(const Entry& other) const {
        int result = 0;
        if (a == other.a)
            result = 1;
        return result;
    }
};

struct Owner {
    u8 pad[0x7d];
    u8 count;
    u8 pad2[0x174 - 0x7e];
    Entry entries[8];
};

// TODO: clean up, give variables real names
extern "C" int fn_80137074(Owner* self, const Entry* src) {
    u8 count = self->count;
    int result = 0;

    if (count < 8U) {
        for (u32 i = 0; i < count; ++i) {
            if (self->entries[i].equals(*src)) {
                result = 1;
                break;
            }
        }

        if (result == 0) {
            self->entries[count] = *src;
            ++self->count;
            result = 1;
        }
    }

    return result;
}
}

namespace jj_801371D8 {
struct Entry {
    u32 a, b, c;
    u8 d, e;
    int equals(const Entry& other) const {
        int result = 0;
        if (a == other.a) result = 1;
        return result;
    }
};
struct Owner {
    u8 pad[0x7d];
    u8 count;
    u8 pad2[0x174 - 0x7e];
    Entry entries[8];
};
// TODO: clean up, give variables real names
extern "C" void fn_801371D8(Owner* self, const Entry* src) {
    u8 count = self->count;
    for (u32 i = 0; i < count; ++i) {
        if (self->entries[i].equals(*src)) {
            for (; i < self->count - 1; ++i) {
                self->entries[i] = self->entries[i + 1];
            }
            Entry& last = self->entries[self->count - 1];
            last.a = 0;
            last.b = 0;
            last.c = 0;
            last.d = 0;
            --self->count;
            return;
        }
    }
}
}

namespace jj_80137114 {
struct Entry {
    u8 pad[0xE];
    u16 flags;
};

extern "C" int fn_80114A60(void** a, Entry** b);

// TODO: clean up, replace the raw offset accesses with a struct with named fields and give variables real names
extern "C" int fn_80137114(u8* self, Entry** arg) {
    Entry** p = (Entry**)(self + 0x174);
    int ret = 0;
    u32 i = 0;
    while (i < self[0x7D]) {
        int f;
        Entry* b = *arg;
        int eq = 0;
        Entry* cur = *p;
        if (cur == b) {
            eq = 1;
        }
        if (eq) {
            ret = 1;
            break;
        }
        f = 0;
        if (cur->flags & b->flags & 0xF) {
            f = 1;
        }
        if (f) {
            if (fn_80114A60((void**)p, arg)) {
                ret = 1;
                break;
            }
        }
        p += 4;
        i++;
    }
    return ret;
}
}

namespace jj_80137298 {
extern "C" int fn_80114A60(void*, void*);

struct Entry {
    u32 a, b, c;
    u8 d, e;
    u8 pad[2];
};

inline bool valid(Entry* p) {
    return p->a != 0 && p->b != 0 && p->c != 0;
}

// TODO: clean up, replace the raw offset accesses with a struct with named fields and give variables real names
extern "C" void fn_80137298(void* arg0, u32* arg1) {
    u8* base = (u8*)arg0;
    Entry* p = (Entry*)(base + 0x174);
    u32 i = 0;

    while (i < base[0x7D]) {
        if (valid(p)) {
            int equal = 0;
            if (p->a == *arg1)
                equal = 1;

            if (equal != 0 || fn_80114A60(p, arg1) != 0) {
                u8* q = base + i * 0x10;

                for (; i < base[0x7D] - 1; i++, q += 0x10) {
                    *(u32*)(q + 0x174) = *(u32*)(q + 0x184);
                    *(u32*)(q + 0x178) = *(u32*)(q + 0x188);
                    *(u32*)(q + 0x17C) = *(u32*)(q + 0x18C);
                    q[0x180] = q[0x190];
                    q[0x181] = q[0x191];
                }

                u8* last = base + (base[0x7D] - 1) * 0x10;
                *(u32*)(last + 0x174) = 0;
                *(u32*)(last + 0x178) = 0;
                *(u32*)(last + 0x17C) = 0;
                last[0x180] = 0;
                base[0x7D] = base[0x7D] - 1;
                return;
            }
        }

        p++;
        i++;
    }
}
}

namespace jj_80135950 {


extern const f32 lbl_805A2A40;

struct Self {
    u8 pad0[8];
    u32 flags;
    u8 pad1[0x3C - 0x0C];
    Vec2f a;
    Vec2f b;
    Vec2f c;
    u8 pad2[4];
    s32 mode;
};

// TODO: clean up, give variables real names
extern "C" void fn_80135950(const Self* self, Vec2f* base, f32* out) {
    Vec2f a, b, c;
    __memcpy(&a, &self->a, sizeof(Vec2f));
    s32 flags = self->flags;
    __memcpy(&b, &self->b, sizeof(Vec2f));
    __memcpy(&c, &self->c, sizeof(Vec2f));

    if (((s32)((flags << 0x18) | ((flags >> 8) & 0x800000)) >> 0x18) == 0) {
        a.m_x = -a.m_x;
        b.m_x = -b.m_x;
    } else {
        c.m_x = -c.m_x;
    }

    Vec2f p;
    __memcpy(&p, &(*base + a), sizeof(Vec2f));

    Vec2f q1, q0;
    switch (self->mode) {
    case 0:
    default:
        q1 = p;
        q0 = p + b;
        break;
    case 1:
        q1 = p + c;
        q0 = p;
        break;
    case 2:
        f32 m = b.m_y > c.m_y ? b.m_y : c.m_y;
        q1 = p + Vec2f(b.m_x, m);
        q0 = p + Vec2f(c.m_x, lbl_805A2A40);
        break;
    }

    register f32 f0, f1, f2, f3, f4, f5, f6, f7;
    f6 = q0.m_x;
    f7 = q1.m_x;
    f3 = q0.m_y;
    f4 = q1.m_y;
    asm { ps_sub f5, f7, f6 }
    asm { ps_sub f1, f7, f6 }
    asm { ps_sub f2, f4, f3 }
    asm { ps_sub f0, f4, f3 }
    asm { ps_sel f5, f5, f6, f7 }
    asm { ps_sel f2, f2, f3, f4 }
    asm { ps_sel f1, f1, f7, f6 }
    asm { ps_sel f0, f0, f4, f3 }
    out[0] = f5;
    out[1] = f2;
    out[2] = f1;
    out[3] = f0;
}
}

namespace jj_801361F0 {
class Object;

class NtBuffer {
public:
    virtual void slot0(Object*, int);
};

struct Entry {
    void* a;
    void* b;
    void* c;
    unsigned char d;
    unsigned char e;

    bool valid() const { return a && b && c; }
    void clear() { a = 0; b = 0; c = 0; d = 0; }
};

struct Object {
    char pad0[8];
    signed int state : 8;
    unsigned int stateRest : 24;
    char padC[0x58];
    NtBuffer* callback;
    char pad68[4];
    union {
        unsigned int word;
        struct {
            unsigned char first;
            unsigned char flags : 8;
            unsigned char x, y;
        } bytes;
    } u;
    char pad70[5];
    unsigned char f80 : 1, f40 : 1, f20 : 1, f10 : 1;
    unsigned char f08 : 1, f04 : 1, f02 : 1, f01 : 1;
    char pad76[7];
    unsigned char count;
    char pad7E[0x56];
    Entry entries[8];
    Entry special;
    Entry other;
    Entry list[8];
    Entry last;

    int getState() const { return state; }
};

static inline void clearSpecial(Object* self, Entry* entry) {
    entry->clear();
    self->state = 2;
    if (!(self->u.bytes.flags & 8))
        self->f80 = 0;
}

// TODO: clean up, give variables real names
extern "C" void fn_801361F0(Object* self, void* target) {
    bool changed = false;
    Object* cursor = self;

    for (unsigned int i = 0; i < 8;
         cursor = (Object*)((char*)cursor + 16), i++) {
        Entry& e = cursor->entries[0];
        if (e.valid() && e.c == target) {
            e.clear();
            if (i == 3) {
                self->state = 2;
                if (!(self->u.bytes.flags & 8))
                    self->f80 = 0;
                self->f80 = 0;
            } else if (i == 1) {
                self->f20 = 0;
            } else if (i == 2) {
                self->f10 = 0;
            } else if (i == 0) {
                self->f40 = 0;
            } else if (i == 4) {
                self->f04 = 0;
            } else if (i == 5) {
                self->f08 = 0;
            } else if (i == 6) {
                self->f01 = 0;
            } else if (i == 7) {
                self->f02 = 0;
            }
            if (i <= 3)
                self->u.bytes.flags &= ~(1 << i);
            changed = true;
        }
    }

    if (!self->u.bytes.flags)
        self->u.word &= 0xffffff;

    Entry* p = self->list;
    unsigned int n = self->count;
    unsigned int i = 0;
    while (i < n) {
        if (p->valid() && p->c == target) {
            p->clear();
            --self->count;
            for (unsigned int j = i; j < self->count; j++)
                self->list[j] = self->list[j + 1];
            self->list[self->count].clear();
        } else {
            p++;
            i++;
        }
    }

    for (unsigned int j = self->count; j < 8; j++)
        self->list[j].clear();

    if (self->other.valid() && self->other.c == target)
        self->other.clear();

    if (self->last.valid() && self->last.c == target)
        self->last.clear();

    if (self->getState() == 3 && self->special.c == target) {
        clearSpecial(self, &self->special);
        changed = true;
    }

    if (changed && self->callback)
        self->callback->slot0(self, 0);
}
}

// TODO: clean up, give variables real names
namespace jj_8013344C {
namespace Heaps {
enum HeapType { HEAP_DEFAULT = 0 };
}

extern "C" void* __nw__FUlQ25Heaps8HeapType(unsigned long, Heaps::HeapType);
extern "C" void fn_801334A4(void* p, void* a, void* b);

extern "C" void create__12grCollStatusFiQ212grCollStatus7Settingi(Heaps::HeapType heap, void* a, void* b) {
    void* p = __nw__FUlQ25Heaps8HeapType(0x27C, heap);
    if (p != 0) {
        fn_801334A4(p, a, b);
    }
}
}

