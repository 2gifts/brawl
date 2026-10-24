#pragma once

// SHADOW of BrawlHeaders em/enemy.h (-I include comes first): adds the constructor and destructor the enemy units call,
// the type of the storage the derived enemies hand to it, and lets the derived enemies read the members.

// The module classes of sora_melee are embedded with their real layout, the one the fighter RELs build them with
// (FT_MODULE_BUILDER, which the Enemy unit defines before any include). Four of those headers declare a shorter layout
// without FT_MODULE_BUILDER (soAnimCmdInterpreter, soLinkModuleImpl, soWorkManageModuleImpl,
// soSound3dGeneratorAccesserImpl); in the other units, which include them before this header, those members only keep
// their size.
// TODO(layout): drop EM_MODULE_MEMBER once those four layouts no longer depend on FT_MODULE_BUILDER.
#ifdef FT_MODULE_BUILDER
#define EM_MODULE_MEMBER(Type, name, size) Type name
#else
#define EM_MODULE_MEMBER(Type, name, size) u8 name[size]
#endif

// ft_dol_array_list.h must come before any header that instantiates one of its arrays.
#include <em/em_enemy_modules.h>
#include <StaticAssert.h>
#include <em/em_ai_module_impl.h>
#include <em/em_area_module_impl.h>
#include <em/em_archive.h>
#include <em/em_create.h>
#include <em/resource/em_resource_module_impl.h>
#include <em/stop/em_stop_module_impl.h>
#include <gr/gr_gimmick_motion_path.h>
#include <memory.h>
#include <mt/mt_vector.h>
#include <so/stageobject.h>
#include <types.h>

#include <so/anim/so_anim_cmd_module_impl.h>
#include <so/area/so_area_instance.h>
#include <so/article/so_generate_article_manage_module.h>
#include <so/camera/so_camera_module_impl.h>
#include <so/camera/so_camera_subject.h>
#include <so/collision/so_collision_attack_module_impl.h>
#include <so/collision/so_collision_catch_event_presenter.h>
#include <so/collision/so_collision_search_module_impl.h>
#include <so/collision/so_collision_shield_module_impl.h>
#include <so/color/so_color_blend_module_impl.h>
#include <so/damage/so_damage_event_presenter.h>
#include <so/effect/so_effect_module_impl.h>
#include <so/event/so_event_manage_module_impl.h>
#include <so/event/so_event_system.h>
#include <so/model/so_model_event_presenter.h>
#include <so/motion/so_motion_module_impl.h>
#include <so/posture/so_posture_module_impl.h>
#include <so/resource/so_resource_id_accesser.h>
#include <so/shadow/so_shadow_module_impl.h>
#include <so/situation/so_situation_module_impl.h>
#include <so/slope/so_slope_module_impl.h>
#include <so/so_heap_module_impl.h>
#include <so/so_instance_manager.h>
#include <so/sound/so_sound_3d_generator_accesser_impl.h>
#include <so/sound/so_sound_module_impl.h>
#include <so/team/so_team_impl.h>
#include <so/visibility/so_visibility_module_simple.h>
#include <so/work/so_general_work_simple.h>
#include <so/work/so_work_manage_module_impl.h>

class soArticleMediator;
class soReflectModule;
class emTargetSearchUnit;

// Shared null objects of sora_melee every enemy constructor passes on.
extern char g_soReflectModuleNull[];
// HYPOTHESIS: a null area object (0x10 bytes of sora_melee bss) for the enemy area module.
extern char lbl_27_bss_A5C[];

// One AI entry of an enemy (emInfo::getAINum of them), handed to emAIModuleImpl by the Enemy constructor. Constructed
// and destroyed by functions of the Enemy unit; only the pointer and the byte it copies are known.
// HYPOTHESIS: the pointer is the AI data the entry was set from (its byte +0x14 is copied into m_flag).
struct emAIEntry {
    void* m_data;
    u8 m_flag;
    emAIEntry();
    ~emAIEntry();
};

