#define FT_SOUND_GENERATOR_DTOR_NOINLINE
#define FT_MODULE_ACCESSER_BUILDER_BASE_BUILD_DATA
#define FT_POOL_ROOT_NONPOLYMORPHIC
#define SO_GENERAL_WORK_BOUNDED_CLEAR
#define SO_INSTANCE_MANAGER_FULL_PROPERTY_DTOR_OUT_OF_LINE
#define FT_TEAM_TYPED_INTERFACE
#define SO_ARRAY_SPLIT_VECTOR_CTOR
#define FT_SAMUS_ARTICLE_POOLS
#define WN_ARTICLE_MANAGE_BUILDER_OUT_OF_LINE
#define FT_KINETIC_IMPL_DTOR_OUT_OF_LINE
#define WN_PARAM_ACCESSER_INLINE_DTOR // the grapple beam teardown expands the empty soParamAccesser destructor
#define WN_KINETIC_MEDIATOR_INLINE_DTOR
#define SO_STATUS_KINETIC_SLOT // the grapple beam article: an empty slot between its status and kinetic builders (ft_module_accesser_builder.h)
#include <ft/builder/ft_dol_array_list.h>
#include <ft/ft_manager.h>
#include <so/work/so_general_work_simple.h>
#include <ft/ft_class_info_impl.h>
#include <ft/samus/ft_samus.h>
#include <ft/samus/ft_samus_status_uniq_process.h>
#include <ft/ft_change_succeed_work.h>
#include <ft/samus/ft_samus_extend_param_accesser.h>
#include <so/so_external_value_accesser.h>
#include <ft/ft_owner.h>
#include <so/article/so_article_mediator.h>
#include <so/so_value_accesser.h>

// The sound builder teardown inlines this empty destructor (same form as ft_koopa.cpp).
inline soSound3dGeneratorAccesserImpl::~soSound3dGeneratorAccesserImpl() { }

#define FT_BC ftSamusBuildConfig
#include <ft/builder/ft_builder_noinline.h>

// Status teardown destroys its owned change-request queue before observers.
#pragma dont_inline on
#ifndef FT_REL_LINK_EXTERN // with FT_REL_LINK_EXTERN the queue destructor is the sora_melee function (ft_dol_instances.h)
template soArrayVector<s32, 8>::~soArrayVector();
#endif
#pragma dont_inline off
soStatusModuleImpl::~soStatusModuleImpl() { }

ftKineticEnergyController::~ftKineticEnergyController() { }

// ftManager::setParamPattern selects the shared parameter-table variation.
extern int g_soValueVariation;
#pragma dont_inline on
int soValueAccesser::getValueVariation() { return g_soValueVariation; }
#pragma dont_inline off

// HYPOTHESIS: unnamed status process instances (not yet named in symbols.txt); the repeated ones are the null placeholders of the
// unused status ranges.
extern "C" {
extern soStatusUniqProcess lbl_27_bss_75C4;
extern soStatusUniqProcess lbl_27_bss_75E4;
extern soStatusUniqProcess lbl_27_bss_75F4;
extern soStatusUniqProcess lbl_27_bss_75D4;
extern soStatusUniqProcess lbl_27_bss_7604;
}

ftSamusExtendParamAccesser g_ftSamusExtendParamAccesser;

ftClassInfoImpl<Fighter_Samus, ftSamus> g_ftClassInfoSamus;

ftSamus::ftSamus(s32 entryId,
                 Heaps::HeapType instHeap,
                 Heaps::HeapType nwModelInstHeap,
                 Heaps::HeapType nwMotionInstHeap) :
    ftFighterBuilder<ftSamusBuildConfig>(entryId,
                                         Fighter_Samus,
                                         instHeap,
                                         nwModelInstHeap,
                                         nwMotionInstHeap),
    m_data(g_ftCommonDataAccesser.getData(Fighter_Samus)) {
    // Register the status processes in action order.
    soStatusUniqProcess* processes[27] = {0};
    processes[1] = &g_ftSamusStatusUniqProcessSpecialS;
    processes[2] = &g_ftSamusStatusUniqProcessSpecialHi;
    processes[4] = &g_ftSamusStatusUniqProcessFinal;
    processes[5] = &lbl_27_bss_75C4;
    processes[6] = &lbl_27_bss_75E4;
    processes[8] = &lbl_27_bss_75F4;
    processes[9] = &lbl_27_bss_75D4;
    processes[10] = &lbl_27_bss_7604;
    processes[11] = &g_ftSamusStatusUniqProcessSpecialGroundLw;
    processes[12] = &g_ftSamusStatusUniqProcessSpecialAirLw;
    processes[14] = &g_ftSamusStatusUniqProcessSpecialGroundLw;
    processes[15] = &g_ftSamusStatusUniqProcessSpecialAirLw;
    processes[16] = &g_ftSamusStatusUniqProcessSpecialS;
    processes[17] = &g_ftSamusStatusUniqProcessSpecialS;
    processes[18] = &g_ftSamusStatusUniqProcessSpecialS;
    processes[19] = &g_ftSamusStatusUniqProcessSpecialS;
    processes[21] = &g_ftSamusStatusUniqProcessFinal;
    processes[22] = &g_ftSamusStatusUniqProcessFinal;
    processes[23] = &g_ftSamusStatusUniqProcessFinal;
    processes[24] = &g_ftSamusStatusUniqProcessFinal;
    processes[25] = &g_ftSamusStatusUniqProcessFinal;
    processes[26] = &g_ftSamusStatusUniqProcessFinal;
    m_moduleAccesser->getStatusModule().addRangeUniqProc(processes, 27);
}



























// ---- the article holders --------------------------------------------------------------------------------------------
#include <ft/builder/ft_builder_article.h>

struct ftSamusArticleKind : wnArticleKindInfo {
    ftKind getSubKind() const { return static_cast<ftKind>(subKind); }
    ftSamusArticleKind() {
        kind = 0;
        subKind = Fighter_Samus;
    }
};

