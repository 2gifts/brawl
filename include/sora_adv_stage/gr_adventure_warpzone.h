#pragma once

#include <StaticAssert.h>
#include <gr/gr_gimmick_warpzone.h>
#include <memory.h>
#include <types.h>

// The warp zone (door between areas) of an adventure stage: warps both players after a screen fade.
class grAdventureWarpZone : public grGimmickWarpZone {
    u8 m_isSecondWarped; // 0x1A0, the second fighter was warped too

public:
    grAdventureWarpZone(const char* taskName) : grGimmickWarpZone(taskName) {
        m_isSecondWarped = 0;
    }
    virtual ~grAdventureWarpZone();
    virtual void processFixPosition();
    virtual void onGimmickEvent(soGimmickEventArgs* eventInfo, int* taskId);

    static grAdventureWarpZone* create(int mdlIndex, const char* taskName);
};
static_assert(sizeof(grAdventureWarpZone) == 0x1A4, "Class is wrong size!");
