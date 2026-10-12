#pragma once

#include <havok/hkArray.h>
#include <havok/hkMathUtil.h> // hkMatrix3Storage (hkMatrix3.h can't be included next to hkTransform.h)
#include <havok/hkVector4.h>

struct hkMatrix3;

// Mass, volume, center of mass and inertia tensor of a body or a piece of one (0x50 bytes)
struct hkMassProperties {
    hkReal m_volume;                  // 0x00
    hkReal m_mass;                    // 0x04
    hkVector4 m_centerOfMass;         // 0x10
    hkMatrix3Storage m_inertiaTensor; // 0x20 an hkMatrix3, kept as raw storage here

    hkMassProperties(); // everything zero
};

// one piece of a compound body: its own mass properties plus where it sits (0x90 bytes)
struct hkMassElement {
    hkMassProperties m_properties;   // 0x00
    hkMatrix3Storage m_rotation;     // 0x50 the piece's transform: rotation...
    hkVector4 m_translation;         // 0x80 ...and translation
};

struct hkInertiaTensorComputer {
    // adds up the pieces into one set of mass properties (returns 1 if the total mass isn't positive)
    static hkResult combineMassProperties(const hkArray<hkMassElement>& elements, hkMassProperties& result);

    // move an inertia tensor to / from the center of mass by shift (parallel axis theorem)
    static void shiftInertiaToCom(hkVector4& shift, hkReal mass, hkMatrix3& inertia);
    static void shiftInertiaFromCom(hkVector4& shift, hkReal mass, hkMatrix3& inertia);
};
