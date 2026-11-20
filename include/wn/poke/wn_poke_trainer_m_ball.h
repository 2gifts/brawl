#pragma once
// wnPokeTrainerMBall: the Poke Ball article of the Pokemon Trainer (article kind 16). Only this unit includes this header;
// the fighter unit keeps its opaque stand-in in ft/poke/ft_poke_trainer_article_pools.h.
// The member layout follows the sora_melee article unit mode of wn/zelda/wn_zelda_lighting_bow.h.

#include <wn/wn_weapon_builder.h>
#include <wn/wn_extend_param_accesser.h>
#include <wn/wn_anim_cmd_full_property.h>
#include <wn/wn_inside_event_all_units.h>
#include <wn/zelda/wn_szerosuit_status_builder.h>
#include <so/so_array.h>

struct wnArticleBuildInfo;

// HYPOTHESIS: the parameter block of the ball (read through the article data +0x34); only the ids the accesser exposes.
struct wnPokeTrainerMBallParam {
    s32 unk0;
    s32 unk4;
    s32 unk8;
    s32 unkC;
    s32 unk10;
    float unk14;
    float unk18;
    float unk1C;
    float unk20;
    float unk24;
    float unk28;
    float unk2C;
    s32 unk30;
    s32 unk34;
    float unk38;
    float unk3C;
};

// HYPOTHESIS: layout of the untouched part of the static article data (as the other articles' data blocks).
struct wnPokeTrainerMBallData {
    s32 m_resGroupNo; // the constructor reads the low byte of this first word
    u8 unk4[0x30];
    const wnPokeTrainerMBallParam* m_param; // +0x34
    u8 m_extra[4];                          // +0x38: handed to the module accesser build data
};

class wnPokeTrainerMBallParamAccesser : public wnExtendParamAccesser<3999, 9, 23999, 7, 0, 0> {
public:
    wnPokeTrainerMBallParamAccesser(const wnPokeTrainerMBallParam* param) : wnExtendParamAccesser<3999, 9, 23999, 7, 0, 0>(16) {
        m_floats[0] = &param->unk14;
        m_floats[1] = &param->unk18;
        m_floats[2] = &param->unk1C;
        m_floats[3] = &param->unk20;
        m_floats[4] = &param->unk24;
        m_floats[5] = &param->unk28;
        m_floats[6] = &param->unk2C;
        m_floats[7] = &param->unk38;
        m_floats[8] = &param->unk3C;
        m_ints[0] = &param->unk0;
        m_ints[1] = &param->unk4;
        m_ints[2] = &param->unk8;
        m_ints[3] = &param->unkC;
        m_ints[4] = &param->unk10;
        m_ints[5] = &param->unk30;
        m_ints[6] = &param->unk34;
    }
    virtual ~wnPokeTrainerMBallParamAccesser();
};
static_assert(sizeof(wnPokeTrainerMBallParamAccesser) == 0x68, "Poke Ball parameter accesser extent");

// The second command thread of the Poke Ball has no stack of its own: its interpreter takes the shared null array and the
// unit keeps no member for it, so the address pack follows the table directly.
template <>
class soAnimCmdControlUnitBuilder<soAnimCmdControlUnitBuildConfig<1, 2, 3, 0, 0, 1, 0, 0> > {
    typedef soAnimCmdControlUnitBuildConfig<1, 2, 3, 0, 0, 1, 0, 0> BC;
    soAnimCmdFirstMember<soArrayContractibleTable<const acAnimCmdConv*> > m_table; // +0
    u32 unk10;                                                                       // +0x10
    soAnimCmdAddressPackArraySeparate m_pack;                                        // +0x14
    soAnimCmdInterpreter m_interpreter;                                              // +0x30
    static soArrayNull<acCmdInterpreterStackData>* getNullStack() {
        return soSingletonHolder<soArrayNull<acCmdInterpreterStackData> >::getInstance();
    }
public:
    soAnimCmdControlUnitBuilder(soModuleAccesser* acc, void* data0, void* data1, void* data2) :
        m_table((const acAnimCmdConv**)data0, BC::Capacity),
        m_pack(&m_table.m_member, (soArrayFixed<const acAnimCmdConv*>*)g_soAnimCmdAddrArrayNull,
               (soArrayFixed<const acAnimCmdConv*>*)g_soAnimCmdAddrArrayNull),
        m_interpreter(ftGetManageId(acc),
                      ftInterpreterStack(getNullStack()),
                      BC::UnitId, BC::InterpMode, 1.0f, ftAddressOf((u8)0)) {
        soAttributeFlag attr(BC::AttrMask);
        soAnimCmdControlUnit unit = { &m_interpreter, &m_pack };
        acc->getAnimCmdModule().registInterpreter(unit, attr);
    }
    ~soAnimCmdControlUnitBuilder() { }
};

