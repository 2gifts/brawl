#define MT_VEC3F_ASSIGN_NOINLINE
#define WN_WEAPON_BUILDER
#define WN_ARTICLE_NATIVE_BUILDERS
#define WN_SHARED_ARRAYS_1
#define FT_REL_LINK_EXTERN
#include <wn/wn_module_accesser_builder.h>
#include <ft/robot/ft_robot_article_pools.h>
#include <ft/robot/ft_robot_link_event.h>
#include <memory.h>
#include <mt/mt_prng.h>
#include <wn/robot/wn_robot_gyro_status_uniq_process.h>
#include <wn/wn_activate_desc.h>
#include <so/so_value_accesser.h>
#include <so/so_module_accesser.h>

// Shared transition tables remain imported from the original data owner.
// HYPOTHESIS: table names and source grouping; the receiver indexes three
// pairs by current state, or one pair when the configured volley count is two.
extern const s32 lbl_120_rodata_A0[3][2];
extern const s32 lbl_120_rodata_B8[2];

inline soModuleAccesserBuilder<wnRobotGyroHolderModuleAccesserBuildConfig>::soModuleAccesserBuilder(StageObject* owner, const wnModuleAccesserBuildData& bdRef) :
    m_moduleAccsr(
        owner,
        soResourceModuleBuilder<typename BC::ResourceModuleBuildConfig>::getModule(&m_resourceModuleBuilder),
        soModelModuleBuilder<typename BC::ModelModuleBuildConfig>::getModule(&m_modelModuleBuilder),
        (soMotionModule*)m_motionBuilder.getModule(),
        (soPostureModule*)m_postureModuleBuilder.getModule(),
        (soGroundModule*)m_groundModuleBuilder.getModule(),
        (soSituationModule*)m_situationModuleBuilder.getModule(),
        (soTeamModule*)m_teamBuilder.getModule(),
        (soCollisionAttackModule*)m_attackModuleBuilder.getModule(),
        (soCollisionHitModule*)m_hitModuleBuilder.getModule(),
        (soCollisionShieldModule*)m_shieldModuleBuilder.getModule(),
        (soCollisionShieldModule*)m_reflectorModuleBuilder.getModule(),
        ftReflectorBase<typename BC::CollisionReflectorModuleBuildConfig>::getAbsorber(&m_reflectorModuleBuilder),
        (void*)m_collisionCatchModuleBuilder.getModule(),
        (soCollisionSearchModule*)m_searchBuilder.getModule(),
        (soDamageModule*)m_damageModuleBuilder.getModule(),
        (void*)m_catchModuleBuilder.getModule(),
        (void*)m_captureModuleBuilder.getModule(),
        (soStopModule*)m_stopModuleBuilder.getModule(),
        (void*)m_turnModuleBuilder.getModule(),
        (void*)m_shakeModuleBuilder.getModule(),
        (soSoundModule*)m_soundModuleBuilder.getModule(),
        (soLinkModule*)m_linkModuleBuilder.getModule(),
        (soVisibilityModule*)m_visibilityModuleBuilder.getModule(),
        (soControllerModule*)m_controllerModuleBuilder.getModule(),
        (soCameraModule*)m_cameraModuleBuilder.getModule(),
        (soWorkManageModule*)m_workManageModuleBuilder.getModule(),
        (void*)g_soDebugModuleNull,
        (soAnimCmdModule*)m_animCmdBuilder.getModule(),
        (soStatusModule*)m_statusBuilder.getModule(),
        (void*)lbl_27_bss_170, // HYPOTHESIS: the articles' null general term decide module
        (void*)lbl_27_bss_188, // HYPOTHESIS: the articles' null switch decide module
        (soKineticModule*)lbl_27_bss_530,
        (soEventManageModule*)unk194.getModule(),
        (void*)m_generateArticleBuilder.getModule(),
        (soEffectModule*)m_effectModuleBuilder.getModule(),
        (void*)m_comboModuleBuilder.getModule(),
        (soAreaModule*)m_areaBuilder.getModule(),
        (void*)g_soTerritoryModuleNull,
        (void*)g_soTargetSearchModuleNull,
        (void*)m_physicsModuleBuilder.getModule(),
        (void*)m_slopeModuleBuilder.getModule(),
        (soShadowModule*)m_shadowModuleBuilder.getModule(),
        (soItemManageModule*)m_itemManageModuleBuilder.getModule(),
        (soColorBlendModule*)m_colorBlendModuleBuilder.getModule(),
        (void*)m_jostleModuleBuilder.getModule(),
        (void*)m_abnormalModuleBuilder.getModule(),
        (soSlowModule*)m_slowModuleBuilder.getModule(),
        (void*)m_reflectModuleBuilder.getModule(),
        (void*)&m_heapModuleBuilder,
        (soParamCustomizeModule*)&m_paramCustomizeModuleBuilder,
        (void*)m_glowModuleBuilder.getModule()),
    m_heapModuleBuilder(bdRef.getInstanceHeap(), bdRef.getNWModelInstanceHeap(), bdRef.getNWMotionInstanceHeap(), bdRef.getHeapSlotNo()),
    m_paramCustomizeModuleBuilder(&m_moduleAccsr),
    m_resourceModuleBuilder(bdRef.getMdlResId(), bdRef.getAnmResId(), bdRef.getResGroupNo(), &m_moduleAccsr),
    m_modelModuleBuilder(&m_moduleAccsr, bdRef.getModelScale(), bdRef.getModelExtendNodeTable(), FT_NULL_OBSERVER_DESC),
    m_motionBuilder(&m_moduleAccsr, bdRef.getMotionData()),
    m_postureModuleBuilder(&m_moduleAccsr, FT_NULL_OBSERVER_DESC),
    m_groundModuleBuilder(&m_moduleAccsr, bdRef.getGroundConditionChecker()),
    m_situationModuleBuilder(ftGetManageId(&m_moduleAccsr), &m_moduleAccsr, FT_NULL_OBSERVER_DESC),
    m_teamBuilder(bdRef.getTeam(), &m_moduleAccsr),
    m_attackModuleBuilder(&m_moduleAccsr, owner->m_taskId, static_cast<gfTask::Category>(static_cast<u8>(owner->m_taskCategory)), FT_NULL_OBSERVER_DESC),
    m_hitModuleBuilder(&m_moduleAccsr, owner->m_taskId, static_cast<gfTask::Category>(static_cast<u8>(owner->m_taskCategory)), FT_NULL_OBSERVER_DESC),
    m_shieldModuleBuilder(&m_moduleAccsr, owner->m_taskId, static_cast<gfTask::Category>(static_cast<u8>(owner->m_taskCategory))),
    m_reflectorModuleBuilder(&m_moduleAccsr, owner->m_taskId, static_cast<gfTask::Category>(static_cast<u8>(owner->m_taskCategory))),
    m_absorberBuilder(&m_moduleAccsr, owner->m_taskId, static_cast<gfTask::Category>(static_cast<u8>(owner->m_taskCategory))),
    m_collisionCatchModuleBuilder(&m_moduleAccsr, owner->m_taskId, static_cast<gfTask::Category>(static_cast<u8>(owner->m_taskCategory)), FT_NULL_OBSERVER_DESC),
    m_searchBuilder(&m_moduleAccsr, owner->m_taskId, static_cast<gfTask::Category>(static_cast<u8>(owner->m_taskCategory))),
    m_damageModuleBuilder(&m_moduleAccsr, FT_NULL_OBSERVER_DESC),
    m_catchModuleBuilder(&m_moduleAccsr),
    m_captureModuleBuilder(&m_moduleAccsr),
    m_stopModuleBuilder(&m_moduleAccsr),
    m_turnModuleBuilder(&m_moduleAccsr),
    m_shakeModuleBuilder(&m_moduleAccsr, bdRef.getShakeData()),
    m_soundModuleBuilder(&m_moduleAccsr, bdRef.getSoundIdExchanger(), FT_NULL_OBSERVER_DESC),
    m_linkModuleBuilder(&m_moduleAccsr),
    m_visibilityModuleBuilder(&m_moduleAccsr, bdRef.getVisibilityData()),
    m_controllerModuleBuilder(&m_moduleAccsr, ftGetManageId(&m_moduleAccsr)),
    m_cameraModuleBuilder(&m_moduleAccsr, (soSet<soCameraRange>*)bdRef.getCameraRangeSet(), (soSet<soCameraClipSphere>*)bdRef.getCameraClipSphereSet(), FT_NULL_OBSERVER_DESC),
    m_workManageModuleBuilder(&m_moduleAccsr, bdRef.getParamAccesser()),
    m_animCmdBuilder(ftGetManageId(&m_moduleAccsr)),
    m_statusBuilder(&m_moduleAccsr, bdRef.getStatusData(), bdRef.getPreCheckAnimCmdData()),
    m_generalWorkBuilder(),
    m_generateArticleBuilder(&m_moduleAccsr),
    m_effectModuleBuilder(&m_moduleAccsr, bdRef.getEffectNodeData(), bdRef.getEffectEmitData(), bdRef.getEffectCommonData(), bdRef.getEffectScreenData(), FT_NULL_OBSERVER_DESC),
    m_comboModuleBuilder(&m_moduleAccsr),
    m_areaBuilder(&m_moduleAccsr, bdRef.getAreaCategory(), FT_NULL_OBSERVER_DESC),
    m_physicsModuleBuilder(&m_moduleAccsr, (bdRef.getTerritoryRect(), bdRef.getTerritoryParam(), bdRef.getTargetSearchParam(), bdRef.getIkData())),
    m_slopeModuleBuilder(&m_moduleAccsr, bdRef.getSlopeAngleLimit()),
    m_shadowModuleBuilder(&m_moduleAccsr),
    m_itemManageModuleBuilder(&m_moduleAccsr, bdRef.getItemNodeData()),
    m_colorBlendModuleBuilder(&m_moduleAccsr),
    m_jostleModuleBuilder(&m_moduleAccsr, bdRef.getJostleData()),
    m_abnormalModuleBuilder(&m_moduleAccsr),
    m_slowModuleBuilder(&m_moduleAccsr),
    m_reflectModuleBuilder(&m_moduleAccsr),
    m_glowModuleBuilder(&m_moduleAccsr) {
    // Register the common work bank only when the build config owns one.
    if (m_generalWorkBuilder.getModule()->isNull() == false)
        m_moduleAccsr.getWorkManageModule().setWork(1, m_generalWorkBuilder.getModule());
}

