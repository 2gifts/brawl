#pragma once

#include <havok/hkMatrix3.h>
#include <havok/hkQuaternion.h>

// 3x3 rotation matrix: an hkMatrix3 (three columns) with no fields of its own.
struct hkRotation : hkMatrix3 {
    void set(const hkQuaternion& q);
    // tolerance is passed in f1 (HYPOTHESIS: a caller-supplied epsilon).
    bool isOrthonormal(hkReal tolerance) const;
};
