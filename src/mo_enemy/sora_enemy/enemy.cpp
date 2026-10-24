// The Enemy unit builds the StageObject virtuals that the SDK defines inline (getInput, isActive, ...) as weak copies,
// like the yakumono stage units: the vtables reference them.
#define YK_STAGE_FULL
// YK_STAGE_FULL gives StageObject the long notifyEventAnimCmd slot; Weapon must override that same slot (see em_prim.cpp).
#define WN_WEAPON_ANIMCMD_LONG
// The Enemy constructor builds every module of the enemy itself, like the fighter module builders: the module classes of
// sora_melee have their full layout and their constructors are called (not expanded) under these macros.
#define FT_MODULE_BUILDER
#define FT_REL_LINK_EXTERN
#define EM_ENEMY_BUILDER
#define SO_ARRAY_LIST_CTOR_FLAG
#define WN_TRANSITION_CTOR_INLINE
#include <em/enemy.h>
#include <em/em_info.h>
#include <em/em_manager.h>
#include <gf/gf_task.h>
#include <so/so_value_accesser.h>
#include <st/st_trigger.h>

// Null objects of sora_melee the Enemy constructor hands to the modules it does not build.
// HYPOTHESIS: the roles are those of the soModuleAccesser / module constructor parameter they are passed as.
extern char g_soCollisionAbsorberModuleNull[];
extern char g_soDebugModuleNull[];
extern char g_soItemManageModuleNull[];
extern char g_soTeamModuleNullArg[];
extern char g_u32ArrayNull[];
extern char g_soAnimCmdAddressPackArrayNull[];
extern char lbl_27_bss_47C[]; // controller module
extern char lbl_27_bss_584[]; // combo module
extern char lbl_27_bss_5EC[]; // physics module
extern char lbl_27_bss_66C[]; // abnormal module
extern char lbl_27_bss_684[]; // slow module
extern char lbl_27_bss_6D0[]; // param customize module
extern char lbl_27_bss_8B4[]; // null anim cmd table (last address pack)
extern char lbl_27_bss_8D4[]; // status module: null uniq process array
extern char lbl_27_bss_91C[]; // model module: null virtual node array
extern char lbl_27_bss_99C[]; // effect module: null effect time array
extern char lbl_27_bss_9AC[]; // effect module: null screen handle array
extern char lbl_27_bss_9BC[]; // motion module: null transition term pack array
extern char lbl_27_bss_A04[]; // camera module: null camera subject array
extern char lbl_27_bss_D7C[]; // motion module: null transition module
extern char lbl_27_bss_DBC[]; // motion module: null anim obj cache

extern "C" {
// HYPOTHESIS: notifies the trigger manager about the enemy created with this id (kind 3: destroyed, 4: removed).
void fn_27_2330C4(u32 id, int kind);
}

class emGeneralTermDisideModuleImpl {
public:
    static void* getInstance();
};
class emSwitchDecideModuleImpl {
public:
    static void* getInstance();
};

// The interpreter stacks have their own capacity; soAnimCmdInterpreter's constructor (so/anim/so_anim_cmd_module_impl.h)
// is declared with the fighters' 8-entry stack.
// TODO(layout): the constructor should take soArray<acCmdInterpreterStackData>*.
#define ANIM_CMD_STACK(stack) reinterpret_cast<soArrayVector<acCmdInterpreterStackData, 8>*>(&stack)
#define ANIM_CMD_TABLE(id) static_cast<const acAnimCmdConv**>(soValueAccesser::getConstantIndefinite(&moduleAccesser, id, 0))

static const u8 s_resetOff = 0;
static const u8 s_resetOn = 1;

// The camera subjects of the derived enemy, unless the enemy kind has no camera parameters or turns them off.
static inline soArray<soCameraSubject>* selectCameraSubjects(soModuleAccesser* acc, soArray<soCameraSubject>* subjects) {
    bool noCamera = false;
    if (soValueAccesser::getConstantIndefinite(acc, 0xa811, 0) == NULL || soValueAccesser::getValueInt(acc, 0x59fb, 0) == 0) {
        noCamera = true;
    }
    if (noCamera) {
        subjects = reinterpret_cast<soArray<soCameraSubject>*>(lbl_27_bss_A04);
    }
    return subjects;
}

