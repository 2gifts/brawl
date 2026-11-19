#include <nw4r/ut/ut_algorithm.h>
#include <nw4r/math/math_types.h>
#include <mt/mt_common.h>
#include <types.h>
#include <gf/gf_area_manager.h>
#include <gr/collision/gr_collision.h>
#include <gr/collision/gr_collision_collection.h>
#include <gr/collision/gr_collision_test_types.h>
#include <mt/mt_coll2d.h>
#include <mt/mt_prng.h>
#include <mt/mt_vector.h>
#include <ph/ph2ndaryWorld.h>
#include <gf/gf_slow_manager.h>
#include <gm/gm_global.h>

// Brawl physics wrapper translation unit ph_2ndary_world.o (main.dol 0x80082438-0x8008AC5C).
// Not yet decompiled. Functions in address order with their map names:
//   0x80082438   268  __ct
//   0x80082544   456  __dt
//   0x8008270C  2256  operationExecute
//   0x80082FDC   464  sameZelda
//   0x800831AC   484  samePeach
//   0x80083390   548  operationAwakeExecute
//   0x800835B4   808  constraintInertiaChild
//   0x800838DC  3144  operationGravityWeapon
//   0x80084524  1460  operationGravityWeaponTrunModelMatrix
//   0x80084AD8  1776  operationWeapon
//   0x800851C8  4776  operationGravity
//   0x80086470   204  getSamusRandomAccel
//   0x8008653C   212  addConstraintArray
//   0x80086610   132  removeConstraintArray
//   0x80086694   184  updateCollision
//   0x8008674C   156  updateCollision2Pos
//   0x800867E8   728  checkCollisionHit
//   0x80086AC0  2188  chCollisionGround
//   0x8008734C   880  correctCollisionSlope
//   0x800876BC   416  correctCollisionVertical
//   0x8008785C   416  correctCollisionLevel
//   0x800879FC  5564  chCollisionGround2
//   0x80088FB8   620  chCollisionRigidBody
//   0x80089224   460  getWindArray
//   0x800893F0   452  setNodeWindArray
//   0x800895B4  1784  getNodeWindArray
//   0x80089CAC  1824  operationExecuteSleep
//   0x8008A3CC   900  operationExecuteSleepCreateLocalSpace
//   0x8008A750   876  operationExecuteSleepRestructuring
//   0x8008AABC   416  isCollisionBetweenPoints


extern const f32 lbl_805A1BB8;
extern const f32 lbl_805A1BBC;
extern const f32 lbl_805A1C14;

void ph2ndaryWorld::operationAwakeExecute(ph2ndaryLine* line, ph2ndaryLine* body, const f32* gravity,
                                        int flag1, int flag2, int flag3, int flag4,
                                        u8 flag5, u8 flag6, u8 flag7, u8 flag8,
                                        u8 flag9, int index, u8 flag10, u8 flag11) {
    if (gfSlowManager::getSlowRate() != 1) {
        unk3C = 1;
        unk40 = 1;
        if (g_GameGlobal->isPrevJustGameFrame() == true) {
            unk40 = 1;
        } else if (g_GameGlobal->isJustGameFrame() == true) {
            unk40 = 0;
        } else {
            unk40 = 2;
        }
        m_unk48 = 1.0f / gfSlowManager::getSlowRate();
        f32 frame = g_GameGlobal->getGameFrame();
        unk44 = frame - (int)frame;
    } else {
        unk44 = 0.0f;
        m_unk48 = 1.0f;
        unk3C = 0;
        unk3D = 0;
    }
    if ((s32)unk4CValue > 1) {
        m_unk48 /= (s32)unk4CValue;
    }
    getWindArray();
    operationGravity(line, body, gravity, flag1, flag2, flag3, flag4,
                     flag5, flag6, flag7, flag11);
    if (unk2C.mustDeallocate()) {
        hkThreadMemory::s_instance->deallocateChunk(unk2C.m_data, (unk2C.m_capacityAndFlags & hkArrayBase::CAPACITY_MASK) * 4, 0x15);
    }
    unk2C.m_data = 0;
    unk2C.m_size = 0;
    unk2C.m_capacityAndFlags = (unk2C.m_capacityAndFlags & hkArrayBase::FORCE_SIGN_FLAG) | hkArrayBase::DONT_DEALLOCATE_FLAG;
    if (unk20.mustDeallocate()) {
        hkThreadMemory::s_instance->deallocateChunk(unk20.m_data, (unk20.m_capacityAndFlags & hkArrayBase::CAPACITY_MASK) * 4, 0x15);
    }
    unk20.m_data = 0;
    unk20.m_size = 0;
    unk20.m_capacityAndFlags = (unk20.m_capacityAndFlags & hkArrayBase::FORCE_SIGN_FLAG) | hkArrayBase::DONT_DEALLOCATE_FLAG;
}

ph2ndaryWorld::ph2ndaryWorld() {
    unkC = -1;
    unk10 = 0;
    unk14 = 0;
    unk18 = 0;
    unk1C = 0;
    unk20.m_data = hkThreadMemory::s_instance->allocateChunk(0, 0x15);
    unk20.m_size = 0;
    unk20.m_capacityAndFlags = 0;
    unk2C.m_data = hkThreadMemory::s_instance->allocateChunk(0, 0x15);
    unk2C.m_size = 0;
    unk2C.m_capacityAndFlags = 0;
    unk38 = 1;
    unk39 = 0;
    unk3A = 0;
    unk3B = 0;
    unk3C = 0;
    unk3D = 0;
    unk40 = 0;
    unk44 = lbl_805A1BB8;
    m_unk48 = lbl_805A1BB8;
    unk4CValue = 1;
    unk50 = 0;
    unk54 = 0;
    unk58 = lbl_805A1BBC;
    unk5C = 0;
    unk60 = 0;
    unk64 = 0;
    unk68 = 1;
    m_nextId = 0;
    unk10 = new (Heaps::Physics) grCollCollection(0xDC);
}

s32 ph2ndaryWorld::addConstraintArray(void* ptr, const u8* kind) {
    for (int i = 0; i < m_constraints.getSize(); i++) {
        if (((ConstraintEntry*)m_constraints.m_data)[i].ptr == ptr) {
            return -1;
        }
    }
    u8 k = *kind;
    s32 id = m_nextId;
    ConstraintEntry& entry = m_constraints.expandOne();
    entry.kind = k;
    entry.ptr = ptr;
    entry.id = id;
    return m_nextId++;
}

f32 ph2ndaryWorld::getSamusRandomAccel(f32 a, f32 b, f32 c) {
    f32 r = lbl_805A1BBC * randf();
    if (r > lbl_805A1C14) {
        if (a < lbl_805A1BB8) {
            return a - m_unk48 * (b * (lbl_805A1BBC * randf()));
        }
        return a + m_unk48 * (c * (lbl_805A1BBC * randf()));
    }
    return lbl_805A1BB8;
}

void ph2ndaryWorld::removeConstraintArray(void* ptr) {
    for (int i = 0; i < m_constraints.m_size; i++) {
        if (((ConstraintEntry*)m_constraints.m_data)[i].ptr == ptr) {
            ((ConstraintEntry*)m_constraints.m_data)[i].ptr = 0;
            m_constraints.m_size--;
            ConstraintEntry* last = &((ConstraintEntry*)m_constraints.m_data)[m_constraints.m_size];
            ConstraintEntry* cur = &((ConstraintEntry*)m_constraints.m_data)[i];
            cur->kind = last->kind;
            cur->ptr = last->ptr;
            cur->id = last->id;
            return;
        }
    }
}
u32 ph2ndaryWorld::isCollisionBetweenPoints(const Vec2f* from, const Vec2f* to) {
    Vec2f dir = *to - *from;
    dir.normalize();
    grCollSegmentEx moveSeg;
    moveSeg.m_segment.m_p0Pos = *to;
    moveSeg.m_segment.m_p1Pos = *from;
    moveSeg.m_unk10 = 0;
    u32 result = 0;
    for (u32 i = 0; i < unk10->m_lineWorks.m_num; i++) {
        grCollisionLineWork* lineWork = &unk10->m_lineWorks.m_works[i];
        grCollSegmentEx lineSeg;
        lineWork->m_collision->getSegment(&lineSeg, lineWork->m_collisionLine, 0);
        Vec2f normal = lineWork->getNormalVec();
        normal.normalize();
        if (!(dir.m_x * normal.m_x + dir.m_y * normal.m_y > 0.0f)) {
            if (!lineWork->m_collisionLine->m_isTargetOther) {
                if (clSegment2D::checkIntersect(&lineSeg.m_segment, &moveSeg.m_segment)) {
                    if (lineWork->m_collisionLine->m_isLeft || lineWork->m_collisionLine->m_isRight) {
                        result |= 1;
                    }
                    if (lineWork->m_collisionLine->m_isUnder) {
                        result |= 2;
                    }
                    if (lineWork->m_collisionLine->m_isUpper) {
                        result |= 4;
                    }
                }
            }
        }
    }
    return result;
}

