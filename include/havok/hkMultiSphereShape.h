#pragma once

#include <havok/hkSphereRepShape.h>

// Shape made of up to 8 spheres (not in the image's reflection). From hkMultiSphereAgent's queries: the spheres are
// stored inline at 0x10 as (center, radius in w). HYPOTHESIS: the count at 0x0C, the array length and the names.
struct hkMultiSphereShape : hkSphereRepShape {
    int m_numSpheres;        // 0x0C
    hkVector4 m_spheres[8];  // 0x10
};
