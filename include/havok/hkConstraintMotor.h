#pragma once

#include <havok/hkBase.h>

// Base of the constraint motors (0x0C bytes). Layout and names from main.dol's hkConstraintMotorClass.
struct hkConstraintMotor : hkReferencedObject {
    enum MotorType {
        TYPE_INVALID = 0,
        TYPE_POSITION = 1,
        TYPE_VELOCITY = 2,
        TYPE_SPRING_DAMPER = 3,
        TYPE_MAX = 4
    };

    s8 m_type; // 0x08 (MotorType)
};
