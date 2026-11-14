// Havok translation unit hkShapeCollectionAgent.o (main.dol 0x802C0A38-0x802C23B8).
// Functions in address order:
//   0x802C0A38   204  registerAgent
//   0x802C0B04   124  createListAAgent
//   0x802C0B80   132  createListBAgent
//   0x802C0C04   152  __dt
//   0x802C0D2C   552  __ct
//   0x802C0F54   148  cleanup
//   0x802C0FE8   132  invalidateTim
//   0x802C106C   164  warpTime
//   0x802C1110   352  processCollision
//   0x802C1270   352  getClosestPoints
//   0x802C13D0   512  staticGetClosestPoints
//   0x802C15D0   360  linearCast
//   0x802C1738   520  staticLinearCast
//   0x802C1940   376  getPenetrations
//   0x802C1AB8   536  staticGetPenetrations
//   0x802C1CD0   684  updateShapeCollectionFilter

#include <havok/hkShapeCollectionAgent.h>
#include <havok/hkCollisionDispatcher.h>
#include <havok/hkShapeType.h>
#include <havok/hkShapeCollection.h>
#include <havok/hkMonitorStream.h>
#include <havok/hkCdBody.h>
#include <havok/hkCdBodyPairCollector.h>
#include <havok/hkCollisionInput.h>
#include <havok/hkBvAgent.h>

// Stand-ins for functions in other TUs or not yet recovered (the names are the map names).
// Dispatcher registration (not recovered yet).

// HYPOTHESIS: registration record passed to the collision dispatcher (same layout as hkPhantomAgent.cpp).

hkShapeCollectionAgent* hkShapeCollectionAgent::createListAAgent(void* a, void* b, void* c, int d) {
    hkShapeCollectionAgent* agent = (hkShapeCollectionAgent*)hkMemory::s_instance->allocateChunk(sizeof(hkShapeCollectionAgent), 0x1d);
    agent->m_memSizeAndFlags = sizeof(hkShapeCollectionAgent);
    new (agent) hkShapeCollectionAgent(*(const hkCdBody*)a, *(const hkCdBody*)b, *(hkCollisionInput*)c,
                                       (hkContactMgr*)d);
    return agent;
}

hkShapeCollectionAgent* hkShapeCollectionAgent::createListBAgent(void* a, void* b, void* c, int d) {
    hkShapeCollectionAgent* agent = (hkShapeCollectionAgent*)hkMemory::s_instance->allocateChunk(sizeof(hkShapeCollectionAgent), 0x1d);
    agent->m_memSizeAndFlags = sizeof(hkShapeCollectionAgent);
    agent = new (agent) hkShapeCollectionAgentVariant(*(const hkCdBody*)b, *(const hkCdBody*)a,
                                                      *(hkCollisionInput*)c, (hkContactMgr*)d);
    return agent;
}

// Registers the agent for shape collection x any shape (-1) and the reverse.
void hkShapeCollectionAgent::registerAgent(void* dispatcher) {
    hkAgentFuncs regAB;
    regAB.create = (hkAgentFunc)&hkShapeCollectionAgent::createListBAgent;
    regAB.staticGetPenetrations = (hkAgentFunc)hkSymmetricAgentLinearCast<hkShapeCollectionAgent>::staticGetPenetrations;
    regAB.staticGetClosestPoints = (hkAgentFunc)hkSymmetricAgentLinearCast<hkShapeCollectionAgent>::staticGetClosestPoints;
    regAB.staticLinearCast = (hkAgentFunc)hkSymmetricAgentLinearCast<hkShapeCollectionAgent>::staticLinearCast;
    regAB.symmetricA = true;
    regAB.symmetricB = true;
    ((hkCollisionDispatcher*)dispatcher)->registerCollisionAgent(&regAB, HK_SHAPE_ALL, HK_SHAPE_COLLECTION);
    hkAgentFuncs regBA;
    regBA.create = (hkAgentFunc)&hkShapeCollectionAgent::createListAAgent;
    regBA.staticGetPenetrations = (hkAgentFunc)staticGetPenetrations;
    regBA.staticGetClosestPoints = (hkAgentFunc)staticGetClosestPoints;
    regBA.staticLinearCast = (hkAgentFunc)staticLinearCast;
    regBA.symmetricA = false;
    regBA.symmetricB = true;
    ((hkCollisionDispatcher*)dispatcher)->registerCollisionAgent(&regBA, HK_SHAPE_COLLECTION, HK_SHAPE_ALL);
}