class Enemy : public StageObject, public soStatusEventObserver, public soCollisionAttackEventObserver, public soCollisionSearchEventObserver, public soCaptureEventObserver, public soCollisionAbsorberEventObserver, public soCollisionReflectorEventObserver {
protected:
    EnemyKind m_enemyKind;                 // 0xAC, emCreate +0x4
    // HYPOTHESIS: an id of the creator (first constructor argument); the trigger manager is notified with it on remove (4)
    // and on destruction (3).
    u32 m_triggerNotifyId;                 // 0xB0
    int m_connectedTriggerId;              // 0xB4, emCreate +0x38
    u32 m_difficulty;                      // 0xB8, emCreate +0x0
    u32 m_level;                           // 0xBC, emCreate +0x20
    u8 unkC0[0x8];                         // 0xC0, not set by the constructor
    grGimmickMotionPath* m_motionPath;     // 0xC8, emCreate +0x44
    int unkCC;                             // 0xCC, -1
    float unkD0;                           // 0xD0
    float unkD4;                           // 0xD4
    bool unkD8;                            // 0xD8
    soModuleAccesser moduleAccesser;       // 0xDC

    // Event system: the units of the event manage module, one per observer kind (capacity in the template argument).
    soInstanceManagerSimpleEntity<soEventUnit*, soArrayVector<soInstanceUnit<soEventUnit*>, 19> > m_eventUnits; // 0x1BC
    soEventManageModuleImpl m_eventManageModule;                                                   // 0x274
    soEventUnitWithWorkArea<soStatusEventObserver, 23> m_statusEventUnit;                         // 0x288
    soEventUnitWithWorkArea<soAnimCmdEventObserver, 37> m_animCmdEventUnit;                       // 0x3C8
    soEventUnitWithWorkArea<soDamageEventObserver, 4> m_damageEventUnit;                          // 0x5B0
    soEventUnitWithWorkArea<soLinkEventObserver, 2> m_linkEventUnit;                              // 0x60C
    soEventUnitWithWorkArea<soSituationEventObserver, 2> m_situationEventUnit;                    // 0x650
    soEventUnitWithWorkArea<soCollisionHitEventObserver, 4> m_collisionHitEventUnit;              // 0x694
    soEventUnitWithWorkArea<soCollisionAttackEventObserver, 5> m_collisionAttackEventUnit;        // 0x6F0
    soEventUnitWithWorkArea<soCollisionSearchEventObserver, 1> m_collisionSearchEventUnit;        // 0x758
    soEventUnitWithWorkArea<soCaptureEventObserver, 1> m_captureEventUnit;                        // 0x790
    soEventUnitWithWorkArea<soCollisionAbsorberEventObserver, 1> m_collisionAbsorberEventUnit;    // 0x7C8
    soEventUnitWithWorkArea<soCollisionReflectorEventObserver, 1> m_collisionReflectorEventUnit;  // 0x800
    soEventUnitWithWorkArea<soCollisionShieldEventObserver, 1> m_collisionShieldEventUnit;        // 0x838
    soEventUnitWithWorkArea<soModelEventObserver, 4> m_modelEventUnit;                            // 0x870
    soEventUnitWithWorkArea<soCollisionCatchEventObserver, 1> m_collisionCatchEventUnit;          // 0x8CC

