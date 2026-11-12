// Havok translation unit hkPredGskCylinderAgent3.o (main.dol 0x803020E0-0x80302C78).
// Not yet decompiled. Functions in address order (method names from the Havok TU map; classes still to be identified):
//   0x803020E0   224  registerAgent3   [map: hkPredGskCylinderAgent3__registerAgent3]
//   0x803021C0   176  create   [map: hkPredGskCylinderAgent3__create]
//   0x80302270  1656  process   [map: hkPredGskCylinderAgent3__process]
//   0x803028E8     4  __ct   [map: hkCdBody____ct]
//   0x803028EC    16  getVertex   [map: hkCylinderShape__getVertex]
//   0x803028FC     8  getTransform   [map: hkCdBody__getTransform]
//   0x80302904    76  resetGskManifold   [map: hkGskManifold__resetGskManifold]
//   0x80302950   312  replaceCylindersWithCapsulesInCdBodies   [map: hkCdBody__replaceCylindersWithCapsulesInCdBodies]
//   0x80302A88    32  __dl   [map: hkShape____dl]
//   0x80302AA8   216  convertCylinderCacheToCapsuleCache   [map: hkGskCache__convertCylinderCacheToCapsuleCache]
//   0x80302B80   180  restoreOriginalShapesInCdBodies   [map: hkCdBody__restoreOriginalShapesInCdBodies]
//   0x80302C34    68  __as   [map: hkGskCache____as]

#include <havok/hkPredGskCylinderAgent3.h>
#include <havok/hkGskManifoldUtil.h>
#include <havok/hkGskCache.h>
#include <havok/hkCollisionDispatcher.h>
#include <havok/hkAgent1nTrack.h>

// The predictive GSK agent's functions with this agent's create and process, for cylinder x convex in both orders.
void hkPredGskCylinderAgent3::registerAgent3(hkCollisionDispatcher* dispatcher) {
    hkAgent3Funcs f;
    f.m_createFunc = (hkAgentFunc)create;
    f.m_processFunc = (hkAgentFunc)process;
    f.m_sepNormalFunc = (hkAgentFunc)hkPredGskAgent3::sepNormal;
    f.m_cleanupFunc = (hkAgentFunc)hkPredGskAgent3::cleanup;
    f.m_removePointFunc = (hkAgentFunc)hkPredGskAgent3::removePoint;
    f.m_commitPotentialFunc = (hkAgentFunc)hkPredGskAgent3::commitPotential;
    f.m_createZombieFunc = (hkAgentFunc)hkPredGskAgent3::createZombie;
    f.m_destroyFunc = (hkAgentFunc)hkPredGskAgent3::destroy;
    f.m_isPredictive = true;
    f.m_ignoreSymmetricVersion = true;
    dispatcher->registerAgent3(f, HK_SHAPE_CYLINDER, HK_SHAPE_CONVEX);
    dispatcher->registerAgent3(f, HK_SHAPE_CONVEX, HK_SHAPE_CYLINDER);
}

void* hkPredGskCylinderAgent3::create(const hkAgent3Input& input, hkAgentEntry* entry, void* agentData) {
    entry->unk4 = 0;
    if (input.m_bodyA->getShape()->getType() == HK_SHAPE_CYLINDER) {
        entry->unk4 |= 1;
    }
    if (input.m_bodyB->getShape()->getType() == HK_SHAPE_CYLINDER) {
        entry->unk4 |= 2;
    }
    return hkPredGskAgent3::create(input, entry, agentData);
}

void hkGskManifold::resetGskManifold(hkGskManifold* manifold, hkGskManifold* dst, void* arg) {
    hkGskManifoldUtil::hkGskManifold_cleanup(manifold, arg);
    dst->m_countC = manifold->m_countC;
    *(u32*)manifold = 0;
}

void hkGskCache::operator=(const hkGskCache& other) {
    m_vertices[0] = other.m_vertices[0];
    m_vertices[1] = other.m_vertices[1];
    m_vertices[2] = other.m_vertices[2];
    m_vertices[3] = other.m_vertices[3];
    m_dimA = other.m_dimA;
    m_dimB = other.m_dimB;
    unkA = other.unkA;
    unkB = other.unkB;
}
