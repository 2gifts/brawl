#pragma once

#include <StaticAssert.h>
#include <mt/mt_vector.h>
#include <so/so_module_accesser.h>
#include <so/so_value_accesser.h>
#include <types.h>
#include <wn/wn_extend_param_accesser.h>
#include <wn/wn_weapon_builder.h>

// Olimar's Pikmin Chain (up special): a chain of Pikmin that is shot upward, reaches out like Link's clawshot and lets Olimar
// pull himself to the end. It is a lasso-style article (see the wnLasso status views), carrying up to a number of Pikmin kept in
// the work variables. HYPOTHESIS: all field names; offsets follow the native code.

// The parameter block of the chain. HYPOTHESIS: five floats (ids 4000-4004).
struct wnPikminChainParam {
    float unk0, unk4, unk8, unkC, unk10;
};
static_assert(sizeof(wnPikminChainParam) == 0x14, "Chain parameter block extent");

class wnPikminChainParamAccesser : public wnExtendParamAccesser<3999, 5, 23999, 0, 0, 0> {
public:
    wnPikminChainParamAccesser(const wnPikminChainParam* param) : wnExtendParamAccesser<3999, 5, 23999, 0, 0, 0>(0x90) {
        m_floats[0] = &param->unk0;
        m_floats[1] = &param->unk4;
        m_floats[2] = &param->unk8;
        m_floats[3] = &param->unkC;
        m_floats[4] = &param->unk10;
    }
};
static_assert(sizeof(wnPikminChainParamAccesser) == 0x3C, "Chain parameter accesser extent");

struct wnPikminChainModuleAccesserBuildConfig;
template <>
class wnWeaponBuilder<wnPikminChainModuleAccesserBuildConfig> : public Weapon {
    // The module storage of the generic article builder (the builder is reconstructed with the constructors).
    u8 unkCC[0x1F00 - sizeof(Weapon)];
public:
    virtual ~wnWeaponBuilder();
    virtual void deactivateDescendantForce();
};
static_assert(sizeof(wnWeaponBuilder<wnPikminChainModuleAccesserBuildConfig>) == 0x1F00, "Chain builder extent");

class wnPikminChain : public wnWeaponBuilder<wnPikminChainModuleAccesserBuildConfig> {
public:
    const wnPikminChainParam* m_param;         // 0x1F00
    wnPikminChainParamAccesser m_paramAccesser; // 0x1F04

    void activate(float lr, int founderTaskId, int kind, int team, bool unk, int pikminNum);
    virtual void onDeactivate();
    virtual void updateNodeSRT();
    virtual void notifyEventChangeStatus(int statusKind, int prevStatusKind, soStatusData* statusData, soModuleAccesser* moduleAccesser);
    virtual void processFixCamera();
    virtual void renderDebug();
};
static_assert(sizeof(wnPikminChain) == 0x1F40, "Chain article extent");

// Shared helper of the chain article and its statuses.
class wnPikminChainUtility {
public:
    // Keeps the number of visible Pikmin of the chain in step with its current reach and tells the parent link.
    static void updatePikminVisibility(soModuleAccesser* moduleAccesser);
};