hkShapeCollectionAgent::hkShapeCollectionAgent(const hkCdBody& bodyA, const hkCdBody& bodyB,
                                               hkCollisionInput& input, hkContactMgr* mgr)
    : hkCollisionAgent((int)mgr) {
    const hkShapeCollection* collection = (const hkShapeCollection*)bodyA.m_shape;
    hkCdBody childBody(&bodyA);
    hkShapeContainer::ShapeBuffer shapeBuffer;
    int numChildren = collection->getNumChildShapes();
    m_agents.reserveSmart(numChildren);
    u32 key = collection->getFirstKey();
    for (int i = 0; i < numChildren; i++) {
        childBody.m_shape = (hkShape*)collection->getChildShape(key, shapeBuffer);
        childBody.m_shapeKey = key;
        if (input.m_filter->isCollisionEnabled(input, bodyB, bodyA, *collection, key)) {
            hkShapeCollectionAgentKeyAgentPair* pair = m_agents.expandByUnchecked(1);
            pair->m_agent = input.m_dispatcher->getNewCollisionAgent(childBody, bodyB, input, mgr);
            pair->m_key = key;
        }
        key = collection->getNextKey(key);
    }
}

hkShapeCollectionAgent::~hkShapeCollectionAgent() {}

void hkShapeCollectionAgent::cleanup() {
    for (int i = 0; i < m_agents.getSize(); i++) {
        m_agents[i].m_agent->cleanup();
    }
    delete this;
}

void hkShapeCollectionAgent::invalidateTim(void* arg) {
    for (int i = 0; i < m_agents.getSize(); i++) {
        m_agents[i].m_agent->invalidateTim(arg);
    }
}

void hkShapeCollectionAgent::warpTime(float t0, float t1, void* arg) {
    for (int i = 0; i < m_agents.getSize(); i++) {
        m_agents[i].m_agent->warpTime(t0, t1, arg);
    }
}

// Runs every child agent on its child shape of the collection (body A) against body B.
void hkShapeCollectionAgent::processCollision(const hkCdBody& unk0, const hkCdBody& unk1, hkProcessCollisionInput& unk2, void* unk3) {
    HK_TIMER_BEGIN("ShapeCollection");
    const hkCdBody& bodyA = unk0;
    const hkShapeCollection* collection = (const hkShapeCollection*)bodyA.m_shape;
    hkCdBody childBody(&bodyA);
    hkShapeContainer::ShapeBuffer shapeBuffer;
    hkShapeCollectionAgentKeyAgentPair* pair = m_agents.begin();
    for (int i = m_agents.m_size - 1; i >= 0; i--) {
        u32 key = pair->m_key;
        const u32& keyRef = key; // MATCH-ONLY: the reference keeps the key in a callee-saved register
        childBody.m_shape = (hkShape*)collection->getChildShape(keyRef, shapeBuffer);
        childBody.m_shapeKey = key;
        pair->m_agent->processCollision(childBody, unk1, unk2, unk3);
        pair++;
    }
    HK_TIMER_END();
}

void hkShapeCollectionAgent::getClosestPoints(const hkCdBody& a, const hkCdBody& b, hkCollisionInput& c, void* target) {
    const hkCdBody& bodyA = a;
    HK_TIMER_BEGIN("ShapeCollection");
    const hkShapeCollection* collection = (const hkShapeCollection*)bodyA.m_shape;
    hkCdBody childBody(&bodyA);
    hkShapeContainer::ShapeBuffer shapeBuffer;
    hkShapeCollectionAgentKeyAgentPair* pair = m_agents.begin();
    for (int i = m_agents.m_size - 1; i >= 0; i--) {
        u32 key = pair->m_key;
        const u32& keyRef = key; // MATCH-ONLY: the reference keeps the key in a callee-saved register
        childBody.m_shape = (hkShape*)collection->getChildShape(keyRef, shapeBuffer);
        childBody.m_shapeKey = key;
        pair->m_agent->getClosestPoints(childBody, b, c, target);
        pair++;
    }
    HK_TIMER_END();
}