wnRobotGyro::wnRobotGyro(s32 articleId, const ftRobotArticleConstructionInfo& info, void* data)
    : wnWeaponBuilder<wnRobotGyroModuleAccesserBuildConfig>(
          "RobotGyro", articleId, info,
          wnModuleAccesserBuildData(0x2d, data,
              info.getHeapModule()->getInstanceHeap(),
              info.getHeapModule()->getNWModelInstanceHeap(),
              info.getHeapModule()->getNWMotionInstanceHeap(),
              static_cast<u8>(static_cast<wnRobotArticleData*>(data)->m_resGroupNo), 0, &g_soParamAccesserNull,
              0, 0, 0, 0, 0, 0, 0)) {
    m_param = static_cast<wnRobotArticleData*>(data)->m_param;
    // HYPOTHESIS: the original registers its three embedded processes through the first one's address.
    soStatusUniqProcess* procs[3];
    procs[0] = &m_hold;
    procs[1] = &m_hold;
    procs[2] = &m_hold;
    m_moduleAccesser->getStatusModule().addRangeUniqProc(procs, 3);
    Weapon::deactivate(false);
}

wnRobotGyroHolder::wnRobotGyroHolder(s32 articleId, const ftRobotArticleConstructionInfo& info, void* data)
    : wnWeaponBuilder<wnRobotGyroHolderModuleAccesserBuildConfig>(
          "RobotGyroHolder", articleId, info,
          wnModuleAccesserBuildData(0x2f, data,
              info.getHeapModule()->getInstanceHeap(),
              info.getHeapModule()->getNWModelInstanceHeap(),
              info.getHeapModule()->getNWMotionInstanceHeap(),
              static_cast<u8>(static_cast<wnRobotArticleData*>(data)->m_resGroupNo), -1, &g_soParamAccesserNull,
              0, 0, 0, 0, 0, 0, 0)) {
    Weapon::deactivate(false);
}

