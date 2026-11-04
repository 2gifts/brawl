#pragma once

#include <havok/hkBase.h>
#include <havok/hkVector4.h>

// Surface material of an entity (0x0C bytes). Layout and names from main.dol's hkMaterialClass.
struct hkMaterial {
    enum ResponseType {
        RESPONSE_INVALID = 0,
        RESPONSE_SIMPLE_CONTACT = 1,
        RESPONSE_REPORTING = 2,
        RESPONSE_NONE = 3,
        RESPONSE_MAX_ID = 4
    };

    s8 m_responseType;    // 0x00 (ResponseType)
    hkReal m_friction;    // 0x04
    hkReal m_restitution; // 0x08
};