// Without agents: every child the filter accepts goes to the dispatcher's static function for its shape types.
void hkShapeCollectionAgent::staticGetClosestPoints(const hkCdBody& bodyA, const hkCdBody& bodyB,
                                                    hkCollisionInput& input, void* target) {
    HK_TIMER_BEGIN("ShapeCollection");
    const hkShapeCollection* collection = (const hkShapeCollection*)bodyA.m_shape;
    hkCdBody childBody(&bodyA);
    hkShapeContainer::ShapeBuffer shapeBuffer;
    int typeB = bodyB.m_shape->getType();
    for (u32 key = collection->getFirstKey(); key != 0xFFFFFFFF;
         key = collection->getNextKey(key)) {
        if (input.m_filter->isCollisionEnabled(input, bodyB, bodyA, *collection, key)) {
            childBody.m_shape = (hkShape*)collection->getChildShape(key, shapeBuffer);
            childBody.m_shapeKey = key;
            int typeA = childBody.m_shape->getType();
            typedef void (*QueryFunc)(const hkCdBody&, const hkCdBody&, hkCollisionInput&, void*);
            ((QueryFunc)input.m_dispatcher->getGetClosestPointsFunc(typeA, typeB))(childBody, bodyB, input, target);
        }
    }
    HK_TIMER_END();
}

void hkShapeCollectionAgent::linearCast(const hkCdBody& a, const hkCdBody& b, hkLinearCastCollisionInput& c, void* target, void* d) {
    const hkCdBody& bodyA = a;
    HK_TIMER_BEGIN("ShapeCollection");
    const hkShapeCollection* collection = (const hkShapeCollection*)bodyA.m_shape;
    hkCdBody childBody(&bodyA);
    hkShapeContainer::ShapeBuffer shapeBuffer;
    hkShapeCollectionAgentKeyAgentPair* pair = m_agents.begin();
    for (int i = m_agents.m_size - 1; i >= 0; i--) {
        u32 key = pair->m_key;
        const u32& keyRef = key; // MATCH-ONLY: the reference keeps the key in a callee-saved register
        childBody.m_shape = (hkShape*)collection->getChildShape(keyRef, shapeBuffer);
        childBody.m_shapeKey = key;
        pair->m_agent->linearCast(childBody, b, c, target, d);
        pair++;
    }
    HK_TIMER_END();
}

void hkShapeCollectionAgent::staticLinearCast(const hkCdBody& bodyA, const hkCdBody& bodyB,
                                              hkLinearCastCollisionInput& input, void* target, void* d) {
    HK_TIMER_BEGIN("ShapeCollection");
    const hkShapeCollection* collection = (const hkShapeCollection*)bodyA.m_shape;
    hkCdBody childBody(&bodyA);
    hkShapeContainer::ShapeBuffer shapeBuffer;
    int typeB = bodyB.m_shape->getType();
    for (u32 key = collection->getFirstKey(); key != 0xFFFFFFFF;
         key = collection->getNextKey(key)) {
        if (input.m_filter->isCollisionEnabled(input, bodyB, bodyA, *collection, key)) {
            childBody.m_shape = (hkShape*)collection->getChildShape(key, shapeBuffer);
            childBody.m_shapeKey = key;
            int typeA = childBody.m_shape->getType();
            typedef void (*LinearCastFunc)(const hkCdBody&, const hkCdBody&, hkLinearCastCollisionInput&, void*, void*);
            ((LinearCastFunc)input.m_dispatcher->getLinearCastFunc(typeA, typeB))(childBody, bodyB, input, target, d);
        }
    }
    HK_TIMER_END();
}

