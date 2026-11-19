#pragma once

#include <havok/hkArray.h>

// Memory statistics visitor handed to calcStatistics. Slot order from the calls of hkWorld, hkSimulationIsland and
// the agent machine statistics (the same slots as the local collector views of the other units).
// HYPOTHESIS: all names.
struct hkStatisticsCollector {
    virtual ~hkStatisticsCollector();                                                             // 0x08
    virtual void beginObject(const char* name, int type, const void* object);                    // 0x0C
    virtual void addArray(const char* name, int type, const void* data, int size, int capacity); // 0x10
    virtual void addChildObject(const char* name, int type, const void* object);                 // 0x14
    virtual void pushDir(const char* name);                                                      // 0x18
    virtual void popDir();                                                                       // 0x1C
    virtual void endObject();                                                                    // 0x20

    // An array's storage counts only when the array owns it.
    void addArrayStorage(const char* name, int type, const hkArrayBase& array, int elemSize) {
        if (!(array.m_capacityAndFlags & hkArrayBase::DONT_DEALLOCATE_FLAG)) {
            addArray(name, type, array.m_data, array.m_size * elemSize,
                     (array.m_capacityAndFlags & hkArrayBase::CAPACITY_MASK) * elemSize);
        }
    }
};
