// The Enemy unit builds the StageObject virtuals that the SDK defines inline (getInput, isActive, ...) as weak copies,
// like the yakumono stage units: the vtables reference them.
#define YK_STAGE_FULL
// YK_STAGE_FULL gives StageObject the long notifyEventAnimCmd slot; Weapon must override that same slot (see em_prim.cpp).
#define WN_WEAPON_ANIMCMD_LONG
// MATCH-ONLY: the unit calls its weak out-of-line copies of Vec3f(float, float, float), operator- and operator+=.
#define MT_VEC3F_CTOR_NOINLINE
#define MT_VEC3F_OPERATORS_NOINLINE
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
#include <em/em_scaled_value.h>
#include <em/em_shot.h>
#include <ft/ft_reflector_group.h>
#include <gf/gf_gameframe_counter.h>
#include <gf/gf_task.h>
#include <gf/gf_task_scheduler.h>
#include <it/it_manager.h>
#include <mt/mt_prng.h>
#include <so/capture/so_capture_module.h>
#include <so/catch/so_catch_module.h>
#include <so/damage/so_damage_util_actor.h>
#include <so/so_external_value_accesser.h>
#include <so/so_module_hyp.h>
#include <so/so_slow.h>
#include <so/so_value_accesser.h>
#include <st/st_trigger.h>
#include <string.h>
#include <wn/wn_em_simple.h>
#include <wn/wn_em_sw_holder.h>

// HYPOTHESIS: the loupe number the enemy's model is constructed with (Enemy::activate); the first object of the unit's
// .data.
static u8 s_modelLoupeNo = 0xff;

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
// stTriggerMng (sora_melee st_trigger), address names.
// HYPOTHESIS: the number of enemies the trigger creates, whether its index-th one can be created, and the creation of
// that one from the description (returns its create id).
u32 fn_27_2331C4(stTriggerMng* manager, int triggerId);
bool fn_27_233208(stTriggerMng* manager, int triggerId, u32 index);
int fn_27_233264(stTriggerMng* manager, int triggerId, void* desc, int index);
}

// HYPOTHESIS: a global of another sora_enemy unit (bss 0x1C8, not split yet); the shoot / addParts anim cmds only run
// while it is set.
extern void* lbl_41_bss_1C8;

// HYPOTHESIS: the node the enemy's center is taken from (getCenterPos reads its global position).
static const u32 ValueInt_CenterNodeId = 0x59e6;

// Work ids of the enemy (soWorkManageModule).
// HYPOTHESIS: roles from the code that writes them.
static const u32 WorkFlag_InvalidParent = 0x12000000;   // the parent marks itself invalid for its children
static const u32 WorkFlag_Bullet = 0x12000001;          // Enemy::isBullet; the attack check stops and quakes the camera
static const u32 WorkFlag_AttackChecked = 0x12000005;   // set by the attack check, cleared on status change
static const u32 WorkFlag_SearchHitWeapon = 0x1200000e; // the search collision found a weapon
static const u32 WorkInt_StatusDataKind = 0x10000004;   // emStatusData::m_unkC of the current status
// The instance id of the item the enemy holds / communicates with (cleared by emShootItem, removed by anim cmd 28).
static const u32 WorkInt_CommunicationItemId = 0x1000000f;

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

// The ground module reads the shapes of the enemy kind through this pair (shape array, shape count).
struct GroundShapeSet {
    soGroundShapeData** m_shapes;
    u32 m_count;
};

// MATCH-ONLY: the reset mode of each interpreter is a temporary in the frame of the constructor (as ftAddressOf in
// ft/builder/ft_builder_animcmd.h does for the fighters).
static inline const u8* resetMode(u8 value) {
    return &value;
}

// Moves one side of the territory rectangle by size * strength (a side whose size is 0 stays).
static inline void widenSide(float& side, float size, float strength) {
    if (0.0f != size) {
        side += size * strength;
    }
}

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

// The task category of a collision log is a plain byte at 0x22; BrawlHeaders declares it as a bit-field of the word at
// 0x20, which loads differently.
// TODO(layout): a soCollisionLog shadow with a u8 m_taskCategory (wnWarioBike and wnNessYoyoHead cast the byte too).
static inline gfTask::Category getTaskCategory(const soCollisionLog* log) {
    return static_cast<gfTask::Category>(reinterpret_cast<const u8*>(log)[0x22]);
}

// The catch module slots after catchCut (0x2C), which so/catch/so_catch_module.h leaves to its users (the fighter
// processes add their own views of 0x30-0x3C the same way).
// HYPOTHESIS: name; Enemy::cutCatch calls 0x34 before catchCut when the enemy is model-constrained.
class emCatchModuleView : public soCatchModule {
public:
    virtual void unk30();
    virtual void cutConstraint(); // 0x34
};

// The catch and capture modules of the module accesser (void* in the enumeration).
static inline emCatchModuleView& getCatchModule(const soModuleAccesser& acc) {
    return *static_cast<emCatchModuleView*>(static_cast<soCatchModule*>(acc.m_enumerationStart->m_catchModule));
}
static inline soCaptureModule& getCaptureModule(const soModuleAccesser& acc) {
    return *static_cast<soCaptureModule*>(acc.m_enumerationStart->m_captureModule);
}

// HYPOTHESIS: the status data of an enemy status; the top four bits of its word 0xC go to a work int on every status
// change.
struct emStatusData : public soStatusData {
    u8 unk0[0xC];
    s32 m_unkC : 4;
};

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
            fn_27_2330C4(enemy->m_createId, 4);
        }
        enemy->deactivate();
        enemy->exit();
    }
}

// TODO(nonmatching): 98.43%, (a) non-semantic: every call, argument and constant agrees (masked diff). The rest is
// the store order of the default shield shapes (indexed stores in the original), the evaluation order of the hit
// module's task category (the original converts it through a signed temporary before the module accesser calls) and of
// the resource and slope module arguments, the frame slots of the attribute temporaries and one register in
// selectCameraSubjects.
Enemy::Enemy(u32 p1, emCreate* create, soArray<soInterpolation<Vec3f> >* interpolations, soArray<soAreaInstance>* areaInstances,
             void* areaWork, emTargetSearchUnit* targetSearchUnits, emAIEntry* aiEntries, soArticleMediator* articleMediator,
             void* wind, soArray<soCameraSubject>* cameraSubjects, soReflectModule* reflectModule) :
    StageObject("", 2, 7, 4, true, NULL),
    soStatusEventObserver(-1, -1),
    soCollisionAttackEventObserver(-1, -1),
    soCollisionSearchEventObserver(-1, -1),
    soCaptureEventObserver(-1, -1),
    soCollisionAbsorberEventObserver(-1, -1),
    soCollisionReflectorEventObserver(-1, -1),
    m_enemyKind(create->m_enemyKind),
    m_createId(p1),
    m_connectedTriggerId(create->m_connectedTriggerId),
    m_difficulty(create->m_difficulty),
    m_level(create->m_level),
    m_motionPath(create->m_motionPath),
    unkCC(-1),
    m_goalPosX(35791.0f),
    m_goalPosY(35791.0f),
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
                   &m_kineticModule, &m_eventManageModule,
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
    m_heapModule(Heaps::EnemyInstance, Heaps::StageResource, Heaps::EnemyInstance, 0),
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
    m_collisionAttackModule(&moduleAccesser, m_taskId, (gfTask::Category)(u8)m_taskCategory, &m_attackParts, &m_attackGroups,
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
    m_animCmdInterpreter1(m_eventManageModule.getManageId(), ANIM_CMD_STACK(m_animCmdStack1), 1, 1, -1.0f, resetMode(0)),
    m_animCmdInterpreter2(m_eventManageModule.getManageId(), ANIM_CMD_STACK(m_animCmdStack2), 2, 1, -1.0f, resetMode(0)),
    m_animCmdInterpreter3(m_eventManageModule.getManageId(), ANIM_CMD_STACK(m_animCmdStack3), 3, 1, -1.0f, resetMode(0)),
    m_animCmdInterpreter0(m_eventManageModule.getManageId(), ANIM_CMD_STACK(m_animCmdStack0), 0, 0, 1.0f, resetMode(0)),
    m_animCmdInterpreter4(m_eventManageModule.getManageId(), ANIM_CMD_STACK(m_animCmdStack4), 4, 0, 1.0f, resetMode(1)),
    m_animCmdInterpreter5(m_eventManageModule.getManageId(), ANIM_CMD_STACK(m_animCmdStack5), 5, 1, -1.0f, resetMode(0)),
    m_animCmdInterpreter6(m_eventManageModule.getManageId(), ANIM_CMD_STACK(m_animCmdStack6), 6, 0, 1.0f, resetMode(0)),
    m_animCmdInterpreter7(m_eventManageModule.getManageId(), ANIM_CMD_STACK(m_animCmdStack7), 7, 0, 1.0f, resetMode(0)),
    m_animCmdInterpreter8(m_eventManageModule.getManageId(), ANIM_CMD_STACK(m_animCmdStack8), 8, 1, -1.0f, resetMode(0)),
    m_animCmdInterpreter9(m_eventManageModule.getManageId(), ANIM_CMD_STACK(m_animCmdStack9), 9, 1, -1.0f, resetMode(0)),
    m_animCmdInterpreter10(m_eventManageModule.getManageId(), ANIM_CMD_STACK(m_animCmdStack10), 10, 1, -1.0f, resetMode(0)),
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

    // The ground shape and the hit shapes of the enemy kind.
    soGroundShapeData* groundShapeData =
        static_cast<soGroundShapeData*>(soValueAccesser::getConstantIndefinite(&moduleAccesser, 0xa80f, 0));
    GroundShapeSet groundShapes = {&groundShapeData, 1};
    moduleAccesser.getGroundModule().setup(&groundShapes, m_taskId, 0);
    soSet<soCollisionHitData>* hitData =
        static_cast<soSet<soCollisionHitData>*>(soValueAccesser::getConstantIndefinite(&moduleAccesser, 0xa7fd, 0));
    if (hitData != NULL) {
        moduleAccesser.getCollisionHitModule().add(hitData, 0);
    }

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

    static_cast<soJostleModule&>(m_jostleModule).setByte48(soValueAccesser::getValueInt(&moduleAccesser, 0x59ee, 0) != 0);
    static_cast<soJostleModule&>(m_jostleModule).setByte46(soValueAccesser::getValueInt(&moduleAccesser, 0x59f1, 0));

    // The absorber and the reflector get one shape each (a unit sphere at the origin, inactive until the enemy sets it).
    soCollisionShieldData absorberShapes[1];
    for (int i = 0; i < 1; i++) {
        absorberShapes[i].setOffset(0, 0.0f);
        absorberShapes[i].setOffset(1, 0.0f);
        absorberShapes[i].setOffset(2, 0.0f);
        absorberShapes[i].setOffset(3, 0.0f);
        absorberShapes[i].setOffset(4, 0.0f);
        absorberShapes[i].setOffset(5, 0.0f);
        absorberShapes[i].m_size = 1.0f;
        absorberShapes[i].m_nodeIndex = 0;
        absorberShapes[i].m_shapeType = 1;
    }
    soCollisionShieldGroupData absorberGroup;
    absorberGroup.m_flags = 0;
    absorberGroup.m_shieldDataSet = soSet<soCollisionShieldData>(absorberShapes, 1);
    m_moduleAccesser->getCollisionAbsorberModule().add(&absorberGroup, 0);
    m_moduleAccesser->getCollisionAbsorberModule().clearAll();

    soCollisionShieldData reflectorShapes[1];
    for (int i = 0; i < 1; i++) {
        reflectorShapes[i].setOffset(0, 0.0f);
        reflectorShapes[i].setOffset(1, 0.0f);
        reflectorShapes[i].setOffset(2, 0.0f);
        reflectorShapes[i].setOffset(3, 0.0f);
        reflectorShapes[i].setOffset(4, 0.0f);
        reflectorShapes[i].setOffset(5, 0.0f);
        reflectorShapes[i].m_size = 1.0f;
        reflectorShapes[i].m_nodeIndex = 0;
        reflectorShapes[i].m_shapeType = 1;
    }
    ftReflectorGroupDraft reflectorGroup;
    FT_REFLECTOR_GROUP_CLEAR(reflectorGroup);
    memset(&reflectorGroup, 0, sizeof(reflectorGroup));
    reflectorGroup.m_shapes = reflectorShapes;
    reflectorGroup.m_count = 1;
    reflectorGroup.m_f0 = 1.0f;
    reflectorGroup.m_f1 = 1.0f;
    reflectorGroup.m_f2 = 1.0f;
    reflectorGroup.m_f3 = 0.5f;
    reflectorGroup.m_flag = 0;
    m_moduleAccesser->getCollisionReflectorModule().add(reinterpret_cast<soCollisionReflectorGroupData*>(&reflectorGroup), 0);
    m_moduleAccesser->getCollisionReflectorModule().clearAll();

    // The capture and catch nodes, and whether the hit shapes can be caught.
    static_cast<soCaptureModule*>(m_moduleAccesser->m_enumerationStart->m_captureModule)
        ->setNodes(0, soValueAccesser::getValueInt(&moduleAccesser, 0x59f3, 0), 0.0f);
    m_moduleAccesser->getCollisionHitModule().setCheckCatch(soValueAccesser::getValueInt(&moduleAccesser, 0x59f2, 0) != 0, 0);
    static_cast<soCatchModule*>(m_moduleAccesser->m_enumerationStart->m_catchModule)->setNodes(0, 0, 0.0f);
}

