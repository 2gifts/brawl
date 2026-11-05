#pragma once

#include <havok/hkArray.h>
#include <havok/hkThreadMemory.h>

// An array whose storage comes off the thread's scratch stack instead of the heap (0x10 bytes: the usual
// array plus the stack position to give back). If it ever outgrows the stack chunk it moves to normal
// heap memory and the base class frees that like any other array.
template <typename T>
struct hkLocalArray : hkArray<T> {
    // The functions holding one have copy propagation off in the target; that comes from the inlined
    // hkThreadMemory::allocateStack (see the comment there), not from this class.
    void* m_localMemory; // 0x0C

    // The base constructor leaves the array empty; the storage is a block off the thread's stack.
    hkLocalArray(int capacity) {
        void* p = hkThreadMemory::s_instance->allocateStack(capacity * sizeof(T));
        this->m_data = p;
        this->m_capacityAndFlags = capacity | hkArrayBase::DONT_DEALLOCATE_FLAG;
        m_localMemory = p;
    }

    ~hkLocalArray() {
        hkThreadMemory::s_instance->deallocateStack(m_localMemory);
    }
};
