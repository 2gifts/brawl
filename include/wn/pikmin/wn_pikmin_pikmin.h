#pragma once

#include <StaticAssert.h>
#include <so/collision/so_collision_search_event_presenter.h>
#include <so/damage/so_damage_event_presenter.h>
#include <so/situation/so_situation_event_presenter.h>
#include <so/so_array.h>
#include <so/so_kinetic_energy_normal.h>
#include <so/templates/so_array_value_wnPikminPikminOwnerInfo.h>
#include <wn/wn_extend_param_accesser.h>
#include <wn/wn_weapon_builder.h>
#include <mt/mt_vector.h>
#include <so/link/so_link_event_presenter.h>
#include <so/so_module_accesser.h>
#include <so/so_value_accesser.h>
#include <types.h>

// The individual Pikmin that Olimar plucks, throws and leads (the article, not the fighter). The article reads its
// per-colour tables from a data block owned by the fighter (wnPikminPikminData) and a parameter block (wnPikminPikminParam).
// HYPOTHESIS: all field names; offsets and the way each field is used are verified by the native code.

// The parameter block of the article. HYPOTHESIS: field names; the accesser exposes these as ids 4000.. (floats) and
// 24000.. (integers) in the order of its constructor.
struct wnPikminPikminParam {
    float unk0, unk4, unk8, unkC, unk10, unk14;
    s32 unk18, unk1C;
    float bloomFrame1; // 0x20: frames of life after which the first (bud) bloom shows
    float bloomFrame2; // 0x24: frames after which the second (flower) bloom shows
    float unk28, unk2C, unk30, unk34, unk38;
    s32 unk3C;
    float unk40, unk44, unk48, unk4C, unk50, unk54, unk58, unk5C, unk60, unk64, unk68, unk6C;
    s32 unk70, unk74, unk78;
    float unk7C, unk80;
    s32 unk84;
    float unk88;
    s32 unk8C;
    float unk90, unk94, unk98, unk9C, unkA0, unkA4, unkA8, unkAC, unkB0;
};
static_assert(sizeof(wnPikminPikminParam) == 0xB4, "Pikmin parameter block extent");

// The per-colour parameters (one entry per variation). HYPOTHESIS: field names.
struct wnPikminPikminParamParticular {
    s32 unk0;
    float unk4, unk8, unkC, unk10, unk14, unk18, unk1C, unk20, unk24, unk28, unk2C, unk30, unk34, unk38, unk3C, unk40, unk44, unk48;
    s32 unk4C;
    float unk50, unk54, unk58, unk5C, unk60, unk64, unk68, unk6C, unk70;
};
static_assert(sizeof(wnPikminPikminParamParticular) == 0x74, "Pikmin per-colour parameter extent");

class wnPikminPikminParamAccesser : public wnExtendParamAccesser<3999, 65, 23999, 11, 0, 0> {
    soArrayContractibleTable<const wnPikminPikminParamParticular> m_particular; // +0x158
public:
    wnPikminPikminParamAccesser(const wnPikminPikminParam* param, const wnPikminPikminParamParticular* particular)
        __attribute__((never_inline)) :
        wnExtendParamAccesser<3999, 65, 23999, 11, 0, 0>(0x43), m_particular(particular, 5) {
        m_floats[0] = &param->unk0;
        m_floats[1] = &param->unk4;
        m_floats[2] = &param->unk8;
        m_floats[3] = &param->unkC;
        m_floats[4] = &param->unk10;
        m_floats[5] = &param->unk14;
        m_floats[6] = &param->bloomFrame1;
        m_floats[7] = &param->bloomFrame2;
        m_floats[8] = &param->unk28;
        m_floats[9] = &param->unk2C;
        m_floats[10] = &param->unk30;
        m_floats[11] = &param->unk34;
        m_floats[12] = &param->unk38;
        m_floats[13] = &param->unk40;
        m_floats[14] = &param->unk44;
        m_floats[15] = &param->unk48;
        m_floats[16] = &param->unk4C;
        m_floats[17] = &param->unk50;
        m_floats[18] = &param->unk54;
        m_floats[19] = &param->unk58;
        m_floats[20] = &param->unk5C;
        m_floats[21] = &param->unk60;
        m_floats[22] = &param->unk64;
        m_floats[23] = &param->unk68;
        m_floats[24] = &param->unk6C;
        m_floats[25] = &param->unk7C;
        m_floats[26] = &param->unk80;
        m_floats[27] = &param->unk88;
        m_floats[28] = &param->unk90;
        m_floats[29] = &param->unk94;
        m_floats[30] = &param->unk98;
        m_floats[31] = &param->unk9C;
        m_floats[32] = &param->unkA0;
        m_floats[33] = &param->unkA4;
        m_floats[34] = &param->unkA8;
        m_floats[35] = &param->unkAC;
        m_floats[36] = &param->unkB0;
        m_ints[0] = &param->unk18;
        m_ints[1] = &param->unk1C;
        m_ints[2] = &param->unk3C;
        m_ints[3] = &param->unk70;
        m_ints[4] = &param->unk74;
        m_ints[5] = &param->unk78;
        m_ints[6] = &param->unk84;
        m_ints[7] = &param->unk8C;
    }
    virtual ~wnPikminPikminParamAccesser() { }
    virtual float getParamFloat(soModuleAccesser* moduleAccesser, s32 id, s32 index);
    virtual s32 getParamInt(soModuleAccesser* moduleAccesser, s32 id, s32 index);
};

