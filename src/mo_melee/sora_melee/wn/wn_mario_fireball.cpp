#define WN_MARIO_FIREBALL_UNIT
#define SO_COLLISION_GROUP_ALIGNED
#define WN_WEAPON_BUILDER
#define WN_ARTICLE_NATIVE_BUILDERS
#define WN_STATUS_NO_TAIL
#define WN_TRANSITION_CTOR_INLINE
#define WN_KINETIC_MEDIATOR_INLINE_DTOR
#define FT_REL_LINK_EXTERN
#define SO_NULLABLE_INLINE_CTOR
#include <wn/wn_module_accesser_builder.h>
#include <wn/mario/wn_mario_fireball.h>
#include <wn/wn_class_info_impl.h>

wnMarioFireball::wnMarioFireball(s32 articleId, const wnArticleBuildInfo& info, const wnMarioFireballData* data) :
    wnWeaponBuilder<wnMarioFireballModuleAccesserBuildConfig>(
        "MarioFireball", articleId, info,
        wnModuleAccesserBuildData(0, data, info.getHeapModule()->getInstanceHeap(),
            info.getHeapModule()->getNWModelInstanceHeap(),
            info.getHeapModule()->getNWMotionInstanceHeap(),
            static_cast<u8>(data->m_resGroupNo), -1, &g_soParamAccesserNull, 0, 0, 0, 0, 0, 0, 0)),
    m_param(data->m_param) {
    Weapon::deactivate(false);
}

// Registers the Mario fireball article (kind 0) in the class info table during static initialization.
wnClassInfoImpl<0, wnMarioFireball, wnMarioFireballData> g_wnMarioFireballClassInfo;

template soInsideEventManageModuleBuilder<
    wnMarioFireballModuleAccesserBuildConfig::InsideEventManageModuleBuildConfig,
    wnMarioFireballModuleAccesserBuildConfig::InsideEventManageModuleTypes>::soInsideEventManageModuleBuilder();
template soMotionModuleBuilder<
    wnMarioFireballModuleAccesserBuildConfig::MotionModuleBuildConfig>::soMotionModuleBuilder(soModuleAccesser*, void*);
template soPostureModuleBuilder<
    wnMarioFireballModuleAccesserBuildConfig::PostureModuleBuildConfig>::soPostureModuleBuilder(soModuleAccesser*, soEventObserverRegistrationDesc*);
template soCollisionAttackModuleBuilder<
    wnMarioFireballModuleAccesserBuildConfig::CollisionAttackModuleBuildConfig>::soCollisionAttackModuleBuilder(soModuleAccesser*, int, u8, soEventObserverRegistrationDesc*);
template soSoundModuleBuilder<
    wnMarioFireballModuleAccesserBuildConfig::SoundModuleBuildConfig>::soSoundModuleBuilder(soModuleAccesser*, soSoundIdExchanger*, soEventObserverRegistrationDesc*);
template soStatusModuleBuilder<
    wnMarioFireballModuleAccesserBuildConfig::StatusModuleBuildConfig>::soStatusModuleBuilder(soModuleAccesser*, const soStatusData*, void*);
template soKineticModuleBuilder<
    wnMarioFireballModuleAccesserBuildConfig::KineticModuleBuildConfig>::soKineticModuleBuilder(soModuleAccesser*);
template soEffectModuleBuilder<
    wnMarioFireballModuleAccesserBuildConfig::EffectModuleBuildConfig>::soEffectModuleBuilder(soModuleAccesser*, void*, void*, void*, void*, soEventObserverRegistrationDesc*);