// HYPOTHESIS: stage wind table, a pointer to it lives in lbl_8049DEC8.
struct WindTable {
    u8 _00[0x7C];
    ph2ndaryWorld::WindElement* m_elements; // 0x7C
    u32 m_count;                            // 0x80
};
extern WindTable* lbl_8049DEC8[];

void ph2ndaryWorld::getWindArray() {
    if (unk2C.mustDeallocate()) {
        hkThreadMemory::s_instance->deallocateChunk(unk2C.m_data, (unk2C.m_capacityAndFlags & hkArrayBase::CAPACITY_MASK) * 4, 0x15);
    }
    unk2C.m_data = 0;
    unk2C.m_size = 0;
    unk2C.m_capacityAndFlags = (unk2C.m_capacityAndFlags & hkArrayBase::FORCE_SIGN_FLAG) | hkArrayBase::DONT_DEALLOCATE_FLAG;
    if (unk20.mustDeallocate()) {
        hkThreadMemory::s_instance->deallocateChunk(unk20.m_data, (unk20.m_capacityAndFlags & hkArrayBase::CAPACITY_MASK) * 4, 0x15);
    }
    unk20.m_data = 0;
    unk20.m_size = 0;
    unk20.m_capacityAndFlags = (unk20.m_capacityAndFlags & hkArrayBase::FORCE_SIGN_FLAG) | hkArrayBase::DONT_DEALLOCATE_FLAG;
    if (unk50) {
        unk14 = 0;
        return;
    }
    int found = 0;
    for (u32 i = 0; i < lbl_8049DEC8[0]->m_count; i++) {
        WindElement* element = &lbl_8049DEC8[0]->m_elements[i];
        unk18 = &element->m_entry;
        if (unk18->m_kind == 12 || unk18->m_kind == 13) {
            unk1C = lbl_8059FF90->testIDInclude(0x1A, element->m_id);
            if (unk1C != 0) {
                if (unk2C.m_size == (unk2C.m_capacityAndFlags & hkArrayBase::CAPACITY_MASK)) {
                    hkArrayUtil::_reserveMore(&unk2C, 4);
                }
                ((gfArea**)unk2C.m_data)[unk2C.m_size++] = unk1C;
                if (unk20.m_size == (unk20.m_capacityAndFlags & hkArrayBase::CAPACITY_MASK)) {
                    hkArrayUtil::_reserveMore(&unk20, 4);
                }
                ((WindEntry**)unk20.m_data)[unk20.m_size++] = unk18;
                found = 1;
            }
        }
    }
    if (!found) {
        unk14 = 0;
    } else {
        unk14 = 1;
    }
}

// TODO: clean up, give variables real names
namespace jj_8008674C {

struct Rect {
    f32 left;
    f32 top;
    f32 right;
    f32 bottom;
};

struct Node {
    Rect rect;
    u8 pad[10];
    s16 flag;
};

struct Obj {
    u8 pad[16];
    Node* node;
};

extern "C" void fn_8011728C(Node*, int, int);

// TODO: clean up, give variables real names
extern "C" void fn_8008674C(Obj* self, const Vec2f* a, const Vec2f* b) {
    register f32 t0, t1, t2, by, ay, bx, ax;
    Rect rect;

    bx = b->m_x;
    ax = a->m_x;
    by = b->m_y;
    ay = a->m_y;
    asm { ps_sub t0, ax, bx }
    asm { ps_sub t1, ax, bx }
    asm { ps_sub t2, ay, by }
    asm { ps_sel t0, t0, bx, ax }
    asm { ps_sel t1, t1, ax, bx }
    asm { ps_sel t2, t2, by, ay }
    rect.left = t0;
    rect.top = t2;
    rect.right = t1;
    asm { ps_sub t0, ay, by }
    asm { ps_sel t0, t0, ay, by }
    rect.bottom = t0;

    self->node->flag = 0;
    __memcpy(&self->node->rect, &rect, sizeof(rect));
    fn_8011728C(self->node, 0, 0);
}
}

namespace jj_80086694 {
struct RectF { float l, t, r, b; };
struct RectI { int l, t, r, b; };
struct Buf { RectF rect; char pad[10]; short flag; };
struct Outer { char pad[16]; Buf* buf; };
extern "C" void fn_8011728C(Buf*, int, int);

static inline void makeRect(RectF& r, const Vec2f& a, const Vec2f& b) {
    register float t0, t2, t1, t3, ax, bx, by, ay;
    bx = b.m_x;
    ax = a.m_x;
    by = b.m_y;
    ay = a.m_y;
    asm { ps_sub t0, bx, ax }
    asm { ps_sub t1, bx, ax }
    asm { ps_sub t2, by, ay }
    asm { ps_sel t0, t0, ax, bx }
    asm { ps_sub t3, by, ay }
    asm { ps_sel t2, t2, ay, by }
    asm { ps_sel t1, t1, bx, ax }
    asm { ps_sel t3, t3, by, ay }
    r.l = t0;
    r.t = t2;
    r.r = t1;
    r.b = t3;
}

// TODO: clean up, give variables real names
extern "C" void fn_80086694(Outer* self, const Vec2f* pos, const float* radius) {
    self->buf->flag = 0;
    Vec2f b(pos->m_x + *radius, pos->m_y - *radius);
    Vec2f a(pos->m_x - *radius, pos->m_y + *radius);
    RectF r;
    makeRect(r, a, b);
    *(RectI*)&self->buf->rect = *(RectI*)&r;
    fn_8011728C(self->buf, 0, 0);
}
}

// TODO: clean up, give variables real names
namespace jj_800831AC {


struct Block48 {
    u32 w[12];
};

struct FTElement {
    u32 pad[13];
    nw4r::math::VEC3 pos;
    nw4r::math::VEC3 normal;
    u32 pad2[36];
    Block48 data;
};

extern "C" void fn_801EC868(void* pIn, void* pOut, f32 x, f32 y, f32 z);

// TODO: clean up, give variables real names
extern "C" void fn_800831AC(s32 unused, s32 i, s32 j, FTElement*** pArr) {
    s32 oj = j << 2;
    s32 oi = i << 2;
    nw4r::math::VEC3 posI, posJ;
    Block48 buf;

    s32 off;
    s32 k = 1;

    posI.x = (*pArr)[i]->pos.x;
    posI.y = (*pArr)[i]->pos.y;
    posI.z = (*pArr)[i]->pos.z;
    posJ.x = (*pArr)[j]->pos.x;
    posJ.y = (*pArr)[j]->pos.y;
    posJ.z = (*pArr)[j]->pos.z;

    nw4r::math::VEC3 diff;
    diff = posJ - posI;

    off = 4;
    do {
        FTElement* src = *reinterpret_cast<FTElement**>(
            reinterpret_cast<u8*>(*pArr) + oi + off);
        fn_801EC868(&src->data, &buf, diff.x, diff.y, diff.z);

        FTElement* dst = *reinterpret_cast<FTElement**>(
            reinterpret_cast<u8*>(*pArr) + oj + off);
        dst->data = buf;

        (*reinterpret_cast<FTElement**>(
            reinterpret_cast<u8*>(oj + reinterpret_cast<u32>(*pArr)) + off))->pos =
            (*reinterpret_cast<FTElement**>(
                reinterpret_cast<u8*>(*pArr) + oi + off))->pos + diff;
        (*reinterpret_cast<FTElement**>(
            reinterpret_cast<u8*>(oj + reinterpret_cast<u32>(*pArr)) + off))->normal =
            (*reinterpret_cast<FTElement**>(
                reinterpret_cast<u8*>(*pArr) + oi + off))->normal + diff;
        ++k;
        off += 4;
    } while (k < 3);
}
}