    soHeapModuleImpl m_heapModule;                       // 0x904
    // The archive of the enemy kind (emManager::getPreloadArchivePtrFromKind).
    emArchive* m_preloadArchive;                         // 0x91C
    soResourceIdAccesserImpl m_resourceIdAccesser;       // 0x920
    emResourceModuleImpl m_resourceModule;               // 0x938
    // The work of the status module (7 ints, 1 float, 9 flags).
    s32 m_statusIntWorks[7];                             // 0x974
    float m_statusFloatWorks[1];                         // 0x990
    u32 m_statusFlagWorks[9];                            // 0x994
    soGeneralWorkSimple m_statusWork;                    // 0x9B8
    // The general work of the enemy (work manage module, index 1): 27 ints, 16 floats, 20 flags.
    s32 m_intWorks[27];                                  // 0x9DC
    float m_floatWorks[16];                              // 0xA48
    u32 m_flagWorks[20];                                 // 0xA88
    soGeneralWorkSimple m_generalWork;                   // 0xAD8
    EM_MODULE_MEMBER(soWorkManageModuleImpl, m_workManageModule, 0x34); // 0xAFC
    emAreaModuleImpl m_areaModule;                       // 0xB30
    emAIModuleImpl m_aiModule;                           // 0x1108
    soArrayVector<soModelNodeSetUp, 11> m_modelNodeSetUps; // 0x116C
    soModelModuleImpl m_modelModule;                     // 0x13B4
    soArrayContractibleTable<const soStatusData> m_statusTable; // 0x1480
    soArrayContractibleTable<const soMotionData> m_motionTable; // 0x1490
    soArrayVector<soPartialAnim, 2> m_partialAnims;      // 0x14A0
    soArrayVector<soOtherAnim, 3> m_otherAnims;          // 0x153C
    soMotionModuleImpl m_motionModule;                   // 0x15CC
    soSlopeModuleImpl m_slopeModule;                     // 0x173C
    soArrayVector<soGroundShapeImpl, 1> m_groundShapes;  // 0x17BC
    emGroundModuleImpl m_groundModule;                   // 0x180C
    soTargetSearchModuleImpl m_targetSearchModule;       // 0x1898
    soArrayVector<soLinkConnection, 5> m_linkConnections; // 0x18CC
    EM_MODULE_MEMBER(emLinkModuleImpl, m_linkModule, 0x84); // 0x19DC
    soTeamImpl m_team;                                   // 0x1A60, team number emCreate +0x8
    soTeamModuleImpl m_teamModule;                       // 0x1A70
    soArrayVector<soCollisionAttackPart, 5> m_attackParts;             // 0x1AB4
    soArrayVector<soCollisionGroup, 5> m_attackGroups;                 // 0x1D90
    soArrayVector<soCollisionAttackAbsolute, 1> m_attackAbsolutes;     // 0x1FF4
    soCollisionAttackModuleImpl m_collisionAttackModule;               // 0x2068
    soArrayVector<soCollisionHitPart, 10> m_hitParts;                  // 0x2108
    soArrayVector<soCollisionGroup, 1> m_hitGroups;                    // 0x2524
    soArrayVector<soCollisionHitGroup, 1> m_hitGroupInfos;             // 0x25A8
    emCollisionHitModuleImpl m_collisionHitModule;                     // 0x25EC
    soArrayVector<soCollisionSearchPart, 1> m_searchParts;             // 0x265C
    soCollisionSearchModuleImpl m_collisionSearchModule;               // 0x26C8
    soArrayVector<soCollisionShieldPart, 1> m_absorberParts;           // 0x27D0
    soArrayVector<soCollisionGroup, 1> m_absorberGroups;               // 0x283C
    soArrayVector<soCollisionShieldGroup, 1> m_absorberShieldGroups;   // 0x28C0
    soCollisionShieldEventPresenterAbsorber m_absorberPresenter;       // 0x2984
    soCollisionShieldModuleImpl m_collisionAbsorberModule;             // 0x2994
    soArrayVector<soCollisionShieldPart, 1> m_reflectorParts;          // 0x29E8
    soArrayVector<soCollisionGroup, 1> m_reflectorGroups;              // 0x2A54
    soArrayVector<soCollisionShieldGroup, 1> m_reflectorShieldGroups;  // 0x2AD8
    soCollisionShieldEventPresenterReflector m_reflectorPresenter;     // 0x2B9C
    soCollisionShieldModuleImpl m_collisionReflectorModule;            // 0x2BAC
    soArrayVector<soCollisionCatchPart, 2> m_catchParts;               // 0x2C00
    soCollisionCatchModuleImpl m_collisionCatchModule;                 // 0x2CC4
    soArrayVector<soEffectContinual, 1> m_effectContinuals;            // 0x2DA8
    soArrayVector<u32, 1> m_effectHandles;                             // 0x2DE0
    soEffectModuleImpl m_effectModule;                                 // 0x2DF0
    soColorBlendModuleImpl m_colorBlendModule;                         // 0x2F28
    EM_MODULE_MEMBER(soSound3dGeneratorAccesserImpl, m_sound3dGeneratorAccesser, 0xC); // 0x307C
    soSoundModuleImpl m_soundModule;                                   // 0x3088
    emTerritoryModuleImpl m_territoryModule;                           // 0x30E4
    soSituationModuleImpl m_situationModule;                           // 0x3134
    soPostureModuleImpl m_postureModule;                               // 0x3170
    soArrayVector<soShakeTerm, 2> m_shakeTerms;                        // 0x31E8
    soShakeModuleImpl m_shakeModule;                                   // 0x322C
    soArrayVector<soDamage, 1> m_damages;                              // 0x3248
    emDamageModuleImpl m_damageModule;                                 // 0x32F4
    emTurnModuleImpl m_turnModule;                                     // 0x33D4
    soVisibilityModuleSimple m_visibilityModule;                       // 0x3420
    soShadowModuleImpl m_shadowModule;                                 // 0x3434
    soGlowModuleImpl m_glowModule;                                     // 0x347C
    emStopModuleImpl m_stopModule;                                     // 0x34A0
    soCameraModuleImpl m_cameraModule;                                 // 0x34C4
    soCatchModuleImpl m_catchModule;                                   // 0x34E8
    soCaptureModuleImpl m_captureModule;                               // 0x354C
    soArrayVector<soTargetSearchRayCheckCache, 8> m_rayCheckCaches;    // 0x3580, target search module
    soJostleModuleImpl m_jostleModule;                                 // 0x372C