// The data blocks of the charge shot and the grapple beam come from the common data accessor of sora_melee (only declared here).
extern "C" void* fn_27_1492F8(const ftCommonDataAccesser* accesser, s32 subKind, bool* resourceGroup);
extern "C" void* fn_27_149464(const ftCommonDataAccesser* accesser, s32 subKind, bool* resourceGroup);

FT_DEFINE_ARTICLE_HOLDER(, wnSamusCShot, wnSamusCShotData, 0, ftSamusArticleKind, fn_27_1492F8)
FT_DEFINE_ARTICLE_HOLDER_MEMBER(, wnSamusBomb, const wnSamusBombData, 1, ftSamusArticleKind, getWeaponData<wnSamusBombData>)
FT_DEFINE_ARTICLE_HOLDER_MEMBER(, wnSamusMissile, const wnSamusMissileData, 2, ftSamusArticleKind, getWeaponData<wnSamusMissileData>)
FT_DEFINE_ARTICLE_HOLDER_MEMBER(inline, wnSamusLaser, const wnSamusLaserData, 3, ftSamusArticleKind, getWeaponData<wnSamusLaserData>)
FT_DEFINE_ARTICLE_HOLDER_MEMBER(, wnSamusLaser2, const wnSamusLaser2Data, 4, ftSamusArticleKind, getWeaponData<wnSamusLaser2Data>)
FT_DEFINE_ARTICLE_HOLDER_MEMBER(inline, wnSamusGun, const wnSamusGunData, 5, ftSamusArticleKind, getWeaponData<wnSamusGunData>)
FT_DEFINE_ARTICLE_HOLDER_MEMBER(, wnSamusSuperMissile, const wnSamusSuperMissileData, 6, ftSamusArticleKind, getWeaponData<wnSamusSuperMissileData>)
FT_DEFINE_ARTICLE_HOLDER(inline, wnSamusGBeam, wnSamusGBeamData, 7, ftSamusArticleKind, fn_27_149464)

// ---- the articles and their mediator --------------------------------------------------------------------------------

// The native ownership chain calls these destructors out of line.
#pragma dont_inline on
template <class W>
wnArticleHolder<W>::~wnArticleHolder() { }
template <class W, int N>
wnArticleSubPool<W, N>::~wnArticleSubPool() { }
template wnArticleHolder<wnSamusCShot>::~wnArticleHolder();
template wnArticleHolder<wnSamusBomb>::~wnArticleHolder();
template wnArticleHolder<wnSamusMissile>::~wnArticleHolder();
template wnArticleHolder<wnSamusLaser>::~wnArticleHolder();
template wnArticleHolder<wnSamusLaser2>::~wnArticleHolder();
template wnArticleHolder<wnSamusGun>::~wnArticleHolder();
template wnArticleHolder<wnSamusSuperMissile>::~wnArticleHolder();
template wnArticleHolder<wnSamusGBeam>::~wnArticleHolder();
template <class Mediator>
wnSelectedArticleMediator<Mediator>::~wnSelectedArticleMediator() { }
template <class Mediator, int N>
wnArticleManageModuleBuilder<Mediator, N>::~wnArticleManageModuleBuilder() { }
template <class Mediator, int N>
wnArticleManageModuleBuilder<Mediator, N>::wnArticleManageModuleBuilder(soModuleAccesser* acc) :
    m_articles(), m_observers(N, 0), m_mediator(acc), m_module(acc, &m_articles, m_mediator.getMediator(), &m_observers) { }
template wnSelectedArticleMediator<ftSamusArticleMediator>::~wnSelectedArticleMediator();
template wnArticleManageModuleBuilder<ftSamusArticleMediator, 10>::~wnArticleManageModuleBuilder();
template wnArticleManageModuleBuilder<ftSamusArticleMediator, 10>::wnArticleManageModuleBuilder(soModuleAccesser*);
ftSamusArticleMediator::~ftSamusArticleMediator() { }
#pragma dont_inline off

s32 ftSamusArticleMediator::getMediateNum() { return 8; }
void ftSamusArticleMediator::setAutoRecycle(bool enabled) { m_autoRecycle = enabled; }

// ---- the fighter -----------------------------------------------------------------------------------------------------
void ftSamus::onStart(int startKind) {
    soModuleAccesser* acc = getModuleAccesser();
    soWorkManageModule& workManage = acc->getWorkManageModule();
    if (startKind == 8) {
        // HYPOTHESIS: int 0x10000040 holds the effect handle of the charge glow that is removed on this start kind
        int effectId = workManage.getInt(0x10000040);
        if (effectId != 0) {
            m_moduleAccesser->getEffectModule().remove(effectId);
        }
    }
    workManage.setInt(0, 0x10000040);
    workManage.setInt(0, 0x10000041);
    workManage.setInt(0, 0x10000042);
    workManage.offFlag(0x1200003e);
    workManage.offFlag(0x1200003d);
    acc->getEffectModule().removeCommon(0x1a);
    Fighter::onStart(startKind);
}

void ftSamus::onDeadEnd() {
    soModuleAccesser* acc = m_moduleAccesser;
    soWorkManageModule& workManage = acc->getWorkManageModule();
    if (workManage.isFlag(0x1200003d)) {
        workManage.offFlag(0x1200003d);
        if (static_cast<u8>(g_ftManager->getFighterCount(m_entryId)) > 1) {
            int fighterNo = g_ftManager->getFighterNo(m_entryId, m_taskId);
            // The motion module is set back to frame 0 before the next fighter is chosen.
            acc->getMotionModule().setFrame(0.0f);
            u8 other = (u8)fighterNo == 0 ? 1 : 0;
            g_ftManager->toChange(m_entryId, other, 0, 0);
        }
    }
    Fighter::onDeadEnd();
}