namespace jj_80088FB8 {
using namespace nw4r::math;
struct A {
    char pad00[0x34];
    VEC3 pos;
    VEC3 prev;
    char pad4c[0x30];
    float radius;
    char pad80[0x38];
    float weight;
};
struct B {
    char pad00[0x74];
    float scale;
    char pad78[4];
    float radius;
    char pad80[0xbc];
    MTX34 matrix;
    char pad16c[0x40];
    VEC3 offset;
    VEC3 pos;
};
extern "C" {
extern const float lbl_805A1BB8;
void fn_801EC158(MTX34*);
void fn_801EC868(const MTX34*, MTX34*, float, float, float);
void fn_801EC1B8(const MTX34*, const MTX34*, MTX34*);
float fn_801ED04C(const VEC3*);
void fn_801ED008(const VEC3*, VEC3*);
// TODO: clean up, give variables real names
int fn_80088FB8(void*, A* a, B* b) {
    int result = 0;
    if (lbl_805A1BB8 != a->weight && b != 0) {
        MTX34 identity, translation, combined;
        fn_801EC158(&identity);
        fn_801EC868(&identity, &translation, b->offset.x, b->offset.y, b->offset.z);
        fn_801EC1B8(&b->matrix, &translation, &combined);
        b->pos.x = combined._03;
        float r1 = b->radius;
        r1 *= b->scale;
        b->pos.y = combined._13;
        b->pos.z = combined._23;
        VEC3 delta = a->pos - a->prev;
        float length = fn_801ED04C(&delta);
        float r2 = a->radius;
        float sum = r1 + r2;
        int active = 1;
        int i = 0;
        while (active) {
            if (i > 3) break;
            VEC3 diff = a->pos - b->pos;
            if (fn_801ED04C(&diff) < sum) {
                fn_801ED008(&diff, &diff);
                a->pos = b->pos + diff * (r1 + r2);
                VEC3 move;
                VEC3Sub(&move, &a->pos, &a->prev);
                if (length < fn_801ED04C(&move)) {
                    fn_801ED008(&move, &move);
                    a->pos = a->prev + move * length;
                } else {
                    active = 0;
                }
                result = 1;
            } else {
                active = 0;
            }
            ++i;
        }
    }
    return result;
}
}
}

namespace jj_800835B4 {

using namespace nw4r::math;

struct Node;
struct Link { Node* parent; Node* child; };
struct Node {
    char pad0[0x34];
    VEC3 position;
    VEC3 previous;
    char pad4c[0x34];
    float length;
    char pad84[0x2c];
    float weight;
    char padb4[0x114];
    VEC3 velocity;
    float damping;
    VEC3 acceleration;
    float factor;
    char pad1e8[0x10];
    Link* parent;
};

extern "C" float fn_801ED04C(const VEC3*);
extern "C" void fn_801ED008(const VEC3*, VEC3*);
extern const float lbl_805A1BB8;
extern const float lbl_805A1BBC;
extern const float lbl_805A1BD0;
extern const float lbl_805A1BD4;
extern const float lbl_805A1BD8;
extern const float lbl_805A1BDC;
extern const float lbl_805A1BE0;
extern const float lbl_805A1BE4;
extern const float lbl_805A1BE8;

// TODO: clean up, give variables real names
extern "C" void fn_800835B4(void* self, Node* node, int arg2, int arg3)
{
    if (lbl_805A1BB8 != node->weight && node->parent != 0) {
        VEC3 previous = node->previous;
        VEC3 position = node->position;
        if (arg2 == 0) {
            if (arg3 != 0) {
                position += node->velocity;
                position += node->acceleration * node->factor;
                node->velocity += node->acceleration * lbl_805A1BD0;
                node->velocity *= node->damping;
            }
        } else {
            float scale = lbl_805A1BD4;
            float accelerationScale = lbl_805A1BD8;
            if (lbl_805A1BB8 != node->length) {
                if (node->length >= lbl_805A1BDC) {
                    scale *= mtSqrtf(lbl_805A1BBC + (node->length - lbl_805A1BDC) / lbl_805A1BE0);
                    if (scale > lbl_805A1BE4)
                        scale = lbl_805A1BE4;
                } else {
                    scale *= mtSqrtf(lbl_805A1BBC - (lbl_805A1BDC - node->length) / lbl_805A1BE8);
                }
            }
            position += node->velocity;
            position += node->acceleration * scale;
            node->velocity += node->acceleration * accelerationScale;
            node->velocity *= node->damping;
        }
        VEC3 direction = position - previous;
        if (lbl_805A1BB8 != fn_801ED04C(&direction))
            fn_801ED008(&direction, &direction);
        previous = position - direction * node->length;
        node->position.x = position.x;
        node->previous = previous;
        node->position.y = position.y;
        node->position.z = position.z;
    }
    if (node->parent != 0)
        fn_800835B4(self, node->parent->parent, arg2, arg3);
}
}

namespace jj_8008A750 {
using namespace nw4r::math;
struct Node;
struct Link { Node* parent; Node* child; };
struct Node {
    char pad00[0xDC];
    MTX34 matrix;
    MTX34 world;
    char pad13C[0xBC];
    Link* parent;
    Link** children;
    int childCount;
};
extern "C" void fn_801EC1B8(const MTX34*, const MTX34*, MTX34*);
// TODO: clean up, give variables real names
extern "C" void fn_8008A750(void* arg0, Node* node, MTX34 input) {
    int j, k;
    Node *child, *grand;
    int oi, oj, ok, i;
    if (node->parent)
        fn_801EC1B8(&input, &node->world, &node->matrix);
    else
        node->matrix = input;
    i = 0;
    oi = 0;
    for (; i < node->childCount; oi += 4, ++i) {
        Link* link = *(Link**)((char*)node->children + oi);
        if (link) {
            child = link->child;
            MTX34 m = node->matrix;
            if (child->parent)
                fn_801EC1B8(&m, &child->world, &child->matrix);
            else
                child->matrix = m;
            j = 0;
            oj = 0;
            for (; j < child->childCount; oj += 4, ++j) {
                Link* link2 = *(Link**)((char*)child->children + oj);
                if (link2) {
                    grand = link2->child;
                    MTX34 m2 = child->matrix;
                    if (grand->parent)
                        fn_801EC1B8(&m2, &grand->world, &grand->matrix);
                    else
                        grand->matrix = m2;
                    k = 0;
                    ok = 0;
                    for (; k < grand->childCount; ok += 4, ++k) {
                        Link* link3 = *(Link**)((char*)grand->children + ok);
                        if (link3) {
                            Node* next = link3->child;
                            fn_8008A750(arg0, next, grand->matrix);
                        }
                    }
                }
            }
        }
    }
}
}

namespace jj_8008A3CC {
using namespace nw4r::math;

struct Node;
struct Link { Node* parent; Node* child; };
struct Node {
    char pad00[0xDC];
    MTX34 matrix;
    MTX34 world;
    char pad13C[0xBC];
    Link* parent;
    Link** children;
    int childCount;
};

extern "C" {
void fn_801EC41C(const MTX34*, MTX34*);
void fn_801EC1B8(const MTX34*, const MTX34*, MTX34*);
}

// TODO: clean up, give variables real names
extern "C" void fn_8008A3CC(void* arg0, Node* node) {
    int j;
    Node* child;
    Node* grand;
    int offI;
    int offJ;
    int i;
    int k;
    int offK;
    if (node->parent != 0) {
        MTX34 parent = node->parent->parent->matrix;
        MTX34 inverse;
        fn_801EC41C(&parent, &inverse);
        MTX34 combined;
        fn_801EC1B8(&inverse, &node->matrix, &combined);
        node->world = combined;
    }
    i = 0;
    offI = 0;
    for (; i < node->childCount; offI += 4, ++i) {
        child = (*(Link**)((char*)node->children + offI))->child;
        if (child->parent != 0) {
            MTX34 combined;
            MTX34 inverse;
            MTX34 parent = child->parent->parent->matrix;
            fn_801EC41C(&parent, &inverse);
            fn_801EC1B8(&inverse, &child->matrix, &combined);
            child->world = combined;
        }
        j = 0;
        offJ = 0;
        for (; j < child->childCount; offJ += 4, ++j) {
            grand = (*(Link**)((char*)child->children + offJ))->child;
            if (grand->parent != 0) {
                MTX34 combined;
                MTX34 inverse;
                MTX34 parent = grand->parent->parent->matrix;
                fn_801EC41C(&parent, &inverse);
                fn_801EC1B8(&inverse, &grand->matrix, &combined);
                grand->world = combined;
            }
            k = 0;
            offK = 0;
            for (; k < grand->childCount; offK += 4, ++k) {
                fn_8008A3CC(arg0, (*(Link**)((char*)grand->children + offK))->child);
            }
        }
    }
}
}

