// Havok translation unit hkSimpleConstraintContactMgr.o (main.dol 0x802D8064-0x802D94A8).
// Functions in address order (names from the vtable slots; HYPOTHESIS where the slot meaning is inferred):
//   0x802D8064     8  getConstraintInstance (weak)
//   0x802D806C   220  __ct
//   0x802D8148   188  hkSimpleContactConstraintData::__dt (weak)
//   0x802D8204   264  __dt
//   0x802D830C    52  reserveContactPoints
//   0x802D8340   476  addToi (not written)
//   0x802D851C   192  removeToi (not written)
//   0x802D85DC   352  processToi (not written)
//   0x802D873C  1440  addContactPoint (not written)
//   0x802D8CDC   336  removeContactPoint (not written)
//   0x802D8E2C    32  getContactPoint
//   0x802D8E4C    44  getContactPointProperties
//   0x802D8E78   168  getAllContactPointIds
//   0x802D8F20    16  toiCollisionResponseBeginCallback (weak)
//   0x802D8F30    16  toiCollisionResponseEndCallback (weak)
//   0x802D8F40   880  processContact (not written)
//   0x802D92B0    28  Factory::__ct
//   0x802D92CC   132  Factory::createContactMgr
//   0x802D9350   212  calcStatistics
//   0x802D9424     8  getType (weak)
//   0x802D942C    32  cleanup (weak)
//   0x802D944C    92  Factory::__dt (weak)

#include <havok/hkSimpleConstraintContactMgr.h>
#include <havok/hkWorld.h>
#include <havok/hkRigidBody.h>
#include <havok/hkCollidable.h>
#include <havok/hkCollisionDispatcher.h>
#include <havok/hkWorldOperationUtil.h>

hkSimpleConstraintContactMgr::hkSimpleConstraintContactMgr(hkWorld* world, hkRigidBody* bodyA, hkRigidBody* bodyB)
    : m_contactConstraintData(&m_constraint), m_constraint(hkConstraintInstance::PRIORITY_PSI) {
    m_constraint.m_data = &m_contactConstraintData;
    m_constraint.m_entities[0] = bodyA;
    m_constraint.m_entities[1] = bodyB;
    m_world = world;
    m_skipNextNprocessCallbacks = 0;
    m_reservedContactPoints = 0;
    m_contactConstraintData.m_memSizeAndFlags = 0;
    m_constraint.m_memSizeAndFlags = 0;
    hkCollisionDispatcher* dispatcher = world->m_collisionDispatcher;
    int index = dispatcher->getCollisionQualityIndex(bodyA->m_collidable.m_broadPhaseHandle.m_objectQualityType,
                                                     bodyB->m_collidable.m_broadPhaseHandle.m_objectQualityType);
    m_constraint.setPriority(
        (hkConstraintInstance::ConstraintPriority)dispatcher->getCollisionQualityInfo(index)->unk38);
}

hkSimpleConstraintContactMgr::~hkSimpleConstraintContactMgr() {
    if (m_contactConstraintData.m_atom->m_numContactPoints != 0) {
        hkWorldOperationUtil::removeConstraintImmediately(m_world, &m_constraint, true);
    }
    m_constraint.m_entities[0] = 0;
    m_constraint.m_entities[1] = 0;
    m_constraint.m_data = 0;
}

hkResult hkSimpleConstraintContactMgr::reserveContactPoints(int numPoints) {
    int reserved = m_reservedContactPoints + numPoints;
    if (m_contactConstraintData.m_atom->m_numContactPoints + reserved > 0xFE) {
        return 1;
    }
    m_reservedContactPoints = (u8)reserved;
    return 0;
}

hkContactPoint* hkSimpleConstraintContactMgr::getContactPoint(hkContactPointId id) {
    return &m_contactConstraintData.m_atom->getContactPoints()[m_contactConstraintData.m_idMgrA.m_values[id]];
}

hkContactPointProperties* hkSimpleConstraintContactMgr::getContactPointProperties(hkContactPointId id) {
    return &m_contactConstraintData.m_atom->getContactPointProperties()[m_contactConstraintData.m_idMgrA.m_values[id]];
}

void hkSimpleConstraintContactMgr::getAllContactPointIds(hkArray<hkContactPointId>& contactPointIds) const {
    for (int i = 0; i < m_contactConstraintData.m_idMgrA.m_values.m_size; i++) {
        if (m_contactConstraintData.m_idMgrA.m_values[i] != 0xFF) {
            contactPointIds.pushBack(i);
        }
    }
}

hkSimpleConstraintContactMgr::Factory::Factory(hkWorld* world) {
    m_world = world;
}

hkContactMgr* hkSimpleConstraintContactMgr::Factory::createContactMgr(const hkCollidable& a, const hkCollidable& b,
                                                                      const hkCollisionInput& input) {
    hkRigidBody* bodyA = (hkRigidBody*)((const char*)&a + a.m_ownerOffset);
    hkRigidBody* bodyB = (hkRigidBody*)((const char*)&b + b.m_ownerOffset);
    return new hkSimpleConstraintContactMgr(m_world, bodyA, bodyB);
}

// Statistics collector interface as called here (slot offsets from the calls).
struct hkSimpleConstraintContactMgrStatisticsCollector {
    virtual void unk08();
    virtual void beginObject(const char* name, int type, const void* object);                    // 0x0C
    virtual void addArray(const char* name, int type, const void* data, int size, int capacity); // 0x10
    virtual void unk14();
    virtual void unk18();
    virtual void unk1C();
    virtual void endObject();                                                                     // 0x20
};

void hkSimpleConstraintContactMgr::calcStatistics(hkStatisticsCollector* collector) const {
    hkSimpleConstraintContactMgrStatisticsCollector* c = (hkSimpleConstraintContactMgrStatisticsCollector*)collector;
    c->beginObject(0, 8, this);
    c->addArray("ContactAtom", 8, m_contactConstraintData.m_atom, m_contactConstraintData.m_atom->m_sizeOfAllAtoms, 0);
    const char* idsName = "ContactIds";
    if (m_contactConstraintData.m_idMgrA.m_values.mustDeallocate()) {
        c->addArray(idsName, 8, m_contactConstraintData.m_idMgrA.m_values.m_data, m_contactConstraintData.m_idMgrA.m_values.m_size,
                    m_contactConstraintData.m_idMgrA.m_values.getCapacity());
    }
    c->endObject();
}