// The charge glow of the Charge Shot: it appears once the charge counter reaches the threshold and follows the scale of the fighter.
// HYPOTHESIS: int 0x10000041 is the charge counter and int 0x10000040 the handle of the glow effect.
void ftSamus::processUpdate() {
    Fighter::processUpdate();
    soModuleAccesser* acc = getModuleAccesser();
    soWorkManageModule& workManage = acc->getWorkManageModule();
    soEffectModule& effects = acc->getEffectModule();
    int* chargeParam = static_cast<int*>(g_ftCommonDataAccesser.getData(Fighter_Samus)->extendParam[0]);
    if (acc->getWorkManageModule().getInt(0x10000041) >= *chargeParam) {
        if (workManage.getInt(0x10000040) == 0) {
            // BYTES: the native function constructs this offset (out-of-line Vec3f ctor at the top of the frame) and never reads it.
            Vec3f chargeOffset(-8.0f, 0.0f, 0.0f);
            int handle = effects.reqFollow(static_cast<EfID>(0x40006), 0x2b, &Vec3f(-8.0f, 0.0f, 0.0f), &Vec3f(0.0f, 0.0f, 0.0f), 1.0f, true, 0, 0, -1);
            workManage.setInt(handle, 0x10000040);
        }
        int handle = acc->getWorkManageModule().getInt(0x10000040);
        float scale = acc->getPostureModule().getScale();
        soEffectModule& effects2 = m_moduleAccesser->getEffectModule();
        effects2.setScale(handle, &Vec3f(scale, scale, scale));
        if (acc->getVisibilityModule().get(1) != 1) {
            effects2.setPos(handle, &Vec3f(-8.0f, 0.0f, 0.0f));
        } else {
            effects2.setPos(handle, &Vec3f(0.0f, 0.0f, 0.0f));
        }
    } else if (workManage.getInt(0x10000040) != 0) {
        effects.kill(acc->getWorkManageModule().getInt(0x10000040), true, true);
        workManage.setInt(0, 0x10000040);
    }
}

// The charge shot is started with the kind of the fighter; its charge ratio is set when it is shot.
bool ftSamus::activeArticle(wnSamusCShot* weapon, soModuleAccesser* acc) {
    Fighter& fighter = dynamic_cast<Fighter&>(acc->getStageObject());
    soResourceModule& resources = acc->getResourceModule();
    int founderTaskId = fighter.m_taskId;
    weapon->activate(founderTaskId, resources.getResourceIdAccesser()->getEtcResId(), fighter.getOwner()->getTeam(), acc->getStageObject().soGetSubKind());
    return true;
}

// HYPOTHESIS: int 0x10000042 is the charge counter that is shot; the ratio to the charge limit scales the shot.
bool ftSamus::shootArticle(wnSamusCShot* weapon, soModuleAccesser* acc) {
    int* chargeLimit = static_cast<int*>(g_ftCommonDataAccesser.getParamSamusChargeShot(static_cast<ftKind>(acc->getStageObject().soGetSubKind())));
    weapon->setCharge(static_cast<float>(acc->getWorkManageModule().getInt(0x10000042)) / static_cast<float>(*chargeLimit));
    soPostureModule& posture = acc->getPostureModule();
    weapon->toShoot(posture.getLr(), acc->getWorkManageModule().getInt(0x10000042));
    return false;
}

// The bomb starts with the effect resource and the team of the fighter.
bool ftSamus::activeArticle1(wnSamusBomb* weapon, soModuleAccesser* acc) {
    Fighter& fighter = dynamic_cast<Fighter&>(acc->getStageObject());
    soResourceModule& resources = acc->getResourceModule();
    int founderTaskId = fighter.m_taskId;
    weapon->activate(founderTaskId, resources.getResourceIdAccesser()->getEtcResId(), fighter.getOwner()->getTeam());
    return true;
}

bool ftSamus::shootArticle1(wnSamusBomb* weapon, soModuleAccesser* acc) {
    ftSamusExtendParamSpecialLw* param = static_cast<ftSamusExtendParamSpecialLw*>(g_ftCommonDataAccesser.getParamSamusSpecialLw(static_cast<ftKind>(acc->getStageObject().soGetSubKind())));
    weapon->toShoot(acc->getPostureModule().getLr(), param->unk20[0], param->unk20[1], param->unk20[2]);
    return false;
}

bool ftSamus::activeArticle2(wnSamusMissile* weapon, soModuleAccesser* acc) {
    Fighter& fighter = dynamic_cast<Fighter&>(acc->getStageObject());
    soResourceModule& resources = acc->getResourceModule();
    int founderTaskId = fighter.m_taskId;
    weapon->activate(founderTaskId, resources.getResourceIdAccesser()->getEtcResId(), fighter.getOwner()->getTeam());
    return true;
}

bool ftSamus::shootArticle2(wnSamusMissile* weapon, soModuleAccesser* acc) {
    ftSamusExtendParamSpecialS* param = static_cast<ftSamusExtendParamSpecialS*>(g_ftCommonDataAccesser.getParamSamusSpecialS(static_cast<ftKind>(acc->getStageObject().soGetSubKind())));
    weapon->toShoot(acc->getPostureModule().getLr(), param->unk0C);
    return false;
}

bool ftSamus::activeArticle3(wnSamusSuperMissile* weapon, soModuleAccesser* acc) {
    Fighter& fighter = dynamic_cast<Fighter&>(acc->getStageObject());
    soResourceModule& resources = acc->getResourceModule();
    int founderTaskId = fighter.m_taskId;
    weapon->activate(founderTaskId, resources.getResourceIdAccesser()->getEtcResId(), fighter.getOwner()->getTeam());
    return true;
}

bool ftSamus::shootArticle3(wnSamusSuperMissile* weapon, soModuleAccesser* acc) {
    ftSamusExtendParamSpecialS* param = static_cast<ftSamusExtendParamSpecialS*>(g_ftCommonDataAccesser.getParamSamusSpecialS(static_cast<ftKind>(acc->getStageObject().soGetSubKind())));
    weapon->toShoot(acc->getPostureModule().getLr(), param->unk0C);
    return false;
}