    // Anim cmd: one interpreter per thread kind (1, 2, 3, 0, 4-10) with its own stack.
    soArrayVector<acCmdInterpreterStackData, 7> m_animCmdStack1;       // 0x3778
    soArrayVector<acCmdInterpreterStackData, 6> m_animCmdStack2;       // 0x3810
    soArrayVector<acCmdInterpreterStackData, 6> m_animCmdStack3;       // 0x3894
    soArrayVector<acCmdInterpreterStackData, 16> m_animCmdStack0;      // 0x3918
    soArrayVector<acCmdInterpreterStackData, 7> m_animCmdStack4;       // 0x3A64
    soArrayVector<acCmdInterpreterStackData, 6> m_animCmdStack5;       // 0x3AFC
    soArrayVector<acCmdInterpreterStackData, 12> m_animCmdStack6;      // 0x3B80
    soArrayVector<acCmdInterpreterStackData, 6> m_animCmdStack7;       // 0x3C7C
    soArrayVector<acCmdInterpreterStackData, 7> m_animCmdStack8;       // 0x3D00
    soArrayVector<acCmdInterpreterStackData, 6> m_animCmdStack9;       // 0x3D98
    soArrayVector<acCmdInterpreterStackData, 6> m_animCmdStack10;      // 0x3E1C
    EM_MODULE_MEMBER(soAnimCmdInterpreter, m_animCmdInterpreter1, 0x50); // 0x3EA0
    EM_MODULE_MEMBER(soAnimCmdInterpreter, m_animCmdInterpreter2, 0x50); // 0x3EF0
    EM_MODULE_MEMBER(soAnimCmdInterpreter, m_animCmdInterpreter3, 0x50); // 0x3F40
    EM_MODULE_MEMBER(soAnimCmdInterpreter, m_animCmdInterpreter0, 0x50); // 0x3F90
    EM_MODULE_MEMBER(soAnimCmdInterpreter, m_animCmdInterpreter4, 0x50); // 0x3FE0
    EM_MODULE_MEMBER(soAnimCmdInterpreter, m_animCmdInterpreter5, 0x50); // 0x4030
    EM_MODULE_MEMBER(soAnimCmdInterpreter, m_animCmdInterpreter6, 0x50); // 0x4080
    EM_MODULE_MEMBER(soAnimCmdInterpreter, m_animCmdInterpreter7, 0x50); // 0x40D0
    EM_MODULE_MEMBER(soAnimCmdInterpreter, m_animCmdInterpreter8, 0x50); // 0x4120
    EM_MODULE_MEMBER(soAnimCmdInterpreter, m_animCmdInterpreter9, 0x50); // 0x4170
    EM_MODULE_MEMBER(soAnimCmdInterpreter, m_animCmdInterpreter10, 0x50); // 0x41C0

