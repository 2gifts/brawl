// Havok translation unit hkTypedBroadPhaseDispatcher.o (main.dol 0x802CAA50-0x802CB41C).
// The agent3 bridge (an agent3 registration that runs a classic hkCollisionAgent stored in the entry) comes first,
// then the typed broad-phase dispatcher and its null listener.
// HYPOTHESIS: the bridge functions belong to this unit (no separate unit boundary is visible in the image).

#include <types.h>
#include <havok/hkTypedBroadPhaseDispatcher.h>
#include <havok/hkCollisionDispatcher.h>
#include <havok/hkCollisionAgent.h>
#include <havok/hkCollisionFilter.h>
#include <havok/hkAgent1nTrack.h>
#include <havok/hkCollidable.h>
#include <havok/hkBroadPhase.h>
#include <havok/hkShape.h>
#include <havok/hkShapeType.h>
#include <havok/hkPointerMapBase.h>

// Runs a classic collision agent through the agent3 stream: the agent pointer lives in the entry's 0x04 word.
struct hkAgent3Bridge {
    static void registerAgent3(hkCollisionDispatcher* dispatcher);
    static void* create(const hkAgent3Input& input, hkAgentEntry* entry, void* agentData);
    static void* process(const hkAgent3ProcessInput& input, hkAgentEntry* entry, void* agentData,
                         hkVector4* separatingNormal, void* result);
    static void destroy(hkAgentEntry* entry);
    static void updateFilter(hkAgentEntry* entry, void* agentData, const hkCdBody& bodyA, const hkCdBody& bodyB,
                             hkCollisionInput& input);
    static void invalidateTim(hkAgentEntry* entry, void* agentData, void* input);
    static void warpTime(hkAgentEntry* entry, void* agentData, hkReal oldTime, hkReal newTime, void* input);
    static void removePoint(hkAgentEntry* entry, void* agentData, u16 idToRemove);
    static void commitPotential(hkAgentEntry* entry, void* agentData, u16 newId);
    static void createZombie(hkAgentEntry* entry, void* agentData, u16 idToConvert);

    static hkCollisionAgent* getAgent(hkAgentEntry* entry) {
        return (hkCollisionAgent*)entry->unk4;
    }
};

// Stream command of a bridged agent entry.
enum {
    HK_AGENT3_BRIDGE_COMMAND = 6,
};

// Any shape pair, predictive.
void hkAgent3Bridge::registerAgent3(hkCollisionDispatcher* dispatcher) {
    hkAgent3Funcs f;
    f.m_commitPotentialFunc = (hkAgentFunc)commitPotential;
    f.m_createZombieFunc = (hkAgentFunc)createZombie;
    f.m_removePointFunc = (hkAgentFunc)removePoint;
    f.m_createFunc = (hkAgentFunc)create;
    f.m_processFunc = (hkAgentFunc)process;
    f.m_sepNormalFunc = 0;
    f.m_cleanupFunc = 0;
    f.m_destroyFunc = (hkAgentFunc)destroy;
    f.m_updateFilterFunc = (hkAgentFunc)updateFilter;
    f.m_invalidateTimFunc = (hkAgentFunc)invalidateTim;
    f.m_warpTimeFunc = (hkAgentFunc)warpTime;
    f.m_isPredictive = true;
    dispatcher->registerAgent3(f, HK_SHAPE_ALL, HK_SHAPE_ALL);
}

void* hkAgent3Bridge::create(const hkAgent3Input& input, hkAgentEntry* entry, void* agentData) {
    hkCollisionAgent* agent = input.m_input->m_dispatcher->getNewCollisionAgent(*input.m_bodyA, *input.m_bodyB,
                                                               *(hkCollisionInput*)input.m_input, input.m_contactMgr);
    entry->unk4 = (u32)agent;
    entry->unk0 = HK_AGENT3_BRIDGE_COMMAND;
    entry->m_numContactPoints = 0xff;
    return agentData;
}

void* hkAgent3Bridge::process(const hkAgent3ProcessInput& input, hkAgentEntry* entry, void* agentData,
                              hkVector4* separatingNormal, void* result) {
    getAgent(entry)->processCollision(*input.m_bodyA, *input.m_bodyB, *(hkProcessCollisionInput*)input.m_input,
                                      result);
    return agentData;
}

void hkAgent3Bridge::destroy(hkAgentEntry* entry) {
    getAgent(entry)->cleanup();
}

void hkAgent3Bridge::updateFilter(hkAgentEntry* entry, void* agentData, const hkCdBody& bodyA, const hkCdBody& bodyB,
                                  hkCollisionInput& input) {
    getAgent(entry)->updateShapeCollectionFilter(bodyA, bodyB, input);
}

void hkAgent3Bridge::invalidateTim(hkAgentEntry* entry, void* agentData, void* input) {
    getAgent(entry)->invalidateTim(input);
}

void hkAgent3Bridge::warpTime(hkAgentEntry* entry, void* agentData, hkReal oldTime, hkReal newTime, void* input) {
    getAgent(entry)->warpTime(oldTime, newTime, input);
}

void hkAgent3Bridge::removePoint(hkAgentEntry* entry, void* agentData, u16 idToRemove) {
    getAgent(entry)->removePoint(idToRemove);
}

void hkAgent3Bridge::commitPotential(hkAgentEntry* entry, void* agentData, u16 newId) {
    getAgent(entry)->commitPotential(newId);
}

void hkAgent3Bridge::createZombie(hkAgentEntry* entry, void* agentData, u16 idToConvert) {
    getAgent(entry)->createZombie(idToConvert);
}