bool ftSamus::activeArticle4(wnSamusLaser* weapon, soModuleAccesser* acc) {
    Fighter& fighter = dynamic_cast<Fighter&>(acc->getStageObject());
    soResourceModule& resources = acc->getResourceModule();
    int founderTaskId = fighter.m_taskId;
    weapon->activate(founderTaskId, resources.getResourceIdAccesser()->getEtcResId(), fighter.getOwner()->getTeam());
    return true;
}

bool ftSamus::shootArticle4(wnSamusLaser* weapon, soModuleAccesser* acc) {
    g_ftCommonDataAccesser.getParamSamusFinal(static_cast<ftKind>(acc->getStageObject().soGetSubKind()));
    weapon->toShoot(acc->getPostureModule().getLr());
    return true;
}

bool ftSamus::activeArticle5(wnSamusLaser2* weapon, soModuleAccesser* acc) {
    Fighter& fighter = dynamic_cast<Fighter&>(acc->getStageObject());
    soResourceModule& resources = acc->getResourceModule();
    int founderTaskId = fighter.m_taskId;
    float unkFloat = acc->getWorkManageModule().getFloat(0x21000004);
    weapon->activate(founderTaskId, resources.getResourceIdAccesser()->getEtcResId(), fighter.getOwner()->getTeam(), unkFloat);
    return true;
}

bool ftSamus::shootArticle5(wnSamusLaser2* weapon, soModuleAccesser* acc) {
    g_ftCommonDataAccesser.getParamSamusFinal(static_cast<ftKind>(acc->getStageObject().soGetSubKind()));
    weapon->toShoot(acc->getPostureModule().getLr());
    return true;
}

// The gun gets the model and the texture of the fighter.
bool ftSamus::activeArticle6(wnSamusGun* weapon, soModuleAccesser* acc) {
    Fighter& fighter = dynamic_cast<Fighter&>(acc->getStageObject());
    int founderTaskId;
    soResourceModule* const resources = &acc->getResourceModule();
    founderTaskId = fighter.m_taskId;
    weapon->activate(founderTaskId, resources->getResourceIdAccesser()->getMdlResId(), resources->getResourceIdAccesser()->getTexResId(), 1, 0, fighter.getOwner()->getTeam());
    return true;
}

bool ftSamus::notifyEventAnimCmd(acAnimCmd* acmd, soModuleAccesser* moduleAccesser, s32 unk3) {
    s8 group = acmd->getGroup();
    if (!isObserv(group)) {
        return false;
    }
    switch (acmd->getType()) {
    case 5:
        // HYPOTHESIS: command 5 switches to the other fighter of a Samus pair (the transformation).
        if (static_cast<u8>(g_ftManager->getFighterCount(m_entryId)) > 1) {
            int fighterNo = g_ftManager->getFighterNo(m_entryId, m_taskId);
            u8 other = (u8)fighterNo == 0 ? 1 : 0;
            g_ftManager->toChange(m_entryId, other, 1, 0);
        }
        return true;
    }
    return Fighter::notifyEventAnimCmd(acmd, moduleAccesser, unk3);
}

// The events of the links with other stage objects. HYPOTHESIS: the event payload sits behind the two header words: a flag
// word at +4, the value or position at +8.
struct ftSamusLinkEventView {
    int kind;
    bool done;
    int value;
};

void ftSamus::notifyEventLink(soLinkEventArgs* eventInfo, soModuleAccesser* moduleAccesser, StageObject* stageObj, int unk4) {
    ftSamusLinkEventView* event = reinterpret_cast<ftSamusLinkEventView*>(eventInfo);
    switch (stageObj->soGetSubKind()) {
    case 0x54:
        switch (event->kind) {
        case 0x838:
            switch (m_moduleAccesser->getStatusModule().getStatusKind()) {
            case 0:
            case 1:
            case 3:
            case 4:
            case 5:
            case 6:
            case 7:
            case 8:
            case 0xa:
            case 0xb:
            case 0xc:
            case 0xd:
            case 0xe:
            case 0xf:
            case 0x11:
            case 0x12:
            case 0x15:
            case 0x16:
            case 0x17:
            case 0x18:
            case 0x115:
            case 0x11d:
            case 0x11e: {
                ftSamusExtendParamSpecialLw* param = static_cast<ftSamusExtendParamSpecialLw*>(g_ftCommonDataAccesser.getData(Fighter_Samus)->extendParam[2]);
                Vec3f pos = moduleAccesser->getPostureModule().getPos();
                Vec3f other = soExternalValueAccesser::getPos(stageObj);
                float ratio = (pos.m_x - other.m_x) / param->unk48;
                if (ratio >= 1.0f) {
                    ratio = 1.0f;
                }
                if (ratio <= -1.0f) {
                    ratio = -1.0f;
                }
                // HYPOTHESIS: the Bomb is launched at an angle around straight up that depends on the distance to the object.
                float angle = 1.5707964f + -param->unk34 * ratio;
                moduleAccesser->getWorkManageModule().setFloat(angle, ftSamusWork::SpecialLw_Angle);
                m_moduleAccesser->getStatusModule().changeStatusRequest(ftSamusStatus::SpecialLw_Unk11F, m_moduleAccesser);
                m_moduleAccesser->getControllerModule().setRumble(0, 10, false, -1);
                event->done = true;
                break;
            }
            }
            return;
        }
    case 0x84:
        switch (event->kind) {
        case 0x838:
            if (m_moduleAccesser->getLinkModule().isLinked(0) == true) {
                StageObject* node = m_moduleAccesser->getLinkModule().getNode(0);
                if (node != NULL) {
                    Vec3f pos = soExternalValueAccesser::getPos(node);
                    *reinterpret_cast<Vec3f*>(&event->value) = pos;
                    event->done = true;
                }
            }
            break;
        case 0x839:
            m_moduleAccesser->getStatusModule().changeStatusRequest(0x39, m_moduleAccesser);
            return;
        case 0x83a:
            event->value = moduleAccesser->getModelModule().getCorrectNodeId(0x12e);
            event->done = true;
            return;
        case 0x83b:
            {
                int* value = static_cast<int*>(soValueAccesser::getConstantIndefinite(moduleAccesser, 0xA80E, 0));
                moduleAccesser->getWorkManageModule().setInt(*value, 0x20000001);
            }
            m_moduleAccesser->getStatusModule().changeStatusRequest(0x81, m_moduleAccesser);
            if (moduleAccesser->getGenerateArticleManageModule().isExist(7) == true) {
                moduleAccesser->getLinkModule().removeModelConstraint(false);
                soLinkModule& link = moduleAccesser->getLinkModule();
                link.setModelConstraintPosOrt(3, moduleAccesser->getModelModule().getCorrectNodeId(0x12e), event->value, 5, false);
            }
            return;
        case 0x83c:
            if (moduleAccesser->getGroundModule().isStCliff(0)) {
                moduleAccesser->getStatusModule().changeStatusRequest(0x75, moduleAccesser);
            } else {
                moduleAccesser->getStatusModule().changeStatusRequest(0x7b, moduleAccesser);
            }
            return;
        case 0x83e:
            moduleAccesser->getStatusModule().changeStatusRequest(0x83, moduleAccesser);
            return;
        }
        break;
    }
    Fighter::notifyEventLink(eventInfo, moduleAccesser, stageObj, unk4);
}

