#pragma once

#include <wn/wn_weapon_builder.h>

struct wnDededeFollowerModuleAccesserBuildConfig;

// The native constructor and RTTI establish this direct builder base.
// Only pointer-based status operations are declared here; the builder storage
// and the follower's appended fields remain unreconstructed.
class wnDededeFollower : public wnWeaponBuilder<wnDededeFollowerModuleAccesserBuildConfig> {
public:
    virtual void processUpdate();
    virtual void processHit();
    void toShoot(Vec3f* pos, float lr, float unk1, float unk2, bool unk3);
    virtual void notifyEventChangeStatus(int statusKind, int prevStatusKind, soStatusData* statusData, soModuleAccesser* moduleAccesser);
    bool isPickup();
    bool isFinal();
    void updateAttackInfoStatus();
    virtual bool isCanInhaled();
    virtual bool isCanEat();
    void had(int ownerTaskId, int nodeId, int teamNo);
    bool isPossibleDeadChange();
    void toDead();
};
