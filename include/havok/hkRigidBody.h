#pragma once

#include <havok/hkEntity.h>

// Rigid body (0x1F0 bytes). Adds no fields to hkEntity; layout from hkRigidBodyClass.cpp.
// The motion (hkFixedRigidMotion / hkKeyframedRigidMotion style) is reached through hkEntity::getMotion().
struct hkRigidBody : hkEntity {
    hkRigidBody(hkFinishLoadedObjectFlag flag) : hkEntity(flag) {}
    // Out of line (__ct__11hkRigidBodyFi, 0x802E2840); phShape calls it. HYPOTHESIS: the int parameter.
    hkRigidBody(int flag);

    // Loaded-object support (static init registration).
    static void finishLoadedObjecthkRigidBody(void* p);
    static void cleanupLoadedObjecthkRigidBody(void* p);
    static const void* getVtablehkRigidBody();

    virtual hkMotionState* getMotionState();
    // Accessors into the body's motion block (out of line in src/havok/hkSimulation.cpp).
    hkMotion* getRigidMotion();
    hkTransform& getTransform();
    hkVector4& getCenterOfMassInWorld();

    void setDeactivator(hkEntityDeactivator* deactivator);
    void setMotionType(u8 motionType, u8 a, u8 b);
    void setMass(hkReal mass);
    void setCenterOfMassLocal(const hkVector4& com);
    // HYPOTHESIS: computes the shape bounds and a normalized direction into out (0x10 bytes).
    // Out of line at 0x802E23CC (hkRigidBody__updateCachedShapeInfo), not written yet.
    void updateCachedShapeInfo(hkShape* shape, hkVector4* out);
    void setPosition(const hkVector4& position);
    void setPositionAndRotation(const hkVector4& position, const hkQuaternion& rotation);
    void setTransform(const hkTransform& transform);
    // Turns the body to the new rotation about its centre of mass (the centre of mass stays where it is).
    void setRotationAroundCentreOfMass(const hkQuaternion& newRotation);
    // HYPOTHESIS: refreshes the broadphase entry and collision information after the body is warped.
    void updateBroadphaseAndResetCollisionInformationOfWarpedBody();
};
