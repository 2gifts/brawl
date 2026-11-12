// Havok translation unit hkEntity.o (main.dol 0x802E0E40-0x802E1FCC).
// Functions in address order (method names from the Havok TU map):
//   0x802E0E40    52  finishLoadedObjecthkEntity   [map: hkEntity__finishLoadedObjecthkEntity]
//   0x802E0E74    20  cleanupLoadedObjecthkEntity   [map: hkEntity__cleanupLoadedObjecthkEntity]
//   0x802E0E88    72  getVtablehkEntity   [map: hkEntity__getVtablehkEntity]
//   0x802E0ED0   228  __ct   [map: hkEntity____ct]
//   0x802E0FB4   284  __dt   [map: hkWorldObject____dt]
//   0x802E10D0    80  __dt   [map: hkMotion____dt]
//   0x802E1120   100  __dt   [map: hkMaxSizeMotion____dt]
//   0x802E1184   440  __ct   [map: hkEntity____ct1]
//   0x802E133C    80  __dt   [map: hkSphereMotion____dt]
//   0x802E138C    80  __dt   [map: hkBoxMotion____dt]
//   0x802E13DC   380  calcStatistics   [map: hkEntity__calcStatistics]
//   0x802E1558   700  __dt   [map: hkEntity____dt]
//   0x802E1814   156  setDeactivator   [map: hkEntity__setDeactivator]
//   0x802E18B0   192  addEntityListener   [map: hkEntity__addEntityListener]
//   0x802E1970   192  addCollisionListener   [map: hkEntity__addCollisionListener]
//   0x802E1A30    36  isActive   [map: hkEntity__isActive]
//   0x802E1A54   128  activate   [map: hkEntity__activate]
//   0x802E1AD4    60  deactivate   [map: hkEntity__deactivate]
//   0x802E1B10   816  deallocateInternalArrays   [map: hkEntity__deallocateInternalArrays]
//   0x802E1E40    16  getNumConstraints   [map: hkEntity__getNumConstraints]
//   0x802E1E50    48  getConstraint   [map: hkEntity__getConstraint]
//   0x802E1E80     8  getMotionState   [map: hkEntity__getMotionState]
//   0x802E1E88    80  __dt   [map: hkStabilizedSphereMotion____dt]
//   0x802E1ED8    80  __dt   [map: hkStabilizedBoxMotion____dt]
//   0x802E1F28    80  __dt   [map: hkThinBoxMotion____dt]
//   0x802E1F78    84  __sinit_\hkEntity_cpp   [map: hkEntitycpp____sinit_]

#include <new>
#include <havok/hkEntity.h>
#include <havok/hkRegistry.h>
#include <havok/hkWorldOperationUtil.h>
#include <havok/hkSphereMotion.h>
#include <havok/hkStabilizedSphereMotion.h>
#include <havok/hkBoxMotion.h>
#include <havok/hkStabilizedBoxMotion.h>
#include <havok/hkFixedRigidMotion.h>
#include <havok/hkThinBoxMotion.h>
#include <havok/hkEntityCallbackUtil.h>
#include <havok/hkEntityDeactivator.h>


void hkEntity::finishLoadedObjecthkEntity(void* p) {
    hkFinishLoadedObjectFlag flag;
    flag.m_finishing = 1;
    new (p) hkEntity(flag);
}

void hkEntity::cleanupLoadedObjecthkEntity(void* p) {
    ((hkEntity*)p)->~hkEntity();
}

#pragma push
#pragma auto_inline off
const void* hkEntity::getVtablehkEntity() {
    hkFinishLoadedObjectFlag flag;
    flag.m_finishing = 0;
    hkVector4 buf[31]; // 0x1F0 bytes of 16-byte-aligned storage for the placed object
    new (buf) hkEntity(flag);
    return *(const void**)buf;
}
#pragma pop

