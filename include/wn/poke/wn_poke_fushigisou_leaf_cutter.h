#pragma once

#include <wn/wn_weapon_builder.h>

// HYPOTHESIS: descriptive parameter names. The activation reads life at 0 and
// spin limits at 8/C; the constructor also registers these with its accessor.
struct wnPokeFushigisouLeafCutterParam {
    int lifeFrames;
    float horizontalBrake;
    float spinMin;
    float spinMax;
};

struct wnPokeFushigisouLeafCutterModuleAccesserBuildConfig;
// Constructor vptr installation and parameter placement prove this builder
// extent. Its module members remain unreconstructed.
template <>
class wnWeaponBuilder<wnPokeFushigisouLeafCutterModuleAccesserBuildConfig> : public Weapon {
    u8 unkCC[0x16B4 - sizeof(Weapon)];
public:
    virtual ~wnWeaponBuilder();
    virtual void deactivateDescendantForce();
};
static_assert(sizeof(wnWeaponBuilder<wnPokeFushigisouLeafCutterModuleAccesserBuildConfig>) == 0x16B4,
              "Razor Leaf builder extent");

// Behavior prefix only: the parameter accessor starts at 16B8 and is not
// modeled here. No allocation or by-value construction uses this prefix size.
class wnPokeFushigisouLeafCutter : public wnWeaponBuilder<wnPokeFushigisouLeafCutterModuleAccesserBuildConfig> {
    const wnPokeFushigisouLeafCutterParam* m_param; // 16B4
public:
    virtual ~wnPokeFushigisouLeafCutter();
    void activate(int founderTaskId, int resourceId, int team, Vec3f* position,
                  float lr, float speed, float angle);
    virtual void updateNodeSRT();
    virtual bool notifyEventAnimCmd(acAnimCmd*, soModuleAccesser*, int);
    virtual bool notifyEventCollisionAttackCheck(u32 flags);
};
