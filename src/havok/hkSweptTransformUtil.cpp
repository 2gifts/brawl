// hkSweptTransformUtil.o (main.dol 0x80285DA4-0x802871F4)
// stuff that moves a body's pose around, using the swept transform stored in its hkMotionState.
// drafted from the havok 5.1 source, which has most of these. HYPOTHESIS: this build is older so
// argument orders / details can differ. every signature here was checked against the asm though
//   0x80285DA4   876  lerp2
//   0x80286110   880  lerp2Ha
//   0x80286480   960  backStepMotionState
//   0x80286840   980  freezeMotionState
//   0x80286C14    12  setTimeInformation
//   0x80286C20   340  warpTo(pos, quat, ms)
//   0x80286D74   548  warpTo(transform, ms)
//   0x80286F98   332  warpToPosition
//   0x802870E4    20  warpToRotation
//   0x802870F8   252  setCentreOfRotationLocal
#include <havok/hkSweptTransformUtil.h>

// sweptTransform keeps the step's start time in centerOfMass0.w and 1/deltaTime in centerOfMass1.w
static inline hkReal getBaseTime(const hkSweptTransform& st) { return st.m_centerOfMass0.w; }
static inline hkReal getInvDeltaTime(const hkSweptTransform& st) { return st.m_centerOfMass1.w; }

// 0..1 for how far through the step we are
static inline hkReal getInterpolationValue(const hkSweptTransform& st, hkReal t) {
    return (t - getBaseTime(st)) * getInvDeltaTime(st);
}

static inline hkReal getInterpolationValueHiAccuracy(const hkSweptTransform& st, hkReal t, hkReal addOn) {
    hkReal dt = t - getBaseTime(st);
    dt += addOn;
    return dt * getInvDeltaTime(st);
}

// out = a + (b - a) * t
static inline void setInterpolate4(hkVector4& out, const hkVector4& a, const hkVector4& b, hkReal t) {
    out.x = a.x + (b.x - a.x) * t;
    out.y = a.y + (b.y - a.y) * t;
    out.z = a.z + (b.z - a.z) * t;
    out.w = a.w + (b.w - a.w) * t;
}

// blends the two end rotations. the "0.75 - 0.125 * len2" thing is a quick 1/sqrt for the midpoint,
// then we lerp start->mid for the first half of the step and mid->end for the second half
static void lerp2Quat(const hkSweptTransform& st, hkReal t, hkQuaternion& out) {
    hkVector4 rot0 = st.m_rotation0.m_vec;
    hkVector4 rot1 = st.m_rotation1.m_vec;
    {
        out.m_vec.setAdd4(rot0, rot1);
        hkReal len2 = out.m_vec.lengthSquared4();
        hkReal r = 0.75f - 0.125f * len2;
        r = r * (1.5f - 0.5f * len2 * r * r);
        out.m_vec.mul4(r);
    }

    if (t < 0.5f) {
        setInterpolate4(out.m_vec, rot0, out.m_vec, 2.0f * t);
    } else {
        setInterpolate4(out.m_vec, out.m_vec, rot1, 2.0f * t - 1.0f);
    }
    hkReal len2 = out.m_vec.lengthSquared4();
    out.m_vec.mul4(1.0f / sqrtf(len2));
}

// pose at t: blended rotation, blended center of mass, then shove the origin back by the rotated local center
static void lerp2Transform(const hkSweptTransform& st, hkReal t, hkTransform& out) {
    hkQuaternion qt;
    lerp2Quat(st, t, qt);

    out.m_rotation.set(qt);
    setInterpolate4(out.m_translation, st.m_centerOfMass0, st.m_centerOfMass1, t);

    hkVector4 centerShift;
    centerShift.setRotatedDir(out.m_rotation, st.m_centerOfMassLocal);
    out.m_translation.sub4(centerShift);
}

// same idea as lerp2Transform but pins the rotation to the end of the step
static void calcTransAtT1(const hkSweptTransform& st, hkTransform& out) {
    out.m_rotation.set(st.m_rotation1);

    hkVector4 centerShift;
    centerShift.setRotatedDir(out.m_rotation, st.m_centerOfMassLocal);
    out.m_translation.setSub4(st.m_centerOfMass1, centerShift);
}

void hkSweptTransformUtil::lerp2(const hkSweptTransform& st, hkReal t, hkTransform& out) {
    hkReal r = getInterpolationValue(st, t);
    lerp2Transform(st, r, out);
}

void hkSweptTransformUtil::lerp2Ha(const hkSweptTransform& st, hkReal t, hkReal tAddOn, hkTransform& out) {
    hkReal r = getInterpolationValueHiAccuracy(st, t, tAddOn);
    lerp2Transform(st, r, out);
}

void hkSweptTransformUtil::backStepMotionState(hkReal time, hkMotionState& ms) {
    hkSweptTransform& st = ms.m_sweptTransform;
    hkReal t = getInterpolationValue(st, time);
    if (t < 1.1920929e-07f) {
        t = 1.1920929e-07f;
    }

    lerp2Quat(st, t, st.m_rotation1);
    hkReal newInvDeltaTime = getInvDeltaTime(st) / t;

    setInterpolate4(st.m_centerOfMass1, st.m_centerOfMass0, st.m_centerOfMass1, t);
    st.m_centerOfMass1.w = newInvDeltaTime;
    ms.m_deltaAngle.mul4(t);

    calcTransAtT1(st, ms.m_transform);
}