wnRobotFinalBeam::wnRobotFinalBeam(s32 articleId, const ftRobotArticleConstructionInfo& info, void* data)
    : wnWeaponBuilder<wnRobotFinalBeamModuleAccesserBuildConfig>(
          "RobotFinalBeam", articleId, info,
          wnModuleAccesserBuildData(0x30, data,
              info.getHeapModule()->getInstanceHeap(),
              info.getHeapModule()->getNWModelInstanceHeap(),
              info.getHeapModule()->getNWMotionInstanceHeap(),
              static_cast<u8>(static_cast<wnRobotArticleData*>(data)->m_resGroupNo), -1, &g_soParamAccesserNull,
              0, 0, 0, 0, 0, 0, 0)) {
    m_param = static_cast<wnRobotArticleData*>(data)->m_param;
    Weapon::deactivate(false);
}

void wnRobotGyroStatusUniqProcessHold::initStatus(soModuleAccesser*) { }
void wnRobotGyroStatusUniqProcessHold::execStatus(soModuleAccesser*) { }
void wnRobotGyroStatusUniqProcessHold::execFixPos(soModuleAccesser*) { }
void wnRobotGyroStatusUniqProcessMove::initStatus(soModuleAccesser*) { }
void wnRobotGyroStatusUniqProcessMove::execStatus(soModuleAccesser*) { }
void wnRobotGyroStatusUniqProcessStop::initStatus(soModuleAccesser*) { }
void wnRobotGyroStatusUniqProcessStop::execStatus(soModuleAccesser*) { }