class wnPokeTrainerMBallModuleAccesserBuildConfig : public wnCommonBuildConfig {
public:
    typedef soInsideEventManageModuleBuildConfig<11, 19, 0, 1, 0, 0, 2, 4, 1, 0, 1, 0, 0, 0, 0, 0, 0, 7, 0, wnAllInsideEventManageModuleTypes>
        InsideEventManageModuleBuildConfig;
    typedef wnAllInsideEventManageModuleTypes InsideEventManageModuleTypes;
    typedef soModelModuleBuildConfig<7, 0, soModelModuleImpl> ModelModuleBuildConfig;
    typedef soMotionModuleBuildConfig<3, soMotionModuleImpl, 0, 1, soTransitionModuleBuildConfig<wnMotionTransitionTypeList>,
                                      soMotionAnimObjCacheModuleBuildConfig<0, soMotionAnimObjCacheModuleNull> >
        MotionModuleBuildConfig;
    typedef soCollisionAttackModuleBuildConfigNull CollisionAttackModuleBuildConfig;
    typedef soCollisionHitModuleBuildConfigNull CollisionHitModuleBuildConfig;
    typedef soDamageModuleBuildConfigNull DamageModuleBuildConfig;
    typedef soStopModuleBuildConfigNull StopModuleBuildConfig;
    typedef soVisibilityModuleBuildConfig<soVisibilityModuleImpl, 1> VisibilityModuleBuildConfig;
    typedef soLinkModuleBuildConfigCap<6, soLinkModuleImpl> LinkModuleBuildConfig;
    typedef soCameraModuleBuildConfig<soCameraModuleImpl> CameraModuleBuildConfig;
    typedef wnStatusModuleBuildConfigNoUniq<9, soGeneralWorkBuildConfigNull, 9, soTransitionModuleBuildConfig<soTypeList<soIntToType<2>, soTypeListNullType> > >
        StatusModuleBuildConfig;
    typedef wnAnimCmdModuleSubBuildConfigFullProperty<1, 9, 0, 0, 2, 1, 3, 0, 0, 0, 1, 3, 0, 0, 2, 1, 3, 0, 0, 3> AnimCmdModuleSubBuildConfig;
    typedef soGeneralWorkBuildConfig<11, 1, 1> GeneralWorkBuildConfig;
    typedef wnKineticModuleBuildConfigSized<wnKineticModuleImpl, soKineticMediatorNull, 2> KineticModuleBuildConfig;
    typedef soReflectModuleBuildConfigNull ReflectModuleBuildConfig;
    typedef soGlowModuleBuildConfigNull GlowModuleBuildConfig;
    typedef soGenerateArticleManageModuleBuilder<soGenerateArticleManageModuleBuildConfigNull> GenerateArticleManageModuleBuilder;
};

template <>
class soModuleAccesserBuilder<wnPokeTrainerMBallModuleAccesserBuildConfig> : public utUnCopyable {
public:
    typedef wnPokeTrainerMBallModuleAccesserBuildConfig BC;
    WN_ZELDA_LIGHTING_BOW_BUILDER_MEMBERS

    soModuleAccesserBuilder(StageObject* owner, const wnModuleAccesserBuildData& bd);
    ~soModuleAccesserBuilder();
    soModuleAccesser* getModuleAccesser();
};

class wnPokeTrainerMBall : public wnWeaponBuilder<wnPokeTrainerMBallModuleAccesserBuildConfig> {
public:
    // HYPOTHESIS: field grouping follows copies; field meanings remain unknown.
    class BallInfo {
    public:
        struct { u32 unk0, unk4; } unk0;
        u32 unk8;
        struct { u32 unk0, unk4; } unkc;
        u32 unk14;
        struct { u32 unk0, unk4; } unk18;
        u32 unk20;
        struct { u32 unk0, unk4; } unk24;
        u32 unk2c;
        u32 unk30;
        u32 unk34;
        float unk38;
    };
    class ParticleInfo {
    public:
        struct { u32 unk0, unk4; } unk0;
        u32 unk8;
        struct { u32 unk0, unk4; } unkc;
        u32 unk14;
        struct { u32 unk0, unk4; } unk18;
        u32 unk20;
        float unk24;
        u32 unk28;
        u32 unk2c;
        u32 unk30;
        u32 unk34;
        u32 unk38;
        float unk3c;
    };
private:
    const wnPokeTrainerMBallParam* m_param;           // +0x17D8
    wnPokeTrainerMBallParamAccesser m_paramAccesser;  // +0x17DC
    soArrayVector<BallInfo, 2> m_ballInfos;           // +0x1844
    soArrayVector<ParticleInfo, 2> m_particleInfos;   // +0x18C8
public:
    wnPokeTrainerMBall(s32 articleId, const wnArticleBuildInfo& info, const wnPokeTrainerMBallData* data);
    virtual ~wnPokeTrainerMBall();
};
static_assert(sizeof(wnPokeTrainerMBall) == 0x1954, "Poke Ball article extent");
