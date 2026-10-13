#pragma once
#include <ft/fighter.h>

// Status view of Ness: the native class allocator requests 0x4AE28 bytes,
// and getExtendParam reads the instance parameter data at 0x4AE20.
struct ftData;
class ftNess : public Fighter {
    u8 unkTail[0x4AE20 - sizeof(Fighter)];
    ftData* m_data;
    u8 unk4AE24[4];
public:
    void** getExtendParam();
};
static_assert(sizeof(ftNess) == 0x4AE28, "Ness allocation size is wrong!");
