// Havok translation unit hkTwoJointsIkSolver.o (main.dol 0x8032E138-0x8032E740). Names from the DebugMode map.
// Functions in address order:
//   0x8032E138  1080  hkTwoJointsIkSolver::solve
//   0x8032E570   464  hkQuaternion::setShortestRotationDamped (weak copy)
// The unit is compiled without inlining: every accessor and vector helper is called out of line.

#include <havok/hkPose.h>
#include <havok/hkQsTransform.h>
#include <havok/hkQuaternion.h>
#include <havok/hkMath.h>
#include <havok/hkMonitorStream.h>

// Two-bone IK: bends the second joint so the end bone reaches the target distance, then turns the first joint
// towards the target. HYPOTHESIS: the member names (the layout and the roles come from solve).
struct hkTwoJointsIkSolver {
    struct Setup {
        s16 m_firstJointIdx;          // 0x00
        s16 m_secondJointIdx;         // 0x02
        s16 m_endBoneIdx;             // 0x04
        u8 unk06[0x10 - 0x06];
        hkVector4 m_hingeAxisLS;      // 0x10 hinge axis of the second joint, in its local space
        hkReal m_cosineMinHingeAngle; // 0x20 lower clamp of the hinge cosine
        hkReal m_cosineMaxHingeAngle; // 0x24 upper clamp of the hinge cosine
        hkReal m_firstJointIkGain;    // 0x28
        hkReal m_secondJointIkGain;   // 0x2C
        hkVector4 m_endTargetMS;      // 0x30 target of the end bone, in model space
    };

    static hkBool solve(const Setup& setup, hkPose& pose);
};

#pragma push
#pragma dont_inline on

hkBool hkTwoJointsIkSolver::solve(const Setup& setup, hkPose& pose) {
    HK_TIMER_BEGIN_LIST_ACCESSORS("TWOJOINTSOLVER", "INIT");
    HK_TIMER_SPLIT_LIST_ACCESSORS("FIXLENGTH");
    {
        const hkVector4& firstPos = pose.getBoneModelSpace(setup.m_firstJointIdx)->getTranslation();
        const hkVector4& secondPos = pose.getBoneModelSpace(setup.m_secondJointIdx)->getTranslation();
        const hkVector4& endPos = pose.getBoneModelSpace(setup.m_endBoneIdx)->getTranslation();

        hkVector4 firstToSecond;
        firstToSecond.setSub4(secondPos, firstPos);
        hkVector4 secondToEnd;
        secondToEnd.setSub4(endPos, secondPos);
        hkVector4 firstToTarget;
        firstToTarget.setSub4(setup.m_endTargetMS, firstPos);

        const hkReal distToTargetSq = firstToTarget.lengthSquared3();
        const hkReal firstLenSq = firstToSecond.lengthSquared3();
        const hkReal secondLenSq = secondToEnd.lengthSquared3();
        const hkReal firstLen = hkMath::sqrt(firstLenSq);
        const hkReal secondLen = hkMath::sqrt(secondLenSq);

        // law of cosines for the hinge angle that puts the end bone at the target distance, clamped
        const hkReal cosAngle = hkMath::max2f(setup.m_cosineMinHingeAngle,
            hkMath::min2(setup.m_cosineMaxHingeAngle,
                         (distToTargetSq - firstLenSq - secondLenSq) / (-2.0f * firstLen * secondLen)));
        const hkReal desiredAngle = hkMath::acos(cosAngle);

        firstToSecond.normalize3();
        firstToSecond.setNeg3(firstToSecond);
        secondToEnd.normalize3();
        const hkReal currentAngle = hkMath::acos(firstToSecond.dot3(secondToEnd));
        const hkReal diffAngle = desiredAngle - currentAngle;

        hkQsTransform* secondJoint = pose.accessBoneModelSpace(setup.m_secondJointIdx, 1);
        hkQuaternion secondRotation(secondJoint->getRotation());
        hkVector4 hingeAxisMS;
        hingeAxisMS.setRotatedDir(secondRotation, setup.m_hingeAxisLS);
        hkQuaternion hingeRotation(hingeAxisMS, diffAngle * setup.m_secondJointIkGain);
        secondJoint->m_rotation.setMul(hingeRotation, secondRotation);
        secondJoint->m_rotation.normalize();
    }
    HK_TIMER_SPLIT_LIST_ACCESSORS("FIXDIRECTION");
    {
        const hkVector4& firstPos = pose.getBoneModelSpace(setup.m_firstJointIdx)->getTranslation();
        const hkVector4& endPos = pose.getBoneModelSpace(setup.m_endBoneIdx)->getTranslation();

        hkVector4 toEnd;
        toEnd.setSub4(endPos, firstPos);
        toEnd.normalize3();
        hkVector4 toTarget;
        toTarget.setSub4(setup.m_endTargetMS, firstPos);
        toTarget.normalize3();

        hkQuaternion fixRotation;
        fixRotation.setShortestRotationDamped(setup.m_firstJointIkGain, toEnd, toTarget);

        hkQsTransform* firstJoint = pose.accessBoneModelSpace(setup.m_firstJointIdx, 1);
        hkQuaternion firstRotation(firstJoint->getRotation());
        firstJoint->m_rotation.setMul(fixRotation, firstRotation);
        firstJoint->m_rotation.normalize();
    }
    HK_TIMER_END_LIST_ACCESSORS();
    return true;
}

#pragma pop