// The article data block: three tables of animation command lists, one entry for each of the five colour variations.
struct wnPikminPikminData {
    u8 unk0[0x1C];
    int unk1C;                                 // 0x1C
    const void* waitAnimTable;                 // 0x74 of the data block of the original; kept as a pointer here
    u8 unk24[0x14];
    const acAnimCmdConv** animCmdTable[3][5];  // 0x38 / 0x4C / 0x60: lists for the three animation command groups
};

// Link events between the article and its leader (Olimar) and the thrown article. HYPOTHESIS: values are the sub kinds of
// soLinkEventArgs used by the native checks.
struct wnPikminLinkEvent : soLinkEventArgs {
    wnPikminLinkEvent(int kind) : soLinkEventArgs(kind) { }
};

// Link events that reach a Pikmin from its parent (HYPOTHESIS: names and layouts follow how the receiver reads them).
struct wnPikminLinkEventVisibility : soLinkEventArgs {
    u8 visible; // 0x5
    wnPikminLinkEventVisibility(int kind) : soLinkEventArgs(kind), visible(0) { }
};
struct wnPikminLinkEventBind : soLinkEventArgs {
    int nodeId; // 0x8: filled in by the receiver
    wnPikminLinkEventBind(int kind) : soLinkEventArgs(kind), nodeId(0) { }
};

// The link event 0xb a Pikmin sends to its nodes when it starts to cling to a fighter (HYPOTHESIS: names; unkC and unk10
// are filled in by the receiver).
struct wnPikminLinkEventCling : soLinkEventArgs {
    int statusKind; // 0x8
    int unkC;
    int unk10;
    int unk14;
    float unk18;
    bool unk1C;
    wnPikminLinkEventCling(int kind, int status) : soLinkEventArgs(kind), statusKind(status), unkC(-1), unk10(-1), unk14(-1), unk18(1.0f), unk1C(true) { }
};

// The article's status kinds. HYPOTHESIS: names follow what each status does in the native code.
enum wnPikminPikminStatus {
    PikminStatus_Wait = 0,
    PikminStatus_Fall = 1,
    PikminStatus_Fall2 = 6,
    PikminStatus_Thrown = 0x17,
    PikminStatus_Dead = 0x1e,
};

enum soCollisionHitStatus { soCollisionHitStatus_Unk0 };

// HYPOTHESIS: kinetic energy 5 of the article; it only slows the article down and is pushed by hits.
class soKineticEnergyStop : public soKineticEnergyNormal {
public:
    void setStopSpeed(float speed);
};

struct wnPikminPikminModuleAccesserBuildConfig;
template <>
class wnWeaponBuilder<wnPikminPikminModuleAccesserBuildConfig> : public Weapon {
    // The module storage of the generic article builder (the builder is reconstructed with the constructors).
    u8 unkCC[0x3118 - sizeof(Weapon)];
public:
    virtual ~wnWeaponBuilder();
    virtual void deactivateDescendantForce();
};
static_assert(sizeof(wnWeaponBuilder<wnPikminPikminModuleAccesserBuildConfig>) == 0x3118, "Pikmin builder extent");

