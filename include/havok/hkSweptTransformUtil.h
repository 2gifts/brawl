#pragma once

#include <havok/hkMotionState.h>

// helpers that move a body's hkMotionState around (the swept transform lives inside it).
// signatures are from the asm. most match havok 5.1 but setTimeInformation takes the motion state FIRST here,
// 5.1 has it last. so this build is a bit older, don't assume the rest line up either
struct hkSweptTransformUtil {
    // pose at time t, blended between the start and end of the step. result goes in out
    static void lerp2(const hkSweptTransform& st, hkReal t, hkTransform& out);
    // same but t gets tAddOn nudged on for extra precision
    static void lerp2Ha(const hkSweptTransform& st, hkReal t, hkReal tAddOn, hkTransform& out);

    // rewind the end of the step back to time (only touches the end pose + the transform)
    static void backStepMotionState(hkReal time, hkMotionState& ms);
    // pin the body at time: start and end pose become the same and invDeltaTime goes to 0
    static void freezeMotionState(hkReal time, hkMotionState& ms);

    static void setTimeInformation(hkMotionState& ms, hkReal t0, hkReal t1);

    // teleport stuff. these all kill the delta angle and make the swept transform a single point
    static void warpTo(const hkVector4& position, const hkQuaternion& rotation, hkMotionState& ms);
    static void warpTo(const hkTransform& transform, hkMotionState& ms);
    static void warpToPosition(const hkVector4& position, hkMotionState& ms);
    static void warpToRotation(const hkQuaternion& rotation, hkMotionState& ms);

    // move the center of mass offset (in local space) without moving the body, so the swept
    // centers have to shift by the rotated difference
    static void setCentreOfRotationLocal(const hkVector4& newCenter, hkMotionState& ms);
};