void wnRobotGyro::activate(s32 founderTaskId, u32 resourceId, s32 team,
                           const Vec3f& position, float lr, float power) {
    wnActivateDesc desc;
    desc.founderTaskId = founderTaskId;
    desc.resourceId = resourceId;
    desc.unk8 = resourceId;
    desc.unkC = resourceId;
    desc.unk10 = -1;
    desc.unk14 = -1;
    desc.unk18 = 0;
    desc.unk1C = 0;
    // MATCH-ONLY: activation copies position with integer moves.
    __memcpy(&desc.pos, &position, sizeof(Vec3f));
    desc.lr = lr;
    desc.team = team;
    desc.life = 0;
    desc.unk38 = 2;
    desc.unk3C = 0x80;
    desc.unk40 = 0;
    desc.unk44 = 0x35f;
    desc.unk48 = 0;
    desc.flagsHighPair = 3;
    desc.flagsMidPair = 0;
    desc.flagsBit3 = 1;
    desc.flagsLow = 0;
    desc.unk4DHigh = 0;
    Weapon::activate(&desc);
    // HYPOTHESIS: this work float carries the founder's Gyro charge/power.
    m_moduleAccesser->getWorkManageModule().setFloat(power, 0x11000001);
    m_moduleAccesser->getStatusModule().changeStatusForce(0, m_moduleAccesser);
}

void wnRobotGyro::onDeactivate() { }