// Stops at the first child once the collector has seen enough.
void hkShapeCollectionAgent::getPenetrations(const hkCdBody& a, const hkCdBody& b, hkCollisionInput& c, void* target) {
    const hkCdBody& bodyA = a;
    HK_TIMER_BEGIN("ShapeCollection");
    const hkShapeCollection* collection = (const hkShapeCollection*)bodyA.m_shape;
    hkCdBody childBody(&bodyA);
    hkShapeContainer::ShapeBuffer shapeBuffer;
    hkShapeCollectionAgentKeyAgentPair* pair = m_agents.begin();
    for (int i = m_agents.m_size - 1; i >= 0; i--) {
        u32 key = pair->m_key;
        const u32& keyRef = key; // MATCH-ONLY: the reference keeps the key in a callee-saved register
        childBody.m_shape = (hkShape*)collection->getChildShape(keyRef, shapeBuffer);
        childBody.m_shapeKey = key;
        pair->m_agent->getPenetrations(childBody, b, c, target);
        if (((hkCdBodyPairCollector*)target)->m_earlyOut) {
            break;
        }
        pair++;
    }
    HK_TIMER_END();
}

void hkShapeCollectionAgent::staticGetPenetrations(const hkCdBody& bodyA, const hkCdBody& bodyB,
                                                   hkCollisionInput& input, void* target) {
    HK_TIMER_BEGIN("ShapeCollection");
    const hkShapeCollection* collection = (const hkShapeCollection*)bodyA.m_shape;
    hkCdBody childBody(&bodyA);
    hkShapeContainer::ShapeBuffer shapeBuffer;
    int typeB = bodyB.m_shape->getType();
    for (u32 key = collection->getFirstKey(); key != 0xFFFFFFFF;
         key = collection->getNextKey(key)) {
        if (input.m_filter->isCollisionEnabled(input, bodyB, bodyA, *collection, key)) {
            childBody.m_shape = (hkShape*)collection->getChildShape(key, shapeBuffer);
            childBody.m_shapeKey = key;
            int typeA = childBody.m_shape->getType();
            typedef void (*QueryFunc)(const hkCdBody&, const hkCdBody&, hkCollisionInput&, void*);
            ((QueryFunc)input.m_dispatcher->getGetPenetrationsFunc(typeA, typeB))(childBody, bodyB, input, target);
            if (((hkCdBodyPairCollector*)target)->m_earlyOut) {
                break;
            }
        }
    }
    HK_TIMER_END();
}

// Index of the child agent for a key, -1 when the child has none.
static inline int findAgent(const hkArray<hkShapeCollectionAgentKeyAgentPair>& agents, u32 key) {
    for (int i = 0; i < agents.getSize(); i++) {
        if (agents[i].m_key == key) {
            return i;
        }
    }
    return -1;
}

// Brings the child agents in line with the filter: new agents for children that now collide, cleanup for those
// that no longer do, and the update passed on to the others.
void hkShapeCollectionAgent::updateShapeCollectionFilter(const hkCdBody& bodyA, const hkCdBody& bodyB,
                                                         hkCollisionInput& input) {
    const hkShapeCollection* collection = (const hkShapeCollection*)bodyA.m_shape;
    hkCdBody childBody(&bodyA);
    hkShapeContainer::ShapeBuffer shapeBuffer;
    int numChildren = collection->getNumChildShapes();
    u32 key = collection->getFirstKey();
    for (int i = 0; i < numChildren; i++) {
        int index = findAgent(m_agents, key);
        if (input.m_filter->isCollisionEnabled(input, bodyB, bodyA, *collection, key)) {
            childBody.m_shape = (hkShape*)collection->getChildShape(key, shapeBuffer);
            childBody.m_shapeKey = key;
            if (index == -1) {
                hkShapeCollectionAgentKeyAgentPair& pair = m_agents.expandOne();
                pair.m_key = key;
                pair.m_agent = input.m_dispatcher->getNewCollisionAgent(childBody, bodyB, input, (hkContactMgr*)unk8);
            } else {
                m_agents[index].m_agent->updateShapeCollectionFilter(childBody, bodyB, input);
            }
        } else if (index != -1) {
            m_agents[index].m_agent->cleanup();
            m_agents.removeAt(index);
        }
        key = collection->getNextKey(key);
    }
}

#include <havok/hkShapeCollectionAgent.h>
#include <havok/hkCollisionInput.h>
#include <havok/hkSymmetricAgentFlipCollectors.h>
#include <havok/hkSymmetricAgentProcessCollision.h>
#include <havok/hkBvAgent.h>

