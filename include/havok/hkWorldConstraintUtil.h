#pragma once

#include <havok/hkBase.h>
#include <havok/hkArray.h>
#include <havok/hkWorld.h>
#include <havok/hkConstraintInternal.h>

struct hkConstraintInstance;

// Constraint add/remove work (map names hkWorldConstraintUtil__addConstraint and __removeConstraint).
struct hkWorldConstraintUtil {
    static void addConstraint(hkWorld* world, hkConstraintInstance* constraint);
    static void removeConstraint(hkConstraintInstance* constraint);
};
