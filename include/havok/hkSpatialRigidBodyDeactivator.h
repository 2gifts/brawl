#pragma once

#include <havok/hkRigidBodyDeactivator.h>
#include <havok/hkVector4.h>
#include <havok/hkQuaternion.h>

// One reference sample (0x20 bytes). Layout from hkSpatialRigidBodyDeactivatorClass.cpp.
struct hkSpatialRigidBodyDeactivatorSample {
    hkVector4 m_refPosition;    // 0x00
    hkQuaternion m_refRotation; // 0x10
};

// Spatial deactivator (0x70 bytes). Layout from hkSpatialRigidBodyDeactivatorClass.cpp.
// Still abstract: shouldDeactivateHighFrequency / shouldDeactivateLowFrequency are not written yet, so the
// loaded-object helpers (finish, getVtable, static init) are not declared here.
struct hkSpatialRigidBodyDeactivator : hkRigidBodyDeactivator {
    hkSpatialRigidBodyDeactivatorSample m_highFrequencySample; // 0x10
    hkSpatialRigidBodyDeactivatorSample m_lowFrequencySample;  // 0x30
    hkReal m_radiusSqrd;                  // 0x50
    hkReal m_minHighFrequencyTranslation; // 0x54
    hkReal m_minHighFrequencyRotation;    // 0x58
    hkReal m_minLowFrequencyTranslation;  // 0x5C
    hkReal m_minLowFrequencyRotation;     // 0x60

    virtual ~hkSpatialRigidBodyDeactivator(); // 0x08 (out of line in src/havok/hkSpatialRigidBodyDeactivator.cpp)

    static void cleanupLoadedObjecthkSpatialRigidBodyDeactivator(void* p);

    virtual int getRigidBodyDeactivatorType() const; // 0x0C
};
