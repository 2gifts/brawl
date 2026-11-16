#define FT_POOL_ROOT_NONPOLYMORPHIC // BYTES: without it the hierarchy destructor is never_inline and the mediator calls it instead of expanding it
#define WN_ANIMCMD_STACK_PTR // MATCH-ONLY: a pointer return stays in a callee-saved register across getManageId (ft_builder_animcmd.h)
#define WN_PARAM_CUSTOMIZE_INLINE_DTOR // BYTES: the builder destructors expand the empty param customize module destructor
#define WN_PARAM_ACCESSER_INLINE_DTOR // BYTES: the empty soParamAccesser base destructor is inlined in sora_melee
#define MT_VEC3F_IMPLICIT_ASSIGN // BYTES: activate copies the Vec3f position as an aggregate (word moves), see mt_vector.h
#define FT_MODULE_BUILDER
#define WN_MARIO_FIREBALL_UNIT // the sora_melee article unit mode shared with the Mario fireball (builders out of line)
#define WN_PEACH_KINOPIO_UNIT
#define WN_PEACH_KINOPIO_TOAD_UNIT // the unit that owns the Toad: its own spore pool chain, observer list and builder constructor order
#define SO_ARTICLE_EVENT_OBSERVER_FULL // BYTES: the 16-byte article event observer with its out-of-line constructor (so/article/so_article.h)
#define SO_COLLISION_GROUP_ALIGNED
#define WN_WEAPON_BUILDER
#define WN_ARTICLE_NATIVE_BUILDERS
#define WN_STATUS_NO_TAIL
#define WN_ARTICLE_ANIMCMD_INT // BYTES: the article and builder vtables take Weapon's int-index notifyEventAnimCmd thunk (@72@), as in the original (.data 0x89368, 0x895D4)
#define WN_TRANSITION_CTOR_INLINE
#define WN_KINETIC_MEDIATOR_INLINE_DTOR
#define WN_KINETIC_GENERIC_BUILDER
#define FT_REL_LINK_EXTERN
#define SO_NULLABLE_INLINE_CTOR
#define WN_ANIMCMD_RESET_MODE_BOX // BYTES: word-aligned reset-mode temporaries (frame size of the anim cmd sub builder)
#define WN_ANIMCMD_PRESENTER_LOCAL_STORE // BYTES: the interpreters' presenters keep the looked-up list in a local (as in the Mario fireball)
// HYPOTHESIS: the transition term pools' 3-parameter soLineHierarchy (ft/builder/ft_builder_transition.h) has another
// name or parameter list in the original; it is renamed while the builders are included so that the spore pool chain can
// carry the name of the original symbol (see wn_peach_kinopio.h). Its weak copies are duplicates the linker drops.
#define soLineHierarchy soLineHierarchyOfArrayPool
#include <wn/wn_module_accesser_builder.h>
#undef soLineHierarchy
#include <wn/peach/wn_peach_kinopio.h>
#include <wn/wn_class_info_impl.h>
#include <so/so_external_value_accesser.h>
#include <so/so_module_accesser.h>
#include <so/model/so_model_module_impl.h>
#include <gf/gf_task_scheduler.h>

// The base of the mediators: an empty destructor that sora_melee always expands (it has no out-of-line copy there).
inline soArticleMediator::~soArticleMediator() { }
// BYTES: the same for the empty team, sound generator accesser and resource id accesser destructors: the builder
// destructors (Toad and wnWeaponBuilder<>) expand them to nothing instead of calling the weak copies.
inline soTeam::~soTeam() { }
inline soSound3dGeneratorAccesserImpl::~soSound3dGeneratorAccesserImpl() { }
inline soResourceIdAccesser::~soResourceIdAccesser() { }

// BYTES: the original unit emits the weapon builder destructor right after the article constructor and before the
// destructors it calls. Instantiated from inside the constructor it comes after them; this explicit instantiation ahead
// of the constructor fixes the point of instantiation (the weak copy is still emitted after the constructor).
template wnWeaponBuilder<wnPeachKinopioModuleAccesserBuildConfig>::~wnWeaponBuilder();

