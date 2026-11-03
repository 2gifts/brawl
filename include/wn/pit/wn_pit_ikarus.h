#pragma once

#include <so/damage/so_damage_event_presenter.h>
#include <wn/wn_extend_param_accesser.h>
#include <wn/wn_weapon_builder.h>

class wnPitIkarusModuleAccesserBuildConfig;

template <>
class wnWeaponBuilder<wnPitIkarusModuleAccesserBuildConfig> : public Weapon {
    u8 unkCC[0x22C8 - sizeof(Weapon)];
public:
    virtual ~wnWeaponBuilder();
};

class wnPitIkarusParamAccesser : public wnExtendParamAccesser<3999, 11, 23999, 3, 0, 0> {
public:
    virtual ~wnPitIkarusParamAccesser();
};

class wnPitIkarus : public wnWeaponBuilder<wnPitIkarusModuleAccesserBuildConfig>, public soDamageEventObserver {
    const void* unk22D4;
    wnPitIkarusParamAccesser unk22D8;
public:
    virtual ~wnPitIkarus();
    void activate(u32 founderTaskId, u32 resourceId, int team, const Vec3f& pos, float unk1, float unk2);
    virtual bool notifyEventCollisionAttackCheck(u32 flags);
    virtual void notifyEventOnDamage(soDamage* damage, bool, soModuleAccesser* moduleAccesser);
};

static_assert(sizeof(wnWeaponBuilder<wnPitIkarusModuleAccesserBuildConfig>) == 0x22C8, "Ikarus builder size");
static_assert(sizeof(wnPitIkarusParamAccesser) == 0x60, "Ikarus parameter accesser size");
static_assert(sizeof(wnPitIkarus) == 0x2338, "Ikarus size");