// The fighter that takes over (the transformation partner) is started with the work of this one.
void ftSamus::change(void* work, int mode) {
    onStart(mode);
    Fighter::changeSucceedCore(work, 0x126);
}

// The grapple beam starts at the hand node; the three whip-like statuses 0x34, 0x36 and 0x38 use it as an attack.
// HYPOTHESIS: the model constraint ties the beam to the hand node unless the status is 0x3c.
static inline int getFighterTaskId(Fighter& fighter) {
    return fighter.m_taskId;
}
bool ftSamus::activeArticle7(wnSamusGBeam* weapon, soModuleAccesser* acc) {
    Fighter& fighter = dynamic_cast<Fighter&>(acc->getStageObject());
    Vec3f pos = acc->getModelModule().getNodeGlobalPosition(0x2b, false);
    float lr = acc->getPostureModule().getLr();
    soResourceModule* resources;
    int founderTaskId;
    bool attack = false;
    switch (acc->getStatusModule().getStatusKind()) {
    case 0x34:
    case 0x36:
    case 0x38:
        attack = true;
        break;
    }
    resources = &acc->getResourceModule();
    founderTaskId = getFighterTaskId(fighter);
    weapon->activate(founderTaskId, resources->getResourceIdAccesser()->getEtcResId(), fighter.getOwner()->getTeam(), lr, &pos, attack);
    soLinkModule& link = acc->getLinkModule();
    if (link.link(3, weapon->getTaskId()) == true) {
        if (acc->getStatusModule().getStatusKind() != 0x3c) {
            soLinkModule& linkAgain = acc->getLinkModule();
            linkAgain.setModelConstraintPosOrt(3, acc->getModelModule().getCorrectNodeId(0x12c), 0x50, 0x11, false);
        }
    }
    return true;
}

void ftSamus::setupChangeSucceedWork(void* work) {
    Fighter::setupChangeSucceedWork(work);
    soModuleAccesser* acc = m_moduleAccesser;
    ftSamusExtendParamFinal* param = static_cast<ftSamusExtendParamFinal*>(g_ftCommonDataAccesser.getData(Fighter_Samus)->extendParam[4]);
    wnDummyFighterKineticSettingData hurt;
    hurt.point[0][0] = 0.0f;
    hurt.point[1][0] = soValueAccesser::getConstantFloat(acc, 0xbd6, 0);
    hurt.point[2][0] = 0.0f;
    hurt.point[3][0] = soValueAccesser::getConstantFloat(acc, 0xbd8, 0);
    hurt.point[0][1] = -param->gravity;
    hurt.point[1][1] = soValueAccesser::getConstantFloat(acc, 0xbd1, 0);
    hurt.point[2][1] = param->fallSpeedMax;
    hurt.point[3][1] = param->fallSpeedMax;
    wnDummyFighterKineticSettingData hit;
    hit.point[0][0] = 0.0f;
    hit.point[1][0] = soValueAccesser::getConstantFloat(acc, 0xbbc, 0);
    hit.point[2][0] = 0.0f;
    hit.point[3][0] = -1.0f;
    hit.point[0][1] = 0.0f;
    hit.point[1][1] = 0.0f;
    hit.point[2][1] = 0.0f;
    hit.point[3][1] = 0.0f;
    u32 option = 0;
    if (acc->getStatusModule().getStatusKind() != 0x10b) {
        option = 0x84;
    }
    Vec2f* speed = &acc->getKineticModule().getSumSpeed(soKineticEnergy::AttributeFlag(0xFFFF));
    soPostureModule* const posture = &acc->getPostureModule();
    static_cast<ftChangeSucceedWork*>(work)->activateDummyFighter(&posture->getPos(), posture->getScale(), posture->getLr(), speed, &hit.point[0][0], &hurt.point[0][0], option);
}

// BYTES: defined after its first user (see ft/ft_change_succeed_work.h).
inline wnDummyFighterKineticSettingData::wnDummyFighterKineticSettingData() { }

ftSamus::~ftSamus() { }

// ---- the article mediator -------------------------------------------------------------------------------------------
// Slots: 0 CShot, 1 Bomb, 2 Missile, 3 Laser, 4 Laser2, 5 Gun, 6 SuperMissile, 7 GBeam (the rest are unused).

