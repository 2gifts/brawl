#pragma once

// SHADOW of BrawlHeaders em/enemy.h (-I include comes first): adds the constructor and destructor the enemy units call,
// the type of the storage the derived enemies hand to it, and lets the derived enemies read the members.

#include <StaticAssert.h>
#include <em/em_create.h>
#include <em/em_ai_module_impl.h>
#include <gr/gr_gimmick_motion_path.h>
#include <memory.h>
#include <mt/mt_vector.h>
#include <so/stageobject.h>
#include <types.h>

#include <so/area/so_area_instance.h>
#include <so/camera/so_camera_subject.h>
#include <so/posture/so_posture_module_impl.h>

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
    EnemyKind m_enemyKind; // 0xAC
    u8 unkB0[0x4];
    int m_connectedTriggerId;
    u32 m_difficulty;
    u32 m_level;
    u8 unkC0[0x8];
    grGimmickMotionPath* m_motionPath;
    u8 unkCC[0x10];
    soModuleAccesser moduleAccesser;
    u8 unk1BC[0xF4C];
    emAIModuleImpl m_aiModule; // 0x1108
    u8 unk110C[0x5310 - 0x1108 - sizeof(emAIModuleImpl)];

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

    emAIModuleImpl& getAIModule() { return m_aiModule; }
    EnemyKind getEnemyKind() const { return m_enemyKind; }
    Vec3f getCenterPos();
    s32 getNameMsgId();
    nw4r::g3d::ResFileData* getFaceTexPtr(u32 p1);
    nw4r::g3d::ResFileData* getFaceTexPtr();
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