// TODO: clean up, give variables real names
namespace jj_80084524 {
using namespace nw4r::math;

struct Node;
struct Link { Node* parent; Node* child; };
struct Node {
    char pad00[0x80];
    float length;
    char pad84[0x2C];
    float weight;
    char padB4[0x28];
    MTX34 matrix;
    char pad10C[0xB8];
    unsigned char flag;
    char pad1C5[0x33];
    Link* parent;
    Link** children;
    int childCount;
};
struct Context {
    char pad00[0x3A];
    unsigned char flag;
    unsigned char flip;
};
extern "C" {
void fn_801EC158(MTX34*);
void fn_801ED0B0(const VEC3*, const VEC3*, VEC3*);
void fn_80162974(MTX34*, const VEC3*, float);
void fn_801629AC(MTX34*, float, float, float);
void fn_801EC868(const MTX34*, MTX34*, float, float, float);
void fn_801EC1B8(const MTX34*, const MTX34*, MTX34*);
extern const float lbl_805A1BB8;
extern const float lbl_805A1BBC;
extern const float lbl_805A1BFC;
extern const float lbl_805A1C00;
extern const float lbl_805A1C04;
}
// TODO: clean up, give variables real names
extern "C" void fn_80084524(Context* self, Node* node) {
    if (lbl_805A1BB8 != node->weight || node->parent == 0) {
        MTX34 identity;
        fn_801EC158(&identity);
        MTX34 current = node->matrix;
        VEC3 direction(node->length, lbl_805A1BB8, lbl_805A1BB8);
        VEC3 up(lbl_805A1BB8, lbl_805A1BBC, lbl_805A1BB8);
        VEC3 axis;
        fn_801ED0B0(&up, &direction, &axis);
        MTX34 rotation;
        fn_80162974(&rotation, &axis, lbl_805A1BFC);
        MTX34 translation;
        fn_801EC868(&identity, &translation, direction.x, direction.y, direction.z);
        MTX34 combined;
        fn_801EC1B8(&translation, &rotation, &combined);
        if (node->flag && node->parent) {
            node->matrix = node->parent->parent->matrix;
        } else {
            fn_801EC1B8(&current, &combined, &current);
            node->matrix = current;
        }
    } else if (node->parent) {
        node->matrix = node->parent->parent->matrix;
    }
    if (self->flag) {
        if (self->flip) {
            MTX34 current;
            MTX34 rotation;
            fn_801629AC(&rotation, lbl_805A1BFC, lbl_805A1BB8, lbl_805A1BB8);
            current = node->matrix;
            fn_801EC1B8(&current, &rotation, &current);
            node->matrix = current;
        }
        if (node->parent == 0) {
            MTX34 current;
            MTX34 rotation;
            fn_801629AC(&rotation, lbl_805A1BB8, lbl_805A1BB8, lbl_805A1C00);
            current = node->matrix;
            fn_801EC1B8(&current, &rotation, &current);
            node->matrix = current;
        } else if (node->parent && node->parent->parent->parent == 0) {
            MTX34 current;
            MTX34 rotation;
            fn_801629AC(&rotation, lbl_805A1BB8, lbl_805A1BB8, lbl_805A1C04);
            current = node->matrix;
            fn_801EC1B8(&current, &rotation, &current);
            node->matrix = current;
        }
    }
    int offset;
    int i;
    i = 0;
    offset = 0;
    for (; i < node->childCount; offset += 4, ++i) {
        Link* link = *(Link**)((char*)node->children + offset);
        if (link)
            fn_80084524(self, link->child);
    }
}
}

namespace jj_80084AD8 {
using namespace nw4r::math;

struct Node;
struct Link { Node* parent; Node* child; };
struct Node {
    int kind;
    char pad04[0x24];
    VEC3 initial;
    VEC3 position;
    VEC3 anchor;
    VEC3 previous;
    VEC3 oldAnchor;
    char pad64[0x1c];
    float length;
    char pad84[0x2c];
    float weight;
    char padB4[0x28];
    MTX34 matrix;
    char pad10C[0xec];
    Link* parent;
    Link** children;
    int childCount;
};
struct Context {
    char pad00[0x3c];
    unsigned char flag;
    char pad3D[3];
    int mode;
    float pad44;
    float scale;
};

extern "C" {
void fn_801EC158(MTX34*);
void fn_801EC41C(const MTX34*, MTX34*);
void fn_801EC1B8(const MTX34*, const MTX34*, MTX34*);
void fn_8009BD44(const MTX34*, MTX34*, MTX34*);
float fn_801ED04C(const VEC3*);
void fn_801ED008(const VEC3*, VEC3*);
void fn_800893F0(Context*, Node*);
int fn_800867E8(Context*, Node*, unsigned char, VEC2*);
int fn_800879FC(Context*, Node*, unsigned int, int);
void fn_80081DC4(Node*);
extern const float lbl_805A1BB8;
extern const float lbl_805A1BBC;
extern const float lbl_805A1C08;
extern const float lbl_805A1C0C;
}

// TODO: clean up, give variables real names
extern "C" int fn_80084AD8(Context* self, Node* node, int unused,
    float* scale, void* arg4, int useInitial, unsigned int mode,
    unsigned char depth, unsigned char a, unsigned char b, unsigned char c) {
    int offset;
    unsigned int nextDepth;
    int result = 0;
    int hit = 0;
    int moved = 0;
    MTX34 identity;
    fn_801EC158(&identity);
    VEC2 contact;
    if (lbl_805A1BB8 != node->weight && node->parent != 0 &&
        (mode == 0 || depth == 0)) {
        MTX34 parent = node->parent->parent->matrix;
        if (node->kind == 0) {
            VEC3 target;
            if (lbl_805A1BB8 != node->parent->parent->weight)
                target = node->parent->parent->position;
            else if (useInitial)
                target = node->initial;
            else
                target = node->anchor;
            VEC3 delta = target - node->anchor;
            node->anchor += delta;
            if (mode != 3)
                fn_800893F0(self, node);
            VEC3 acceleration;
            if (mode == 1 || mode == 3)
                acceleration = VEC3(lbl_805A1BB8, lbl_805A1BB8, lbl_805A1BB8);
            else
                acceleration = VEC3(lbl_805A1BB8, lbl_805A1C08, lbl_805A1BB8);
            node->position += acceleration * node->weight * *scale * self->scale;
            VEC3 direction = node->position - node->anchor;
            if (lbl_805A1BB8 != fn_801ED04C(&direction))
                fn_801ED008(&direction, &direction);
            node->position = node->anchor + direction * node->length;
            if ((!self->flag || !self->mode) &&
                (node->childCount == 0 ||
                 (node->childCount != 0 && node->children[0]->child->kind != 0))) {
                if (direction.x >= lbl_805A1BB8) {
                    node->position.x += lbl_805A1C0C;
                    node->anchor.x -= lbl_805A1BBC;
                    node->oldAnchor.x -= lbl_805A1BBC;
                } else {
                    node->position.x -= lbl_805A1C0C;
                    node->anchor.x += lbl_805A1BBC;
                    node->oldAnchor.x += lbl_805A1BBC;
                }
                hit = fn_800867E8(self, node, a, &contact);
                if (direction.x >= lbl_805A1BB8) {
                    node->position.x -= lbl_805A1C0C;
                    node->anchor.x += lbl_805A1BBC;
                    node->oldAnchor.x += lbl_805A1BBC;
                } else {
                    node->position.x += lbl_805A1C0C;
                    node->anchor.x -= lbl_805A1BBC;
                    node->oldAnchor.x -= lbl_805A1BBC;
                }
            }
            if (mode != 3) {
                moved = fn_800879FC(self, node, mode, 0);
                if (mode != 0 && hit != 0)
                    moved = 1;
            }
            MTX34 inverse;
            fn_801EC41C(&parent, &inverse);
            MTX34 base = identity;
            if (mode != 3) {
                MTX34 combined;
                MTX34 current = node->matrix;
                fn_801EC1B8(&inverse, &current, &combined);
                MTX34 local, rotation, scaling;
                fn_8009BD44(&combined, &rotation, &scaling);
                fn_801EC1B8(&rotation, &base, &local);
                fn_801EC1B8(&parent, &local, &node->matrix);
            }
            fn_80081DC4(node);
        }
    }
    if (hit == 0)
        node->previous = node->position;
    else {
        node->previous.x = contact.x;
        node->previous.y = contact.y;
    }
    node->oldAnchor = node->anchor;
    nextDepth = depth - 1;
    int i = 0;
    offset = 0;
    for (; i < node->childCount; offset += 4, ++i) {
        if (*(int*)((char*)node->children + offset) != 0) {
            if (mode == 1 && depth >= 1)
                result = fn_80084AD8(self, (*(Link**)((char*)node->children + offset))->child, 0, scale,
                    arg4, useInitial, mode, (unsigned char)nextDepth, a, b, c);
            else
                result = fn_80084AD8(self, (*(Link**)((char*)node->children + offset))->child, 0, scale,
                    arg4, useInitial, mode, 0, a, b, c);
        }
    }
    if (result != 0) return result;
    if (hit != 0) return 2;
    if (moved != 0 && (node->childCount == 0 ||
        (node->childCount != 0 && node->children[0]->child->kind != 0)))
        return 1;
    return 0;
}
}

