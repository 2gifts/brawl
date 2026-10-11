#pragma once
#include <ft/fighter.h>
// Status/kinetic view of Yoshi. The constructor allocates 0x222B0 bytes;
// getExtendParam establishes the instance-data pointer at 0x22284.
struct ftData;

class ftYoshi : public Fighter {
    u8 unkTail[0x22284 - sizeof(Fighter)];
    ftData* m_data; // Instance parameter data, proved by getExtendParam.
    u8 unk22288[0x28];
public:
    void** getExtendParam();
    void createGuardColorAnim();
    void deleteGuardColorAnim();
    void updateGuardColorAnim();
};
static_assert(sizeof(ftYoshi) == 0x222b0, "Yoshi allocation size is wrong!");