    soArrayQueueImpl<soArrayVector<s32, 2> > m_statusChangeRequests;  // 0x4210
    emStatusModuleImpl m_statusModule;                                 // 0x4228
    soTransitionModuleEntity<3, 1> m_statusTransition;                 // 0x42DC

    // The anim cmd tables of the enemy kind (emInfo vtable +0x18 / +0x1C, sizes from soValueAccesser constants), zipped
    // three by three into the address packs of the control units.
    soArrayContractibleTable<const acAnimCmdConv*> m_animCmdTable0;    // 0x4350
    soArrayContractibleTable<const acAnimCmdConv*> m_animCmdTable1;    // 0x4360
    soArrayContractibleTable<const acAnimCmdConv*> m_animCmdTable2;    // 0x4370
    soArrayContractibleTable<const acAnimCmdConv*> m_animCmdTable3;    // 0x4380
    soArrayContractibleTable<const acAnimCmdConv*> m_animCmdTable4;    // 0x4390
    soArrayContractibleTable<const acAnimCmdConv*> m_animCmdTable5;    // 0x43A0
    soArrayContractibleTable<const acAnimCmdConv*> m_animCmdTable6;    // 0x43B0
    soArrayContractibleTable<const acAnimCmdConv*> m_animCmdTable7;    // 0x43C0
    soArrayContractibleTable<const acAnimCmdConv*> m_animCmdTable8;    // 0x43D0
    soArrayContractibleTable<const acAnimCmdConv*> m_animCmdTable9;    // 0x43E0
    soArrayContractibleTable<const acAnimCmdConv*> m_animCmdTable10;   // 0x43F0
    soArrayContractibleTable<const acAnimCmdConv*> m_animCmdTable11;   // 0x4400
    soArrayContractibleTable<const acAnimCmdConv*> m_animCmdTable12;   // 0x4410
    soAnimCmdAddressPackArraySeparate m_animCmdAddressPack0;          // 0x4420
    soAnimCmdAddressPackArraySeparate m_animCmdAddressPack1;          // 0x443C
    soAnimCmdAddressPackArraySeparate m_animCmdAddressPack2;          // 0x4458
    soAnimCmdAddressPackArraySeparate m_animCmdAddressPack3;          // 0x4474
    soAnimCmdAddressPackArraySeparate m_animCmdAddressPack4;          // 0x4490
    soInstanceManagerFullPropertyUniqImpl<soAnimCmdControlUnit, 11> m_animCmdControlUnits; // 0x44AC
    soAnimCmdModuleImpl m_animCmdModule;                               // 0x45CC
    emKineticModuleImpl m_kineticModule;                               // 0x45F0
    soArrayVector<soArticle*, 10> m_articles;                          // 0x51C4
    soArrayVector<soArticleEventObserver, 10> m_articleEventObservers; // 0x51F8
    soGenerateArticleManageModuleImpl m_generateArticleManageModule;   // 0x52A4
    // HYPOTHESIS: four entries of an id (-1 when empty) and two words, cleared by the constructor.
    struct Unk52E0 {
        int m_id;
        int unk4;
        int unk8;
        Unk52E0() : m_id(-1), unk4(0), unk8(0) { }
    } unk52E0[4];                                                      // 0x52E0

public:
    // The storage of the modules whose capacity depends on the enemy is owned by the derived class: the posture
    // interpolations, the area instances and their work, the target search units, the AI entries, the article mediator, the
    // camera subjects and the reflect module.
    // HYPOTHESIS: parameter types; areaWork is stored by the enemy area module, wind is a shared null object of sora_melee.
    Enemy(u32 p1, emCreate* create, soArray<soInterpolation<Vec3f> >* interpolations, soArray<soAreaInstance>* areaInstances,
          void* areaWork, emTargetSearchUnit* targetSearchUnits, emAIEntry* aiEntries, soArticleMediator* articleMediator,
          void* wind, soArray<soCameraSubject>* cameraSubjects, soReflectModule* reflectModule);

    struct Kinetic {
        struct Energy {
            enum Id {
                Id_Jostle = 0x8,
                Id_Ground_Movement = 0x9,
            };
        };
    };