namespace jj_80086AC0 {


extern "C" {
extern const float lbl_805A1BB8;
extern const float lbl_805A1BBC;
extern const float lbl_805A1BE4;
extern const float lbl_805A1C20;

float mtSqrtf__Ff(float);
float rsqrtf__Ff(float);
void fn_8003DBA8(Vec2f*);
int fn_800409D4(void*, void*);
void fn_80111968(void*, void*, void*, int);
}

struct CollisionEntry {
    void* object;
    char padding[4];
    void* owner;
    char padding2[4];
};

struct CollisionList {
    char padding[0x1A];
    unsigned short count;
    CollisionEntry* entries;
};

struct CollisionOwner {
    char padding[0x10];
    CollisionList* list;
};

struct Segment2 {
    Vec2f a;
    Vec2f b;
    int flags;
};

static inline bool finiteComponent(float value) {
    bool result = true;
    if ((unsigned char)(*(unsigned int*)&value >> 23) == 0xFF)
        result = false;
    return result;
}

static inline bool finiteVector(const Vec2f& value) {
    return finiteComponent(value.m_x) && finiteComponent(value.m_y);
}

// TODO: clean up, replace the raw offset accesses with a struct with named fields and give variables real names
extern "C" int fn_80086AC0(CollisionOwner* self, unsigned char* arg1, int arg2) {
    Vec2f start(
        *(float*)(arg1 + 0x40),
        lbl_805A1BBC + *(float*)(arg1 + 0x44));
    Vec2f end(
        *(float*)(arg1 + 0x34),
        *(float*)(arg1 + 0x38));
    Vec2f previous(
        *(float*)(arg1 + 0x58),
        lbl_805A1BBC + *(float*)(arg1 + 0x5C));

    int result = 0;
    if (!finiteVector(start))
        return 0;
    if (!finiteVector(end))
        return 0;

    Vec2f movement;
    Vec2f::copy(movement, end - start);
    float radius = mtSqrtf__Ff(
        movement.m_x * movement.m_x + movement.m_y * movement.m_y);

    Segment2 movementSegment;
    movementSegment.a = start;
    movementSegment.b = end;
    movementSegment.flags = 0;

    CollisionEntry* entry;
    int offset;
    unsigned int i;
    i = 0;
    offset = 0;
    while (i < self->list->count) {
        entry = (CollisionEntry*)((char*)self->list->entries + offset);
        Segment2 collision;
        fn_80111968(entry->owner, &collision, entry->object, 0);

        if (fn_800409D4(&collision, &movementSegment)) {
            if (arg2 == 0) {
                unsigned char flags = *((unsigned char*)entry->object + 0xF);
                if (((flags >> 2) & 1) || ((flags >> 3) & 1)) {
                    result = 0;
                    goto next;
                }
            }

            Vec2f left;
            Vec2f right;
            Vec2f::copy(left, collision.a);
            Vec2f::copy(right, collision.b);
            if (collision.a.m_x > collision.b.m_x) {
                left = collision.b;
                right = collision.a;
            }

            Vec2f direction;
            Vec2f relative;
            Vec2f::copy(direction, right - left);
            Vec2f::copy(relative, start - left);
            float absolute;
            float dot = direction.m_x * relative.m_x +
                        direction.m_y * relative.m_y;
            float lengthSquared = direction.m_x * direction.m_x + direction.m_y * direction.m_y;
            absolute = (float)(double)nw4r::ut::Abs(lengthSquared);
            float length = absolute <= lbl_805A1C20
                ? lbl_805A1BB8
                : lengthSquared * rsqrtf__Ff(lengthSquared);

            fn_8003DBA8(&direction);
            float projection = dot / length;
            Vec2f projected;
            Vec2f perpendicular;
            Vec2f::copy(projected, left + direction * projection);
            Vec2f::copy(perpendicular, start - projected);
            float perpendicularSquared = perpendicular.lengthSq();
            float perpendicularLength =
                (float)(double)nw4r::ut::Abs(perpendicularSquared) <= lbl_805A1C20
                ? lbl_805A1BB8
                : perpendicularSquared * rsqrtf__Ff(perpendicularSquared);

            radius = mtSqrtf__Ff((float)(double)nw4r::ut::Abs(
                radius * radius - perpendicularLength * perpendicularLength));
            fn_8003DBA8(&relative);
            fn_8003DBA8(&movement);

            float distance;
            if (end.m_x - previous.m_x > lbl_805A1BB8)
                distance = projection + radius;
            else
                distance = projection - radius;

            Vec2f intersection;
            Vec2f::copy(intersection, left + direction * distance);
            float lower;
            float upper;
            if (left.m_x < right.m_x) {
                lower = left.m_x;
                upper = right.m_x;
            } else {
                lower = right.m_x;
                upper = left.m_x;
            }

            if (intersection.m_x > lower && intersection.m_x < upper) {
                *(float*)(arg1 + 0x34) = intersection.m_x;
                *(float*)(arg1 + 0x38) = intersection.m_y;
            } else {
                Vec2f fromLeft;
                Vec2f::copy(fromLeft, intersection - left);
                float leftSquared = fromLeft.lengthSq();
                float leftDistance =
                    (float)(double)nw4r::ut::Abs(leftSquared) <= lbl_805A1C20
                    ? lbl_805A1BB8
                    : leftSquared * rsqrtf__Ff(leftSquared);

                Vec2f fromRight;
                Vec2f::copy(fromRight, intersection - right);
                float rightSquared = fromRight.lengthSq();
                float rightDistance =
                    (float)(double)nw4r::ut::Abs(rightSquared) <= lbl_805A1C20
                    ? lbl_805A1BB8
                    : rightSquared * rsqrtf__Ff(rightSquared);

                Vec2f adjustment;
                Vec2f origin(
                    *(float*)(arg1 + 0x40),
                    *(float*)(arg1 + 0x44));
                if (leftDistance < rightDistance) {
                    Vec2f along;
                    Vec2f::copy(along, right - left);
                    fn_8003DBA8(&along);
                    Vec2f point;
                    Vec2f::copy(point, left + along * lbl_805A1BE4);
                    adjustment = point - origin;
                } else {
                    Vec2f along;
                    Vec2f::copy(along, left - right);
                    fn_8003DBA8(&along);
                    Vec2f point;
                    Vec2f::copy(point, right + along * lbl_805A1BE4);
                    adjustment = point - origin;
                }

                fn_8003DBA8(&adjustment);
                Vec2f answer;
                Vec2f::copy(answer, origin + adjustment * *(float*)(arg1 + 0x80));
                *(float*)(arg1 + 0x34) = answer.m_x;
                *(float*)(arg1 + 0x38) = answer.m_y;
            }

            result = 1;
            break;
        }

    next:
        offset += 0x10;
        ++i;
    }

    return result;
}
}