// Registers an interpreter with the anim cmd module: the control unit (interpreter + address pack) and its attribute.
// (A macro: the original expands this in the constructor, an inline function was kept out of line.)
#define REGIST_INTERPRETER(interpreter, pack, attribute)                                                               \
    do {                                                                                                               \
        soAttributeFlag flag = attribute;                                                                              \
        soAnimCmdControlUnit unit;                                                                                     \
        unit.m_animCmdInterpreter = &interpreter;                                                                      \
        unit.m_animCmdAddressPackArraySeparate = pack;                                                                 \
        static_cast<soAnimCmdModule&>(m_animCmdModule).registInterpreter(unit, flag);                                 \
    } while (0)

Enemy* Enemy::create(u32 p1, emCreate* create) {
    if (create == NULL) {
        return NULL;
    }
    if (emInfo::getInstance()->isInvalidKind(create->m_enemyKind) == true) {
        return NULL;
    }
    Enemy* enemy = emInfo::getInstance()->getCreateFuncPtr(create->m_enemyKind)(p1, create);
    if (enemy == NULL) {
        return NULL;
    }
    return enemy;
}

void Enemy::createPost(Enemy* enemy, emCreate* create) {
    if (create != NULL) {
        enemy->init();
        float unk = soValueAccesser::getValueFloat(enemy->m_moduleAccesser, 0xbf9, 0);
        enemy->activate(&create->m_startPos, create->m_startLr, unk, create->m_startStatusKind);
    }
}

void Enemy::remove(Enemy* enemy) {
    if (enemy != NULL) {
        if (g_stTriggerMng != NULL) {
            fn_27_2330C4(enemy->m_triggerNotifyId, 4);
        }
        enemy->deactivate();
        enemy->exit();
    }
}