void hkSweptTransformUtil::freezeMotionState(hkReal time, hkMotionState& ms) {
    hkSweptTransform& st = ms.m_sweptTransform;

    // we freeze at the earliest moment we're allowed to, which is the step's start time or later
    if (time < getBaseTime(st)) {
        time = getBaseTime(st);
    }
    hkReal t = getInterpolationValue(st, time);

    lerp2Quat(st, t, st.m_rotation1);
    st.m_rotation0 = st.m_rotation1;

    setInterpolate4(st.m_centerOfMass1, st.m_centerOfMass0, st.m_centerOfMass1, t);
    st.m_centerOfMass0 = st.m_centerOfMass1;

    st.m_centerOfMass0.w = time;
    st.m_centerOfMass1.w = 0.0f;

    calcTransAtT1(st, ms.m_transform);
}

void hkSweptTransformUtil::setTimeInformation(hkMotionState& ms, hkReal t0, hkReal t1) {
    ms.m_sweptTransform.m_centerOfMass0.w = t0;
    ms.m_sweptTransform.m_centerOfMass1.w = t1;
}

void hkSweptTransformUtil::warpTo(const hkVector4& position, const hkQuaternion& rotation, hkMotionState& ms) {
    hkSweptTransform& st = ms.m_sweptTransform;
    ms.m_deltaAngle.setZero4();

    st.m_rotation0 = rotation;
    st.m_rotation1 = rotation;

    ms.m_transform.m_rotation.set(rotation);
    ms.m_transform.m_translation = position;

    hkVector4 centerShift;
    centerShift.setRotatedDir(ms.m_transform.m_rotation, st.m_centerOfMassLocal);

    hkReal baseTime = st.m_centerOfMass0.w;

    st.m_centerOfMass0.setAdd4(position, centerShift);
    st.m_centerOfMass1 = st.m_centerOfMass0;

    st.m_centerOfMass0.w = baseTime;
    st.m_centerOfMass1.w = 0.0f;  // invDeltaTime
}

void hkSweptTransformUtil::warpTo(const hkTransform& transform, hkMotionState& ms) {
    hkSweptTransform& st = ms.m_sweptTransform;
    ms.m_deltaAngle.setZero4();

    hkQuaternion rotation;
    rotation.set(transform.m_rotation);
    ms.m_transform = transform;

    st.m_rotation0 = rotation;
    st.m_rotation1 = rotation;

    hkVector4 centerShift;
    centerShift.setRotatedDir(transform.m_rotation, st.m_centerOfMassLocal);

    hkReal baseTime = st.m_centerOfMass0.w;

    st.m_centerOfMass0.setAdd4(transform.m_translation, centerShift);
    st.m_centerOfMass1 = st.m_centerOfMass0;

    st.m_centerOfMass0.w = baseTime;
    st.m_centerOfMass1.w = 0.0f;  // invDeltaTime
}

void hkSweptTransformUtil::warpToPosition(const hkVector4& position, hkMotionState& ms) {
    const hkRotation& currentRotation = ms.m_transform.m_rotation;
    hkSweptTransform& st = ms.m_sweptTransform;

    ms.m_deltaAngle.setZero4();
    ms.m_transform.m_translation = position;

    hkVector4 centerShift;
    centerShift.setRotatedDir(currentRotation, st.m_centerOfMassLocal);

    hkReal baseTime = st.m_centerOfMass0.w;

    st.m_centerOfMass0.setAdd4(position, centerShift);
    st.m_centerOfMass1 = st.m_centerOfMass0;

    st.m_rotation0 = st.m_rotation1;

    st.m_centerOfMass0.w = baseTime;
    st.m_centerOfMass1.w = 0.0f;  // invDeltaTime
}

void hkSweptTransformUtil::warpToRotation(const hkQuaternion& rotation, hkMotionState& ms) {
    warpTo(ms.m_transform.m_translation, rotation, ms);
}

// 5.1 doesn't have this one so it's straight from the asm: new local center goes in, and since the
// body isn't supposed to move, both swept centers shift by (rotated difference between old and new)
void hkSweptTransformUtil::setCentreOfRotationLocal(const hkVector4& newCenter, hkMotionState& ms) {
    hkSweptTransform& st = ms.m_sweptTransform;

    hkVector4 diff;
    diff.setSub4(newCenter, st.m_centerOfMassLocal);
    st.m_centerOfMassLocal = newCenter;

    hkVector4 shift;
    shift.setRotatedDir(ms.m_transform.m_rotation, diff);
    shift.w = 0.0f;

    st.m_centerOfMass0.x += shift.x;
    st.m_centerOfMass0.y += shift.y;
    st.m_centerOfMass0.z += shift.z;
    st.m_centerOfMass1.x += shift.x;
    st.m_centerOfMass1.y += shift.y;
    st.m_centerOfMass1.z += shift.z;
}
