#pragma once

#include <types.h>

class utRef {
public:
    u8 m_count; // HYPOTHESIS: reference count
    utRef() : m_count(0) { }
    virtual ~utRef();

    // HYPOTHESIS: inlined reference decrement
    bool release() {
        if (m_count != 0) {
            m_count--;
            if (m_count == 0) {
                return true;
            }
        }
        return false;
    }
};
