// Havok translation unit hkKeyframedRigidMotion.o (main.dol 0x802E5564-0x802E5A28).
// Not yet decompiled. Functions in address order:
//   0x802E5564    32  finishLoadedObjecthkKeyframedRigidMotion
//   0x802E5584    20  cleanupLoadedObjecthkKeyframedRigidMotion
//   0x802E5598    60  getVtablehkKeyframedRigidMotion
//   0x802E55D4    32  finishLoadedObjecthkMaxSizeMotion
//   0x802E55F4    20  cleanupLoadedObjecthkMaxSizeMotion
//   0x802E5608    60  getVtablehkMaxSizeMotion
//   0x802E5644   100  __ct
//   0x802E56A8   176  __dt
//   0x802E5758     4  setMass
//   0x802E575C     4  setMassInv
//   0x802E5760    96  getInertiaLocal
//   0x802E57C0    96  getInertiaWorld
//   0x802E5820     4  setInertiaLocal
//   0x802E5824     4  setInertiaInvLocal
//   0x802E5828    96  getInertiaInvLocal
//   0x802E5888    96  getInertiaInvWorld
//   0x802E58E8     4  applyLinearImpulse
//   0x802E58EC     4  applyPointImpulse
//   0x802E58F0     4  applyAngularImpulse
//   0x802E58F4     4  applyForce
//   0x802E58F8     4  applyForce
//   0x802E58FC     4  applyTorque
//   0x802E5900     4  setStepPosition
//   0x802E5904   156  setStoredMotion
//   0x802E59A0   136  __sinit_\hkKeyframedRigidMotion_cpp
#include <new>
#include <havok/hkKeyframedRigidMotion.h>
#include <havok/hkRegistry.h>

static hkMotionTypeInfo s_hkKeyframedRigidMotionTypeInfo("hkKeyframedRigidMotion",
    hkKeyframedRigidMotion::finishLoadedObjecthkKeyframedRigidMotion,
    hkKeyframedRigidMotion::cleanupLoadedObjecthkKeyframedRigidMotion,
    hkKeyframedRigidMotion::getVtablehkKeyframedRigidMotion());

static hkMotionTypeInfo s_hkMaxSizeMotionTypeInfo("hkMaxSizeMotion",
    hkKeyframedRigidMotion::finishLoadedObjecthkMaxSizeMotion,
    hkKeyframedRigidMotion::cleanupLoadedObjecthkMaxSizeMotion,
    hkKeyframedRigidMotion::getVtablehkMaxSizeMotion());

void hkKeyframedRigidMotion::finishLoadedObjecthkKeyframedRigidMotion(void* p) {
    new (p) hkKeyframedRigidMotion(hkFinishLoadedObjectFlag());
}

void hkKeyframedRigidMotion::cleanupLoadedObjecthkKeyframedRigidMotion(void* p) {
    ((hkKeyframedRigidMotion*)p)->~hkKeyframedRigidMotion();
}

const void* hkKeyframedRigidMotion::getVtablehkKeyframedRigidMotion() {
    hkVector4 buf[17]; // 0x110 bytes of 16-byte-aligned storage for the placed object
    new (buf) hkKeyframedRigidMotion(hkFinishLoadedObjectFlag());
    return *(const void**)buf;
}

void hkKeyframedRigidMotion::finishLoadedObjecthkMaxSizeMotion(void* p) {
    new (p) hkMaxSizeMotion(hkFinishLoadedObjectFlag());
}

void hkKeyframedRigidMotion::cleanupLoadedObjecthkMaxSizeMotion(void* p) {
    ((hkMaxSizeMotion*)p)->~hkMaxSizeMotion();
}

const void* hkKeyframedRigidMotion::getVtablehkMaxSizeMotion() {
    hkVector4 buf[17]; // 0x110 bytes of 16-byte-aligned storage for the placed object
    new (buf) hkMaxSizeMotion(hkFinishLoadedObjectFlag());
    return *(const void**)buf;
}

hkKeyframedRigidMotion::hkKeyframedRigidMotion(const hkVector4& position, const hkQuaternion& rotation)
    : hkMotion(position, rotation) {
    m_savedMotion = 0;
    m_savedQualityTypeIndex = 0;
    m_inertiaAndMassInv.setZero4();
    m_type = MOTION_KEYFRAMED;
}

hkKeyframedRigidMotion::~hkKeyframedRigidMotion() {
    if (m_savedMotion) {
        m_savedMotion->removeReference();
    }
}

void hkKeyframedRigidMotion::setMass(hkReal mass) {}

void hkKeyframedRigidMotion::setMassInv(hkReal massInv) {}

// Inertia is not tracked for keyframed bodies: the matrices are returned as zero.
void hkKeyframedRigidMotion::getInertiaLocal(hkMatrix3& out) const {
    out.setZero();
}

void hkKeyframedRigidMotion::getInertiaWorld(hkMatrix3& out) const {
    out.setZero();
}

void hkKeyframedRigidMotion::setInertiaLocal(const hkMatrix3& in) {}

void hkKeyframedRigidMotion::setInertiaInvLocal(const hkMatrix3& in) {}

void hkKeyframedRigidMotion::getInertiaInvLocal(hkMatrix3& out) const {
    out.setZero();
}

void hkKeyframedRigidMotion::getInertiaInvWorld(hkMatrix3& out) const {
    out.setZero();
}

void hkKeyframedRigidMotion::applyLinearImpulse(const hkVector4& impulse) {}

void hkKeyframedRigidMotion::applyPointImpulse(const hkVector4& impulse, const hkVector4& point) {}

void hkKeyframedRigidMotion::applyAngularImpulse(const hkVector4& impulse) {}

void hkKeyframedRigidMotion::applyForce(hkReal timestep, const hkVector4& force) {}

void hkKeyframedRigidMotion::applyForce(hkReal timestep, const hkVector4& force, const hkVector4& point) {}

void hkKeyframedRigidMotion::applyTorque(hkReal timestep, const hkVector4& torque) {}

void hkKeyframedRigidMotion::setStepPosition(hkReal stepPosition) {}

// Replaces the stored motion: the new one gains a reference, the old one loses one.
void hkKeyframedRigidMotion::setStoredMotion(hkMaxSizeMotion* motion) {
    if (motion) {
        motion->addReference();
    }
    if (m_savedMotion) {
        m_savedMotion->removeReference();
    }
    m_savedMotion = motion;
}
