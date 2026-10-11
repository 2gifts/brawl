#pragma once

#include <havok/hkArray.h>
#include <havok/hkThreadMemory.h>

// An array whose storage comes off the thread's scratch stack instead of the heap (0x10 bytes: the usual
// array plus the stack position to give back). If it ever outgrows the stack chunk it moves to normal
// heap memory and the base class frees that like any other array.
template <typename T>
struct hkLocalArray : hkArray<T> {
    void* m_localMemory; // 0x0C

    hkLocalArray(int capacity) {
        hkThreadMemory* tm = hkThreadMemory::s_instance;
        int bytes = (capacity * (int)sizeof(T) + 16) & ~15;
        char* top = tm->m_stackTop;
        char* newTop = top + bytes;
        void* p;
        if (newTop > tm->m_stackEnd) {
            // scratch stack is full, the thread memory finds us somewhere else to live
            p = tm->onStackOverflow(bytes);
        } else {
            tm->m_stackTop = newTop;
            p = top;
        }
        this->m_data = p;
        this->m_size = 0;
        this->m_capacityAndFlags = capacity | hkArrayBase::DONT_DEALLOCATE_FLAG;
        m_localMemory = p;
    }

    ~hkLocalArray() {
        // hand the scratch stack back; hitting the bottom of the block means the next chunk can be let go
        hkThreadMemory* tm = hkThreadMemory::s_instance;
        tm->m_stackTop = (char*)m_localMemory;
        if ((char*)m_localMemory == tm->m_stackBase) {
            tm->onStackUnderflow();
        }
    }
};