#define FT_SAMUS_POOL_CSHOT_BASE static_cast<wnArticlePool<wnSamusCShot, 4, ftSamusBombPool>&>(m_pools)
#define FT_SAMUS_POOL_CSHOT FT_SAMUS_POOL_CSHOT_BASE.getSub()
#define FT_SAMUS_POOL_BOMB_BASE static_cast<wnArticlePool<wnSamusBomb, 3, ftSamusMissilePool>&>(m_pools)
#define FT_SAMUS_POOL_BOMB FT_SAMUS_POOL_BOMB_BASE.getSub()
#define FT_SAMUS_POOL_MISSILE_BASE static_cast<wnArticlePool<wnSamusMissile, 3, ftSamusLaserPool>&>(m_pools)
#define FT_SAMUS_POOL_MISSILE FT_SAMUS_POOL_MISSILE_BASE.getSub()
#define FT_SAMUS_POOL_LASER_BASE static_cast<wnArticlePool<wnSamusLaser, 1, ftSamusLaser2Pool>&>(m_pools)
#define FT_SAMUS_POOL_LASER FT_SAMUS_POOL_LASER_BASE.getSub()
#define FT_SAMUS_POOL_LASER2_BASE static_cast<wnArticlePool<wnSamusLaser2, 4, ftSamusGunPool>&>(m_pools)
#define FT_SAMUS_POOL_LASER2 FT_SAMUS_POOL_LASER2_BASE.getSub()
#define FT_SAMUS_POOL_GUN_BASE static_cast<wnArticlePool<wnSamusGun, 1, ftSamusSuperMissilePool>&>(m_pools)
#define FT_SAMUS_POOL_GUN FT_SAMUS_POOL_GUN_BASE.getSub()
#define FT_SAMUS_POOL_SUPERMISSILE_BASE static_cast<wnArticlePool<wnSamusSuperMissile, 2, ftSamusGBeamPool>&>(m_pools)
#define FT_SAMUS_POOL_SUPERMISSILE FT_SAMUS_POOL_SUPERMISSILE_BASE.getSub()
#define FT_SAMUS_POOL_GBEAM_BASE static_cast<wnArticlePool<wnSamusGBeam, 1, soInstancePoolRoot>&>(m_pools)
#define FT_SAMUS_POOL_GBEAM FT_SAMUS_POOL_GBEAM_BASE.getSub()

#define FT_SAMUS_UNUSED_SLOTS(value) \
    WN_ARTICLE_UNUSED_SLOT(8, value) WN_ARTICLE_UNUSED_SLOT(9, value) WN_ARTICLE_UNUSED_SLOT(10, value) \
    WN_ARTICLE_UNUSED_SLOT(11, value) WN_ARTICLE_UNUSED_SLOT(12, value) WN_ARTICLE_UNUSED_SLOT(13, value) \
    WN_ARTICLE_UNUSED_SLOT(14, value) WN_ARTICLE_UNUSED_SLOT(15, value) WN_ARTICLE_UNUSED_SLOT(16, value)

void ftSamusArticleMediator::deactivate() {
    for (s32 i = 0; i < 4; ++i) {
        wnSamusCShot* weapon = FT_SAMUS_POOL_CSHOT.getInstanceAt(i);
        if (!static_cast<soArticle&>(*weapon).setDeactivateDescendant()) {
            return;
        }
    }
    for (s32 i = 0; i < 3; ++i) {
        wnSamusBomb* weapon = FT_SAMUS_POOL_BOMB.getInstanceAt(i);
        if (!static_cast<soArticle&>(*weapon).setDeactivateDescendant()) {
            return;
        }
    }
    for (s32 i = 0; i < 3; ++i) {
        wnSamusMissile* weapon = FT_SAMUS_POOL_MISSILE.getInstanceAt(i);
        if (!static_cast<soArticle&>(*weapon).setDeactivateDescendant()) {
            return;
        }
    }
    for (s32 i = 0; i < 1; ++i) {
        wnSamusLaser* weapon = FT_SAMUS_POOL_LASER.getInstanceAt(i);
        if (!static_cast<soArticle&>(*weapon).setDeactivateDescendant()) {
            return;
        }
    }
    for (s32 i = 0; i < 4; ++i) {
        wnSamusLaser2* weapon = FT_SAMUS_POOL_LASER2.getInstanceAt(i);
        if (!static_cast<soArticle&>(*weapon).setDeactivateDescendant()) {
            return;
        }
    }
    for (s32 i = 0; i < 1; ++i) {
        wnSamusGun* weapon = FT_SAMUS_POOL_GUN.getInstanceAt(i);
        if (!static_cast<soArticle&>(*weapon).setDeactivateDescendant()) {
            return;
        }
    }
    for (s32 i = 0; i < 2; ++i) {
        wnSamusSuperMissile* weapon = FT_SAMUS_POOL_SUPERMISSILE.getInstanceAt(i);
        if (!static_cast<soArticle&>(*weapon).setDeactivateDescendant()) {
            return;
        }
    }
    for (s32 i = 0; i < 1; ++i) {
        wnSamusGBeam* weapon = FT_SAMUS_POOL_GBEAM.getInstanceAt(i);
        if (!static_cast<soArticle&>(*weapon).setDeactivateDescendant()) {
            return;
        }
    }
}

s32 ftSamusArticleMediator::getGenerateMaxNum(s32 articleId) {
    switch (articleId) {
    case 0: return wnArticleSlotGenerateMaxNum(4, WN_ARTICLE_SLOT_TAGS);
    case 1: return wnArticleSlotGenerateMaxNum(3, WN_ARTICLE_SLOT_TAGS);
    case 2: return wnArticleSlotGenerateMaxNum(3, WN_ARTICLE_SLOT_TAGS);
    case 3: return wnArticleSlotGenerateMaxNum(1, WN_ARTICLE_SLOT_TAGS);
    case 4: return wnArticleSlotGenerateMaxNum(4, WN_ARTICLE_SLOT_TAGS);
    case 5: return wnArticleSlotGenerateMaxNum(1, WN_ARTICLE_SLOT_TAGS);
    case 6: return wnArticleSlotGenerateMaxNum(2, WN_ARTICLE_SLOT_TAGS);
    case 7: return wnArticleSlotGenerateMaxNum(1, WN_ARTICLE_SLOT_TAGS);
    FT_SAMUS_UNUSED_SLOTS(0)
    default: return 0;
    }
}