// Symmetric wrapper of hkShapeCollectionAgent (map name hkSymmetricAgentLinearCast_22hkShapeCollectionAgent___
// updateShapeCollectionFilter): swap the bodies and forward to the plain agent.
template <class T>
void hkSymmetricAgentLinearCast<T>::updateShapeCollectionFilter(const hkCdBody& a, const hkCdBody& b, hkCollisionInput& c) {
    T::updateShapeCollectionFilter(b, a, c);
}

template void hkSymmetricAgentLinearCast<hkShapeCollectionAgent>::updateShapeCollectionFilter(const hkCdBody&, const hkCdBody&, hkCollisionInput&);
template void hkSymmetricAgentLinearCast<hkShapeCollectionAgent>::processCollision(const hkCdBody&, const hkCdBody&, hkProcessCollisionInput&, void*);

template <class T>
void hkSymmetricAgentLinearCast<T>::getPenetrations(const hkCdBody& a, const hkCdBody& b, hkCollisionInput& c, void* target) {
    hkSymmetricAgentFlipBodyCollector wrapped(target);
    T::getPenetrations(b, a, c, &wrapped);
}

template <class T>
void hkSymmetricAgentLinearCast<T>::getClosestPoints(const hkCdBody& a, const hkCdBody& b, hkCollisionInput& c, void* target) {
    hkSymmetricAgentFlipCollector wrapped(target);
    T::getClosestPoints(b, a, c, &wrapped);
}

template void hkSymmetricAgentLinearCast<hkShapeCollectionAgent>::getPenetrations(const hkCdBody&, const hkCdBody&, hkCollisionInput&, void*);
template void hkSymmetricAgentLinearCast<hkShapeCollectionAgent>::getClosestPoints(const hkCdBody&, const hkCdBody&, hkCollisionInput&, void*);

// The linear cast with the bodies swapped: the path is negated and each collector is wrapped in one that turns
// the points back.
void hkShapeCollectionAgentVariant::linearCast(const hkCdBody& a, const hkCdBody& b, hkLinearCastCollisionInput& c, void* target, void* d) {
    const hkLinearCastCollisionInput& input = c;
    hkLinearCastCollisionInput flippedInput = input;
    flippedInput.m_path.setNeg4(input.m_path);
    hkSymmetricAgentFlipCastCollector flipped(input.m_path, target);
    if (d != 0) {
        hkSymmetricAgentFlipCastCollector flippedStart(input.m_path, d);
        hkShapeCollectionAgent::linearCast(b, a, flippedInput, &flipped, &flippedStart);
    } else {
        hkShapeCollectionAgent::linearCast(b, a, flippedInput, &flipped, 0);
    }
}

// fn_802C2318: body from https://github.com/jj-link/brawl src/havok/fn_802C2318.cpp (namespaced; memory singletons rebound to our names).
#include <types.h>
#include <havok/hkMemory.h>
#include <havok/hkThreadMemory.h>
#include <havok/hkArray.h>
namespace jj_fn_802C2318 {
typedef unsigned int u32;


class SoHeapAllocator {
public:
    virtual void vf0();
    virtual void vf1();
    virtual void vf2();
    virtual void vf3();
    virtual void vf4();
    virtual void free(void*, unsigned short, int);
};


struct Obj {
    void* vtable;
    unsigned short type;
    char pad[6];
    void* data;
    char pad2[4];
    unsigned int raw;

    bool isOwned() const {
        return (raw & 0x80000000) == 0;
    }

    void dealloc() {
        hkThreadMemory::s_instance->deallocateChunk(data, raw << 3, 0x15);
    }
};

// TODO: cleanup (shape from jj-link/brawl fn_802C2318)
extern "C" void* fn_802C2318(Obj* self, int flag) {
    if (self) {
        if ((u32)self && (u32)self && (u32)self + 0xc && (u32)self + 0xc && self->isOwned()) {
            self->dealloc();
        }
        if (flag > 0) {
            ((SoHeapAllocator*)hkMemory::s_instance)->free(self, self->type, 0x1d);
        }
    }
    return self;
}
} // namespace jj_fn_802C2318
