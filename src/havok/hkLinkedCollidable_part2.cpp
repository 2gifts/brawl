// Havok translation unit hkLinkedCollidable_part2.o: hkAgentNnMachine_FindAgent.

#include <havok/hkAgentNnMachine.h>
#include <havok/hkLinkedCollidable.h>

// The agent entry of the pair, found in the collision entries of whichever collidable has fewer.
hkAgentNnEntry* hkAgentNnMachine_FindAgent(const hkLinkedCollidable* collA, const hkLinkedCollidable* collB) {
    const hkLinkedCollidable* scanned;
    const hkLinkedCollidable* partner;
    if (collA->m_collisionEntries.getSize() <= collB->m_collisionEntries.getSize()) {
        scanned = collA;
        partner = collB;
    } else {
        scanned = collB;
        partner = collA;
    }
    const hkArray<hkLinkedCollidable::CollisionEntry>& entries = scanned->m_collisionEntries;
    for (int i = 0; i < entries.getSize(); i++) {
        if (entries[i].m_partner == partner) {
            return entries[i].m_agentEntry;
        }
    }
    return NULL;
}