// The counter is a plain aggregate here; the original counts with a functor whose methods MWCC inlines.
struct ftSamusActiveCounter {
    bool (*m_isActive)(soArticle*);
    s32 m_activeNum;
    s32 m_inactiveNum;
};

s32 ftSamusArticleMediator::getActiveNum(soModuleAccesser* acc, s32 articleId) {
    switch (articleId) {
    case 0: return wnArticleSlotActiveNum<wnArticlePool<wnSamusCShot, 4, ftSamusBombPool>, ftSamusActiveCounter>(this, WN_ARTICLE_SLOT_TAGS);
    case 1: return wnArticleSlotActiveNum<wnArticlePool<wnSamusBomb, 3, ftSamusMissilePool>, ftSamusActiveCounter>(this, WN_ARTICLE_SLOT_TAGS);
    case 2: return wnArticleSlotActiveNum<wnArticlePool<wnSamusMissile, 3, ftSamusLaserPool>, ftSamusActiveCounter>(this, WN_ARTICLE_SLOT_TAGS);
    case 3: return wnArticleSlotActiveNum<wnArticlePool<wnSamusLaser, 1, ftSamusLaser2Pool>, ftSamusActiveCounter>(this, WN_ARTICLE_SLOT_TAGS);
    case 4: return wnArticleSlotActiveNum<wnArticlePool<wnSamusLaser2, 4, ftSamusGunPool>, ftSamusActiveCounter>(this, WN_ARTICLE_SLOT_TAGS);
    case 5: return wnArticleSlotActiveNum<wnArticlePool<wnSamusGun, 1, ftSamusSuperMissilePool>, ftSamusActiveCounter>(this, WN_ARTICLE_SLOT_TAGS);
    case 6: return wnArticleSlotActiveNum<wnArticlePool<wnSamusSuperMissile, 2, ftSamusGBeamPool>, ftSamusActiveCounter>(this, WN_ARTICLE_SLOT_TAGS);
    case 7: return wnArticleSlotActiveNum<wnArticlePool<wnSamusGBeam, 1, soInstancePoolRoot>, ftSamusActiveCounter>(this, WN_ARTICLE_SLOT_TAGS);
    FT_SAMUS_UNUSED_SLOTS(0)
    default: return 0;
    }
}

bool ftSamusArticleMediator::isGeneratable(soModuleAccesser* acc, s32 articleId) {
    switch (articleId) {
    case 0: return wnArticleSlotIsGeneratable<wnArticlePool<wnSamusCShot, 4, ftSamusBombPool> >(this, WN_ARTICLE_SLOT_TAGS);
    case 1: return wnArticleSlotIsGeneratable<wnArticlePool<wnSamusBomb, 3, ftSamusMissilePool> >(this, WN_ARTICLE_SLOT_TAGS);
    case 2: return wnArticleSlotIsGeneratable<wnArticlePool<wnSamusMissile, 3, ftSamusLaserPool> >(this, WN_ARTICLE_SLOT_TAGS);
    case 3: return wnArticleSlotIsGeneratable<wnArticlePool<wnSamusLaser, 1, ftSamusLaser2Pool> >(this, WN_ARTICLE_SLOT_TAGS);
    case 4: return wnArticleSlotIsGeneratable<wnArticlePool<wnSamusLaser2, 4, ftSamusGunPool> >(this, WN_ARTICLE_SLOT_TAGS);
    case 5: return wnArticleSlotIsGeneratable<wnArticlePool<wnSamusGun, 1, ftSamusSuperMissilePool> >(this, WN_ARTICLE_SLOT_TAGS);
    case 6: return wnArticleSlotIsGeneratable<wnArticlePool<wnSamusSuperMissile, 2, ftSamusGBeamPool> >(this, WN_ARTICLE_SLOT_TAGS);
    case 7: return wnArticleSlotIsGeneratable<wnArticlePool<wnSamusGBeam, 1, soInstancePoolRoot> >(this, WN_ARTICLE_SLOT_TAGS);
    FT_SAMUS_UNUSED_SLOTS(false)
    default: return false;
    }
}

template <class W> struct ftSamusArticleActivator;
template <> struct ftSamusArticleActivator<wnSamusCShot> {
    static bool activate(wnSamusCShot* weapon, soModuleAccesser* acc) { return ftSamus::activeArticle(weapon, acc); }
};
template <> struct ftSamusArticleActivator<wnSamusBomb> {
    static bool activate(wnSamusBomb* weapon, soModuleAccesser* acc) { return ftSamus::activeArticle1(weapon, acc); }
};
template <> struct ftSamusArticleActivator<wnSamusMissile> {
    static bool activate(wnSamusMissile* weapon, soModuleAccesser* acc) { return ftSamus::activeArticle2(weapon, acc); }
};
template <> struct ftSamusArticleActivator<wnSamusLaser> {
    static bool activate(wnSamusLaser* weapon, soModuleAccesser* acc) { return ftSamus::activeArticle4(weapon, acc); }
};
template <> struct ftSamusArticleActivator<wnSamusLaser2> {
    static bool activate(wnSamusLaser2* weapon, soModuleAccesser* acc) { return ftSamus::activeArticle5(weapon, acc); }
};
template <> struct ftSamusArticleActivator<wnSamusGun> {
    static bool activate(wnSamusGun* weapon, soModuleAccesser* acc) { return ftSamus::activeArticle6(weapon, acc); }
};
template <> struct ftSamusArticleActivator<wnSamusSuperMissile> {
    static bool activate(wnSamusSuperMissile* weapon, soModuleAccesser* acc) { return ftSamus::activeArticle3(weapon, acc); }
};
template <> struct ftSamusArticleActivator<wnSamusGBeam> {
    static bool activate(wnSamusGBeam* weapon, soModuleAccesser* acc) { return ftSamus::activeArticle7(weapon, acc); }
};

