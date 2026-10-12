#pragma once

#include <wn/wn_weapon_builder.h>

struct wnDededeFollowerModuleAccesserBuildConfig;

// The native constructor and RTTI establish this direct builder base.
// Only pointer-based status operations are declared here; the builder storage
// and the follower's appended fields remain unreconstructed.
class wnDededeFollower : public wnWeaponBuilder<wnDededeFollowerModuleAccesserBuildConfig> {
public:
    void had(int ownerTaskId, int nodeId, int teamNo);
    bool isPossibleDeadChange();
    void toDead();
};