// TODO(nonmatching): 77.32%, (b) incomplete: the tail after the anim cmd registrations is missing (see the end of the
// body). Otherwise the members are built in the original order with the original arguments; the remaining differences
// are calls whose sora_melee / sora_enemy target has only an address (fn_27_* / fn_41_*), the observer bases (the original
// calls soEventObserver<T>::soEventObserver and ::initialize out of line), soTeamImpl (inlined in the original) and the
// interpreters' reset mode (a stack temporary in the original). Draft notes: the module storage, the module wiring and the registrations are in the original order; the
// calls into sora_melee that only have an address there (fn_27_*: observer bases, several array constructors, the target
// search, glow and soArrayList constructors) cannot pair by name, and the tail after the anim cmd registrations
// (jostle, ground shape, hit, absorber/reflector shapes, catch and capture setup) is not reconstructed yet.
Enemy::Enemy(u32 p1, emCreate* create, soArray<soInterpolation<Vec3f> >* interpolations, soArray<soAreaInstance>* areaInstances,
             void* areaWork, emTargetSearchUnit* targetSearchUnits, emAIEntry* aiEntries, soArticleMediator* articleMediator,
             void* wind, soArray<soCameraSubject>* cameraSubjects, soReflectModule* reflectModule) :
    StageObject("", 2, 7, 4, true, NULL),
    soStatusEventObserver(-1, -1),
    soCollisionAttackEventObserver(-1, -1),
    soCollisionSearchEventObserver(-1, -1),
    soCaptureEventObserver(0xd),
    soCollisionAbsorberEventObserver(0x10),
    soCollisionReflectorEventObserver(-1, -1),
    m_enemyKind(create->m_enemyKind),
    m_triggerNotifyId(p1),
    m_connectedTriggerId(create->m_connectedTriggerId),
    m_difficulty(create->m_difficulty),
    m_level(create->m_level),
    m_motionPath(create->m_motionPath),
    unkCC(-1),
    unkD0(35791.0f),
    unkD4(35791.0f),
    unkD8(false),
    moduleAccesser(this, &m_resourceModule, &m_modelModule, &m_motionModule, &m_postureModule, &m_groundModule,
                   &m_situationModule, &m_teamModule, &m_collisionAttackModule, &m_collisionHitModule,
                   reinterpret_cast<soCollisionShieldModule*>(g_soCollisionAbsorberModuleNull), &m_collisionReflectorModule,
                   &m_collisionAbsorberModule, &m_collisionCatchModule, &m_collisionSearchModule,
                   reinterpret_cast<soDamageModule*>(&m_damageModule), &m_catchModule, &m_captureModule,
                   reinterpret_cast<soStopModule*>(&m_stopModule), &m_turnModule, &m_shakeModule, &m_soundModule,
                   &m_linkModule, &m_visibilityModule, reinterpret_cast<soControllerModule*>(lbl_27_bss_47C),
                   &m_cameraModule, &m_workManageModule, g_soDebugModuleNull, &m_animCmdModule, &m_statusModule,
                   emGeneralTermDisideModuleImpl::getInstance(), emSwitchDecideModuleImpl::getInstance(),
                   reinterpret_cast<soKineticModule*>(&m_kineticModule), &m_eventManageModule,
                   &m_generateArticleManageModule, &m_effectModule, lbl_27_bss_584, &m_areaModule, &m_territoryModule,
                   &m_targetSearchModule, lbl_27_bss_5EC, &m_slopeModule, &m_shadowModule,
                   reinterpret_cast<soItemManageModule*>(g_soItemManageModuleNull), &m_colorBlendModule, &m_jostleModule,
                   lbl_27_bss_66C, reinterpret_cast<soSlowModule*>(lbl_27_bss_684), reflectModule, &m_heapModule,
                   reinterpret_cast<soParamCustomizeModule*>(lbl_27_bss_6D0), &m_glowModule),
    m_eventUnits(),
    m_eventManageModule(&m_eventUnits),
    m_statusEventUnit(m_eventManageModule.getManageId(), 4),
    m_animCmdEventUnit(m_eventManageModule.getManageId(), 5),
    m_damageEventUnit(m_eventManageModule.getManageId(), 6),
    m_linkEventUnit(m_eventManageModule.getManageId(), 7),
    m_situationEventUnit(m_eventManageModule.getManageId(), 8),
    m_collisionHitEventUnit(m_eventManageModule.getManageId(), 0),
    m_collisionAttackEventUnit(m_eventManageModule.getManageId(), 1),
    m_collisionSearchEventUnit(m_eventManageModule.getManageId(), 0x11),
    m_captureEventUnit(m_eventManageModule.getManageId(), 0xd),
    m_collisionAbsorberEventUnit(m_eventManageModule.getManageId(), 0x10),
    m_collisionReflectorEventUnit(m_eventManageModule.getManageId(), 0xf),
    m_collisionShieldEventUnit(m_eventManageModule.getManageId(), 0xe),
    m_modelEventUnit(m_eventManageModule.getManageId(), 9),
    m_collisionCatchEventUnit(m_eventManageModule.getManageId(), 2),
    m_heapModule(Heaps::EnemyInstance, Heaps::Fighter1Resource, Heaps::EnemyInstance, 0),
    m_preloadArchive(emManager::getInstance()->getPreloadArchivePtrFromKind(m_enemyKind)),
    m_resourceIdAccesser(m_preloadArchive->brresResourceId, m_preloadArchive->brresResourceId,
                         m_preloadArchive->brresResourceId),
    m_resourceModule(m_preloadArchive->paramResourceId, &m_resourceIdAccesser, m_preloadArchive->enmCommonResourceId, create),
    m_statusWork(m_statusIntWorks, 7, m_statusFloatWorks, 1, m_statusFlagWorks, 9),
    m_generalWork(m_intWorks, 27, m_floatWorks, 16, m_flagWorks, 20),
    m_workManageModule(&moduleAccesser, &g_soParamAccesserNull),
    m_areaModule(&moduleAccesser, emInfo::getInstance()->getAreaNum(m_enemyKind),
                 soValueAccesser::getConstantIndefinite(&moduleAccesser,
                                                        (u32)emInfo::getInstance()->getAreaParamAryPtrGetId(m_enemyKind), 0),
                 soValueAccesser::getConstantIndefinite(&moduleAccesser, 0xa81a, 0), areaInstances, areaWork, wind),
    m_aiModule(&moduleAccesser, aiEntries, soValueAccesser::getConstantIndefinite(&moduleAccesser, 0xa80e, 0),
               emInfo::getInstance()->getAINum(m_enemyKind), 6, 7),
    m_modelNodeSetUps(11, 0),
    m_modelModule(&moduleAccesser, &m_modelNodeSetUps, soValueAccesser::getValueFloat(&moduleAccesser, 0xbe7, 0), NULL,
                  reinterpret_cast<soArray<soModelVirtualNode>*>(lbl_27_bss_91C),
                  reinterpret_cast<soEventObserverRegistrationDesc*>(g_soTeamModuleNullArg)),
    m_statusTable(static_cast<const soStatusData*>(soValueAccesser::getConstantIndefinite(&moduleAccesser, 0xa801, 0)),
                  emInfo::getInstance()->getStatusNum(m_enemyKind)),
    m_motionTable(static_cast<const soMotionData*>(soValueAccesser::getConstantIndefinite(&moduleAccesser, 0xa7fc, 0)),
                  emInfo::getInstance()->getMotionNum(m_enemyKind)),
    m_partialAnims(2, 0),
    m_otherAnims(3, 0),
    m_motionModule(&moduleAccesser, &m_motionTable, reinterpret_cast<soTransitionModule*>(lbl_27_bss_D7C), &m_partialAnims,
                   &m_otherAnims, reinterpret_cast<soArray<soTransitionTermPack>*>(lbl_27_bss_9BC),
                   reinterpret_cast<soArray<u32>*>(g_u32ArrayNull), -1, false,
                   reinterpret_cast<soMotionAnimObjCacheModule*>(lbl_27_bss_DBC)),
    m_slopeModule(&moduleAccesser, 0, 0, true, 20.0f),
    m_groundShapes(1, 0),
    m_groundModule(&moduleAccesser, &m_groundShapes, soValueAccesser::getConstantIndefinite(&moduleAccesser, 0xa813, 0)),
    m_targetSearchModule(&moduleAccesser, targetSearchUnits,
                         soValueAccesser::getConstantIndefinite(
                             &moduleAccesser, (u32)emInfo::getInstance()->getTargetSearchParamAryPtrGetId(m_enemyKind), 0),
                         emInfo::getInstance()->getTargetSearchNum(m_enemyKind), &m_rayCheckCaches),
    m_linkConnections(5, 0),
    m_linkModule(&moduleAccesser, &m_linkConnections),
    m_team(create->m_teamNo),
    m_teamModule(&m_team, &m_team, &m_team, &moduleAccesser, g_soTeamModuleNullArg),
    m_attackParts(5, soCollisionAttackPart(soCollision::Category_Enemy, false), 0),
    m_attackGroups(5, 0),
    m_attackAbsolutes(1, 0),
    m_collisionAttackModule(&moduleAccesser, m_taskId, m_taskCategory, &m_attackParts, &m_attackGroups,
                            &m_attackAbsolutes, reinterpret_cast<soEventObserverRegistrationDesc*>(g_soTeamModuleNullArg),
                            true),
    m_hitParts(10, soCollisionHitPart(soCollision::Category_Enemy, 0x3ff), 0),
    m_hitGroups(1, 0),
    m_hitGroupInfos(1, 0),
    m_collisionHitModule(&moduleAccesser, m_taskId, m_taskCategory, &m_hitParts, &m_hitGroups, &m_hitGroupInfos,
                         soValueAccesser::getConstantIndefinite(&moduleAccesser, 0xa7fe, 0),
                         soValueAccesser::getConstantIndefinite(&moduleAccesser, 0xa81b, 0)),
    m_searchParts(1, soCollisionSearchPart(soCollision::Category_Enemy), 0),
    m_collisionSearchModule(&moduleAccesser, m_taskId, m_taskCategory, &m_searchParts, true),
    m_absorberParts(1, soCollisionShieldPart(soCollision::Category_Enemy, 4), 0),
    m_absorberGroups(1, 0),
    m_absorberShieldGroups(1, 0),
    m_absorberPresenter(&moduleAccesser),
    m_collisionAbsorberModule(&moduleAccesser, m_taskId, m_taskCategory, &m_absorberParts, &m_absorberGroups,
                              &m_absorberShieldGroups, &m_absorberPresenter, 4, true),
    m_reflectorParts(1, soCollisionShieldPart(soCollision::Category_Enemy, 3), 0),
    m_reflectorGroups(1, 0),
    m_reflectorShieldGroups(1, 0),
    m_reflectorPresenter(&moduleAccesser),
    m_collisionReflectorModule(&moduleAccesser, m_taskId, m_taskCategory, &m_reflectorParts, &m_reflectorGroups,
                               &m_reflectorShieldGroups, &m_reflectorPresenter, 3, true),
    m_catchParts(2, soCollisionCatchPart(soCollision::Category_Enemy), 0),
    m_collisionCatchModule(&moduleAccesser, m_taskId, m_taskCategory, &m_catchParts,
                           reinterpret_cast<soEventObserverRegistrationDesc*>(g_soTeamModuleNullArg), false, true),
    m_effectContinuals(1, 0),
    m_effectHandles(1, 0, 0),
    m_effectModule(&moduleAccesser, &m_effectContinuals, soValueAccesser::getConstantIndefinite(&moduleAccesser, 0xa7ff, 0),
                   &m_effectHandles, reinterpret_cast<soArray<soEffectTime>*>(lbl_27_bss_99C), NULL,
                   reinterpret_cast<soEventObserverRegistrationDesc*>(g_soTeamModuleNullArg), NULL, -1, NULL,
                   reinterpret_cast<soArray<efScreenHandle>*>(lbl_27_bss_9AC)),
    m_colorBlendModule(&moduleAccesser, 5, 0),
    m_sound3dGeneratorAccesser(),
    m_soundModule(&moduleAccesser, &m_sound3dGeneratorAccesser, &g_soSoundIdExchangerNull,
                  false, false, reinterpret_cast<soEventObserverRegistrationDesc*>(g_soTeamModuleNullArg)),
    m_territoryModule(&moduleAccesser, soValueAccesser::getConstantIndefinite(&moduleAccesser, 0xa819, 0),
                      &create->m_territoryRange, 1, 2, 3, 4, 5),
    m_situationModule(-1, &moduleAccesser, reinterpret_cast<soEventObserverRegistrationDesc*>(g_soTeamModuleNullArg)),
    m_postureModule(&moduleAccesser, interpolations, reinterpret_cast<soEventObserverRegistrationDesc*>(g_soTeamModuleNullArg)),
    m_shakeTerms(2, 0),
    m_shakeModule(&moduleAccesser, &m_shakeTerms, soValueAccesser::getConstantIndefinite(&moduleAccesser, 0xa7f8, 0)),
    m_damages(1, 0),
    m_damageModule(&moduleAccesser, &m_damages),
    m_turnModule(&moduleAccesser),
    m_visibilityModule(&moduleAccesser, NULL, -1),
    m_shadowModule(&moduleAccesser, soValueAccesser::getValueFloat(&moduleAccesser, 0xbf2, 0),
                   soValueAccesser::getValueInt(&moduleAccesser, 0x59e6, 0)),
    m_glowModule(&moduleAccesser),
    m_stopModule(&moduleAccesser),
    m_cameraModule(&moduleAccesser, selectCameraSubjects(&moduleAccesser, cameraSubjects),
                   NULL,
                   static_cast<soSet<soCameraClipSphere>*>(soValueAccesser::getConstantIndefinite(&moduleAccesser, 0xa812, 0)),
                   reinterpret_cast<soEventObserverRegistrationDesc*>(g_soTeamModuleNullArg)),
    m_catchModule(&moduleAccesser, -1),
    m_captureModule(&moduleAccesser),
    m_rayCheckCaches(8, 0),
    m_jostleModule(&moduleAccesser, soValueAccesser::getValueInt(&moduleAccesser, 0x59ef, 0),
                   soValueAccesser::getValueInt(&moduleAccesser, 0x59f0, 0),
                   soValueAccesser::getConstantIndefinite(&moduleAccesser, 0xa817, 0)),
    m_animCmdStack1(0), m_animCmdStack2(0), m_animCmdStack3(0), m_animCmdStack0(0), m_animCmdStack4(0), m_animCmdStack5(0),
    m_animCmdStack6(0), m_animCmdStack7(0), m_animCmdStack8(0), m_animCmdStack9(0), m_animCmdStack10(0),
    m_animCmdInterpreter1(m_eventManageModule.getManageId(), ANIM_CMD_STACK(m_animCmdStack1), 1, 1, -1.0f, &s_resetOff),
    m_animCmdInterpreter2(m_eventManageModule.getManageId(), ANIM_CMD_STACK(m_animCmdStack2), 2, 1, -1.0f, &s_resetOff),
    m_animCmdInterpreter3(m_eventManageModule.getManageId(), ANIM_CMD_STACK(m_animCmdStack3), 3, 1, -1.0f, &s_resetOff),
    m_animCmdInterpreter0(m_eventManageModule.getManageId(), ANIM_CMD_STACK(m_animCmdStack0), 0, 0, 1.0f, &s_resetOff),
    m_animCmdInterpreter4(m_eventManageModule.getManageId(), ANIM_CMD_STACK(m_animCmdStack4), 4, 0, 1.0f, &s_resetOn),
    m_animCmdInterpreter5(m_eventManageModule.getManageId(), ANIM_CMD_STACK(m_animCmdStack5), 5, 1, -1.0f, &s_resetOff),
    m_animCmdInterpreter6(m_eventManageModule.getManageId(), ANIM_CMD_STACK(m_animCmdStack6), 6, 0, 1.0f, &s_resetOff),
    m_animCmdInterpreter7(m_eventManageModule.getManageId(), ANIM_CMD_STACK(m_animCmdStack7), 7, 0, 1.0f, &s_resetOff),
    m_animCmdInterpreter8(m_eventManageModule.getManageId(), ANIM_CMD_STACK(m_animCmdStack8), 8, 1, -1.0f, &s_resetOff),
    m_animCmdInterpreter9(m_eventManageModule.getManageId(), ANIM_CMD_STACK(m_animCmdStack9), 9, 1, -1.0f, &s_resetOff),
    m_animCmdInterpreter10(m_eventManageModule.getManageId(), ANIM_CMD_STACK(m_animCmdStack10), 10, 1, -1.0f, &s_resetOff),
    m_statusChangeRequests(2, (s32)-1),
    m_statusModule(&moduleAccesser, &m_statusTable, lbl_27_bss_8D4, &m_statusWork, &m_statusTransition,
                   &m_statusChangeRequests, NULL, 0, 0),
    m_statusTransition(),
    m_animCmdTable0(ANIM_CMD_TABLE(0xa802), emInfo::getInstance()->getMotionNum(m_enemyKind)),
    m_animCmdTable1(ANIM_CMD_TABLE(0xa803), emInfo::getInstance()->getMotionNum(m_enemyKind)),
    m_animCmdTable2(ANIM_CMD_TABLE(0xa804), emInfo::getInstance()->getMotionNum(m_enemyKind)),
    m_animCmdTable3(ANIM_CMD_TABLE(0xa805), emInfo::getInstance()->getMotionNum(m_enemyKind)),
    m_animCmdTable4(ANIM_CMD_TABLE(0xa806), emInfo::getInstance()->getMotionNum(m_enemyKind)),
    m_animCmdTable5(ANIM_CMD_TABLE(0xa807), emInfo::getInstance()->getMotionNum(m_enemyKind)),
    m_animCmdTable6(ANIM_CMD_TABLE(0xa808), emInfo::getInstance()->getMotionNum(m_enemyKind)),
    m_animCmdTable7(ANIM_CMD_TABLE(0xa809), emInfo::getInstance()->getMotionNum(m_enemyKind)),
    m_animCmdTable8(ANIM_CMD_TABLE(0xa80a), emInfo::getInstance()->getMotionNum(m_enemyKind)),
    m_animCmdTable9(ANIM_CMD_TABLE(0xa80b), emInfo::getInstance()->getStatusNum(m_enemyKind)),
    m_animCmdTable10(ANIM_CMD_TABLE(0xa80c), emInfo::getInstance()->getStatusNum(m_enemyKind)),
    m_animCmdTable11(ANIM_CMD_TABLE(0xa80d), emInfo::getInstance()->getStatusNum(m_enemyKind)),
    m_animCmdTable12(ANIM_CMD_TABLE(0xa7fb), 26),
    m_animCmdAddressPack0(&m_animCmdTable0, &m_animCmdTable1, &m_animCmdTable2),
    m_animCmdAddressPack1(&m_animCmdTable3, &m_animCmdTable4, &m_animCmdTable5),
    m_animCmdAddressPack2(&m_animCmdTable6, &m_animCmdTable7, &m_animCmdTable8),
    m_animCmdAddressPack3(&m_animCmdTable9, &m_animCmdTable10, &m_animCmdTable11),
    m_animCmdAddressPack4(&m_animCmdTable12, reinterpret_cast<soArrayFixed<const acAnimCmdConv*>*>(lbl_27_bss_8B4),
                          reinterpret_cast<soArrayFixed<const acAnimCmdConv*>*>(lbl_27_bss_8B4)),
    m_animCmdControlUnits(),
    m_animCmdModule(m_eventManageModule.getManageId(), &m_animCmdControlUnits),
    m_kineticModule(&moduleAccesser,
                    emKineticEnergyParamDesc(soValueAccesser::getConstantIndefinite(
                                                 &moduleAccesser,
                                                 (u32)emInfo::getInstance()->getKineticEnergyParamAryPtrGetId(m_enemyKind), 0),
                                             emInfo::getInstance()->getKineticEnergyNum(m_enemyKind))),
    m_articles(0),
    m_articleEventObservers(10, 0),
    m_generateArticleManageModule(&moduleAccesser, &m_articles, articleMediator, &m_articleEventObservers)
{
    m_moduleAccesser = &moduleAccesser;
    soStatusEventObserver::addObserver(m_eventManageModule.getManageId(), 2);
    soAnimCmdEventObserver::addObserver(m_eventManageModule.getManageId(), -1);
    soLinkEventObserver::addObserver(m_eventManageModule.getManageId(), -1);
    soCollisionAttackEventObserver::addObserver(m_eventManageModule.getManageId(), -1);
    soCollisionSearchEventObserver::addObserver(m_eventManageModule.getManageId(), -1);
    soCaptureEventObserver::addObserver(m_eventManageModule.getManageId(), -1);
    soCollisionAbsorberEventObserver::addObserver(m_eventManageModule.getManageId(), -1);
    soCollisionReflectorEventObserver::addObserver(m_eventManageModule.getManageId(), -1);

    // TODO(nonmatching): ground shape data (0xa80f) and hit data (0xa7fd) setup through the module accesser.

    soWorkManageModule* workManage = &m_workManageModule;
    workManage->setWork(1, &m_generalWork);

    // One interpreter per thread kind, with the attribute it is registered with and the address pack it reads.
    REGIST_INTERPRETER(m_animCmdInterpreter1, &m_animCmdAddressPack0, 2);
    REGIST_INTERPRETER(m_animCmdInterpreter2, &m_animCmdAddressPack1, 2);
    REGIST_INTERPRETER(m_animCmdInterpreter3, &m_animCmdAddressPack2, 2);
    REGIST_INTERPRETER(m_animCmdInterpreter0, &m_animCmdAddressPack3, 1);
    REGIST_INTERPRETER(m_animCmdInterpreter4, reinterpret_cast<soAnimCmdAddressPackArraySeparate*>(g_soAnimCmdAddressPackArrayNull), 4);
    REGIST_INTERPRETER(m_animCmdInterpreter5, &m_animCmdAddressPack4, 0x10);
    REGIST_INTERPRETER(m_animCmdInterpreter6, reinterpret_cast<soAnimCmdAddressPackArraySeparate*>(g_soAnimCmdAddressPackArrayNull), 8);
    REGIST_INTERPRETER(m_animCmdInterpreter7, reinterpret_cast<soAnimCmdAddressPackArraySeparate*>(g_soAnimCmdAddressPackArrayNull), 8);
    REGIST_INTERPRETER(m_animCmdInterpreter8, &m_animCmdAddressPack0, 0x20);
    REGIST_INTERPRETER(m_animCmdInterpreter9, &m_animCmdAddressPack1, 0x20);
    REGIST_INTERPRETER(m_animCmdInterpreter10, &m_animCmdAddressPack2, 0x20);

    // TODO(nonmatching): jostle (0x59ee, 0x59f1), absorber / reflector shapes, catch, hit and capture setup.
}