soArticle* ftSamusArticleMediator::generate(s32 articleId, soModuleAccesser* acc) {
    switch (articleId) {
    case 0: return wnArticleSlotGenerate<ftSamusArticleActivator<wnSamusCShot> >(static_cast<wnArticlePool<wnSamusCShot, 4, ftSamusBombPool>&>(m_pools), acc, WN_ARTICLE_SLOT_TAGS);
    case 1: return wnArticleSlotGenerate<ftSamusArticleActivator<wnSamusBomb> >(static_cast<wnArticlePool<wnSamusBomb, 3, ftSamusMissilePool>&>(m_pools), acc, WN_ARTICLE_SLOT_TAGS);
    case 2: return wnArticleSlotGenerate<ftSamusArticleActivator<wnSamusMissile> >(static_cast<wnArticlePool<wnSamusMissile, 3, ftSamusLaserPool>&>(m_pools), acc, WN_ARTICLE_SLOT_TAGS);
    case 3: return wnArticleSlotGenerate<ftSamusArticleActivator<wnSamusLaser> >(static_cast<wnArticlePool<wnSamusLaser, 1, ftSamusLaser2Pool>&>(m_pools), acc, WN_ARTICLE_SLOT_TAGS);
    case 4: return wnArticleSlotGenerate<ftSamusArticleActivator<wnSamusLaser2> >(static_cast<wnArticlePool<wnSamusLaser2, 4, ftSamusGunPool>&>(m_pools), acc, WN_ARTICLE_SLOT_TAGS);
    case 5: return wnArticleSlotGenerate<ftSamusArticleActivator<wnSamusGun> >(static_cast<wnArticlePool<wnSamusGun, 1, ftSamusSuperMissilePool>&>(m_pools), acc, WN_ARTICLE_SLOT_TAGS);
    case 6: return wnArticleSlotGenerate<ftSamusArticleActivator<wnSamusSuperMissile> >(static_cast<wnArticlePool<wnSamusSuperMissile, 2, ftSamusGBeamPool>&>(m_pools), acc, WN_ARTICLE_SLOT_TAGS);
    case 7: return wnArticleSlotGenerate<ftSamusArticleActivator<wnSamusGBeam> >(static_cast<wnArticlePool<wnSamusGBeam, 1, soInstancePoolRoot>&>(m_pools), acc, WN_ARTICLE_SLOT_TAGS);
    FT_SAMUS_UNUSED_SLOTS(wnGetNullArticle())
    default: return wnGetNullArticle();
    }
}

template <int S, int L>
inline bool ftSamusShootSlot0(soModuleAccesser* acc, soArticle* article, const soIntToType<S>&, const soIntToType<L>&) {
    return ftSamus::shootArticle(&dynamic_cast<wnSamusCShot&>(*article), acc);
}

template <int S, int L>
inline bool ftSamusShootSlot1(soModuleAccesser* acc, soArticle* article, const soIntToType<S>&, const soIntToType<L>&) {
    return ftSamus::shootArticle1(&dynamic_cast<wnSamusBomb&>(*article), acc);
}

template <int S, int L>
inline bool ftSamusShootSlot2(soModuleAccesser* acc, soArticle* article, const soIntToType<S>&, const soIntToType<L>&) {
    return ftSamus::shootArticle2(&dynamic_cast<wnSamusMissile&>(*article), acc);
}

template <int S, int L>
inline bool ftSamusShootSlot3(soModuleAccesser* acc, soArticle* article, const soIntToType<S>&, const soIntToType<L>&) {
    return ftSamus::shootArticle4(&dynamic_cast<wnSamusLaser&>(*article), acc);
}

template <int S, int L>
inline bool ftSamusShootSlot4(soModuleAccesser* acc, soArticle* article, const soIntToType<S>&, const soIntToType<L>&) {
    return ftSamus::shootArticle5(&dynamic_cast<wnSamusLaser2&>(*article), acc);
}

template <int S, int L>
inline bool ftSamusShootSlot6(soModuleAccesser* acc, soArticle* article, const soIntToType<S>&, const soIntToType<L>&) {
    return ftSamus::shootArticle3(&dynamic_cast<wnSamusSuperMissile&>(*article), acc);
}

bool ftSamusArticleMediator::shoot(soModuleAccesser* acc, soArticle* article) {
    switch (article->getArticleId()) {
    case 0: return ftSamusShootSlot0(acc, article, WN_ARTICLE_SLOT_TAGS);
    case 1: return ftSamusShootSlot1(acc, article, WN_ARTICLE_SLOT_TAGS);
    case 2: return ftSamusShootSlot2(acc, article, WN_ARTICLE_SLOT_TAGS);
    case 3: return ftSamusShootSlot3(acc, article, WN_ARTICLE_SLOT_TAGS);
    case 4: return ftSamusShootSlot4(acc, article, WN_ARTICLE_SLOT_TAGS);
    case 5: return wnArticleSlotShoot<wnSamusGun>(article, WN_ARTICLE_SLOT_TAGS);
    case 6: return ftSamusShootSlot6(acc, article, WN_ARTICLE_SLOT_TAGS);
    case 7: return wnArticleSlotShoot<wnSamusGBeam>(article, WN_ARTICLE_SLOT_TAGS);
    FT_SAMUS_UNUSED_SLOTS(true)
    default: return false;
    }
}

// The module's own copies of these empty destructors (the first unit of the module emits them).
#pragma dont_inline on
soTeam::~soTeam() { }
soResourceIdAccesser::~soResourceIdAccesser() { }
#pragma dont_inline off
soArticleMediator::~soArticleMediator() { }
#pragma dont_inline on
ftVirtualNodeMatrixPool::~ftVirtualNodeMatrixPool() { }
#pragma dont_inline off
