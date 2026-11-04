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
//   0x80082438   268  __ct   [map: ph2ndaryWorld____ct]
//   0x80082544   456  __dt   [map: ph2ndaryWorld____dt]
//   0x8008270C  2256  operationExecute   [map: ph2ndaryWorld__operationExecute]
//   0x80082FDC   464  sameZelda   [map: ph2ndaryWorld__sameZelda]
//   0x800831AC   484  samePeach   [map: ph2ndaryWorld__samePeach]
//   0x80083390   548  operationAwakeExecute   [map: ph2ndaryWorld__operationAwakeExecute]
//   0x800835B4   808  constraintInertiaChild   [map: ph2ndaryWorld__constraintInertiaChild]
//   0x800838DC  3144  operationGravityWeapon   [map: ph2ndaryWorld__operationGravityWeapon]
//   0x80084524  1460  operationGravityWeaponTrunModelMatrix   [map: ph2ndaryWorld__operationGravityWeaponTrunModelMatrix]
//   0x80084AD8  1776  operationWeapon   [map: ph2ndaryWorld__operationWeapon]
//   0x800851C8  4776  operationGravity   [map: ph2ndaryWorld__operationGravity]
//   0x80086470   204  getSamusRandomAccel   [map: ph2ndaryWorld__getSamusRandomAccel]
//   0x8008653C   212  addConstraintArray   [map: ph2ndaryWorld__addConstraintArray]
//   0x80086610   132  removeConstraintArray   [map: ph2ndaryWorld__removeConstraintArray]
//   0x80086694   184  updateCollision   [map: ph2ndaryWorld__updateCollision]
//   0x8008674C   156  updateCollision2Pos   [map: ph2ndaryWorld__updateCollision2Pos]
//   0x800867E8   728  checkCollisionHit   [map: ph2ndaryWorld__checkCollisionHit]
//   0x80086AC0  2188  chCollisionGround   [map: ph2ndaryWorld__chCollisionGround]
//   0x8008734C   880  correctCollisionSlope   [map: ph2ndaryWorld__correctCollisionSlope]
//   0x800876BC   416  correctCollisionVertical   [map: ph2ndaryWorld__correctCollisionVertical]
//   0x8008785C   416  correctCollisionLevel   [map: ph2ndaryWorld__correctCollisionLevel]
//   0x800879FC  5564  chCollisionGround2   [map: ph2ndaryWorld__chCollisionGround2]
//   0x80088FB8   620  chCollisionRigidBody   [map: ph2ndaryWorld__chCollisionRigidBody]
//   0x80089224   460  getWindArray   [map: ph2ndaryWorld__getWindArray]
//   0x800893F0   452  setNodeWindArray   [map: ph2ndaryWorld__setNodeWindArray]
//   0x800895B4  1784  getNodeWindArray   [map: ph2ndaryWorld__getNodeWindArray]
//   0x80089CAC  1824  operationExecuteSleep   [map: ph2ndaryWorld__operationExecuteSleep]
//   0x8008A3CC   900  operationExecuteSleepCreateLocalSpace   [map: ph2ndaryWorld__operationExecuteSleepCreateLocalSpace]
//   0x8008A750   876  operationExecuteSleepRestructuring   [map: ph2ndaryWorld__operationExecuteSleepRestructuring]
//   0x8008AABC   416  isCollisionBetweenPoints   [map: ph2ndaryWorld__isCollisionBetweenPoints]


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