namespace jj_8008270C {

struct Vec3 {
    float x, y, z;
    Vec3() {}
    Vec3(const Vec3& other) : x(other.x), y(other.y), z(other.z) {}
};

struct Matrix34 {
    float m[3][4];
};

extern "C" void fn_8003E6DC(const Matrix34*, Vec3*);

// TODO: clean up, give variables real names
static inline Vec3 getScale(const Matrix34* m)
{
    Vec3 v;
    fn_8003E6DC(m, &v);
    return v;
}

struct Entry {
    char pad00[0x70];
    Vec3 scale;
    char pad7C[0x34];
    float value;
    char padB4[0x28];
    Matrix34 matrix;
    char pad10C[0xE4];
    int kind;
    char pad1F4[4];
    int state;
    char pad1FC[4];
    int disabled;
};

struct EntryList {
    Entry** entries;
    int count;
    Entry*& at(int i) { return entries[i]; }
};

struct Group {
    unsigned char enabled;
    char pad01[3];
    EntryList* list;
    int id;
};

struct Manager {
    Group* groups;
    int count;
    char pad08[0x18];
    void* buffer20;
    int size24;
    unsigned int flags28;
    void* buffer2C;
    int size30;
    unsigned int flags34;
    unsigned char flag38;
    unsigned char flag39;
    unsigned char flag3A;
    unsigned char flag3B;
    unsigned char slow;
    unsigned char flag3D;
    char pad3E[2];
    int slowMode;
    float frameFraction;
    float rate;
    int divisor;
    unsigned int raw34() const { return flags34; }
    unsigned int raw28() const { return flags28; }
};

extern "C" {
unsigned int fn_8004E96C(void*);
unsigned int fn_8004E958(void*);
void fn_8003E388(const Matrix34*, Matrix34*);
void fn_80089224(Manager*);
void fn_800835B4(Manager*, Entry*, unsigned char, unsigned char);
int fn_800838DC(Manager*, Entry*);
void fn_80084524(Manager*, Entry*);
int fn_80084AD8(Manager*, Entry*, Entry*, int, int, unsigned char, int, int,
                unsigned char, unsigned char, unsigned char);
int fn_800851C8(Manager*, Entry*, Entry*, int, int, unsigned char, int, int,
                unsigned char, unsigned char, unsigned char, unsigned char);
void fn_80082FDC(Manager*, int, int, EntryList*);
void fn_800831AC(Manager*, int, int, EntryList*);
void fn_8027EABC(void*, void*, unsigned int, int);
extern void* lbl_805A0EC8;
}

extern const float lbl_805A1BB8;
extern const float lbl_805A1BBC;
extern const double lbl_805A1BC0;
extern const double lbl_805A1BC8;

static int findGroup(Manager* self, int id)
{
    for (int i = 0; i < self->count; ++i) {
        if (self->groups[i].id == id)
            return i;
    }
    return -1;
}

// TODO: clean up, give variables real names
extern "C" int fn_8008270C(
    Manager* self, int id, int* index, int arg3, int arg4, unsigned char arg5,
    int arg6, int arg7, unsigned char arg8, unsigned char arg9,
    unsigned char arg10, unsigned char arg11, unsigned char otherGroup,
    int otherId, unsigned char arg14, unsigned char arg15)
{
    int groupIndex = findGroup(self, id);

    if (otherGroup && otherId != -1)
        otherId = findGroup(self, otherId);

    if (gfSlowManager::getSlowRate() != 1) {
        self->slow = 1;
        self->slowMode = 1;

        if (fn_8004E96C(g_GameGlobal) == 1)
            self->slowMode = 1;
        else if (fn_8004E958(g_GameGlobal) == 1)
            self->slowMode = 0;
        else
            self->slowMode = 2;

        self->rate = lbl_805A1BBC / (float)gfSlowManager::getSlowRate();
        float frame = g_GameGlobal->getGameFrame();
        self->frameFraction = frame - (float)(int)frame;
    } else {
        self->frameFraction = lbl_805A1BB8;
        self->rate = lbl_805A1BBC;
        self->slow = 0;
        self->flag3D = 0;
    }

    if (self->divisor > 1)
        self->rate /= (float)self->divisor;

    int result = 0;
    if (groupIndex != -1 && self->groups[groupIndex].enabled) {
        fn_80089224(self);

        Group* group = self->groups + groupIndex;
        EntryList* list = group->list;
        EntryList* other;
        if (otherGroup)
            other = self->groups[otherId].list;

        for (int i = list->count - 1; i >= 0; --i) {
            if (list->at(i)->disabled == 0 &&
                (lbl_805A1BB8 != list->at(i)->value || list->at(i)->state != 0) &&
                (arg6 != 0 || *index != -1 || !arg11 || self->flag39)) {
                fn_800835B4(self, list->at(i), arg11, arg14);
            }
        }

        for (int i = 0; i < list->count; ++i) {
            if (lbl_805A1BB8 == list->at(i)->value && list->at(i)->state == 0) {
                int kind = list->at(i)->kind;
                if (kind == 0 || kind % 2 != 0) {
                    if (arg6 != 0) {
                        result = fn_80084AD8(
                            self, list->at(i), 0, arg3, arg4, arg5,
                            arg6, arg7, arg8, arg9, arg10);
                    } else if (*index == -1) {
                        if (!arg11) {
                            result = fn_800851C8(
                                self, list->at(i), 0, arg3, arg4, arg5,
                                arg6, arg7, arg8, arg9, arg10, arg15);
                        } else {
                            if (kind == 101) {
                                self->flag3A = 1;
                                self->flag3B = arg5;
                            } else {
                                self->flag3A = 0;
                            }

                            self->flag38 = 1;
                            result = fn_800838DC(self, list->at(i));
                            fn_80084524(self, list->at(i));
                            self->flag39 = 1;
                        }
                    } else if (!otherGroup) {
                        Matrix34 matrix;
                        Vec3 scale;

                        fn_8003E388(&list->at(*index)->matrix, &matrix);
                        scale = getScale(&matrix);
                        int idx = *index;
                        list->at(idx)->scale = scale;

                        result = fn_800851C8(
                            self, list->at(i), list->at(idx),
                            arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10,
                            arg15);
                    } else {
                        Matrix34 matrix;
                        Vec3 scale;

                        fn_8003E388(&other->at(0)->matrix, &matrix);
                        scale = getScale(&matrix);
                        other->at(0)->scale = scale;

                        result = fn_800851C8(
                            self, list->at(i), other->at(0), arg3,
                            arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg15);
                    }
                }
            }
        }

        for (int i = 0; i < list->count; ++i) {
            int kind = list->entries[i]->kind;

            if (kind == 1) {
                for (int j = 0; j < list->count; ++j) {
                    if (list->entries[j]->kind == 2) {
                        fn_80082FDC(self, i, j, list);
                        break;
                    }
                }
            } else if (kind == 3) {
                for (int j = 0; j < list->count; ++j) {
                    if (list->entries[j]->kind == 4) {
                        fn_80082FDC(self, i, j, list);
                        break;
                    }
                }
            } else if (kind == 5) {
                for (int j = 0; j < list->count; ++j) {
                    if (list->entries[j]->kind == 6) {
                        fn_80082FDC(self, i, j, list);
                        break;
                    }
                }
            } else if (kind == 7) {
                for (int j = 0; j < list->count; ++j) {
                    if (list->entries[j]->kind == 8) {
                        fn_80082FDC(self, i, j, list);
                        break;
                    }
                }
            }
        }

        for (int i = 0; i < list->count; ++i) {
            int kind = list->entries[i]->kind;

            if (kind == 9) {
                for (int j = 0; j < list->count; ++j) {
                    if (list->entries[j]->kind == 10) {
                        fn_800831AC(self, i, j, list);
                        break;
                    }
                }
            } else if (kind == 11) {
                for (int j = 0; j < list->count; ++j) {
                    if (list->entries[j]->kind == 12) {
                        fn_800831AC(self, i, j, list);
                        break;
                    }
                }
            } else if (kind == 13) {
                for (int j = 0; j < list->count; ++j) {
                    if (list->entries[j]->kind == 14) {
                        fn_800831AC(self, i, j, list);
                        break;
                    }
                }
            } else if (kind == 15) {
                for (int j = 0; j < list->count; ++j) {
                    if (list->entries[j]->kind == 16) {
                        fn_800831AC(self, i, j, list);
                        break;
                    }
                }
            }
        }
    }

    if (!(self->flags34 & 0x80000000))
        fn_8027EABC(lbl_805A0EC8, self->buffer2C, self->raw34() * 4, 0x15);

    self->buffer2C = 0;
    self->size30 = 0;
    self->flags34 = (self->flags34 & 0x40000000) | 0x80000000;

    if (!(self->flags28 & 0x80000000))
        fn_8027EABC(lbl_805A0EC8, self->buffer20, self->raw28() * 4, 0x15);

    self->buffer20 = 0;
    self->size24 = 0;
    self->flags28 = (self->flags28 & 0x40000000) | 0x80000000;

    return result;
}
}