class wnPikminPikmin : public wnWeaponBuilder<wnPikminPikminModuleAccesserBuildConfig>,
                       public soDamageEventObserver,
                       public soCollisionSearchEventObserver,
                       public soSituationEventObserver {
public:
    const wnPikminPikminData* m_data;     // 0x313C
    const void* m_waitAnimTable;          // 0x3140
    const wnPikminPikminParam* m_param;   // 0x3144
    u8 unk3148[0x168];                    // the parameter accesser
    // The animation command lists of the colour in use, one table and one address pack for each animation command group.
    soArrayContractibleTable<const acAnimCmdConv*> m_animCmdTable0; // 0x32B0
    soAnimCmdAddressPackArraySeparate m_animCmdPack0;               // 0x32C0
    soArrayContractibleTable<const acAnimCmdConv*> m_animCmdTable1; // 0x32DC
    soAnimCmdAddressPackArraySeparate m_animCmdPack1;               // 0x32EC
    soArrayContractibleTable<const acAnimCmdConv*> m_animCmdTable2; // 0x3308
    soAnimCmdAddressPackArraySeparate m_animCmdPack2;               // 0x3318
    soArrayContractibleTable<soCollisionHitStatus> m_hitStatusTable; // 0x3334

    void activate(float lr, float unk2, float unk3, int unk18, Vec3f* pos, int founderTaskId, int resourceId, int team,
                  int initStatus, bool unk10, int unk11);
    virtual void onDeactivate();
    virtual void processUpdate();
    virtual void processHit();
    virtual void processFixPosition();
    virtual void updatePosture(bool unk1);
    virtual void renderDebug();
    virtual void notifyEventChangeStatus(int statusKind, int prevStatusKind, soStatusData* statusData, soModuleAccesser* moduleAccesser);
    virtual void notifyEventLink(soLinkEventArgs* args, soModuleAccesser* moduleAccesser, StageObject* sender, int unk4);
    virtual void notifyEventCollisionAttack(float power, soCollisionLog* collisionLog, soModuleAccesser* moduleAccesser);
    virtual bool notifyEventCollisionAttackCheck(u32 flags);
    virtual void changeStatus(int statusKind);
    virtual void linkOwner(int, int);
    virtual void notifyEventOnDamage(soDamage* damage, bool unk1, soModuleAccesser* moduleAccesser);
    virtual void notifyEventCollisionSearch(soCollisionLog* collisionLog, soModuleAccesser* moduleAccesser);
    virtual bool notifyEventCollisionSearchCheck();
    virtual void notifyEventChangeSituation(SituationKind kind, SituationKind prevKind, soModuleAccesser* moduleAccesser);

    void setOwnerPos(Vec2f* pos);
    void setTargetPos(Vec2f* pos);
    void setOwnerConditionCurrent(int condition);
    void setOwnerConditionFollow(int condition);
    void setOwnerLrCurrent(float lr);
    void setOwnerLrFollow(float lr);
    bool isLiberty();
    bool isEnableHold();
    float getParamFloat(int index);
    const void* getWaitAnimTable();
    void syncLr(float lr);
    void syncPos(Vec2f* pos);
    bool attachGround();
    int getVariation();
    bool isEnableSort();
    bool isNeedSortRepond();
    Vec2f getTargetDiffVec();
    void onStatusFlag(u32 flag);
    void setStatusFloat(float value, u32 index);
    void onInstanceFlag(u32 flag);
    bool isAutonomy();
    bool isEnableDrownTrigger(int status);
    bool isEnableReflect();
    void updateHitStatus();
    void updateAttackInfoStatus();
    void detachConstraint(bool unk1);
    void setPowerMulStatus(float power);
    bool isHitStopStatus();
    bool isConstraintTarget(u32 taskId);
    void resetTeamOwnerId();
    void setTeam(int team, int teamOwnerId);
    bool isAcceptDamage(soDamage* damage);
    bool reflectRequest(Vec2f* speedMul, Vec2f* accelMul);
    void attachChain(int unk1, int unk2, int logGroup);
    void detachChain(int status);
    void unRefLogGroup();
    int getCatchKind();
    void setJostlePriority(int priority);
    void setVisible(bool visible);
    void setOwnerInfo(wnPikminPikminOwnerInfo* owner, wnPikminPikminOwnerInfo* follow);
    void updateBloomState();
    void updateAutonomy();
};
