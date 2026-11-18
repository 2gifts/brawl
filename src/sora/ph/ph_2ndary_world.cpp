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

