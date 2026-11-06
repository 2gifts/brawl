#pragma once

#include <havok/hkVector4.h>

// Contact point record, 0x20 bytes. Position at 0x00, normal at 0x10; the contact distance is the
// w component of the normal vector (0x1C). Accessors from the hkSimpleContactConstraintInfo TU.
struct hkContactPoint {
    hkVector4 m_position; // 0x00
    hkVector4 m_normal;   // 0x10 (w = distance)

    hkVector4* getPosition();
    // second overload (map: hkContactPoint::getPosition1, out of line in hkSimulation.o). HYPOTHESIS: const form.
    const hkVector4& getPosition() const;
    hkVector4* getNormal();
    // HYPOTHESIS: return type and const (map name only); returns the normal at 0x10
    const hkVector4& getSeparatingNormal() const;
    hkReal getDistance();
    // Added by the hkCollideCapsuleUtil unit: writes the w component of the normal (the contact distance).
    void setDistance(hkReal distance);
};