// Keep the visual article's root synchronized with the weapon posture.
void wnRobotGyro::updateNodeSRT() {
    float size = m_moduleAccesser->getPostureModule().getScale();
    Vec3f scale;
    scale.m_z = size;
    scale.m_y = size;
    scale.m_x = size;
    Vec3f rotation;
    Vec3f position;
    position = m_moduleAccesser->getPostureModule().getPos();
    rotation = m_moduleAccesser->getPostureModule().getRot(0);
    m_moduleAccesser->getModelModule().setNodeSRT(0, &scale, &rotation, &position);
}

void wnRobotGyro::notifyEventLink(soLinkEventArgs* event, soModuleAccesser* acc,
                                 StageObject* parent, int index) {
    switch (event->m_eventKind) {
    case 0x839:
        if (acc->getStatusModule().getStatusKind() == 0) {
            m_moduleAccesser->getStatusModule().changeStatusRequest(1, m_moduleAccesser);
        }
        break;
    case 0x83a:
        m_moduleAccesser->getStatusModule().changeStatusRequest(2, m_moduleAccesser);
        break;
    }
    Weapon::notifyEventLink(event, acc, parent, index);
}

wnRobotGyroHolder::~wnRobotGyroHolder() { }

void wnRobotGyroHolder::activate(s32 founderTaskId, u32 resourceId, s32 team,
                                 float lr, const Vec3f& position, SituationKind situation) {
    wnActivateDesc desc;
    desc.founderTaskId = founderTaskId;
    desc.resourceId = resourceId;
    desc.unk8 = resourceId;
    desc.unkC = resourceId;
    desc.unk10 = -1;
    desc.unk14 = -1;
    desc.unk18 = 0;
    desc.unk1C = 0;
    // MATCH-ONLY: activation copies position with integer moves.
    __memcpy(&desc.pos, &position, sizeof(Vec3f));
    desc.lr = lr;
    desc.team = team;
    desc.life = 0;
    desc.unk38 = 2;
    desc.unk3C = 0x80;
    desc.unk40 = 0;
    desc.unk44 = 0x35f;
    desc.unk48 = 0;
    desc.flagsHighPair = 3;
    desc.flagsMidPair = 0;
    desc.flagsBit3 = 1;
    desc.flagsLow = 0;
    desc.unk4DHigh = 0;
    Weapon::activate(&desc);
    if (situation == Situation_Air) {
        m_moduleAccesser->getWorkManageModule().onFlag(0x22000002);
    }
    m_moduleAccesser->getStatusModule().changeStatusForce(0, m_moduleAccesser);
    // HYPOTHESIS: flag 8 selects the holder's model constraint policy.
    m_moduleAccesser->getLinkModule().setModelConstraintFlag(8);
}

void wnRobotGyroHolder::onDeactivate() { }

void wnRobotGyroHolder::notifyEventLink(soLinkEventArgs* event, soModuleAccesser* acc,
                                       StageObject* parent, int index) {
    switch (event->m_eventKind) {
    case 0x839:
        if (acc->getStatusModule().getStatusKind() == 0) {
            m_moduleAccesser->getStatusModule().changeStatusRequest(1, m_moduleAccesser);
        }
        break;
    case 0x83b:
        m_moduleAccesser->getStatusModule().changeStatusRequest(2, m_moduleAccesser);
        break;
    case 0x83c:
        m_moduleAccesser->getWorkManageModule().offFlag(0x22000002);
        m_moduleAccesser->getWorkManageModule().onFlag(0x22000001);
        break;
    case 0x83d:
        m_moduleAccesser->getWorkManageModule().onFlag(0x22000002);
        m_moduleAccesser->getWorkManageModule().onFlag(0x22000001);
        break;
    }
    Weapon::notifyEventLink(event, acc, parent, index);
}

wnRobotFinalBeam::~wnRobotFinalBeam() { }

