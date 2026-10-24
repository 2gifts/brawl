#pragma once

// SHADOW of BrawlHeaders em/em_archive.h (-I include comes first): the members are public, the Enemy reads them (its
// constructor, getFaceTexPtr, getExtendBrresResourceID).

#include <StaticAssert.h>
#include <types.h>

class emArchive {
public:
    int enmCommonResourceId;
    int brresResourceId;
    int paramResourceId;
    int weaponResourceId;
    int faceTexBrresResourceId;
    int extendBrresResourceId;

    // HYPOTHESIS: the id selects a weapon of the archive; the copy in the Enemy unit returns weaponResourceId.
    int getWeaponResourceID(u32 id);
};
static_assert(sizeof(emArchive) == 24, "Class is wrong size!");
