// Havok translation unit hk_weak_802FB894.o (main.dol 0x802FB894-0x802FC6AC): the capsule x triangle agent of the
// agent3 (stream) machine. Its agent data holds three contact point ids and the triangle's distance cache (0x20
// bytes). Functions in address order: registerAgent3, create, sepNormal (0x802FB9A4, not written yet), cleanup,
// removePoint, commitPotential, createZombie, destroy, process (0x802FBF78, not written yet).

#include <havok/hkCapsuleTriangleAgent3.h>
#include <havok/hkCollisionDispatcher.h>
#include <havok/hkCollisionAgent.h>
#include <havok/hkAgent1nTrack.h>
#include <havok/hkShapeType.h>
#include <havok/hkTriangleShape.h>
#include <havok/hkCollideTriangleUtil.h>

// Agent data of one capsule x triangle pair (0x20 bytes).
struct hkCapsuleTriangleAgent3Data {
    u16 m_contactPointId[3];                                       // 0x00
    u8 unk06[2];                                                   // 0x06
    hkCollideTriangleUtil::PointTriangleDistanceCache m_triangleCache; // 0x08
    u8 unk18[8];                                                   // 0x18
};

enum {
    HK_INVALID_CONTACT_POINT = 0xFFFF,
};

// Capsule x triangle, not predictive, no filter, time-of-impact or warp functions.
void hkCapsuleTriangleAgent3::registerAgent3(hkCollisionDispatcher* dispatcher) {
    hkAgent3Funcs f;
    f.m_createFunc = (hkAgentFunc)create;
    f.m_processFunc = (hkAgentFunc)process;
    f.m_sepNormalFunc = (hkAgentFunc)sepNormal;
    f.m_cleanupFunc = (hkAgentFunc)cleanup;
    f.m_removePointFunc = (hkAgentFunc)removePoint;
    f.m_commitPotentialFunc = (hkAgentFunc)commitPotential;
    f.m_createZombieFunc = (hkAgentFunc)createZombie;
    f.m_updateFilterFunc = 0;
    f.m_destroyFunc = (hkAgentFunc)destroy;
    f.m_isPredictive = false;
    dispatcher->registerAgent3(f, HK_SHAPE_CAPSULE, HK_SHAPE_TRIANGLE);
}

void* hkCapsuleTriangleAgent3::create(const hkAgent3Input& input, hkAgentEntry* entry, void* agentData) {
    const hkTriangleShape* triangle = (const hkTriangleShape*)input.m_bodyB->m_shape;
    hkCapsuleTriangleAgent3Data* data = (hkCapsuleTriangleAgent3Data*)agentData;
    entry->m_numContactPoints = 0;
    data->m_contactPointId[0] = HK_INVALID_CONTACT_POINT;
    data->m_contactPointId[1] = HK_INVALID_CONTACT_POINT;
    data->m_contactPointId[2] = HK_INVALID_CONTACT_POINT;
    hkCollideTriangleUtil::setupPointTriangleDistanceCache(&triangle->m_vertexA, data->m_triangleCache);
    return hkAddByteOffset(agentData, sizeof(hkCapsuleTriangleAgent3Data));
}

void* hkCapsuleTriangleAgent3::cleanup(hkAgentEntry* entry, void* agentData, hkContactMgr* contactMgr) {
    hkCapsuleTriangleAgent3Data* data = (hkCapsuleTriangleAgent3Data*)agentData;
    for (int i = 0; i < 3; i++) {
        if (data->m_contactPointId[i] != HK_INVALID_CONTACT_POINT) {
            contactMgr->removeContactPoint(data->m_contactPointId[i]);
            data->m_contactPointId[i] = HK_INVALID_CONTACT_POINT;
        }
    }
    return hkAddByteOffset(agentData, sizeof(hkCapsuleTriangleAgent3Data));
}

void hkCapsuleTriangleAgent3::removePoint(hkAgentEntry* entry, void* agentData, u16 idToRemove) {
    hkCapsuleTriangleAgent3Data* data = (hkCapsuleTriangleAgent3Data*)agentData;
    for (int i = 0; i < 3; i++) {
        if (idToRemove == data->m_contactPointId[i]) {
            data->m_contactPointId[i] = HK_INVALID_CONTACT_POINT;
            return;
        }
    }
}

void hkCapsuleTriangleAgent3::commitPotential(hkAgentEntry* entry, void* agentData, u16 newId) {
    hkCapsuleTriangleAgent3Data* data = (hkCapsuleTriangleAgent3Data*)agentData;
    for (int i = 0; i < 3; i++) {
        if (data->m_contactPointId[i] == HK_INVALID_CONTACT_POINT) {
            data->m_contactPointId[i] = newId;
            return;
        }
    }
}

void hkCapsuleTriangleAgent3::createZombie(hkAgentEntry* entry, void* agentData, u16 idToConvert) {}

void hkCapsuleTriangleAgent3::destroy(hkAgentEntry* entry, void* agentData, hkContactMgr* contactMgr) {
    cleanup(entry, agentData, contactMgr);
}
