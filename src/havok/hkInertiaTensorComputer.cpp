// Havok translation unit hkInertiaTensorComputer.o (main.dol 0x8032C42C-0x8032D42C).
// Partly decompiled. Functions in address order (method names from the Havok TU map; classes still to be identified):
//   0x8032C42C   120  __ct   [map: hkMassProperties____ct]
//   0x8032C4A4   100  __as   [map: hkMatrix3____as]
//   0x8032C508  1336  computeCapsuleVolumeMassProperties   [map: hkInertiaTensorComputer__computeCapsuleVolumeMassProperties]
//   0x8032CA40    88  acos   [map: hkMath__acos]
//   0x8032CA98     4  setRotation   [map: hkTransform__setRotation]
//   0x8032CA9C   128  setIdentity   [map: hkTransform__setIdentity]
//   0x8032CB1C    36  setTranslation   [map: hkTransform__setTranslation]
//   0x8032CB40   216  __ct   [map: hkMassElement____ct]
//   0x8032CC18     8  getTranslation   [map: hkTransform__getTranslation]
//   0x8032CC20   244  __ct   [map: hkInplaceArray_13hkMassElement_3_____ct]
//   0x8032CD14   148  __dt   [map: hkInplaceArray_13hkMassElement_3_____dt]
//   0x8032CDA8   172  __as   [map: hkTransform____as]
//   0x8032CE54   376  pushBack   [map: hkArray_13hkMassElement___pushBack]
//   0x8032CFCC   776  combineMassProperties   [map: hkInertiaTensorComputer__combineMassProperties]
//   0x8032D2D4   172  shiftInertiaToCom   [map: hkInertiaTensorComputer__shiftInertiaToCom]
//   0x8032D380   172  shiftInertiaFromCom   [map: hkInertiaTensorComputer__shiftInertiaFromCom]
#include <havok/hkInertiaTensorComputer.h>
#include <havok/hkTransform.h>

// Tail-calls the rotation setter with this as the rotation.
void hkTransform::setRotation(const hkQuaternion& q) {
    m_rotation.set(q);
}

void hkTransform::setTranslation(const hkVector4& t) {
    m_translation = t;
}

void hkTransform::setIdentity() {
    m_rotation.m_col0.set(1.0f, 0.0f, 0.0f, 0.0f);
    m_rotation.m_col1.set(0.0f, 1.0f, 0.0f, 0.0f);
    m_rotation.m_col2.set(0.0f, 0.0f, 1.0f, 0.0f);
    m_translation.setZero4();
}

hkVector4& hkTransform::getTranslation() {
    return m_translation;
}


hkMassProperties::hkMassProperties() {
    m_volume = 0.0f;
    m_mass = 0.0f;
    // HYPOTHESIS: Havok's setZero4 is a chained x = y = z = w = 0 (stores w first); the header's set() order gives x first
    m_centerOfMass.x = m_centerOfMass.y = m_centerOfMass.z = m_centerOfMass.w = 0.0f;
    m_inertiaTensor.setZero();
}

// inertia(r, c) is elements[c * 4 + r]. moving the tensor onto the center of mass takes away mass * (shift^2 terms) on the
// diagonal and puts the shift products back on the off-diagonals (which are symmetric)
void hkInertiaTensorComputer::shiftInertiaToCom(hkVector4& shift, hkReal mass, hkMatrix3& inertia) {
    float* m = &inertia.m_col0.x;
    m[0] -= mass * (shift.y * shift.y + shift.z * shift.z);
    m[5] -= mass * (shift.z * shift.z + shift.x * shift.x);
    m[10] -= mass * (shift.x * shift.x + shift.y * shift.y);
    float massX = mass * shift.x;
    m[4] = m[1] += massX * shift.y;
    float massY = mass * shift.y;
    m[9] = m[6] += massY * shift.z;
    float massZ = mass * shift.z;
    m[2] = m[8] += massZ * shift.x;
}

// same thing the other way round: signs flip
void hkInertiaTensorComputer::shiftInertiaFromCom(hkVector4& shift, hkReal mass, hkMatrix3& inertia) {
    float* m = &inertia.m_col0.x;
    m[0] += mass * (shift.y * shift.y + shift.z * shift.z);
    m[5] += mass * (shift.z * shift.z + shift.x * shift.x);
    m[10] += mass * (shift.x * shift.x + shift.y * shift.y);
    float massX = mass * shift.x;
    m[4] = m[1] -= massX * shift.y;
    float massY = mass * shift.y;
    m[9] = m[6] -= massY * shift.z;
    float massZ = mass * shift.z;
    m[2] = m[8] -= massZ * shift.x;
}

hkResult hkInertiaTensorComputer::combineMassProperties(const hkArray<hkMassElement>& elements, hkMassProperties& result) {
    // everything has to end up in one common space, so first find the new center of mass there
    hkVector4 groupCenterOfMass;
    groupCenterOfMass.setZero4();
    hkReal groupMass = 0.0f;
    hkReal groupVolume = 0.0f;
    int i;
    for (i = 0; i < elements.m_size; i++) {
        hkVector4 centerOfMassInCommon;
        centerOfMassInCommon.setTransformedPos(elements[i].m_transform, elements[i].m_properties.m_centerOfMass);
        groupCenterOfMass.x += elements[i].m_properties.m_mass * centerOfMassInCommon.x;
        groupCenterOfMass.y += elements[i].m_properties.m_mass * centerOfMassInCommon.y;
        groupCenterOfMass.z += elements[i].m_properties.m_mass * centerOfMassInCommon.z;
        groupCenterOfMass.w += elements[i].m_properties.m_mass * centerOfMassInCommon.w;
        groupMass += elements[i].m_properties.m_mass;
        groupVolume += elements[i].m_properties.m_volume;
    }

    if (groupMass <= 0.0f) {
        return 1; // HK_FAILURE
    }

    groupCenterOfMass.setMul4(groupCenterOfMass, 1.0f / groupMass);
    result.m_centerOfMass = groupCenterOfMass;
    result.m_mass = groupMass;
    result.m_volume = groupVolume;
    for (i = 0; i < 12; i++) {
        (&result.m_inertiaTensor.m_col0.x)[i] = 0.0f;
    }

    // each tensor gets rotated into the common space, moved away from its own center of mass to the group's, and added up
    for (i = 0; i < elements.m_size; i++) {
        hkMatrix3 inertiaTensorInCommonSpace;
        inertiaTensorInCommonSpace = elements[i].m_properties.m_inertiaTensor;
        inertiaTensorInCommonSpace.changeBasis(elements[i].m_transform.m_rotation);

        hkVector4 shift;
        shift.setTransformedPos(elements[i].m_transform, elements[i].m_properties.m_centerOfMass);
        shift.setSub4(shift, groupCenterOfMass);
        shiftInertiaFromCom(shift, elements[i].m_properties.m_mass, inertiaTensorInCommonSpace);

        result.m_inertiaTensor.add(inertiaTensorInCommonSpace);
    }
    return 0; // HK_SUCCESS
}