void wnRobotFinalBeam::activate(s32 founderTaskId, u32 resourceId, s32 team,
                                const Vec3f* position, float lr, s32 count, s32 selection) {
    wnActivateDesc desc;
    desc.founderTaskId = founderTaskId;
    // This visual beam uses the default resource selection rather than the
    // caller's resource ID, unlike the Gyro and its holder.
    desc.resourceId = 0xffff;
    desc.unk8 = 0xffff;
    desc.unkC = 0xffff;
    desc.unk10 = -1;
    desc.unk14 = -1;
    desc.unk18 = 0;
    desc.unk1C = 0;
    // MATCH-ONLY: activation copies position with integer moves.
    __memcpy(&desc.pos, position, sizeof(Vec3f));
    desc.lr = lr;
    desc.team = team;
    desc.life = 0;
    desc.unk38 = 2;
    desc.unk3C = 0x80;
    desc.unk40 = 0;
    desc.unk44 = 0x35f;
    desc.unk48 = 0;
    desc.flagsHighPair = 3;
    desc.flagsMidPair = 0;
    desc.flagsBit3 = 1;
    desc.flagsLow = 0;
    desc.unk4DHigh = 0;
    Weapon::activate(&desc);
    m_moduleAccesser->getStatusModule().changeStatusForce(selection, m_moduleAccesser);
    m_moduleAccesser->getWorkManageModule().setInt(count, 0x20000000);
    m_moduleAccesser->getPostureModule().setSyncConstraintNode(0);
}

void wnRobotFinalBeam::processUpdate() {
    if (m_moduleAccesser->getWorkManageModule().isFlag(0x22000000)) {
        // Forward the article's pending notification to its linked founder.
        // HYPOTHESIS: meaning of event 0x456; kind and direction are verified.
        ftRobotGyroLinkEvent event(0x456);
        m_moduleAccesser->getLinkModule().sendEventParents(1, event);
        m_moduleAccesser->getWorkManageModule().offFlag(0x22000000);
    }
    Weapon::processUpdate();
}

void wnRobotFinalBeam::processFixPosition() {
    m_moduleAccesser->getModelModule().getNodeGlobalPosition(0, false);
    Weapon::processFixPosition();
}

// The founder requests new Diffusion Beam volleys through these link events.
// State 3 is terminal: later requests still reach the base link handler.
void wnRobotFinalBeam::notifyEventLink(soLinkEventArgs* event, soModuleAccesser* acc,
                                      StageObject* parent, int index) {
    if (acc->getStatusModule().getStatusKind() != 3) {
        switch (event->m_eventKind) {
        case 0x838:
            m_moduleAccesser->getStatusModule().changeStatusRequest(3, acc);
            break;
        case 0x839: {
            int current = acc->getStatusModule().getStatusKind();
            int next = 0;
            if (acc->getWorkManageModule().getInt(0x20000000) > 2) {
                s32 transitions[3][2];
                __memcpy(transitions, lbl_120_rodata_A0, sizeof(transitions));
                s32 random = randi(2);
                next = transitions[current][random];
            } else if (acc->getWorkManageModule().getInt(0x20000000) > 1) {
                s32 transitions[2];
                __memcpy(transitions, lbl_120_rodata_B8, sizeof(transitions));
                next = transitions[randi(2)];
            }
            static_cast<ftRobotFinalLinkEvent*>(event)->result = next;
            acc->getStatusModule().changeStatusRequest(next, acc);
            acc->getPostureModule().setSyncConstraintNode(0);
            break;
        }
        }
    }
    Weapon::notifyEventLink(event, acc, parent, index);
}

wnRobotGyroStatusUniqProcessHold::~wnRobotGyroStatusUniqProcessHold() { }

wnRobotGyroStatusUniqProcessMove::~wnRobotGyroStatusUniqProcessMove() { }

wnRobotGyroStatusUniqProcessStop::~wnRobotGyroStatusUniqProcessStop() { }