namespace jj_800879FC {

struct GroundData { u8 pad[0x54]; u8 flags; };
struct LineData {
    u8 pad[0xf];
    u8 high : 3;
    u8 target : 1;
    u8 right : 1;
    u8 left : 1;
    u8 upper : 1;
    u8 under : 1;
};
struct Entry {
    LineData* line;
    GroundData* ground;
    void* collision;
    u8 byte;
    u8 flag : 1;
    u8 rest : 7;
    bool valid() const { return line && ground && collision; }
    int enabled() const {
        bool ok = valid();
        if (ok && ((ground->flags >> 4) & 1) &&
            ((*(u8*)((char*)collision + 0x2c) >> 7) & 1)) return 1;
        return 0;
    }
    int pairEnabled() const {
        bool ok = false;
        bool pair = line && ground;
        if (pair && collision) ok = true;
        if (ok && ((ground->flags >> 4) & 1) &&
            ((*(u8*)((char*)collision + 0x2c) >> 7) & 1)) return 1;
        return 0;
    }
};
struct List { u8 pad[0x1a]; u16 count; Entry* entries; };
struct Self { u8 pad[0x10]; List* list; };
struct Segment : clSegment2D { int extra; };
struct Hit { u8 failed; Vec2f p; u8 pValid; Vec2f q; u8 qValid; };

extern "C" {
void fn_8003DBA8(Vec2f*);
int fn_800409D4(Segment*, Segment*);
int fn_80041218(clCircle2D*, Segment*);
void fn_8008734C(Hit*, Self*, Segment*, Segment*);
void fn_800876BC(Hit*, Self*, Segment*, Segment*);
void fn_8008785C(Hit*, Self*, Segment*, Segment*);
void fn_80111968(void*, Segment*, void*, int);
void fn_80113BD0(Entry*, Vec2f*);
void fn_80113BF8(Entry*, Vec2f*);
void fn_80113DF8(Entry*, Entry*);
void fn_80113EC0(Entry*, Entry*);
Vec2f fn_80114CBC(Entry*, Vec2f*);
Vec2f fn_80114E9C(Entry*);
float rsqrtf__Ff(float);
extern const float lbl_805A1BB8;
extern const float lbl_805A1BD8;
extern const float lbl_805A1C20;
extern const float lbl_805A1C24;
extern const float lbl_805A1C5C;
}

// TODO: clean up, give variables real names
static inline float norm(Vec2f& v) {
    float d = v.lengthSq();
    return (float)(double)nw4r::ut::Abs(d) <= lbl_805A1C20
        ? lbl_805A1BB8 : d * rsqrtf__Ff(d);
}
static inline void add(Vec2f& a, const Vec2f& b) {
    a.m_x = a.m_x + b.m_x;
    a.m_y = a.m_y + b.m_y;
}
static inline void setCircle(clCircle2D& out, float x, float y, float radius) {
    out.m_centerPos.m_x = x;
    out.m_centerPos.m_y = y;
    out.m_radius = radius;
}
static inline void setVec(Vec2f& out, float x, float y) {
    out.m_x = x;
    out.m_y = y;
}
#define F(p,o) (*(float*)((char*)(p)+(o)))
#define B(p,o) (*(u8*)((char*)(p)+(o)))
#define SOLVE(h,s) \
    if ((float)(double)nw4r::ut::Abs((s).m_p0Pos.m_x - (s).m_p1Pos.m_x) < lbl_805A1C24) \
        fn_800876BC(&(h), self, &(s), &movement); \
    else if ((float)(double)nw4r::ut::Abs((s).m_p0Pos.m_y - (s).m_p1Pos.m_y) < lbl_805A1C24) \
        fn_8008785C(&(h), self, &(s), &movement); \
    else fn_8008734C(&(h), self, &(s), &movement);

// TODO: clean up, replace the raw offset accesses with a struct with named fields and give variables real names
extern "C" int fn_800879FC(Self* self, char* arg, int unused, int ignore) {
    Segment movement;
    Segment segment;
    Hit initial;
    Segment segmentA;
    Segment segmentB;
    Hit hitA;
    Hit hitB;
    Hit hitC;
    Hit hitD;
    Entry neighborA;
    Entry neighborB;
    clCircle2D circleA;
    clCircle2D circleB;
    Vec2f end;
    Vec2f start;
    Vec2f forward;
    Vec2f backward;
    Vec2f result;
    Vec2f pointA;
    Vec2f pointB;
    Vec2f normal;
    Vec2f endpointA;
    Vec2f endpointB;
    Vec2f edgeNormal;
    Vec2f edgeA;
    Vec2f edgeB;
    Vec2f neighborPointA;
    Vec2f towardA;
    Vec2f normalA;
    Vec2f neighborPointB;
    Vec2f towardB;
    Vec2f normalB;
    Vec2f alongA;
    Vec2f alongB;
    Vec2f extendedA;
    Vec2f extendedB;
    Vec2f distAA;
    Vec2f distAB;
    Vec2f otherNormalA;
    Vec2f deltaA;
    Vec2f otherNormalB;
    Vec2f deltaB;
    Vec2f distBA;
    Vec2f distBB;
    Vec2f otherNormalC;
    Vec2f deltaC;
    Vec2f otherNormalD;
    Vec2f deltaD;
    Vec2f previous;
    Vec2f finalA;
    Vec2f finalB;
    end = *(Vec2f*)(arg+0x34);
    start = *(Vec2f*)(arg+0x40);
    Entry* e;
    int offset;
    int found = 0;
    Vec2f::copy(forward, end - start);
    Vec2f::copy(backward, start - end);
    fn_8003DBA8(&forward);
    fn_8003DBA8(&backward);
    add(start, backward * lbl_805A1BD8);
    add(end, forward * lbl_805A1BD8);
    movement.m_p0Pos = end;
    movement.m_p1Pos = start;
    movement.extra = 0;
    pointA.m_x = lbl_805A1BB8;
    pointA.m_y = lbl_805A1BB8;
    pointB.m_x = lbl_805A1BB8;
    pointB.m_y = lbl_805A1BB8;
    int blockedA = 0;
    int blockedB = 0;
    unsigned int i = 0;
    offset = 0;
    while (i < self->list->count) {
        e = (Entry*)((char*)self->list->entries + offset);
        if (!e->line->target) goto next;
        if (!((e->ground->flags >> 6) & 1)) goto next;
        fn_80111968(e->collision, &segment, e->line, 0);
        if (e->line->left || e->line->right) {
            if (B(arg,0x1f4) == 0) { found = 0; goto next; }
        }
        if (ignore) {
            if (e->line->under) goto next;
            if (e->line->upper) goto next;
        }
        Vec2f::copy(normal, fn_80114E9C(e));
        fn_8003DBA8(&normal);
        if (forward.m_x * normal.m_x + forward.m_y * normal.m_y > lbl_805A1BB8) goto next;
        if (!fn_800409D4(&segment, &movement)) goto next;
        found = 1;
        result = end;
        SOLVE(initial,segment)
        if (!initial.failed) { pointA = initial.p; pointB = initial.q; }
        endpointA.m_x = lbl_805A1BB8;
        endpointA.m_y = lbl_805A1BB8;
        endpointB.m_x = lbl_805A1BB8;
        endpointB.m_y = lbl_805A1BB8;
        neighborA.line = 0; neighborA.ground = 0; neighborA.collision = 0; neighborA.byte = 0; neighborA.flag = 0;
        neighborB.line = 0; neighborB.ground = 0; neighborB.collision = 0; neighborB.byte = 0; neighborB.flag = 0;
        fn_80113EC0(e,&neighborA);
        fn_80113DF8(e,&neighborB);
        if (e->enabled()) {
            Vec2f::copy(edgeNormal,fn_80114E9C(e));
            fn_8003DBA8(&edgeNormal);
            fn_80113BD0(e,&edgeA);
            fn_80113BF8(e,&edgeB);
            if (neighborA.pairEnabled()) {
                fn_80111968(neighborA.collision,&segmentA,neighborA.line,0);
                fn_80113BD0(&neighborA,&neighborPointA);
                endpointA = fn_80114CBC(e,&neighborPointA);
                Vec2f::copy(towardA,start-endpointA);
                fn_8003DBA8(&towardA);
                Vec2f::copy(normalA,fn_80114E9C(&neighborA));
                fn_8003DBA8(&normalA);
                float dotA = normalA.m_x*towardA.m_x + normalA.m_y*towardA.m_y;
                float dotB = edgeNormal.m_x*towardA.m_x + edgeNormal.m_y*towardA.m_y;
                blockedA = 0;
                if (dotA > lbl_805A1BB8 && dotB > lbl_805A1BB8) blockedA = 1;
            } else { endpointA = edgeA; endpointB = edgeB; }
            if (neighborB.pairEnabled()) {
                fn_80111968(neighborB.collision,&segmentB,neighborB.line,0);
                fn_80113BD0(&neighborB,&neighborPointB);
                endpointB = fn_80114CBC(e,&neighborPointB);
                Vec2f::copy(towardB,start-endpointB);
                fn_8003DBA8(&towardB);
                Vec2f::copy(normalB,fn_80114E9C(&neighborB));
                fn_8003DBA8(&normalB);
                float dotA = normalB.m_x*towardB.m_x + normalB.m_y*towardB.m_y;
                float dotB = edgeNormal.m_x*towardB.m_x + edgeNormal.m_y*towardB.m_y;
                blockedB = 0;
                if (dotA > lbl_805A1BB8 && dotB > lbl_805A1BB8) blockedB = 1;
            } else { endpointA = edgeA; endpointB = edgeB; }
        }
        Vec2f::copy(alongA,endpointA-start);
        Vec2f::copy(alongB,endpointB-start);
        fn_8003DBA8(&alongA);
        fn_8003DBA8(&alongB);
        Vec2f::copy(extendedA,start+alongA*F(arg,0x84));
        Vec2f::copy(extendedB,start+alongB*F(arg,0x84));
        setCircle(circleA,pointA.m_x,pointA.m_y,lbl_805A1C5C);
        setCircle(circleB,pointB.m_x,pointB.m_y,lbl_805A1C5C);
        if (!fn_80041218(&circleA,&segment)) {
            Vec2f::copy(distAA,endpointA-pointA);
            Vec2f::copy(distAB,endpointB-pointA);
            float lengthA = norm(distAA);
            float lengthB = norm(distAB);
            if (lengthA < lengthB) {
                if (!blockedA) { pointA = extendedA; goto second; }
                if (!neighborA.enabled()) goto second;
                Vec2f::copy(otherNormalA,fn_80114E9C(&neighborA));
                Vec2f::copy(deltaA,pointA-endpointA);
                if (otherNormalA.m_x*deltaA.m_x+otherNormalA.m_y*deltaA.m_y < lbl_805A1BB8) {
                    if (!blockedB) pointA = extendedB;
                    goto second;
                }
                if (!fn_800409D4(&segmentA,&movement)) goto second;
                SOLVE(hitA,segmentA)
                if (!hitA.failed) {
                    if (hitA.pValid && hitA.qValid) { pointA = hitA.q; goto second; }
                    if (hitA.pValid) { pointA = hitA.p; goto second; }
                    if (hitA.qValid) { pointA = hitA.q; goto second; }
                    pointA = extendedA;
                } else pointA = extendedA;
                goto second;
            } else {
                if (!blockedB) { pointA = extendedB; goto second; }
                if (!neighborB.enabled()) goto second;
                Vec2f::copy(otherNormalB,fn_80114E9C(&neighborB));
                Vec2f::copy(deltaB,pointA-endpointB);
                if (otherNormalB.m_x*deltaB.m_x+otherNormalB.m_y*deltaB.m_y < lbl_805A1BB8) {
                    if (!blockedA) pointA = extendedA;
                    goto second;
                }
                if (!fn_800409D4(&segmentB,&movement)) goto second;
                SOLVE(hitB,segmentB)
                if (!hitB.failed) {
                    if (hitB.pValid && hitB.qValid) { pointA = hitB.p; goto second; }
                    if (hitB.pValid) { pointA = hitB.p; goto second; }
                    if (hitB.qValid) { pointA = hitB.q; goto second; }
                    pointA = extendedB;
                } else pointA = extendedB;
            }
        }
second:
        if (!fn_80041218(&circleB,&segment)) {
            Vec2f::copy(distBA,endpointA-pointB);
            Vec2f::copy(distBB,endpointB-pointB);
            float lengthA = norm(distBA);
            float lengthB = norm(distBB);
            if (lengthA < lengthB) {
                if (!blockedA) { pointB = extendedA; goto adjust; }
                if (!neighborA.enabled()) goto adjust;
                Vec2f::copy(otherNormalC,fn_80114E9C(&neighborA));
                Vec2f::copy(deltaC,pointB-endpointA);
                if (otherNormalC.m_x*deltaC.m_x+otherNormalC.m_y*deltaC.m_y < lbl_805A1BB8) {
                    if (!blockedB) pointB = extendedB;
                    goto adjust;
                }
                if (!fn_800409D4(&segmentA,&movement)) goto adjust;
                SOLVE(hitC,segmentA)
                if (!hitC.failed) {
                    if (hitC.pValid && hitC.qValid) { pointB = hitC.q; goto adjust; }
                    if (hitC.pValid) { pointB = hitC.p; goto adjust; }
                    if (hitC.qValid) { pointB = hitC.q; goto adjust; }
                    pointB = extendedA;
                } else pointB = extendedA;
                goto adjust;
            } else {
                if (!blockedB) { pointB = extendedB; goto adjust; }
                if (!neighborB.enabled()) goto adjust;
                Vec2f::copy(otherNormalD,fn_80114E9C(&neighborB));
                Vec2f::copy(deltaD,pointB-endpointB);
                if (otherNormalD.m_x*deltaD.m_x+otherNormalD.m_y*deltaD.m_y < lbl_805A1BB8) {
                    if (!blockedA) pointB = extendedA;
                    goto adjust;
                }
                if (!fn_800409D4(&segmentB,&movement)) goto adjust;
                SOLVE(hitD,segmentB)
                if (!hitD.failed) {
                    if (hitD.pValid && hitD.qValid) { pointB = hitD.p; goto adjust; }
                    if (hitD.pValid) { pointB = hitD.p; goto adjust; }
                    if (hitD.qValid) { pointB = hitD.q; goto adjust; }
                    pointB = extendedB;
                } else pointB = extendedB;
            }
        }
adjust:
        add(pointA, normal*lbl_805A1BD8);
        add(pointB, normal*lbl_805A1BD8);
next:
        offset += 0x10;
        ++i;
    }
    if (found) {
        setVec(previous,F(arg,0x4c),F(arg,0x50));
        Vec2f::copy(finalA,pointA-previous);
        Vec2f::copy(finalB,pointB-previous);
        float a = norm(finalA);
        float b = norm(finalB);
        result.m_x = (a < b ? pointA : pointB).m_x;
        result.m_y = (a < b ? pointA : pointB).m_y;
        F(arg,0x34) = result.m_x;
        F(arg,0x38) = result.m_y;
    }
    return found;
}
}