// The Toad (Peach's neutral special counter article, kind 93). It spawns up to six spores (kind 94).
wnPeachKinopio::wnPeachKinopio(s32 articleId, const wnArticleBuildInfo& info, const wnPeachKinopioData* data) :
    wnWeaponBuilder<wnPeachKinopioModuleAccesserBuildConfig>(
        "PeachKinopio", articleId, info,
        wnModuleAccesserBuildData(93, data, info.getHeapModule()->getInstanceHeap(),
            info.getHeapModule()->getNWModelInstanceHeap(),
            info.getHeapModule()->getNWMotionInstanceHeap(),
            static_cast<u8>(data->m_resGroupNo), -1, &m_paramAccesser, 0, 0, 0, 0, 0, 0, 0)),
    m_param(data->m_param), m_paramAccesser(data->m_param), unkF4(0.0f) {
    Weapon::deactivate(false);
}

// BYTES: the .text order of the original unit is the article constructor (with its weak copies, the weapon builder
// destructor first), the parameter accesser destructor, getModuleAccesser, the module accesser builder constructor, the
// animation command sub builder constructor, activate and the article destructor; MWCC emits weak copies in definition
// order, so these are defined here (the Light Bow arrow's recipe).
inline wnPeachKinopioParamAccesser::~wnPeachKinopioParamAccesser() { }

inline soModuleAccesser* soModuleAccesserBuilder<wnPeachKinopioModuleAccesserBuildConfig>::getModuleAccesser() {
    return &m_moduleAccsr;
}

// Registers the Peach Kinopio article class (kind 93) in the class info table during static initialization.
wnClassInfoImpl<93, wnPeachKinopio, wnPeachKinopioData> g_wnPeachKinopioClassInfo;