Enemy::~Enemy() {
    if (g_stTriggerMng != NULL) {
        fn_27_2330C4(m_triggerNotifyId, 3);
    }
}

s32 Enemy::getNameMsgId() {
    return moduleAccesser.getWorkManageModule().getInt(0x10000016);
}

nw4r::g3d::ResFileData* Enemy::getFaceTexPtr(u32 p1) {
    return m_resourceModule.getInfoTexFile(m_preloadArchive->faceTexBrresResourceId, p1);
}

// TODO(nonmatching): 90.87%, (a) non-semantic: same calls and arguments; the original loads the archive entry before it
// computes the address of the resource module.
nw4r::g3d::ResFileData* Enemy::getFaceTexPtr() {
    return getFaceTexPtr(moduleAccesser.getWorkManageModule().getInt(0x10000017));
}

int Enemy::getExtendBrresResourceID() {
    if (m_preloadArchive == NULL) {
        return 0xFFFF;
    }
    return m_preloadArchive->extendBrresResourceId;
}

bool Enemy::isObserv(char unk1) {
    bool result = false;
    if (StageObject::isObserv(unk1) == true || unk1 == 'e') {
        result = true;
    }
    return result;
}

bool Enemy::notifyEventCollisionSearchCheck() {
    return false;
}

void Enemy::notifyEventCollisionAbsorber(soCollisionAttackModule* attackModule, float power, soCollisionLog* collisionLog,
                                         int groupIndex, float posX, float, soModuleAccesser* moduleAccesser) { }

bool Enemy::notifyEventCollisionAbsorberCheck() {
    return false;
}

void Enemy::notifyEventCollisionReflector(soCollisionAttackModule* attackModule, float power, soCollisionLog* collisionLog,
                                          int groupIndex, float posX, float, soModuleAccesser* moduleAccesser) { }

void Enemy::notifyEventCollisionReflectorSearch(soCollisionSearchModule* searchModule, soCollisionLog* collisionLog,
                                                u32 groupIndex, soModuleAccesser* moduleAccesser) { }

bool Enemy::notifyEventCollisionReflectorCheck() {
    return false;
}

bool Enemy::isTreadPassive() {
    return true;
}

int Enemy::soGetSubKind() {
    return m_enemyKind;
}

soKind Enemy::soGetKind() {
    return StageObject_Enemy;
}
