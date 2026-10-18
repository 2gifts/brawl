#pragma once

#include <wn/wn_weapon_builder.h>

// HYPOTHESIS: the article data type the Landmaster is built from (the common data record of the fighter kind).
struct wnFoxLandMasterData;
struct wnArticleBuildInfo;
class Vec3f;

// Fox's Final Smash vehicle. Extent known from the fighter's article pool; the internal layout stays opaque until the
// constructor (in sora_melee) is reconstructed.
class wnFoxLandMaster : public Weapon {
    u8 m_storage[0x4B14 - sizeof(Weapon)];
public:
    wnFoxLandMaster(s32 articleId, const wnArticleBuildInfo& info, const wnFoxLandMasterData* data);
    virtual ~wnFoxLandMaster();
    // HYPOTHESIS: parameter meanings follow the call in the fighters' article hooks (founder, resource id, team, position,
    // facing, two floats, life/frame count).
    void activate(int founderTaskId, int resourceId, int team, Vec3f* pos, float lr, float scale, int life);
};
static_assert(sizeof(wnFoxLandMaster) == 0x4B14, "Landmaster article extent");
