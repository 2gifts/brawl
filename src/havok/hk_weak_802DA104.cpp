// Havok translation unit hk_weak_802DA104.o (main.dol 0x802DA104-0x802DA49C): the helpers that allocate, grow,
// shrink and edit a simple contact constraint atom (the contact points and their properties live in the atom).
// HYPOTHESIS: the unit name; the functions are hkSimpleContactConstraintData's atom helpers.

#include <types.h>
#include <havok/hkString.h>
#include <havok/hkThreadMemory.h>
#include <havok/hkSimpleContactConstraintAtom.h>

// Memory class of the atoms (allocated and freed through the thread memory).
enum {
    HK_MEMORY_CLASS_CONTACT_ATOM = 4,
    // Bytes before the 16-byte aligned contact points, rounded with them (0x30).
    HK_CONTACT_ATOM_HEADER_SIZE = 0x30,
};

// Atom with room for numReservedContactPoints points and properties, empty.
hkSimpleContactConstraintAtom* hkSimpleContactConstraintAtomUtil::allocateAtom(int numReservedContactPoints) {
    int size = (numReservedContactPoints * (sizeof(hkContactPoint) + sizeof(hkContactPointProperties)) +
                HK_CONTACT_ATOM_HEADER_SIZE + 15) & ~15;
    hkSimpleContactConstraintAtom* atom =
        (hkSimpleContactConstraintAtom*)hkThreadMemory::s_instance->allocateChunk(size, HK_MEMORY_CLASS_CONTACT_ATOM);
    atom->m_info.init();
    atom->m_type = hkSimpleContactConstraintAtom::TYPE_CONTACT;
    atom->m_sizeOfAllAtoms = size;
    atom->m_numContactPoints = 0;
    atom->m_numReservedContactPoints = numReservedContactPoints;
    return atom;
}

// Copies the info and the points of src into dst (dst has at least as many reserved points).
void hkSimpleContactConstraintAtomUtil::copyContents(hkSimpleContactConstraintAtom* dst,
                                                     const hkSimpleContactConstraintAtom* src) {
    dst->m_info.m_flags = src->m_info.m_flags;
    hkContactPoint* dstPoints = dst->getContactPoints();
    hkContactPointProperties* dstProperties = dst->getContactPointProperties();
    const hkContactPoint* srcPoints = src->getContactPoints();
    dst->m_info.m_index = src->m_info.m_index;
    // MATCH-ONLY: the seven solver words are copied as one block of integers (as init clears them).
    struct SolverWords {
        u32 m_words[7];
    };
    *(SolverWords*)&dst->m_info.m_data0C = *(const SolverWords*)&src->m_info.m_data0C;
    dst->m_numContactPoints = src->m_numContactPoints;
    const hkContactPointProperties* srcProperties = src->getContactPointProperties();
    for (int i = 0; i < src->m_numContactPoints; i++) {
        *dstPoints++ = *srcPoints++;
        *dstProperties++ = *srcProperties++;
    }
}

void hkSimpleContactConstraintAtomUtil::deallocateAtom(hkSimpleContactConstraintAtom* atom) {
    hkThreadMemory::s_instance->deallocateChunk(atom, atom->m_sizeOfAllAtoms, HK_MEMORY_CLASS_CONTACT_ATOM);
}

// Adds a point (its properties cleared); a full atom is replaced by one with twice the room.
hkSimpleContactConstraintAtom* hkSimpleContactConstraintAtomUtil::expandOne(hkSimpleContactConstraintAtom* atom) {
    hkSimpleContactConstraintAtom* result = atom;
    int numContactPoints = result->m_numContactPoints + 1;
    result->m_numContactPoints = numContactPoints;
    if ((u16)numContactPoints > result->m_numReservedContactPoints) {
        result = allocateAtom((u16)numContactPoints * 2);
        copyContents(result, atom);
        deallocateAtom(atom);
    }
    hkString::memSet(&result->getContactPointProperties()[result->m_numContactPoints - 1], 0,
                     sizeof(hkContactPointProperties));
    return result;
}

// Removes the point at index, moving the later points and properties down one place.
void hkSimpleContactConstraintAtomUtil::removeAtAndCopy(hkSimpleContactConstraintAtom* atom, int index) {
    atom->m_numContactPoints--;
    hkContactPoint* points = atom->getContactPoints();
    hkContactPointProperties* properties = atom->getContactPointProperties();
    points += index;
    properties += index;
    for (int i = index; i < atom->m_numContactPoints; i++) {
        points[0] = points[1];
        points++;
        properties[0] = properties[1];
        properties++;
    }
}

// Replaces the atom by one with half the room once fewer than half of its points (plus numFreeElemsLeft) are used.
hkSimpleContactConstraintAtom* hkSimpleContactConstraintAtomUtil::optimizeCapacity(hkSimpleContactConstraintAtom* atom,
                                                                                   int numFreeElemsLeft) {
    hkSimpleContactConstraintAtom* result = atom;
    u16 numReserved = atom->m_numReservedContactPoints;
    if ((atom->m_numContactPoints + numFreeElemsLeft) * 2 <= numReserved) {
        result = allocateAtom(numReserved >> 1);
        copyContents(result, atom);
        deallocateAtom(atom);
    }
    return result;
}
