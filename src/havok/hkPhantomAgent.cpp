// Havok translation unit hkPhantomAgent.o (main.dol 0x802C068C-0x802C0A38).
// Functions in address order (method names from the Havok TU map):
//   0x802C068C   140  registerAgent   [map: hkPhantomAgent__registerAgent]
//   0x802C0718   184  __ct   [map: hkPhantomAgent____ct]
//   0x802C07D0   300  createPhantomAgent   [map: hkPhantomAgent__createPhantomAgent]
//   0x802C08FC   152  cleanup   [map: hkPhantomAgent__cleanup]
//   0x802C0994    92  __dt   [map: hkPhantomAgent____dt]
//   0x802C09F0     4  processCollision   [map: hkPhantomAgent__processCollision]
//   0x802C09F4    20  getPenetrations   [map: hkPhantomAgent__getPenetrations]
//   0x802C0A08    32  staticGetPenetrations   [map: hkPhantomAgent__staticGetPenetrations]
//   0x802C0A28     4  getClosestPoints   [map: hkPhantomAgent__getClosestPoints]
//   0x802C0A2C     4  staticGetClosestPoints   [map: hkPhantomAgent__staticGetClosestPoints]
//   0x802C0A30     4  linearCast   [map: hkPhantomAgent__linearCast]
//   0x802C0A34     4  staticLinearCast   [map: hkPhantomAgent__staticLinearCast]

#include <havok/hkPhantomAgent.h>
#include <havok/hkShape.h>
#include <havok/hkCollisionDispatcher.h>

// Registers this agent for phantom callback shapes (type 0x1a) against any other shape type (-1).
void hkPhantomAgent::registerAgent(hkCollisionDispatcher* dispatcher) {
    hkAgentFuncs reg;
    reg.symmetricA = false;
    reg.create = (hkAgentFunc)&hkPhantomAgent::createPhantomAgent;
    reg.staticGetPenetrations = (hkAgentFunc)&hkPhantomAgent::staticGetPenetrations;
    reg.staticGetClosestPoints = (hkAgentFunc)&hkPhantomAgent::staticGetClosestPoints;
    reg.staticLinearCast = (hkAgentFunc)&hkPhantomAgent::staticLinearCast;
    reg.symmetricB = true;
    dispatcher->registerCollisionAgent(&reg, HK_SHAPE_PHANTOM_CALLBACK, -1);
    dispatcher->registerCollisionAgent(&reg, -1, HK_SHAPE_PHANTOM_CALLBACK);
}

// BYTES: the constructor is out of line in the original (createPhantomAgent calls it with bl).
#pragma push
#pragma auto_inline off
hkPhantomAgent::hkPhantomAgent(const hkCdBody& bodyA, const hkCdBody& bodyB, hkContactMgr* contactMgr)
    : hkCollisionAgent((int)contactMgr) {
    m_rootA = bodyA.getRootCollidable();
    m_rootB = bodyB.getRootCollidable();
    m_shapeTypeA = bodyA.m_shape->getType();
    m_shapeTypeB = bodyB.m_shape->getType();
}
#pragma pop

// Creates the agent for a pair of bodies. When a side's shape is a phantom callback shape, that
// shape is told about the pair (slot 0x28) and remembered in the agent.
hkPhantomAgent* hkPhantomAgent::createPhantomAgent(const hkCdBody& bodyA, const hkCdBody& bodyB,
                                                   const hkCollisionInput& input, hkContactMgr* contactMgr) {
    hkPhantomAgent* agent = new hkPhantomAgent(bodyA, bodyB, contactMgr);
    if (agent->m_shapeTypeA == HK_SHAPE_PHANTOM_CALLBACK) {
        hkPhantomCallbackShape* shape = (hkPhantomCallbackShape*)bodyA.m_shape;
        shape->addPairRoots(bodyA.getRootCollidable(), bodyB.getRootCollidable(), input);
        agent->m_phantomA = shape;
    }
    if (agent->m_shapeTypeB == HK_SHAPE_PHANTOM_CALLBACK) {
        hkPhantomCallbackShape* shape = (hkPhantomCallbackShape*)bodyB.m_shape;
        shape->addPairRoots(bodyB.getRootCollidable(), bodyA.getRootCollidable(), input);
        agent->m_phantomB = shape;
    }
    return agent;
}

void hkPhantomAgent::cleanup() {
    if (m_shapeTypeA == HK_SHAPE_PHANTOM_CALLBACK) {
        m_phantomA->removePairRoots(m_rootA, m_rootB);
    }
    if (m_shapeTypeB == HK_SHAPE_PHANTOM_CALLBACK) {
        m_phantomB->removePairRoots(m_rootB, m_rootA);
    }
    delete this;
}

hkPhantomAgent::~hkPhantomAgent() {}

void hkPhantomAgent::processCollision(void* a, void* b, void* c, void* d) {}

void hkPhantomAgent::getPenetrations(void* a, void* b, void* c, void* target) {
    static_cast<hkPenetrationTarget*>(target)->forwardPenetrations(a, b, c);
}

void hkPhantomAgent::staticGetPenetrations(void* a, void* b, void* c, hkPenetrationTarget* target) {
    target->forwardPenetrations(a, b, target);
}

void hkPhantomAgent::getClosestPoints(void* a, void* b, void* c, void* target) {}
void hkPhantomAgent::staticGetClosestPoints() {}
void hkPhantomAgent::linearCast(void* a, void* b, void* c, void* target, void* d) {}
void hkPhantomAgent::staticLinearCast() {}