void hkNullBroadPhaseListener::addCollisionPair(hkBroadPhaseHandlePair& pair) {}

void hkNullBroadPhaseListener::removeCollisionPair(hkBroadPhaseHandlePair& pair) {}

hkNullBroadPhaseListener::~hkNullBroadPhaseListener() {}

hkTypedBroadPhaseDispatcher::hkTypedBroadPhaseDispatcher() {
    m_nullBroadPhaseListener = new hkNullBroadPhaseListener();
    for (int a = 0; a < HK_MAX_BROAD_PHASE_TYPES; a++) {
        for (int b = 0; b < HK_MAX_BROAD_PHASE_TYPES; b++) {
            m_broadPhaseListeners[a][b] = m_nullBroadPhaseListener;
        }
    }
}

hkTypedBroadPhaseDispatcher::~hkTypedBroadPhaseDispatcher() {
    delete m_nullBroadPhaseListener;
}

static inline const hkCollidable* collidableOfHandle(const hkTypedBroadPhaseHandle* handle) {
    return (const hkCollidable*)((const char*)handle + handle->m_ownerOffset);
}

// Hands each pair the filter lets through to the listener registered for the two handle types.
void hkTypedBroadPhaseDispatcher::addPairs(hkBroadPhaseHandlePair* pairs, int numPairs,
                                           const hkCollidableCollidableFilter* filter) {
    while (--numPairs >= 0) {
        const hkCollidable* collA = collidableOfHandle(pairs->m_a);
        const hkCollidable* collB = collidableOfHandle(pairs->m_b);
        if (filter->isCollisionEnabled(*collA, *collB)) {
            m_broadPhaseListeners[pairs->m_a->m_type][pairs->m_b->m_type]->addCollisionPair(*pairs);
        }
        pairs++;
    }
}

void hkTypedBroadPhaseDispatcher::removePairs(hkBroadPhaseHandlePair* pairs, int numPairs) {
    while (--numPairs >= 0) {
        m_broadPhaseListeners[pairs->m_a->m_type][pairs->m_b->m_type]->removeCollisionPair(*pairs);
        pairs++;
    }
}

// Key of a pair independent of the order of its handles: the lower handle address in the high word.
union hkBroadPhasePairKey {
    hkTypedBroadPhaseHandle* m_handles[2];
    u64 m_key;
};

static inline hkBool isValidIndex(const hkPointerMapBase<u64>& map, int index) {
    return index <= map.m_hashMod;
}

static inline u64 getValue(const hkPointerMapBase<u64>& map, int index) {
    return map.m_elem[index + map.m_hashMod + 1];
}

static inline void setValue(hkPointerMapBase<u64>& map, int index, u64 value) {
    map.m_elem[index + map.m_hashMod + 1] = value;
}

// Pairs that were both added and removed in the same update cancel out. Short lists are compared directly; longer
// ones go through a map from the pair key to (index in newPairs << 8 | count).
void hkTypedBroadPhaseDispatcher::removeDuplicates(hkArray<hkBroadPhaseHandlePair>& newPairs,
                                                   hkArray<hkBroadPhaseHandlePair>& delPairs) {
    int shorter = newPairs.getSize() < delPairs.getSize() ? newPairs.getSize() : delPairs.getSize();
    if (shorter < 32) {
        for (int d = 0; d < delPairs.getSize(); d++) {
            for (int n = 0; n < newPairs.getSize(); n++) {
                hkBroadPhaseHandlePair& a = newPairs[n];
                hkBroadPhaseHandlePair& b = delPairs[d];
                if ((a.m_a == b.m_a && a.m_b == b.m_b) || (a.m_b == b.m_a && a.m_a == b.m_b)) {
                    newPairs.removeAt(n);
                    delPairs.removeAt(d);
                    d--;
                    break;
                }
            }
        }
        return;
    }

    hkPointerMapBase<u64> map;
    map.reserve(newPairs.getSize());
    for (int i = 0; i < newPairs.getSize(); i++) {
        hkBroadPhasePairKey key;
        key.m_handles[0] = newPairs[i].m_a;
        key.m_handles[1] = newPairs[i].m_b;
        if ((u32)key.m_handles[0] > (u32)key.m_handles[1]) {
            key.m_handles[0] = newPairs[i].m_b;
            key.m_handles[1] = newPairs[i].m_a;
        }
        u64 pairKey = key.m_key;
        int index = map.findKey(pairKey);
        if (isValidIndex(map, index)) {
            setValue(map, index, getValue(map, index) + 1);
            newPairs[i].m_a = 0;
        } else {
            map.insert(pairKey, (i << 8) | 1);
        }
    }

    for (int i = 0; i < delPairs.getSize(); i++) {
        hkBroadPhasePairKey key;
        key.m_handles[0] = delPairs[i].m_a;
        key.m_handles[1] = delPairs[i].m_b;
        if ((u32)key.m_handles[0] > (u32)key.m_handles[1]) {
            key.m_handles[0] = delPairs[i].m_b;
            key.m_handles[1] = delPairs[i].m_a;
        }
        int index = map.findKey(key.m_key);
        if (isValidIndex(map, index)) {
            u64 value = getValue(map, index);
            if ((int)(value & 0xff) > 1) {
                setValue(map, index, value - 1);
            } else {
                map.remove(index);
                newPairs[(u32)value >> 8].m_a = 0;
            }
            delPairs.removeAt(i);
            i--;
        }
    }

    int numKept = 0;
    for (int i = 0; i < newPairs.getSize(); i++) {
        if (newPairs[i].m_a != 0) {
            newPairs[numKept] = newPairs[i];
            numKept++;
        }
    }
    newPairs.setSize(numKept);
}