// The motion is loaded as raw bytes: rebuild the object of its type in place so it gets its vtable back.
// Dynamic (type 1) motions are not rebuilt.
hkEntity::hkEntity(hkFinishLoadedObjectFlag flag) : hkWorldObject(flag), m_motion(flag) {
    if (flag.m_finishing) {
        switch (m_motion.m_type) {
        case hkMotion::MOTION_SPHERE_INERTIA:
            new (&m_motion) hkSphereMotion(flag);
            break;
        case hkMotion::MOTION_STABILIZED_SPHERE_INERTIA:
            new (&m_motion) hkStabilizedSphereMotion(flag);
            break;
        case hkMotion::MOTION_BOX_INERTIA:
            new (&m_motion) hkBoxMotion(flag);
            break;
        case hkMotion::MOTION_STABILIZED_BOX_INERTIA:
            new (&m_motion) hkStabilizedBoxMotion(flag);
            break;
        case hkMotion::MOTION_KEYFRAMED:
            new (&m_motion) hkKeyframedRigidMotion(flag);
            break;
        case hkMotion::MOTION_FIXED:
            new (&m_motion) hkFixedRigidMotion(flag);
            break;
        case hkMotion::MOTION_THIN_BOX_INERTIA:
            new (&m_motion) hkThinBoxMotion(flag);
            break;
        }
    }
}

hkEntity::~hkEntity() {
    hkEntityCallbackUtil::fireEntityDeleted(this);
    if (m_deactivator) {
        m_deactivator->removeReference();
    }
}

// Replaces the deactivator; reference counts are kept on both objects.
void hkEntity::setDeactivator(hkEntityDeactivator* deactivator) {
    if (deactivator != 0) {
        ((hkReferencedObject*)deactivator)->addReference();
    }
    if (m_deactivator != 0) {
        ((hkReferencedObject*)m_deactivator)->removeReference();
    }
    m_deactivator = deactivator;
}

// Appends the listener to the first empty slot, or to the end of the array.
void hkEntity::addEntityListener(hkEntityListener* listener) {
    // A free (null) slot left by a removal is reused before the array grows.
    int idx = m_entityListeners.indexOf(NULL);
    if (idx >= 0) {
        m_entityListeners[idx] = listener;
    } else {
        m_entityListeners.pushBack(listener);
    }
}

void hkEntity::addCollisionListener(hkCollisionListener* listener) {
    // A free (null) slot left by a removal is reused before the array grows.
    int idx = m_collisionListeners.indexOf(NULL);
    if (idx >= 0) {
        m_collisionListeners[idx] = listener;
    } else {
        m_collisionListeners.pushBack(listener);
    }
}

// HYPOTHESIS: the activity bits are returned in the top byte.
u32 hkEntity::isActive() const {
    return (u32)getActivationState() << 24;
}

void hkEntity::activate() {
    bool wantActivate = false;
    bool notFixed = false;
    if ((s8)(u8)getActivationState() == 0) {
        notFixed = !(getMotion()->m_type == hkMotion::MOTION_FIXED);
    }
    if (notFixed && m_world != 0) {
        wantActivate = true;
    }
    if (wantActivate) {
        hkWorldOperationUtil::markIslandActive(m_world, m_simulationIsland);
    }
}

void hkEntity::deactivate() {
    if ((s8)(u8)getActivationState() != 0) {
        hkWorldOperationUtil::markIslandInactive(m_world, m_simulationIsland);
    }
}

int hkEntity::getNumConstraints() const {
    return m_constraintsMaster.m_size + m_constraintsSlave.m_size;
}

hkConstraintInstance* hkEntity::getConstraint(int index) const {
    int numMaster = m_constraintsMaster.m_size;
    if (index < numMaster) {
        return *(hkConstraintInstance**)((u8*)m_constraintsMaster.m_data + index * 0x24);
    }
    return ((hkConstraintInstance**)m_constraintsSlave.m_data)[index - numMaster];
}

hkMotionState* hkEntity::getMotionState() {
    return 0;
}

static hkStaticTypeInfo hkEntityTypeInfo("hkEntity", hkEntity::finishLoadedObjecthkEntity, hkEntity::cleanupLoadedObjecthkEntity,
                                      hkEntity::getVtablehkEntity());