// BYTES: the module accesser builder constructor in sora_melee calls every module getter and member builder out of line.
#pragma dont_inline on
soModuleAccesserBuilder<wnMarioFireballModuleAccesserBuildConfig>::soModuleAccesserBuilder(
    StageObject* owner, const wnModuleAccesserBuildData& bd) :
    unk194(),
    m_moduleAccsr(
        owner,
        m_resourceModuleBuilder.getModule(),
        m_modelModuleBuilder.getModule(),
        m_motionBuilder.getModule(),
        m_postureModuleBuilder.getModule(),
        m_groundModuleBuilder.getModule(),
        m_situationModuleBuilder.getModule(),
        m_teamBuilder.getModule(),
        m_attackModuleBuilder.getModule(),
        m_hitModuleBuilder.getModule(),
        m_shieldModuleBuilder.getModule(),
        m_reflectorModuleBuilder.getModule(),
        m_absorberBuilder.getModule(),
        m_collisionCatchModuleBuilder.getModule(),
        m_searchBuilder.getModule(),
        m_damageModuleBuilder.getModule(),
        m_catchModuleBuilder.getModule(),
        m_captureModuleBuilder.getModule(),
        m_stopModuleBuilder.getModule(),
        m_turnModuleBuilder.getModule(),
        m_shakeModuleBuilder.getModule(),
        m_soundModuleBuilder.getModule(),
        m_linkModuleBuilder.getModule(),
        m_visibilityModuleBuilder.getModule(),
        m_controllerModuleBuilder.getModule(),
        m_cameraModuleBuilder.getModule(),
        m_workManageModuleBuilder.getModule(),
        m_debugModuleBuilder.getModule(),
        m_animCmdBuilder.getModule(),
        m_statusBuilder.getModule(),
        m_generalTermDecideModuleBuilder.getModule(),
        m_switchDecideModuleBuilder.getModule(),
        m_kineticBuilder.getModule(),
        unk194.getModule(),
        m_generateArticleBuilder.getModule(),
        m_effectModuleBuilder.getModule(),
        m_comboModuleBuilder.getModule(),
        (soAreaModule*)m_areaBuilder.getModule(),
        m_territoryModuleBuilder.getModule(),
        m_targetSearchModuleBuilder.getModule(),
        m_physicsModuleBuilder.getModule(),
        m_slopeModuleBuilder.getModule(),
        m_shadowModuleBuilder.getModule(),
        m_itemManageModuleBuilder.getModule(),
        m_colorBlendModuleBuilder.getModule(),
        m_jostleModuleBuilder.getModule(),
        m_abnormalModuleBuilder.getModule(),
        m_slowModuleBuilder.getModule(),
        m_reflectModuleBuilder.getModule(),
        m_heapModuleBuilder.getModule(),
        m_paramCustomizeModuleBuilder.getModule(),
        m_glowModuleBuilder.getModule()),
    m_heapModuleBuilder(bd.getInstanceHeap(), bd.getNWModelInstanceHeap(), bd.getNWMotionInstanceHeap(), bd.getHeapSlotNo()),
    m_paramCustomizeModuleBuilder(&m_moduleAccsr),
    m_resourceModuleBuilder(bd.getMdlResId(), bd.getAnmResId(), bd.getResGroupNo(), &m_moduleAccsr),
    m_modelModuleBuilder(&m_moduleAccsr, bd.getModelScale(), bd.getModelExtendNodeTable(), soEventObserverRegistrationDescNull::getNullInstance()),
    m_motionBuilder(&m_moduleAccsr, bd.getMotionData()),
    m_postureModuleBuilder(&m_moduleAccsr, soEventObserverRegistrationDescNull::getNullInstance()),
    m_groundModuleBuilder(&m_moduleAccsr, bd.getGroundConditionChecker()),
    m_situationModuleBuilder(&m_moduleAccsr, static_cast<soEventManager&>(m_moduleAccsr.getEventManageModule()).getManageId(), soEventObserverRegistrationDescNull::getNullInstance()),
    m_teamBuilder(bd.getTeam(), &m_moduleAccsr),
    m_attackModuleBuilder(&m_moduleAccsr, owner->getId(), static_cast<u8>(owner->getCategory()), soEventObserverRegistrationDescNull::getNullInstance()),
    m_hitModuleBuilder(&m_moduleAccsr, owner->getId(), static_cast<gfTask::Category>(static_cast<u8>(owner->getCategory())), soEventObserverRegistrationDescNull::getNullInstance()),
    m_shieldModuleBuilder(&m_moduleAccsr, owner->getId(), static_cast<gfTask::Category>(static_cast<u8>(owner->getCategory()))),
    m_reflectorModuleBuilder(&m_moduleAccsr, owner->getId(), static_cast<gfTask::Category>(static_cast<u8>(owner->getCategory()))),
    m_absorberBuilder(&m_moduleAccsr, owner->getId(), static_cast<u8>(owner->getCategory())),
    m_collisionCatchModuleBuilder(&m_moduleAccsr, owner->getId(), static_cast<u8>(owner->getCategory())),
    m_searchBuilder(&m_moduleAccsr, owner->getId(), static_cast<u8>(owner->getCategory())),
    m_damageModuleBuilder(&m_moduleAccsr, soEventObserverRegistrationDescNull::getNullInstance()),
    m_catchModuleBuilder(&m_moduleAccsr),
    m_captureModuleBuilder(&m_moduleAccsr),
    m_stopModuleBuilder(&m_moduleAccsr),
    m_turnModuleBuilder(&m_moduleAccsr),
    m_shakeModuleBuilder(&m_moduleAccsr, bd.getShakeData()),
    m_soundModuleBuilder(&m_moduleAccsr, bd.getSoundIdExchanger(), soEventObserverRegistrationDescNull::getNullInstance()),
    m_linkModuleBuilder(&m_moduleAccsr),
    m_visibilityModuleBuilder(&m_moduleAccsr, bd.getVisibilityData()),
    m_controllerModuleBuilder(&m_moduleAccsr, static_cast<soEventManager&>(m_moduleAccsr.getEventManageModule()).getManageId()),
    m_cameraModuleBuilder(&m_moduleAccsr, (soSet<soCameraRange>*)bd.getCameraRangeSet(), (soSet<soCameraClipSphere>*)bd.getCameraClipSphereSet(), soEventObserverRegistrationDescNull::getNullInstance()),
    m_workManageModuleBuilder(&m_moduleAccsr, bd.getParamAccesser()),
    m_animCmdBuilder(static_cast<soEventManager&>(m_moduleAccsr.getEventManageModule()).getManageId()),
    m_statusBuilder(&m_moduleAccsr, bd.getStatusData(), bd.getPreCheckAnimCmdData()),
    m_generalTermDecideModuleBuilder(),
    m_switchDecideModuleBuilder(),
    m_kineticBuilder(&m_moduleAccsr),
    m_generalWorkBuilder(),
    m_generateArticleBuilder(&m_moduleAccsr),
    m_effectModuleBuilder(&m_moduleAccsr, bd.getEffectNodeData(), bd.getEffectEmitData(), bd.getEffectCommonData(), bd.getEffectScreenData(), soEventObserverRegistrationDescNull::getNullInstance()),
    m_comboModuleBuilder(&m_moduleAccsr),
    m_areaBuilder(&m_moduleAccsr, bd.getAreaCategory(), soEventObserverRegistrationDescNull::getNullInstance()),
    m_territoryModuleBuilder(&m_moduleAccsr, bd.getTerritoryParam(), bd.getTerritoryRect()),
    m_targetSearchModuleBuilder(&m_moduleAccsr, bd.getTargetSearchParam()),
    m_physicsModuleBuilder(&m_moduleAccsr, bd.getIkData()),
    m_slopeModuleBuilder(&m_moduleAccsr, bd.getSlopeAngleLimit()),
    m_shadowModuleBuilder(&m_moduleAccsr),
    m_itemManageModuleBuilder(&m_moduleAccsr, bd.getItemNodeData()),
    m_colorBlendModuleBuilder(&m_moduleAccsr),
    m_jostleModuleBuilder(&m_moduleAccsr, bd.getJostleData()),
    m_abnormalModuleBuilder(&m_moduleAccsr),
    m_slowModuleBuilder(&m_moduleAccsr),
    m_reflectModuleBuilder(&m_moduleAccsr),
    m_glowModuleBuilder(&m_moduleAccsr),
    m_debugModuleBuilder(&m_moduleAccsr, soEventObserverRegistrationDescNull::getNullInstance()) {
    if (m_generalWorkBuilder.getWork()->isNull() == false) {
        getModuleAccesser()->getWorkManageModule().setWork(1, m_generalWorkBuilder.getWork());
    }
}
#pragma dont_inline reset

#pragma push
#pragma inline_max_size(10000)
#pragma inline_max_total_size(20000)
#pragma inline_depth(8)
wnMarioFireball::~wnMarioFireball() { }
#pragma pop
