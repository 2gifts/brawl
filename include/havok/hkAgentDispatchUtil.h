#pragma once

#include <havok/hkArray.h>
#include <havok/hkLocalArray.h>
#include <havok/hkCdBody.h>
#include <havok/hkCollisionInput.h>
#include <havok/hkCollisionDispatcher.h>
#include <havok/hkNullAgent.h>

struct hkContactMgr;

// Keeps a list of (key, agent) entries in step with a list of keys (map name hkAgentDispatchUtil<KEY, ENTRY, HELPER>,
// instantiated in hkBvTreeAgent.o for the tree's hit list). Entries whose key is gone have their agent cleaned up;
// each new key gets an agent from the dispatcher, or the null agent when the collision filter rejects the child.
// ENTRY has m_key and m_collisionAgent; HELPER (hkAgentDispatchUtilHelper) turns a key into a child body of B.
// HYPOTHESIS: parameter types (the map has none); the bodies and input are the caller's, passed through unchanged.
// The members are inline: hkBvTreeAgent::prepareCollisionPartners(+LinearCast) inline both, processCollision calls the
// out-of-line copies (emitted right after prepareCollisionPartners).
template <typename KEY, typename ENTRY, typename HELPER>
struct hkAgentDispatchUtil {
    // The keys are in any order: pass 1 removes the entries whose key is no longer in the list, pass 2 inserts an
    // entry in front of the first entry that does not match the next key.
    static void fastUpdate(hkArray<ENTRY>& entries, hkArray<KEY>& keys, const hkCdBody& bodyA, const hkCdBody& bodyB,
                           hkCollisionInput& input, hkContactMgr* mgr, HELPER& helper);
    // The keys are sorted: the entries (sorted the same way) are merged with them into a new list.
    static void update(hkArray<ENTRY>& entries, hkArray<KEY>& keys, const hkCdBody& bodyA, const hkCdBody& bodyB,
                       hkCollisionInput& input, hkContactMgr* mgr, HELPER& helper);
};

template <typename KEY, typename ENTRY, typename HELPER>
inline void hkAgentDispatchUtil<KEY, ENTRY, HELPER>::fastUpdate(hkArray<ENTRY>& entries, hkArray<KEY>& keys,
                                                         const hkCdBody& bodyA, const hkCdBody& bodyB,
                                                         hkCollisionInput& input, hkContactMgr* mgr,
                                                         HELPER& helper) {
    {
        ENTRY* entry = entries.begin();
        ENTRY* entryEnd = entry + entries.m_size;
        KEY* key = keys.begin();
        KEY* keyEnd = key + keys.m_size;
        for (; entry != entryEnd; entry++) {
            if (key != keyEnd && *key == entry->m_key) {
                key++;
                continue;
            }
            for (key = keys.begin(); key != keyEnd; key++) {
                if (*key == entry->m_key) {
                    key++;
                    goto nextEntry;
                }
            }
            entry->m_collisionAgent->cleanup();
            entries.removeAtAndCopy(entry - entries.begin());
            entry--;
            entryEnd--;
        nextEntry:;
        }
    }

    if (keys.getSize() != entries.getSize()) {
        ENTRY* entry = entries.begin();
        ENTRY* entryEnd = entry + entries.m_size;
        KEY* key = keys.begin();
        KEY* keyEnd = key + keys.m_size;
        for (; key != keyEnd; key++) {
            if (entry != entryEnd && *key == entry->m_key) {
                entry++;
                continue;
            }
            ENTRY* newEntry = entries.expandAt(key - keys.begin(), 1);
            const hkCdBody& childBody = helper.getBodyB(*key);
            if (input.m_filter->isCollisionEnabled(input, bodyA, bodyB, *helper.m_collection, *key)) {
                newEntry->m_collisionAgent = input.m_dispatcher->getNewCollisionAgent(bodyA, childBody, input, mgr);
            } else {
                newEntry->m_collisionAgent = hkNullAgent::getNullAgent();
            }
            newEntry->m_key = *key;
            entry = newEntry + 1;
            entryEnd = entries.begin() + entries.m_size;
        }
    }
}

template <typename KEY, typename ENTRY, typename HELPER>
inline void hkAgentDispatchUtil<KEY, ENTRY, HELPER>::update(hkArray<ENTRY>& entries, hkArray<KEY>& keys,
                                                     const hkCdBody& bodyA, const hkCdBody& bodyB,
                                                     hkCollisionInput& input, hkContactMgr* mgr,
                                                     HELPER& helper) {
    KEY* key = keys.begin();
    KEY* keyEnd = key + keys.getSize();
    ENTRY* entry = entries.begin();
    ENTRY* entryEnd = entry + entries.getSize();

    hkLocalArray<ENTRY> newEntries(keys.getSize());
    newEntries.setSize(keys.getSize());
    ENTRY* newEntry = newEntries.begin();

    while (entry != entryEnd && key != keyEnd) {
        if (*key == entry->m_key) {
            *newEntry = *entry;
            key++;
            entry++;
            newEntry++;
        } else if (*key < entry->m_key) {
            const hkCdBody& childBody = helper.getBodyB(*key);
            if (input.m_filter->isCollisionEnabled(input, bodyA, bodyB, *helper.m_collection, *key)) {
                newEntry->m_collisionAgent = input.m_dispatcher->getNewCollisionAgent(bodyA, childBody, input, mgr);
            } else {
                newEntry->m_collisionAgent = hkNullAgent::getNullAgent();
            }
            newEntry->m_key = *key;
            key++;
            newEntry++;
        } else {
            if (entry->m_collisionAgent) {
                entry->m_collisionAgent->cleanup();
            }
            entry++;
        }
    }

    while (entry != entryEnd) {
        if (entry->m_collisionAgent) {
            entry->m_collisionAgent->cleanup();
        }
        entry++;
    }

    while (key != keyEnd) {
        const hkCdBody& childBody = helper.getBodyB(*key);
        if (input.m_filter->isCollisionEnabled(input, bodyA, bodyB, *helper.m_collection, *key)) {
            newEntry->m_collisionAgent = input.m_dispatcher->getNewCollisionAgent(bodyA, childBody, input, mgr);
        } else {
            newEntry->m_collisionAgent = hkNullAgent::getNullAgent();
        }
        newEntry->m_key = *key;
        key++;
        newEntry++;
    }

    entries = newEntries;
}