    enum MessageKind {
        Message_None = -1,
        Message_Damage = 1,
        Message_Destruct = 3,
        Message_Remove = 4,
    };

    static Enemy* create(u32 p1, emCreate* create);
    static void createPost(Enemy* enemy, emCreate* create);
    static void remove(Enemy* enemy);

    emAIModuleImpl& getAIModule() { return m_aiModule; }
    EnemyKind getEnemyKind() const { return m_enemyKind; }
    Vec3f getCenterPos();
    s32 getNameMsgId();
    nw4r::g3d::ResFileData* getFaceTexPtr(u32 p1);
    nw4r::g3d::ResFileData* getFaceTexPtr();
    int getExtendBrresResourceID();
    bool isCanAutoRemove();
    bool isCanCreateItem();

    virtual void processUpdate();
    virtual void processFixPosition();
    virtual void processHit();
    virtual void processGameProc();
    virtual void init();
    virtual ~Enemy();

    virtual void updatePosture(bool);
    virtual void processFixPositionPreAnimCmd();
    virtual soKind soGetKind();
    virtual int soGetSubKind();
    virtual void updateNodeSRT();
    virtual bool isTreadPassive();
    virtual bool isObserv(char unk1);
#if defined(FT_FIGHTER_ANIMCMD_LONG) || defined(YK_STAGE_FULL)
    // The override must follow the StageObject slot (long) so that it does not add a vtable slot in those units.
    virtual bool notifyEventAnimCmd(acAnimCmd* acmd, soModuleAccesser* moduleAccesser, s32 unk3);
#else
    virtual bool notifyEventAnimCmd(acAnimCmd* acmd, soModuleAccesser* moduleAccesser, int unk3);
#endif
    virtual void notifyEventLink(soLinkEventArgs *eventInfo, soModuleAccesser* moduleAccesser, StageObject*, int unk4);

    virtual float getScore();
    virtual void activate(Vec3f* spawnPos, float lr, float, int statusKind);
    virtual void deactivate();
    virtual bool isSetableNode(int);
    virtual void notifyEventChangeStatus(int statusKind, int prevStatusKind, soStatusData* statusData, soModuleAccesser* moduleAccesser);
    virtual void notifyEventCollisionAttack(float power, soCollisionLog* collisionLog, soModuleAccesser* moduleAccesser);
    virtual bool notifyEventCollisionAttackCheck(u32 flags);
    virtual void notifyEventCollisionSearch(soCollisionLog* collisionLog, soModuleAccesser* moduleAccesser);
    virtual bool notifyEventCollisionSearchCheck();
    // Parameter order of the shadowed soCollisionAbsorberEventObserver (include/so/collision), which this overrides.
    virtual void notifyEventCollisionAbsorber(soCollisionAttackModule* attackModule, float power, soCollisionLog* collisionLog, int groupIndex, float posX, float, soModuleAccesser* moduleAccesser);
    virtual bool notifyEventCollisionAbsorberCheck();
    virtual void notifyEventCollisionReflector(soCollisionAttackModule* attackModule, float power, soCollisionLog* collisionLog, int groupIndex, float posX, float, soModuleAccesser* moduleAccesser);
    virtual void notifyEventCollisionReflectorSearch(soCollisionSearchModule* searchModule, soCollisionLog* collisionLog, u32 groupIndex, soModuleAccesser* moduleAccesser);
    virtual bool notifyEventCollisionReflectorCheck();
    virtual bool notifyEventCaptureStatus(soModuleAccesser* moduleAccesser, int taskId, int, int);
    virtual bool emCreateFigure(int variation);
    virtual bool emShootItem(itKind, int variation, void*, float);
    virtual bool emItemInformationReturnEnemy(int instanceId);
    virtual bool shoot(int, int, int); // TODO: Verify parameters
    virtual bool addParts(int, int);

    template<typename T>
    static Enemy* createInstance(u32 p1, emCreate* create) {
        return new (Heaps::EnemyInstance) T(p1, create);
    }
};
static_assert(sizeof(Enemy) == 0x5310, "Class is wrong size!");