// BYTES: the module accesser builder constructor in sora_melee calls every module getter and member builder out of line.
#pragma dont_inline on
soModuleAccesserBuilder<wnPeachKinopioModuleAccesserBuildConfig>::soModuleAccesserBuilder(
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

// BYTES: defined here (see the specialization in wn_peach_kinopio.h): the original emits it after the module accesser
// builder constructor and its weak copies. The body is the shared template's.
wnAnimCmdModuleSubBuilder<wnPeachKinopioModuleAccesserBuildConfig::AnimCmdModuleSubBuildConfig>::wnAnimCmdModuleSubBuilder(
    soModuleAccesser* acc, const soModuleAccesserBuildData& bd) :
    m_unit0(acc, bd.getAnimCmdData(0, 0), bd.getAnimCmdData(0, 1), bd.getAnimCmdData(0, 2)),
    m_unit1(acc, bd.getAnimCmdData(1, 0), bd.getAnimCmdData(1, 1), bd.getAnimCmdData(1, 2)),
    m_unit2(acc, bd.getAnimCmdData(2, 0), bd.getAnimCmdData(2, 1), bd.getAnimCmdData(2, 2)),
    m_unit3(acc, bd.getAnimCmdData(3, 0), bd.getAnimCmdData(3, 1), bd.getAnimCmdData(3, 2)),
    m_unit4(acc),
    m_unit5(acc, bd.getAnimCmdData(5, 0), bd.getAnimCmdData(5, 1), bd.getAnimCmdData(5, 2)) { }

// Sets the Toad up for a throw: the article stays where it is, linked to the founder, with the common weapon flags.
// HYPOTHESIS: the parameter meanings; the descriptor fields follow the Kassar activator, which this one repeats.
void wnPeachKinopio::activate(int founderTaskId, int resourceId, int team, float lr) {
    Vec3f position(0.0f, 0.0f, 0.0f);
    wnActivateDesc desc(founderTaskId, resourceId, resourceId, resourceId, -1, -1, 0, 0, position, 0.0f, team, 0, 2, 0x80, 0,
                        0x35F, 0);
    // BYTES: the two top flag bits are set one by one (the pair form loads the flag byte into another register).
    desc.flagBit7 = 1;
    desc.flagBit6 = 1;
    desc.flagsMidPair = 0;
    desc.flagsBit3 = 1;
    desc.flagsLow = 0;
    desc.unk4DHigh = 0;
    Weapon::activate(&desc);
    m_moduleAccesser->getWorkManageModule().setFloat(lr, 0x11000001);
}

// The destructor (and wnWeaponBuilder<>'s, 0x332F04) expands the simple module builders down to their impl destructors;
// the big ones (effect, generate-article, kinetic, status, animation command, attack, motion, inside event) stay calls.
// Exact once the empty base destructors above are inline.
wnPeachKinopio::~wnPeachKinopio() { }

// ---- the spore pool ---------------------------------------------------------------------------------------------------
// The mediator hands out the first inactive spore (newest first), else deactivates the oldest active one and reuses it.
// Slot 0 is the only article id; the 16 other slots answer with the neutral value.

// Throws a spore from the node of the Toad's model given by a constant, plus a constant offset; it faces like the Toad's
// founder (the stage object whose task id the Toad keeps in work int 0x10000000), else like the work float 0x11000001.
// HYPOTHESIS: the roles of the constants 0x5DC0 (model node), 0xFA0 / 0xFA1 (offsets) and of the two work values.
// The activator answers false, so generate() hands back the null article even for a spore it just activated (the original
// does this: returning the spore does not match).
inline bool wnPeachKinopioActivateSpore(wnPeachKinopioSpore* spore, soModuleAccesser* acc) {
    Vec3f pos = acc->getModelModule().getNodeGlobalPosition(soValueAccesser::getConstantInt(acc, 0x5DC0, 0), false);
    pos.m_x += soValueAccesser::getConstantFloat(acc, 0xFA0, 0);
    pos.m_y += soValueAccesser::getConstantFloat(acc, 0xFA1, 0);
    pos.m_z = 0.0f;
    float lr = acc->getWorkManageModule().getFloat(0x11000001);
    gfTask* task = gfTaskScheduler::getInstance()->getTaskById(static_cast<gfTask::Category>(10),
                                                                acc->getWorkManageModule().getInt(0x10000000));
    StageObject* founder = dynamic_cast<StageObject*>(task);
    if (founder != NULL) {
        lr = soExternalValueAccesser::getLr(founder);
    }
    s32 ownerTaskId = acc->m_stageObject->m_taskId;
    spore->activate(ownerTaskId, acc->getTeamModule().getTeam()->getNo(), &pos, lr);
    return false;
}

template <class PoolTL, class AssignTL, class KindInfo, class Extra>
template <int S, int L>
inline soArticle* soArticleMediatorImpl<PoolTL, AssignTL, KindInfo, Extra>::generateSlot(soModuleAccesser* acc, const soIntToType<S>&, const soIntToType<L>&) {
    soArticleDeactivateChecker checker;
    wnPeachKinopioSpore* spore = m_pools.find(checker, soIntToType<0>());
    if (spore == NULL) {
        spore = static_cast<wnPeachKinopioSpore*>(checker.getCandidate());
        if (spore == NULL) {
            return wnGetNullArticle();
        }
        spore->deactivateArticle();
    }
    if (wnPeachKinopioActivateSpore(spore, acc) == true) {
        return &static_cast<soArticle&>(*spore);
    }
    return wnGetNullArticle();
}

template <class PoolTL, class AssignTL, class KindInfo, class Extra>
template <int S, int L>
inline bool soArticleMediatorImpl<PoolTL, AssignTL, KindInfo, Extra>::isGeneratableSlot(const soIntToType<S>&, const soIntToType<L>&) {
    bool allActive;
    s32 i;
    for (i = 0; i < 6; ++i) {
        if (static_cast<soArticle&>(*m_pools.getInstanceAt(i)).isActiveArticle() == false) {
            allActive = false;
            goto done;
        }
    }
    allActive = true;
done:
    return allActive == false;
}

template <class PoolTL, class AssignTL, class KindInfo, class Extra>
template <int S, int L>
inline s32 soArticleMediatorImpl<PoolTL, AssignTL, KindInfo, Extra>::getActiveNumSlot(const soIntToType<S>&, const soIntToType<L>&) {
    wnArticleSlotCounter counter;
    counter.m_isActive = wnArticleIsActive;
    counter.m_activeNum = 0;
    counter.m_inactiveNum = 0;
    for (s32 i = 0; i < 6; ++i) {
        wnArticleSlotCounter* c = &counter;
        if (c->m_isActive(&static_cast<soArticle&>(*m_pools.getInstanceAt(i))) == true) {
            ++c->m_activeNum;
        } else {
            ++c->m_inactiveNum;
        }
    }
    return counter.m_activeNum;
}

// BYTES: the mediator's virtuals are members of the class template, instantiated through its vtable, not explicit
// specializations: MWCC then compiles them after it has emitted the unit's vtables, so their jump tables follow the
// vtables in .data (0x89F10) as in the original. Only the Toad's mediator instantiates them.
template <class PoolTL, class AssignTL, class KindInfo, class Extra>
soArticle* soArticleMediatorImpl<PoolTL, AssignTL, KindInfo, Extra>::generate(s32 articleId, soModuleAccesser* acc) {
    switch (articleId) {
    case 0: return generateSlot(acc, WN_ARTICLE_SLOT_TAGS);
    WN_ARTICLE_UNUSED_SLOTS_FROM_1(wnGetNullArticle())
    default: return wnGetNullArticle();
    }
}

template <class PoolTL, class AssignTL, class KindInfo, class Extra>
bool soArticleMediatorImpl<PoolTL, AssignTL, KindInfo, Extra>::isGeneratable(soModuleAccesser*, s32 articleId) {
    switch (articleId) {
    case 0: return isGeneratableSlot(WN_ARTICLE_SLOT_TAGS);
    WN_ARTICLE_UNUSED_SLOTS_FROM_1(false)
    default: return false;
    }
}

template <class PoolTL, class AssignTL, class KindInfo, class Extra>
s32 soArticleMediatorImpl<PoolTL, AssignTL, KindInfo, Extra>::getActiveNum(soModuleAccesser*, s32 articleId) {
    switch (articleId) {
    case 0: return getActiveNumSlot(WN_ARTICLE_SLOT_TAGS);
    WN_ARTICLE_UNUSED_SLOTS_FROM_1(0)
    default: return 0;
    }
}

template <class PoolTL, class AssignTL, class KindInfo, class Extra>
s32 soArticleMediatorImpl<PoolTL, AssignTL, KindInfo, Extra>::getGenerateMaxNum(s32 articleId) {
    switch (articleId) {
    case 0: return wnArticleSlotGenerateMaxNum(6, WN_ARTICLE_SLOT_TAGS);
    WN_ARTICLE_UNUSED_SLOTS_FROM_1(0)
    default: return 0;
    }
}

// Shooting only type-checks the article; the unused slots accept anything.
template <class PoolTL, class AssignTL, class KindInfo, class Extra>
bool soArticleMediatorImpl<PoolTL, AssignTL, KindInfo, Extra>::shoot(soModuleAccesser*, soArticle* article) {
    switch (article->getArticleId()) {
    case 0: return wnArticleSlotShoot<wnPeachKinopioSpore>(article, WN_ARTICLE_SLOT_TAGS);
    WN_ARTICLE_UNUSED_SLOTS_FROM_1(true)
    default: return false;
    }
}

template <class PoolTL, class AssignTL, class KindInfo, class Extra>
void soArticleMediatorImpl<PoolTL, AssignTL, KindInfo, Extra>::deactivate() {
    for (s32 i = 0; i < 6; ++i) {
        wnPeachKinopioSpore* spore = m_pools.getInstanceAt(i);
        if (!static_cast<soArticle&>(*spore).setDeactivateDescendant()) {
            return;
        }
    }
}

// Open items of this unit (see the per-function notes above). The spore itself (constructor, builders, activate, kinetic
// mediator) is the unit wn_peach_kinopio_spore.cpp (0x3373F8..0x338B24) and its transactor
// wn_peach_kinopio_spore_kinetic_transactor.cpp (0x338B24..0x338FE0); this unit ends with the Toad's weak thunks (0x3373F8).
// Every function of the unit is exact (44/44) and the .text order equals the original's (checked against the original
// unit's function order with a direct compile).
// TODO(link): one step left. The unit has its .data/.bss splits and the weak copies it calls are named; a flip with this
//   source minus WN_ARTICLE_ANIMCMD_INT linked sora_melee with every section byte equal and only two relocations
//   different: the notifyEventAnimCmd slots of the two vtables above, which that macro fixes (checked by direct compile
//   only). Flip the unit to Matching in configure.py and run the hash check.
// (Batch 3 note, not re-checked: by the relocations of the exact functions, only fn_27_2F5D2C and fn_27_318C34 still
// differ from the original's call targets.)
// TODO(callees): fn_27_2E1E40 / 2E1E44 / 2E1E48 / 2F563C (situation, stop, visibility, colour blend builders), fn_27_2F4FB4
//   (null ground getModule) and fn_27_30921C / 30922C (motion builder getModule / constructor) are weak functions outside
//   this unit, still unnamed in symbols.txt. The map names
//   fn_27_1E09C4 (in wn_simple's range) the soAnimCmdModuleBuilder<soAnimCmdModuleBuildConfig<6, soAnimCmdModuleImpl>>
//   destructor and fn_27_3085C4 (in wn_yoshi_tamago's range) the two-slot soMotionModuleBuilder destructor that the two
//   builder destructors here call.