Enemy::~Enemy() {
    if (g_stTriggerMng != NULL) {
        fn_27_2330C4(m_createId, 3);
    }
}

// Builds the per-instance state that depends on the parameters: the strength, the territory widened by the enemy's size,
// the area and AI units, and the camera range.
void Enemy::init() {
    emAreaModuleImpl& area = dynamic_cast<emAreaModuleImpl&>(*moduleAccesser.m_enumerationStart->m_areaModule);
    soHyp::soTerritoryModuleHyp& territory = getTerritoryModule(&moduleAccesser);
    soHyp::soTargetSearchModuleHyp& targetSearch = getTargetSearchModule(&moduleAccesser);
    soCameraModule& camera = m_moduleAccesser->getCameraModule();
    m_moduleAccesser->getPostureModule().initRot();
    float strength = initStrength();
    float width = soValueAccesser::getValueFloat(&moduleAccesser, 0xbf6, 0);
    float height = soValueAccesser::getValueFloat(&moduleAccesser, 0xbf7, 0);
    soHyp::soTerritoryRect rect = *territory.getRect();
    widenSide(rect.m_left, -width, strength);
    widenSide(rect.m_right, width, strength);
    widenSide(rect.m_top, height, strength);
    widenSide(rect.m_bottom, -height, strength);
    territory.setRect(&rect);
    territory.unk10();
    targetSearch.unk10();
    area.createAreaDataSet();
    area.createAreaInstance();
    m_aiModule.createUnitAll();
    soSet<soCameraRange>* cameraRanges =
        static_cast<soSet<soCameraRange>*>(soValueAccesser::getConstantIndefinite(&moduleAccesser, 0xa811, 0));
    if (cameraRanges != NULL && soValueAccesser::getValueInt(&moduleAccesser, 0x59fb, 0) == 1) {
        camera.setup(cameraRanges);
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

// TODO(nonmatching): 98.98%, (a) non-semantic: same calls, arguments and constants; the original copies the 4th
// argument of constructInstance as its own constant (li 1), loads the archive before the module pointers in the
// ground collision block, and moves this before the floats for StageObject::activate.
// Activates the enemy at spawnPos with statusKind (createPost): the model and its shadow, the works, the strength, the
// ground collision, the link to the parent, the anim cmd table and the first status. A status kind outside the
// enemy kind's status table activates nothing.
void Enemy::activate(Vec3f* spawnPos, float lr, float unk, int statusKind) {
    if ((u32)statusKind >= emInfo::getInstance()->getStatusNum(m_enemyKind)) {
        return;
    }
    StageObject::activate(lr, unk, spawnPos, false);

    u32 modelFlags = soValueAccesser::getValueInt(&moduleAccesser, 0x59f9, 0) | 0x10;
    if (m_moduleAccesser->getColorBlendModule().isNull() == false) {
        modelFlags |= 0x108;
    }
    soModelModule& model = moduleAccesser.getModelModule();
    model.constructInstance(static_cast<gfSceneRoot::LayerType>(soValueAccesser::getValueInt(&moduleAccesser, 0x59e5, 0)),
                            modelFlags, true, 1, NULL, NULL, NULL, 0xffff, NULL,
                            s_modelLoupeNo, 0);
    emInfo::getInstance()->getNodeNum(m_enemyKind);
    float shadowYOffset = soValueAccesser::getValueFloat(&moduleAccesser, 0xbfe, 0);
    moduleAccesser.getShadowModule().setYOffset(shadowYOffset);
    moduleAccesser.getShadowModule().setSizeMinMax(0.0f, 100.0f);
    m_aiModule.m_isInactive = false;
    for (int i = 0; i < 4; i++) {
        resetFrameCounter(i);
    }
    m_maxAttackPower = 0.0f;
    m_hitStopFrame = 0;
    resetGoalPos();

    soWorkManageModule& work = moduleAccesser.getWorkManageModule();
    work.setFloat(0.0f, 0x11000009);
    work.setFloat(-1.0f, 0x11000001);
    work.setFloat(-1.0f, 0x1100000e);
    work.setFloat(0.0f, 0x11000002);
    work.setFloat(0.0f, 0x11000005);
    work.setFloat(0.0f, 0x11000007);
    work.setFloat(0.0f, 0x11000008);
    work.setFloat(0.0f, 0x11000004);
    work.setFloat(__fabs(soValueAccesser::getValueFloat(&moduleAccesser, 0xbe9, 0)), 0x11000006);
    work.setFloat(3.4028235e+38f, 0x1100000a);
    work.setFloat(3.4028235e+38f, 0x1100000b);
    work.setFloat(3.4028235e+38f, 0x1100000d);
    work.setFloat(0.0f, 0x1100000f);
    work.offFlag(WorkFlag_InvalidParent);
    work.offFlag(0x1200000f);
    work.offFlag(0x12000010);
    work.offFlag(0x12000011);
    work.offFlag(WorkFlag_Bullet);
    work.offFlag(0x12000002);
    work.offFlag(0x12000003);
    work.offFlag(0x12000004);
    work.offFlag(WorkFlag_AttackChecked);
    work.offFlag(WorkFlag_SearchHitWeapon);
    work.offFlag(0x12000009);
    work.offFlag(0x1200000a);
    work.offFlag(0x12000006);
    work.offFlag(0x12000007);
    work.offFlag(0x12000008);
    work.offFlag(0x1200000b);
    work.offFlag(0x1200000c);
    work.offFlag(0x1200000d);
    work.offFlag(0x12000012);
    work.offFlag(0x12000013);
    work.setInt(-1, 0x10000000);
    work.setInt(0, 0x10000002);
    work.setInt(0, 0x1000000c);
    work.setInt(0, 0x1000000d);
    work.setInt(0, 0x1000000e);
    work.setInt(0, WorkInt_CommunicationItemId);
    work.setInt(-1, 0x10000003);
    work.setInt(-1, WorkInt_StatusDataKind);
    work.setInt(0, 0x10000005);
    work.setInt(-1, 0x10000006);
    work.setInt(0, WorkInt_ShotParamId);
    work.setInt(-1, 0x10000008);
    work.setInt(-1, 0x10000009);
    work.setInt(0, 0x1000000a);
    work.setInt(-1, 0x1000000b);
    work.setInt(-1, 0x10000001);
    work.setInt(-1, 0x10000010);
    work.setInt(-1, 0x10000011);
    work.setInt(-1, 0x10000012);
    work.setInt(-1, 0x10000013);
    work.setInt(-1, 0x10000014);
    work.setInt(-1, 0x10000015);
    work.setInt(-1, 0x10000016);
    work.setInt(-1, 0x10000017);
    work.setInt(-1, 0x10000018);
    work.setInt(-1, 0x10000019);
    work.setInt(-1, 0x1000001a);
    // HYPOTHESIS: the 0x2xxxxxxx works are the second work of the work manage module (a derived enemy's own).
    work.offFlag(0x22000000);
    work.offFlag(0x22000001);
    work.offFlag(0x22000002);
    work.offFlag(0x22000003);
    work.offFlag(0x22000004);
    work.offFlag(0x22000005);
    work.offFlag(0x22000006);
    work.offFlag(0x22000007);
    work.offFlag(0x22000008);
    work.setInt(-1, 0x20000000);
    work.setInt(-1, 0x20000001);
    work.setInt(-1, 0x20000002);
    work.setInt(-1, 0x20000003);
    work.setInt(-1, 0x20000004);
    work.setInt(-1, 0x20000005);
    work.setFloat(0.0f, 0x21000000);
    work.setInt(0, 0x20000006);

    resetFrontFlag();
    initRot(emInfo::getInstance()->getNodeNum(m_enemyKind));

    // The strength: a base value that grows with initStrength's level unless the base is -1 or the rate 0.
    float level = initStrength();
    float strength = soValueAccesser::getValueFloat(&moduleAccesser, 0xbf4, 0);
    float strengthRate = soValueAccesser::getValueFloat(&moduleAccesser, 0xbf5, 0);
    if (strength != -1.0f && strengthRate != 0.0f) {
        strength += strengthRate * level;
    }
    work.setFloat(strength, 0x11000001);
    work.setFloat(strength, 0x1100000e);
    work.setFloat(strength, 0x11000003);

    resetGlowParam();
    m_aiModule.reset();
    if ((u8)moduleAccesser.getStatusModule().getStatusGroundCorrect(statusKind) != 5 &&
        (u8)moduleAccesser.getStatusModule().getStatusGroundCorrect(statusKind) != 6) {
        moduleAccesser.getGroundModule().attachGround(0);
    }

    // The ground collision of the enemy kind's model, when it has one.
    if (moduleAccesser.getResourceModule().getBinFile(m_preloadArchive->brresResourceId, 0, -1) != NULL) {
        emGroundModuleImpl& ground = dynamic_cast<emGroundModuleImpl&>(moduleAccesser.getGroundModule());
        if (soValueAccesser::getConstantIndefinite(&moduleAccesser, 0xa813, 0) == NULL) {
            ground.constructGrCollision(m_preloadArchive->brresResourceId, 0, Heaps::EnemyInstance, false, true, false);
        } else {
            ground.constructGrCollision(m_preloadArchive->brresResourceId, 0, Heaps::EnemyInstance, true, true, false);
            ground.updateVtxFromNode(true);
        }
        ground.setGrCollisionMode(true);
    }

    initCameraSubject();
    work.setInt(soValueAccesser::getValueInt(&moduleAccesser, 0x59eb, 0), 0x10000016);
    work.setInt(0, 0x10000017);

    // Link 3 is the parent enemy.
    u32 parentTaskId = getParentEnemyTaskId();
    if (parentTaskId != 0xffffffff) {
        if (m_moduleAccesser->getLinkModule().isLink(3) == true) {
            m_moduleAccesser->getLinkModule().unlink(3);
        }
        m_moduleAccesser->getLinkModule().link(3, parentTaskId);
    }

    soAnimCmdModule& animCmd = moduleAccesser.getAnimCmdModule();
    void* animCmdTable = soValueAccesser::getConstantIndefinite(&moduleAccesser, 0xa810, 0);
    if (animCmdTable != NULL) {
        animCmd.resetInterpreter3(0, animCmdTable, &moduleAccesser);
        animCmd.interpretCmd1(1.0f, 0, &moduleAccesser);
        animCmd.resetInterpreter3(0, NULL, &moduleAccesser);
    }
    moduleAccesser.getStatusModule().changeStatusForce(statusKind, &moduleAccesser);
    unkD8 = true;
}

// Moves the enemy so that its center node is at capturePos (after a capture or catch released it).
void Enemy::adjustConstraintAfterCapture(Vec3f* capturePos) {
    soModelModule& model = moduleAccesser.getModelModule();
    soPostureModule& posture = moduleAccesser.getPostureModule();
    int centerNode = soValueAccesser::getValueInt(&moduleAccesser, ValueInt_CenterNodeId, 0);
    initRot(0);
    updateNodeSRT();
    Vec3f center = model.getNodeGlobalPosition(centerNode, false);
    Vec3f offset = *capturePos - center;
    Vec3f pos = posture.getPos();
    pos += offset;
    posture.setPos(&pos);
    updateNodeSRT();
}

void Enemy::cutCapture() {
    soCaptureModule& capture = getCaptureModule(moduleAccesser);
    if (capture.isCapture() == true) {
        bool constrained = moduleAccesser.getLinkModule().isModelConstraint();
        soModelModule& model = moduleAccesser.getModelModule();
        Vec3f center = model.getNodeGlobalPosition(soValueAccesser::getValueInt(&moduleAccesser, ValueInt_CenterNodeId, 0), false);
        if (constrained == true) {
            getCaptureModule(moduleAccesser).cutConstraint();
        }
        if (getCaptureModule(moduleAccesser).isThrowCut() == true) {
            getCaptureModule(moduleAccesser).throwCut(constrained);
        } else {
            getCaptureModule(moduleAccesser).captureCut(true, constrained);
        }
        adjustConstraintAfterCapture(&center);
        dynamic_cast<emLinkModuleImpl&>(moduleAccesser.getLinkModule()).initConstraintOffsetMtx();
    }
}

void Enemy::cutCatch() {
    if (getCatchModule(moduleAccesser).isCatch() == true) {
        bool constrained = moduleAccesser.getLinkModule().isModelConstraint();
        soModelModule& model = moduleAccesser.getModelModule();
        Vec3f center = model.getNodeGlobalPosition(soValueAccesser::getValueInt(&moduleAccesser, ValueInt_CenterNodeId, 0), false);
        if (constrained == true) {
            getCatchModule(moduleAccesser).cutConstraint();
        }
        getCatchModule(moduleAccesser).catchCut(constrained, false);
        if (constrained == true) {
            adjustConstraintAfterCapture(&center);
        }
    }
}

void Enemy::deactivate() {
    int standByStatusKind = emInfo::getInstance()->StandByStatusKind(m_enemyKind);
    if (standByStatusKind != -1) {
        m_moduleAccesser->getStatusModule().changeStatusForce(standByStatusKind, m_moduleAccesser);
    }
    cutCapture();
    cutCatch();
    m_moduleAccesser->getEffectModule().removeAll(0);
    m_moduleAccesser->getDamageModule().initDamage(0.0f);
    m_moduleAccesser->getDamageModule().initInfo();
    m_moduleAccesser->getLinkModule().removeModelConstraint(true);
    for (int i = 3; i < 5; i++) {
        if (m_moduleAccesser->getLinkModule().isLink(i) == true) {
            m_moduleAccesser->getLinkModule().unlink(i);
        }
    }
    m_moduleAccesser->getModelModule().destructInstance();
    StageObject::deactivate(true);
    m_aiModule.m_isInactive = true;
    unkD8 = false;
}

// TODO(nonmatching): 47.30%, (a) non-semantic: same stores of 35791 to both members; the original first builds a
// two-float temporary in its frame and rounds the value (frsp) before the stores. (Its callers in this unit call it
// out of line; see the declaration.)
void Enemy::resetGoalPos() {
    m_goalPosX = 35791.0f;
    m_goalPosY = 35791.0f;
}

// The enemy has arrived when it is within 0.1 of the goal on each axis that has a goal.
// TODO(nonmatching): 99.24%, (a) non-semantic: the original loads the goal before the position (the operands of the
// subtraction swap registers); same calls, constants and branches.
bool Enemy::isArriveGoalPos() {
    bool arrived = true;
    if (35791.0f != m_goalPosX && 0.1f < (float)fabs(m_goalPosX - moduleAccesser.getPostureModule().getPos().m_x)) {
        arrived = false;
    }
    if (35791.0f != m_goalPosY && 0.1f < (float)fabs(m_goalPosY - moduleAccesser.getPostureModule().getPos().m_y)) {
        arrived = false;
    }
    return arrived;
}

float Enemy::getGoalPosX() {
    return m_goalPosX;
}

float Enemy::getGoalPosY() {
    return m_goalPosY;
}

// While the slow motion adjusts, the attack maxima of the frame are cleared and the float work 0x11000001 is carried over
// to 0x1100000E.
void Enemy::processHit() {
    StageObject::processHit();
    if (soSlow::getInstance()->isAdjust() == true) {
        m_maxAttackPower = 0.0f;
        m_hitStopFrame = 0;
        soWorkManageModule& work = moduleAccesser.getWorkManageModule();
        work.setFloat(work.getFloat(0x11000001), 0x1100000e);
    }
}

// While the slow motion estimates, a running (not stopped) enemy updates its kinetic energies ahead of the frame.
void Enemy::processUpdate() {
    if (soSlow::getInstance()->isEstimate() == true && moduleAccesser.getStopModule().isStop() == false) {
        dynamic_cast<emKineticModuleImpl&>(*moduleAccesser.m_enumerationStart->m_kineticModule).updateEnergyPre(-1);
    }
    StageObject::processUpdate();
}

void Enemy::setValueInt(const emScaledValue* value, u32 workId) {
    soWorkManageModule& work = moduleAccesser.getWorkManageModule();
    int result = value->m_base;
    float scale = soValueAccesser::getValueFloat(&moduleAccesser, ValueFloat_LevelScale, 0);
    if (value->m_rate != 0.0f) {
        result = result + value->m_rate * scale;
    }
    work.setInt(result, workId);
}

void Enemy::setValueFloat(const emScaledValue* value, u32 workId) {
    soWorkManageModule& work = moduleAccesser.getWorkManageModule();
    float result = value->m_base;
    float scale = soValueAccesser::getValueFloat(&moduleAccesser, ValueFloat_LevelScale, 0);
    if (value->m_rate != 0.0f) {
        result += value->m_rate * scale;
    }
    work.setFloat(result, workId);
}

void Enemy::setValueRndInt(const emScaledValue* range, u32 workId) {
    soWorkManageModule& work = moduleAccesser.getWorkManageModule();
    int min = range[0].m_base;
    int max = range[1].m_base;
    float scale = soValueAccesser::getValueFloat(&moduleAccesser, ValueFloat_LevelScale, 0);
    if (range[0].m_rate != 0.0f) {
        min = min + range[0].m_rate * scale;
    }
    if (range[1].m_rate != 0.0f) {
        max = max + range[1].m_rate * scale;
    }
    if (min > max) {
        max = min;
    }
    work.rndInt(min, max, workId);
}

void Enemy::setValueRndFloat(const emScaledValue* range, u32 workId) {
    soWorkManageModule& work = moduleAccesser.getWorkManageModule();
    float min = range[0].m_base;
    float max = range[1].m_base;
    float scale = soValueAccesser::getValueFloat(&moduleAccesser, ValueFloat_LevelScale, 0);
    if (range[0].m_rate != 0.0f) {
        min += range[0].m_rate * scale;
    }
    if (range[1].m_rate != 0.0f) {
        max += range[1].m_rate * scale;
    }
    if (min > max) {
        max = min;
    }
    work.rndFloat(min, max, workId);
}

int Enemy::getValueInt(const emScaledValue* value, soModuleAccesser* moduleAccesser) {
    int result = value->m_base;
    float scale = soValueAccesser::getValueFloat(moduleAccesser, ValueFloat_LevelScale, 0);
    if (value->m_rate != 0.0f) {
        result = result + value->m_rate * scale;
    }
    return result;
}

float Enemy::getValueFloat(const emScaledValue* value, soModuleAccesser* moduleAccesser) {
    float result = value->m_base;
    float scale = soValueAccesser::getValueFloat(moduleAccesser, ValueFloat_LevelScale, 0);
    if (value->m_rate != 0.0f) {
        result += value->m_rate * scale;
    }
    return result;
}

float Enemy::getValueRndFloat(const emScaledValue* range, soModuleAccesser* moduleAccesser) {
    float min = range[0].m_base;
    float max = range[1].m_base;
    float scale = soValueAccesser::getValueFloat(moduleAccesser, ValueFloat_LevelScale, 0);
    if (range[0].m_rate != 0.0f) {
        min += range[0].m_rate * scale;
    }
    if (range[1].m_rate != 0.0f) {
        max += range[1].m_rate * scale;
    }
    if (min > max) {
        max = min;
    }
    // A random value in [min, max]; the FLT_MIN term keeps max reachable.
    if (max < min) {
        return 0.0f;
    }
    if (max == min) {
        return max;
    } else {
        return min + (1.17549435e-38f + (max - min)) * randf();
    }
}

bool Enemy::isBullet() {
    return moduleAccesser.getWorkManageModule().isFlag(WorkFlag_Bullet);
}

bool Enemy::isObserv(char unk1) {
    bool result = false;
    if (StageObject::isObserv(unk1) == true || unk1 == 'e') {
        result = true;
    }
    return result;
}

// The argument types of acCmdArg::getArgType that the commands accept besides plain values.
static const s32 CmdArgType_Address = 2;
static const s32 CmdArgType_ValueIndex = 5;

// An argument of the command. (In this unit MWCC inlines single-expression helpers like this one, but not functions
// with a branch: the acCmdArg getters stay out of line, as in the original.)
static inline acCmdArg getCmdArg(const soArrayFixed<const acCmdArgConv>& args, s32 index) {
    return acCmdArg(&args.at(index));
}


// Read an argument that is either a plain value or a variable (a value index the enemy's value accesser resolves); a
// parameter block is either an address or a variable naming an indefinite constant.
// MATCH-ONLY: statement macros, because the original expands these reads in place. As inline functions that return
// the value (readFloatArg and so on, tried) MWCC keeps them out of line in this unit and notifyEventAnimCmd drops from
// 99.99% to 57%.
#define READ_FLOAT_ARG(args, index, acc, out)                                                                          \
    if (getCmdArg(args, index).getArgType() == CmdArgType_ValueIndex) {                                                \
        out = soValueAccesser::getValueFloat(acc, getCmdArg(args, index).getValueIndexData(), 0);                      \
    } else {                                                                                                           \
        out = getCmdArg(args, index).getFloatData();                                                                   \
    }
#define READ_INT_ARG(args, index, acc, out)                                                                            \
    if (getCmdArg(args, index).getArgType() == CmdArgType_ValueIndex) {                                                \
        out = soValueAccesser::getValueInt(acc, getCmdArg(args, index).getValueIndexData(), 0);                        \
    } else {                                                                                                           \
        out = getCmdArg(args, index).getIntData();                                                                     \
    }
#define READ_ADDRESS_ARG(args, index, acc, out)                                                                        \
    if (getCmdArg(args, index).getArgType() == CmdArgType_ValueIndex) {                                                \
        out = soValueAccesser::getConstantIndefinite(acc, getCmdArg(args, index).getValueIndexData(), 0);              \
    } else {                                                                                                           \
        out = getCmdArg(args, index).getAddressData();                                                                 \
    }

// The anim cmds of the enemies (group 'e'); the other groups go to StageObject. The command ids come from the jump table
// of the switch; their names describe what each case does. A command with the wrong number of arguments is ignored.
// HYPOTHESIS: all command names.
// TODO(nonmatching): 99.9994%, (a) non-semantic: the code is identical; the only difference is the name of the
// unit's literal pool (lbl_41_rodata_0 in the original, an anonymous pool here).
bool Enemy::notifyEventAnimCmd(acAnimCmd* acmd, soModuleAccesser* moduleAccesser, s32 unk3) {
    if (acmd->getGroup() != 'e') {
        return StageObject::notifyEventAnimCmd(acmd, moduleAccesser, unk3);
    }
    s8 type = acmd->getType();
    s32 argNum = acmd->getArgNum();
    if (type <= -1 || type >= 42) {
        return false;
    }
    switch (type) {
    case 0:
        // Remove the enemy.
        if (argNum > 0) {
            return true;
        }
        emManager::getInstance()->removeEnemy(m_createId);
        return true;
    case 5: {
        // Notify the trigger manager (argument: the message kind).
        if (argNum < 1 || argNum > 1) {
            return true;
        }
        soArrayContractibleTable<const acCmdArgConv> args = acmd->getArgList();
        int kind = getCmdArg(args, 0).getIntData();
        if (g_stTriggerMng != NULL) {
            fn_27_2330C4(m_createId, kind);
        }
        return true;
    }
    case 1:
        // Flag the death of this enemy's children.
        if (argNum > 0) {
            return true;
        }
        emManager::getInstance()->doFuncOtherEnemy(m_createId, onChildDeathFlag);
        return true;
    case 2:
    case 3: {
        // Reset the rotation of one node (2: by id, 3: by name) or, without arguments, of the whole posture.
        if (argNum < 0 || argNum > 1) {
            return true;
        }
        soArrayContractibleTable<const acCmdArgConv> args = acmd->getArgList();
        u32 nodeId = emInfo::getInstance()->getNodeNum(m_enemyKind);
        if (argNum == 1) {
            if (type == 2) {
                nodeId = getCmdArg(args, 0).getIntData();
            } else if (type == 3) {
                soModelModule& modelModule = moduleAccesser->getModelModule();
                nodeId = modelModule.getNodeId(getCmdArg(args, 0).getStringData());
            }
            if (nodeId == -1) {
                return true;
            }
        }
        initRot(nodeId);
        return true;
    }
    case 10:
    case 11:
    case 12:
    case 13: {
        // Set (10, 11) or add to (12, 13) the rotation of a node, given by id (10, 12) or by name (11, 13).
        if (argNum < 4 || argNum > 4) {
            return true;
        }
        soArrayContractibleTable<const acCmdArgConv> args = acmd->getArgList();
        u32 nodeId = -1;
        if (type == 10 || type == 12) {
            nodeId = getCmdArg(args, 0).getIntData();
        } else if (type == 11 || type == 13) {
            soModelModule& modelModule = moduleAccesser->getModelModule();
            nodeId = modelModule.getNodeId(getCmdArg(args, 0).getStringData());
        }
        if (nodeId == -1) {
            return true;
        }
        Vec3f rot(0.0f, 0.0f, 0.0f);
        READ_FLOAT_ARG(args, 1, moduleAccesser, rot.m_x)
        READ_FLOAT_ARG(args, 2, moduleAccesser, rot.m_y)
        READ_FLOAT_ARG(args, 3, moduleAccesser, rot.m_z)
        if (type == 10 || type == 11) {
            this->moduleAccesser.getPostureModule().setRot(&rot, nodeId);
        } else if (type == 12 || type == 13) {
            soPostureModule& postureModule = this->moduleAccesser.getPostureModule();
            Vec3f newRot = postureModule.getRot(nodeId);
            newRot += rot;
            postureModule.setRot(&newRot, nodeId);
        }
        return true;
    }
    case 30: {
        // Set the posture from a posture parameter block.
        if (argNum < 1 || argNum > 1) {
            return true;
        }
        soArrayContractibleTable<const acCmdArgConv> args = acmd->getArgList();
        void* postureParam;
        READ_ADDRESS_ARG(args, 0, moduleAccesser, postureParam)
        setPostureFromParam(postureParam, 1);
        return true;
    }
    case 31:
    case 32: {
        // Write the first (31) or the second (32) float of a parameter block to a work float.
        if (argNum < 2 || argNum > 2) {
            return true;
        }
        soArrayContractibleTable<const acCmdArgConv> args = acmd->getArgList();
        void* param;
        READ_ADDRESS_ARG(args, 0, moduleAccesser, param)
        const float* values = static_cast<const float*>(param);
        u32 workId = getCmdArg(args, 1).getValueIndexData();
        if (type == 31) {
            this->moduleAccesser.getWorkManageModule().setFloat(values[0], workId);
        } else if (type == 32) {
            this->moduleAccesser.getWorkManageModule().setFloat(values[1], workId);
        }
        return true;
    }
    case 14: {
        // Shoot (arguments: the shot, as a variable; optionally two more ints).
        if (argNum < 1 || argNum > 3) {
            return true;
        }
        soArrayContractibleTable<const acCmdArgConv> args = acmd->getArgList();
        u32 shotId = 0;
        if (getCmdArg(args, 0).getArgType() == CmdArgType_Address) {
            // HYPOTHESIS: an address argument is read but not used.
            getCmdArg(args, 0).getAddressData();
        } else if (getCmdArg(args, 0).getArgType() == CmdArgType_ValueIndex) {
            shotId = getCmdArg(args, 0).getValueIndexData();
        } else {
            return true;
        }
        int arg1 = 0;
        if (argNum == 2) {
            arg1 = getCmdArg(args, 1).getIntData();
        }
        int arg2 = -1;
        if (argNum == 3) {
            arg2 = getCmdArg(args, 2).getIntData();
        }
        if (lbl_41_bss_1C8 != NULL) {
            shoot(arg1, shotId, arg2);
        }
        return true;
    }
    case 26: {
        // Shoot an item (arguments: item kind, variation, parameter block, two optional floats).
        if (argNum < 3 || argNum > 5) {
            return true;
        }
        soArrayContractibleTable<const acCmdArgConv> args = acmd->getArgList();
        itKind kind = (itKind)getCmdArg(args, 0).getIntData();
        int variation = getCmdArg(args, 1).getIntData();
        float arg4 = 1.0f;
        float arg3 = 0.0f;
        // HYPOTHESIS: an unused offset.
        Vec3f offset(0.0f, 0.0f, 0.0f);
        if (argNum > 3) {
            READ_FLOAT_ARG(args, 3, moduleAccesser, arg3)
        }
        if (argNum > 4) {
            READ_FLOAT_ARG(args, 4, moduleAccesser, arg4)
        }
        void* param;
        READ_ADDRESS_ARG(args, 2, moduleAccesser, param)
        emShootItem(kind, variation, param, arg3, arg4);
        return true;
    }
    case 28: {
        // Remove the item the enemy holds (argument: whether the enemy is told first).
        if (argNum < 0 || argNum > 1) {
            return true;
        }
        soArrayContractibleTable<const acCmdArgConv> args = acmd->getArgList();
        bool returnToEnemy = false;
        if (argNum == 1) {
            returnToEnemy = getCmdArg(args, 0).getBoolData();
        }
        int instanceId = this->moduleAccesser.getWorkManageModule().getInt(WorkInt_CommunicationItemId);
        if (instanceId != 0) {
            BaseItem* item = itManager::getInstance()->getItemFromInstanceId(instanceId);
            if (item != NULL) {
                if (returnToEnemy == true) {
                    emItemInformationReturnEnemy(instanceId);
                }
                itManager::getInstance()->removeItem(item);
            }
        }
        return true;
    }
    case 15: {
        // Add parts (arguments: the parts, as a variable; optionally an int).
        if (argNum < 1 || argNum > 2) {
            return true;
        }
        soArrayContractibleTable<const acCmdArgConv> args = acmd->getArgList();
        u32 partsId = 0;
        if (getCmdArg(args, 0).getArgType() == CmdArgType_Address) {
            // HYPOTHESIS: an address argument is read but not used.
            getCmdArg(args, 0).getAddressData();
        } else if (getCmdArg(args, 0).getArgType() == CmdArgType_ValueIndex) {
            partsId = getCmdArg(args, 0).getValueIndexData();
        } else {
            return true;
        }
        int arg1 = 0;
        if (argNum == 2) {
            arg1 = getCmdArg(args, 1).getIntData();
        }
        if (lbl_41_bss_1C8 != NULL) {
            addParts(arg1, partsId, 0);
        }
        return true;
    }
    case 4: {
        // Restart a frame counter (argument: its index).
        if (argNum < 1 || argNum > 1) {
            return true;
        }
        soArrayContractibleTable<const acCmdArgConv> args = acmd->getArgList();
        resetFrameCounter(getCmdArg(args, 0).getIntData());
        return true;
    }
    case 16: {
        // Pause a frame counter.
        if (argNum < 1 || argNum > 1) {
            return true;
        }
        soArrayContractibleTable<const acCmdArgConv> args = acmd->getArgList();
        pauseFrameCounter(getCmdArg(args, 0).getIntData());
        return true;
    }
    case 17: {
        // Resume a frame counter.
        if (argNum < 1 || argNum > 1) {
            return true;
        }
        soArrayContractibleTable<const acCmdArgConv> args = acmd->getArgList();
        resumeFrameCounter(getCmdArg(args, 0).getIntData());
        return true;
    }
    case 8:
    case 9: {
        // Write a level-scaled value to a work int (8) or float (9).
        if (argNum < 2 || argNum > 2) {
            return true;
        }
        soArrayContractibleTable<const acCmdArgConv> args = acmd->getArgList();
        void* param;
        READ_ADDRESS_ARG(args, 0, moduleAccesser, param)
        const emScaledValue* value = static_cast<const emScaledValue*>(param);
        u32 workId = getCmdArg(args, 1).getValueIndexData();
        if (type == 8) {
            setValueInt(value, workId);
        } else if (type == 9) {
            setValueFloat(value, workId);
        }
        return true;
    }
    case 6:
    case 7: {
        // Write a random value between two level-scaled bounds to a work int (6) or float (7).
        if (argNum < 2 || argNum > 2) {
            return true;
        }
        soArrayContractibleTable<const acCmdArgConv> args = acmd->getArgList();
        void* param;
        READ_ADDRESS_ARG(args, 0, moduleAccesser, param)
        const emScaledValue* range = static_cast<const emScaledValue*>(param);
        u32 workId = getCmdArg(args, 1).getValueIndexData();
        if (type == 6) {
            setValueRndInt(range, workId);
        } else if (type == 7) {
            setValueRndFloat(range, workId);
        }
        return true;
    }
    case 27: {
        // Write a percentage of a value to a work float: value * rate / 100, or with the flag value * (100 - rate) / 100.
        if (argNum < 4 || argNum > 4) {
            return true;
        }
        soArrayContractibleTable<const acCmdArgConv> args = acmd->getArgList();
        float value;
        READ_FLOAT_ARG(args, 0, moduleAccesser, value)
        float rate;
        READ_FLOAT_ARG(args, 1, moduleAccesser, rate)
        u32 workId = getCmdArg(args, 2).getValueIndexData();
        float result;
        if (getCmdArg(args, 3).getBoolData() == true) {
            float rest = 100.0f - rate;
            if (rest <= 0.0f) {
                result = 0.0f;
            } else {
                result = value * rest / 100.0f;
            }
        } else {
            if (100.0f - rate <= 0.0f) {
                result = value;
            } else {
                result = value * rate / 100.0f;
            }
        }
        this->moduleAccesser.getWorkManageModule().setFloat(result, workId);
        return true;
    }
    case 33:
    case 34: {
        // Write the damage offset of a node (33: by id, 34: by name) to three work floats.
        if (argNum < 4 || argNum > 4) {
            return true;
        }
        soArrayContractibleTable<const acCmdArgConv> args = acmd->getArgList();
        u32 nodeId = -1;
        if (type == 33) {
            nodeId = getCmdArg(args, 0).getIntData();
        } else if (type == 34) {
            soModelModule& modelModule = moduleAccesser->getModelModule();
            nodeId = modelModule.getNodeId(getCmdArg(args, 0).getStringData());
        }
        if (nodeId == -1) {
            return true;
        }
        u32 workIdX = getCmdArg(args, 1).getValueIndexData();
        u32 workIdY = getCmdArg(args, 2).getValueIndexData();
        u32 workIdZ = getCmdArg(args, 3).getValueIndexData();
        Vec3f offset = getDamageOffsetFromNode(nodeId);
        soWorkManageModule& workManageModule = this->moduleAccesser.getWorkManageModule();
        workManageModule.setFloat(offset.m_x, workIdX);
        workManageModule.setFloat(offset.m_y, workIdY);
        workManageModule.setFloat(offset.m_z, workIdZ);
        return true;
    }
    case 18:
        // Reset the front flag.
        if (argNum < 0 || argNum > 0) {
            return true;
        }
        resetFrontFlag();
        return true;
    case 19: {
        // Request the area-out effect.
        if (argNum < 2 || argNum > 2) {
            return true;
        }
        soArrayContractibleTable<const acCmdArgConv> args = acmd->getArgList();
        int arg0;
        READ_INT_ARG(args, 0, moduleAccesser, arg0)
        int arg1;
        READ_INT_ARG(args, 1, moduleAccesser, arg1)
        reqEffectAreaOut(arg0, arg1);
        return true;
    }
    case 39: {
        // Let the ground shape follow the nodes (argument: on / off).
        if (argNum < 1 || argNum > 1) {
            return true;
        }
        soArrayContractibleTable<const acCmdArgConv> args = acmd->getArgList();
        bool on = getCmdArg(args, 0).getBoolData();
        dynamic_cast<emGroundModuleImpl&>(this->moduleAccesser.getGroundModule()).updateVtxFromNode(on);
        return true;
    }
    case 20:
    case 21: {
        // Set the goal position's x (20) or y (21).
        if (argNum < 1 || argNum > 1) {
            return true;
        }
        soArrayContractibleTable<const acCmdArgConv> args = acmd->getArgList();
        float value;
        READ_FLOAT_ARG(args, 0, moduleAccesser, value)
        if (type == 20) {
            m_goalPosX = value;
        } else if (type == 21) {
            m_goalPosY = value;
        }
        return true;
    }
    case 22:
        // Reset the goal position.
        if (argNum < 0 || argNum > 0) {
            return true;
        }
        resetGoalPos();
        return true;
    case 23:
    case 24: {
        // Create the enemies of a trigger at a node (23: by id, 24: by name), synchronizing up to four work variables.
        if (argNum < 9 || argNum > 13) {
            return true;
        }
        soArrayContractibleTable<const acCmdArgConv> args = acmd->getArgList();
        int triggerId;
        READ_INT_ARG(args, 0, moduleAccesser, triggerId)
        int index;
        READ_INT_ARG(args, 1, moduleAccesser, index)
        u32 nodeId = -1;
        if (type == 23) {
            nodeId = getCmdArg(args, 2).getIntData();
        } else if (type == 24) {
            soModelModule& modelModule = moduleAccesser->getModelModule();
            nodeId = modelModule.getNodeId(getCmdArg(args, 2).getStringData());
        }
        if (nodeId == -1) {
            return true;
        }
        Vec3f offset(0.0f, 0.0f, 0.0f);
        READ_FLOAT_ARG(args, 3, moduleAccesser, offset.m_x)
        READ_FLOAT_ARG(args, 4, moduleAccesser, offset.m_y)
        READ_FLOAT_ARG(args, 5, moduleAccesser, offset.m_z)
        float lr;
        READ_FLOAT_ARG(args, 6, moduleAccesser, lr)
        int statusKind;
        READ_INT_ARG(args, 7, moduleAccesser, statusKind)
        u8 unk;
        READ_INT_ARG(args, 8, moduleAccesser, unk)
        u32 syncWorkIds[4] = { -1, -1, -1, -1 };
        int syncWorkNum = 0;
        for (int i = 9; i < argNum; i++) {
            if (getCmdArg(args, i).getArgType() == CmdArgType_ValueIndex) {
                syncWorkIds[syncWorkNum] = getCmdArg(args, i).getValueIndexData();
                syncWorkNum++;
            }
            if (syncWorkNum >= 4) {
                break;
            }
        }
        ceateEnemyFromTriggerId(triggerId, nodeId, offset, lr, statusKind, unk, syncWorkIds, syncWorkNum, index);
        return true;
    }
    case 25: {
        // Sync a work variable with the parent.
        if (argNum < 1 || argNum > 1) {
            return true;
        }
        soArrayContractibleTable<const acCmdArgConv> args = acmd->getArgList();
        u32 workId = getCmdArg(args, 0).getValueIndexData();
        syncEnemyWorkWithParent(&workId, 1);
        return true;
    }
    case 29: {
        // Create a figure (argument: its variation).
        if (argNum < 1 || argNum > 1) {
            return true;
        }
        soArrayContractibleTable<const acCmdArgConv> args = acmd->getArgList();
        int variation;
        READ_INT_ARG(args, 0, moduleAccesser, variation)
        emCreateFigure(variation);
        return true;
    }
    case 35: {
        // Reset the level.
        if (argNum < 1 || argNum > 1) {
            return true;
        }
        soArrayContractibleTable<const acCmdArgConv> args = acmd->getArgList();
        int level;
        READ_INT_ARG(args, 0, moduleAccesser, level)
        resetLevel(level);
        return true;
    }
    case 36:
        // Release what the enemy captured.
        if (argNum < 0 || argNum > 0) {
            return true;
        }
        cutCapture();
        return true;
    case 37:
        // Release what the enemy caught.
        if (argNum < 0 || argNum > 0) {
            return true;
        }
        cutCatch();
        return true;
    case 40: {
        // Send a throw event (three variables, optionally an int).
        if (argNum < 3 || argNum > 4) {
            return true;
        }
        soArrayContractibleTable<const acCmdArgConv> args = acmd->getArgList();
        u32 arg0 = getCmdArg(args, 0).getValueIndexData();
        u32 arg1 = getCmdArg(args, 1).getValueIndexData();
        u32 arg2 = getCmdArg(args, 2).getValueIndexData();
        int arg3 = -1;
        // (argNum is at least 3 here: the original tests >= 2, so argument 3 is always read.)
        if (argNum >= 2) {
            READ_INT_ARG(args, 3, moduleAccesser, arg3)
        }
        sendEventThrow(arg0, arg1, arg2, arg3);
        return true;
    }
    case 38: {
        // Write the angle from one point to another to a work float.
        if (argNum < 5 || argNum > 5) {
            return true;
        }
        soArrayContractibleTable<const acCmdArgConv> args = acmd->getArgList();
        Vec2f from(0.0f, 0.0f);
        READ_FLOAT_ARG(args, 0, moduleAccesser, from.m_x)
        READ_FLOAT_ARG(args, 1, moduleAccesser, from.m_y)
        Vec2f to(0.0f, 0.0f);
        READ_FLOAT_ARG(args, 2, moduleAccesser, to.m_x)
        READ_FLOAT_ARG(args, 3, moduleAccesser, to.m_y)
        // The original reads the work id from argument 3 again (argument 4 is never read).
        u32 workId = getCmdArg(args, 3).getValueIndexData();
        float angle = getAtan2fAngle(&from, &to);
        this->moduleAccesser.getWorkManageModule().setFloat(angle, workId);
        return true;
    }
    case 41:
        // Reset the reflection: its info and the attack multiplier.
        if (argNum < 0 || argNum > 0) {
            return true;
        }
        this->moduleAccesser.getReflectModule().initInfo();
        this->moduleAccesser.getReflectModule().setAttackMul(1.0f);
        return true;
    default:
        return false;
    }
}

void Enemy::notifyEventChangeStatus(int statusKind, int prevStatusKind, soStatusData* statusData, soModuleAccesser* acc) {
    updateNodeSRT();
    soWorkManageModule& work = moduleAccesser.getWorkManageModule();
    work.setInt(static_cast<emStatusData*>(statusData)->m_unkC, WorkInt_StatusDataKind);
    resetFrameCounter(2);
    work.offFlag(WorkFlag_AttackChecked);
    m_aiModule.initAttackAreaTarget();
    work.offFlag(WorkFlag_SearchHitWeapon);
    work.offFlag(0x12000009);
    work.offFlag(0x1200000d);
    work.offFlag(0x12000012);
    work.offFlag(0x12000013);
    resetFrontFlag();
}

void Enemy::resetFrameCounter(int index) {
    if (index <= -1 || index >= 4) {
        return;
    }
    gfGameFrameCounter* counter = &m_frameCounters[index];
    counter->reset();
    counter->start();
}

void Enemy::pauseFrameCounter(int index) {
    if (index <= -1 || index >= 4) {
        return;
    }
    m_frameCounters[index].stop();
}

void Enemy::resumeFrameCounter(int index) {
    if (index <= -1 || index >= 4) {
        return;
    }
    m_frameCounters[index].start();
}

u32 Enemy::getFrameCounter(int index) {
    if (index <= -1 || index >= 4) {
        return 0;
    }
    return m_frameCounters[index].getCounter();
}

// TODO(nonmatching): 94.83%, (a) non-semantic: same calls, arguments, constants and branches; the 1.0f constant of
// calcHitStopFrame and the copy of power before checkRebound are scheduled differently.
// Keeps the largest attack power and hit stop of the frame; notifyEventCollisionAttackCheck applies them.
void Enemy::notifyEventCollisionAttack(float power, soCollisionLog* collisionLog, soModuleAccesser* acc) {
    gfTask* task = gfTaskScheduler::getInstance()->getTaskById(getTaskCategory(collisionLog), collisionLog->m_taskId);
    soCollisionAttackData* attackData =
        acc->getCollisionAttackModule().getData(collisionLog->m_damageIndex, collisionLog->m_isAbsolute);
    float one = 1.0f;
    int hitStopFrame = soDamageUtilActor::calcHitStopFrame(power, one, one, acc, attackData, 0);
    switch (static_cast<u8>(collisionLog->_33)) {
    case 1:
        // The hit stop of a reaction frame target replaces the computed one.
        if (task != NULL) {
            soCollisionHitModule* hit = soExternalValueAccesser::getCollisionHitModule(&dynamic_cast<StageObject&>(*task));
            if (hit->isReactionFrame() == true) {
                hitStopFrame = hit->getReactionFrame();
            }
        }
        break;
    case 0:
        if (acc->getSituationModule().getKind() == 0 && task != NULL) {
            float rebound1 = 0.0f;
            float rebound2 = 0.0f;
            if (!acc->getCollisionAttackModule().checkRebound(power, acc, &dynamic_cast<StageObject&>(*task), collisionLog,
                                                              &rebound1, &rebound2)) {
                return;
            }
        }
        break;
    }
    if (m_maxAttackPower < power) {
        m_maxAttackPower = power;
    }
    if (m_hitStopFrame < hitStopFrame) {
        m_hitStopFrame = hitStopFrame;
    }
}

bool Enemy::notifyEventCollisionAttackCheck(u32 flags) {
    if (m_maxAttackPower == 0.0f) {
        return false;
    }
    if (flags & 8) {
        return false;
    }
    if (moduleAccesser.getWorkManageModule().isFlag(WorkFlag_Bullet) == true) {
        u32 otherStopFrame = soValueAccesser::getValueInt(&moduleAccesser, 0x59df, 0);
        moduleAccesser.getStopModule().setOtherStop(otherStopFrame);
        soCameraModuleSimple::QuakeKind quakeKind =
            static_cast<soCameraModuleSimple::QuakeKind>(soValueAccesser::getValueInt(&moduleAccesser, 0x59e0, 0));
        moduleAccesser.getCameraModule().reqQuake(quakeKind, &moduleAccesser);
        if (soValueAccesser::getValueInt(&moduleAccesser, 0x59ea, 0) == 0) {
            moduleAccesser.getWorkManageModule().setInt(0, 0x10000000);
        }
    } else if (m_hitStopFrame > 0) {
        moduleAccesser.getStopModule().setHitStopFrame(m_hitStopFrame, 0);
    }
    moduleAccesser.getWorkManageModule().onFlag(WorkFlag_AttackChecked);
    return false;
}

void Enemy::notifyEventCollisionSearch(soCollisionLog* collisionLog, soModuleAccesser* acc) {
    u32 taskId = collisionLog->m_taskId;
    if (taskId != 0xffffffff) {
        gfTask* task = gfTaskScheduler::getInstance()->getTaskById(getTaskCategory(collisionLog), taskId);
        if (task != NULL) {
            switch (task->m_taskCategory) {
            case gfTask::Category_Weapon:
                moduleAccesser.getWorkManageModule().onFlag(WorkFlag_SearchHitWeapon);
                break;
            }
        }
    }
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

Vec3f Enemy::getCenterPos() {
    soModelModule& model = moduleAccesser.getModelModule();
    return model.getNodeGlobalPosition(soValueAccesser::getValueInt(&moduleAccesser, ValueInt_CenterNodeId, 0), false);
}

grGimmickMotionPath* Enemy::getMotionPathPtr() {
    return m_motionPath;
}

// HYPOTHESIS: the model scale the enemy's figure is created with.
static const u32 ValueFloat_FigureScale = 0xbf3;

// TODO(nonmatching): 95.20%, (a) non-semantic: same calls and arguments; the original loads the owner task id into a
// saved register before itManager::getInstance, ours loads it after (into the argument register).
// Drops the trophy (figure) of the enemy at its center: the first call only preloads the figure archive.
bool Enemy::emCreateFigure(int variation) {
    if (itManager::getInstance()->isCompItemKindArchive(Item_Figure, 0, true) == false) {
        itManager::getInstance()->preloadItemKindArchive(Item_Figure, 0, itArchive::Temp, true);
        return true;
    }
    Vec3f pos = moduleAccesser.getModelModule().getNodeGlobalPosition(
        soValueAccesser::getValueInt(&moduleAccesser, ValueInt_CenterNodeId, 0), false);
    BaseItem* figure = itManager::getInstance()->createItem(&pos, 1.0f, Item_Figure, variation,
                                                            moduleAccesser.getStageObject().m_taskId);
    if (figure == NULL) {
        return false;
    }
    figure->setModelScale(soValueAccesser::getValueFloat(&moduleAccesser, ValueFloat_FigureScale, 0));
    figure->updateNodeSRT();
    return true;
}

bool Enemy::emShootItem(itKind kind, int variation, void* param, float, float) {
    moduleAccesser.getWorkManageModule().setInt(0, WorkInt_CommunicationItemId);
    return false;
}

bool Enemy::emItemInformationReturnEnemy(int instanceId) {
    return false;
}

int Enemy::getSimpleWeaponResourceID(u32 id) {
    if (id == 0xffff) {
        return 0xffff;
    }
    if (m_preloadArchive != NULL) {
        return m_preloadArchive->getWeaponResourceID(id);
    }
    return 0xffff;
}

// Generates article articleId with the shot description shotParamId; the activator of the article reads it back
// (WorkInt_ShotParamId, Enemy::activeArticle).
bool Enemy::shoot(int articleId, int shotParamId, int unk3) {
    soGenerateArticleManageModule* generate = &moduleAccesser.getGenerateArticleManageModule();
    if (dynamic_cast<soGenerateArticleManageModuleNull*>(generate) != NULL) {
        return false;
    }
    if (generate->isGeneratable(articleId)) {
        m_moduleAccesser->getWorkManageModule().setInt(shotParamId, WorkInt_ShotParamId);
        getWeaponParamWork()->reset();
        getWeaponParamWork()->m_addressPack = &m_animCmdAddressPack4;
        getWeaponParamWork()->unk4 = unk3;
        soArticle* article = generate->generate(articleId, NULL, false);
        if (article->isNull() == false) {
            generate->entry(article);
        }
    } else {
        return false;
    }
    return true;
}

bool Enemy::addParts(int articleId, u32 shotParamId, int) {
    soGenerateArticleManageModule* generate = &moduleAccesser.getGenerateArticleManageModule();
    if (dynamic_cast<soGenerateArticleManageModuleNull*>(generate) != NULL) {
        return false;
    }
    if (generate->isGeneratable(articleId)) {
        m_moduleAccesser->getWorkManageModule().setInt(shotParamId, WorkInt_ShotParamId);
        getWeaponParamWork()->reset();
        getWeaponParamWork()->m_addressPack = &m_animCmdAddressPack4;
        soArticle* article = generate->generate(articleId, NULL, false);
        if (article->isNull() == false) {
            generate->entry(article);
        }
    } else {
        return false;
    }
    return true;
}

// The shared article activators of the enemies: read the description of the shot being fired, compute its launch
// position and flag, and launch the article from the enemy. Both always return false.
bool Enemy::activeArticle(wnemSwHolder* article, soModuleAccesser* moduleAccesser) {
    emShotParam* param = fn_41_473DC(moduleAccesser, moduleAccesser->getWorkManageModule().getInt(WorkInt_ShotParamId));
    Vec3f pos = fn_41_47470(param, moduleAccesser);
    emShotFlag flag;
    fn_41_47544(&flag, param);
    fn_41_47A2C(param, moduleAccesser, true);
    fn_41_47D8C(&pos, flag.m_value, article, param, moduleAccesser);
    return false;
}

bool Enemy::activeArticle1(wnemSimple* article, soModuleAccesser* moduleAccesser) {
    emShotParam* param = fn_41_473DC(moduleAccesser, moduleAccesser->getWorkManageModule().getInt(WorkInt_ShotParamId));
    Vec3f pos = fn_41_47470(param, moduleAccesser);
    emShotFlag flag;
    fn_41_47544(&flag, param);
    fn_41_47A2C(param, moduleAccesser, true);
    fn_41_47E80(&pos, flag.m_value, article, param, moduleAccesser);
    return false;
}

// Whether the enemies of the trigger can be created: the enemy manager has a free entry and every enemy of the
// trigger (index -1) or its index-th one can be created.
bool Enemy::canCreateEnemyFromTriggerId(int triggerId, int index) {
    if (emManager::getInstance()->isEntryEnemyEmpty() == false) {
        return false;
    }
    u32 num = fn_27_2331C4(g_stTriggerMng, triggerId);
    if (num == 0) {
        return false;
    }
    if (index == -1) {
        for (u32 i = 0; i < num; i++) {
            if (fn_27_233208(g_stTriggerMng, triggerId, i) == false) {
                return false;
            }
        }
    } else if (fn_27_233208(g_stTriggerMng, triggerId, index) == false) {
        return false;
    }
    return true;
}

// Creates the index-th enemy of the trigger; the child gets the given works of this enemy and its motion path.
void Enemy::ceateEnemyFromTriggerIdSub(int triggerId, void* desc, u32* syncWorkIds, int syncWorkNum, int index) {
    int createId = fn_27_233264(g_stTriggerMng, triggerId, desc, index);
    Enemy* child = emManager::getInstance()->getEnemyPtrFromId(createId);
    if (child != NULL) {
        if (syncWorkNum > 0 && syncWorkIds != NULL) {
            syncEnemyWork(this, child, syncWorkIds, syncWorkNum);
        }
        child->m_motionPath = m_motionPath;
    }
}

// HYPOTHESIS: the creation description handed to the trigger manager (fn_27_233264); field roles from this caller.
struct emTriggerCreateDesc {
    int m_teamNo;
    Vec3f m_pos;
    float m_lr;
    int m_statusKind;
    u8 unk18;
    u32 m_parentCreateId;
};

// Creates the enemies of the trigger (all of them for index -1) at a node of this enemy. Without unk, an enemy above
// level 5 creates nothing.
void Enemy::ceateEnemyFromTriggerId(int triggerId, int nodeId, Vec3f offset, float lr, int statusKind, u8 unk,
                                    u32* syncWorkIds, int syncWorkNum, int index) {
    if (unk < 1 && m_level > 5) {
        return;
    }
    if (canCreateEnemyFromTriggerId(triggerId, index) == false) {
        return;
    }
    emTriggerCreateDesc desc;
    desc.m_teamNo = moduleAccesser.getTeamModule().getTeam()->getNo();
    desc.m_pos = moduleAccesser.getModelModule().getNodeGlobalPosition(nodeId, &offset, false, false);
    desc.m_lr = lr;
    desc.m_statusKind = statusKind;
    desc.unk18 = unk;
    desc.m_parentCreateId = m_createId;
    if (index == -1) {
        u32 num = fn_27_2331C4(g_stTriggerMng, triggerId);
        for (u32 i = 0; i < num; i++) {
            ceateEnemyFromTriggerIdSub(triggerId, &desc, syncWorkIds, syncWorkNum, i);
            m_moduleAccesser->getWorkManageModule().offFlag(0x12000011);
        }
    } else {
        ceateEnemyFromTriggerIdSub(triggerId, &desc, syncWorkIds, syncWorkNum, index);
        m_moduleAccesser->getWorkManageModule().offFlag(0x12000011);
    }
}

// A work id: the top four bits select the work (1 and 2 are copied), the next four its type (0 int, 1 float, 2 flag).
static inline bool isSyncWorkId(u32 id) {
    int work = id >> 28;
    bool result = false;
    if (work > 0 && work < 3) {
        result = true;
    }
    return result;
}

void Enemy::syncEnemyWork(Enemy* from, Enemy* to, u32* workIds, int workNum) {
    if (workNum < 0 || workIds == NULL) {
        return;
    }
    soWorkManageModule& fromWork = from->m_moduleAccesser->getWorkManageModule();
    soWorkManageModule& toWork = to->m_moduleAccesser->getWorkManageModule();
    for (int i = 0; i < workNum; i++) {
        u32 id = workIds[i];
        if (isSyncWorkId(id)) {
            switch ((id >> 24) & 0xf) {
            case 0:
                toWork.setInt(fromWork.getInt(id), id);
                break;
            case 1:
                toWork.setFloat(fromWork.getFloat(id), id);
                break;
            case 2:
                toWork.setFlag(fromWork.isFlag(id), id);
                break;
            }
        }
    }
}

bool Enemy::isInvalidParentEnemy() {
    int parentId = emManager::getInstance()->getEnemyParentCreateId(m_createId);
    if (emManager::getInstance()->isEnemyExist(parentId) == false) {
        return true;
    }
    Enemy* parent = emManager::getInstance()->getEnemyRefFromId(parentId);
    return parent->m_moduleAccesser->getWorkManageModule().isFlag(WorkFlag_InvalidParent) == true;
}

int Enemy::getParentEnemyTaskId() {
    if (isInvalidParentEnemy() == true) {
        return -1;
    }
    int parentId = emManager::getInstance()->getEnemyParentCreateId(m_createId);
    return emManager::getInstance()->getEnemyRefFromId(parentId)->m_taskId;
}

void Enemy::syncEnemyWorkWithParent(u32* workIds, int workNum) {
    if (workNum < 0 || workIds == NULL) {
        return;
    }
    if (isInvalidParentEnemy() == true) {
        return;
    }
    int parentId = emManager::getInstance()->getEnemyParentCreateId(m_createId);
    syncEnemyWork(emManager::getInstance()->getEnemyRefFromId(parentId), this, workIds, workNum);
}

int Enemy::getParentEnemyStatusKind() {
    int parentId = emManager::getInstance()->getEnemyParentCreateId(m_createId);
    if (emManager::getInstance()->isEnemyExist(parentId) == false) {
        return -1;
    }
    return emManager::getInstance()->getEnemyRefFromId(parentId)->m_moduleAccesser->getStatusModule().getStatusKind();
}

// The strength (ValueFloat_LevelScale work float) of the parent, or this enemy's own without a parent.
float Enemy::getParentEnemyStrength() {
    int parentId = emManager::getInstance()->getEnemyParentCreateId(m_createId);
    if (emManager::getInstance()->isEnemyExist(parentId) == false) {
        return m_moduleAccesser->getWorkManageModule().getFloat(ValueFloat_LevelScale);
    }
    return emManager::getInstance()->getEnemyRefFromId(parentId)->m_moduleAccesser->getWorkManageModule().getFloat(
        ValueFloat_LevelScale);
}

// TODO(nonmatching): 99.76%, (a) non-semantic: the operands of the fmuls are swapped and the second integer-to-float
// temporary gets its own stack slot; the conversions, compares and result agree.
bool Enemy::canCreateEnemyFromEnemyKindAndNum(EnemyKind kind, u32 maxNum, float rate) {
    u32 count = emManager::getInstance()->getEnemyCount();
    u32 kindCount = emManager::getInstance()->getEnemyCountFromKind(kind);
    float rest = (float)maxNum - (float)(count - kindCount) * rate;
    if (rest <= 0.0f) {
        return false;
    }
    return !(rest <= (float)kindCount);
}

int Enemy::getCommunicationItemPortData() {
    int instanceId = moduleAccesser.getWorkManageModule().getInt(WorkInt_CommunicationItemId);
    if (instanceId != 0) {
        BaseItem* item = itManager::getInstance()->getItemFromInstanceId(instanceId);
        if (item != NULL) {
            return item->getParamInt(0x1000000e);
        }
    }
    return 0;
}

int Enemy::getCommunicationItemSituationKind() {
    int instanceId = moduleAccesser.getWorkManageModule().getInt(WorkInt_CommunicationItemId);
    if (instanceId != 0) {
        BaseItem* item = itManager::getInstance()->getItemFromInstanceId(instanceId);
        if (item != NULL) {
            return item->getEmbeddedModuleAccesser().getSituationModule().getKind();
        }
    }
    return 5;
}

bool Enemy::notifyEventCaptureStatus(soModuleAccesser* acc, int taskId, int, int) {
    int captureNode = soValueAccesser::getValueInt(&moduleAccesser, 0x59f4, 0);
    if (captureNode >= 0) {
        emInfo::getInstance()->getStatusNum(m_enemyKind);
    }
    if (getCaptureModule(*m_moduleAccesser).capture(taskId, captureNode, true) == true) {
        emLinkModuleImpl* link = &dynamic_cast<emLinkModuleImpl&>(moduleAccesser.getLinkModule());
        link->setConstraintOffsetMtx(soValueAccesser::getValueInt(&moduleAccesser, 0x59f3, 0));
        moduleAccesser.getWorkManageModule().offFlag(0x12000010);
        return true;
    }
    return false;
}

// HYPOTHESIS: the arguments of the link events the enemy handles; event 0xb clears two words of its arguments.
struct emLinkEventArgs : public soLinkEventArgs {
    u8 unk8[0x4];
    int unkC;
    int unk10;
};

// HYPOTHESIS: the link event kinds below, from what the handler does: 0 / 0x3D an item holding the enemy (removed while
// it is model-constrained); 0x3D also releases the constraint; 0xA the capture is cut; 0xB the capture ends.
void Enemy::notifyEventLink(soLinkEventArgs* eventInfo, soModuleAccesser* acc, StageObject* sender, int unk4) {
    switch (eventInfo->m_eventKind) {
    case 0:
        if (sender->soGetKind() == StageObject_Item && unk4 == 4 && moduleAccesser.getLinkModule().isModelConstraint() == true) {
            emManager::getInstance()->removeEnemy(m_createId);
            return;
        }
        break;
    case 0x3d:
        if (sender->soGetKind() == StageObject_Item && unk4 == 4 && moduleAccesser.getLinkModule().isModelConstraint() == true) {
            emManager::getInstance()->removeEnemy(m_createId);
            return;
        }
        if (moduleAccesser.getLinkModule().isModelConstraint() == true) {
            Vec3f rot(0.0f, 0.0f, 0.0f);
            Vec3f pos(0.0f, 0.0f, 0.0f);
            moduleAccesser.getLinkModule().adjustModelConstraintPosture(&rot, &pos);
            updateNodeSRT();
            m_moduleAccesser->getLinkModule().removeModelConstraint(true);
        }
        break;
    case 0xa: {
        getCaptureModule(*m_moduleAccesser).unk54(0);
        moduleAccesser.getWorkManageModule().onFlag(0x12000010);
        soModelModule& model = moduleAccesser.getModelModule();
        Vec3f center = model.getNodeGlobalPosition(soValueAccesser::getValueInt(&moduleAccesser, ValueInt_CenterNodeId, 0), false);
        getCaptureModule(moduleAccesser).cutConstraint();
        getCaptureModule(moduleAccesser).captureCut(true, true);
        adjustConstraintAfterCapture(&center);
        dynamic_cast<emLinkModuleImpl&>(moduleAccesser.getLinkModule()).initConstraintOffsetMtx();
        dynamic_cast<emGroundModuleImpl&>(moduleAccesser.getGroundModule()).setCorrect(soGroundShapeImpl::CorrectKind(5), 0);
        break;
    }
    case 0xb:
        static_cast<emLinkEventArgs*>(eventInfo)->unkC = 0;
        static_cast<emLinkEventArgs*>(eventInfo)->unk10 = 0;
        getCaptureModule(moduleAccesser).unk3C();
        dynamic_cast<emGroundModuleImpl&>(moduleAccesser.getGroundModule()).setCorrect(soGroundShapeImpl::CorrectKind(0), 0);
        break;
    }
    StageObject::notifyEventLink(eventInfo, acc, sender, unk4);
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
