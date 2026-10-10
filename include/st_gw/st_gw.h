#pragma once

#include <StaticAssert.h>
#include <gf/gf_archive.h>
#include <gm/gm_lib.h>
#include <gr/gr_gimmick.h>
#include <gr/gr_gimmick_spring.h>
#include <memory.h>
#include <mt/mt_vector.h>
#include <st/st_class_info.h>
#include <st/st_data_container.h>
#include <st/st_melee.h>
#include <types.h>

#include <st_gw/gr_gw_scene_oil.h>

template<typename T>
class stClassInfoImpl<Stages::GW, T> : public stClassInfo {
public:
    stClassInfoImpl() : stClassInfo() {
        setClassInfo(Stages::GW, this);
    }

    virtual ~stClassInfoImpl() {
        setClassInfo(Stages::GW, 0);
    }

    virtual T* create() {
        return T::create();
    }

    virtual void preload() { }
};

// Flat Zone 2 (Game & Watch): the stage plays one of four scenes (the fire, the chef, the lion and the oil panic). Every
// scene is a model with its own set of grounds (the people, the fire, the pans...); the stage hands them the shared work
// (the places, the steps of the sequence, the events) and picks the next scene when the timer of the scene is over.
class stGW : public stMelee {
    u8 m_sceneState;                // 0x1D8 0 = start, 1 = wait for the next scene, 2 = wait for the transition, 3 / 4 = the scene plays
    char _1d9[3];
    float m_sceneTimer;             // 0x1DC frames until the next step of the scene state
    u8 m_scene;                     // 0x1E0 the scene that plays (0 fire, 1 chef, 2 lion, 3 oil)
    u8 m_sceneQueue[3];             // 0x1E1 the next scenes (4 = not picked)
    u8 m_event;                     // 0x1E4 1 = a scene may be set up
    char _1e5[3];
    u32 m_unk1E8;                   // 0x1E8
    Vec3f m_limit[2];               // 0x1EC the camera limits (lower left, upper right corner)
    stCollisionWork m_collWork;     // 0x204 the collision of the steps of the fire scene
    Vec3f m_posGimmick[20];         // 0x214 the places that the grounds publish (the steps, the fire...)
    u8 m_ctrlStep;                  // 0x304 the control of the steps of the fire scene
    u8 m_stepManhore;               // 0x305 the step of the fire
    u8 m_typeManhore;               // 0x306 the type of the fire that comes next (0x10 = none)
    char _307[5];
    u8 m_stateMistake[3];           // 0x30C the state of the three mistakes
    u8 m_indexVictim;               // 0x30F which victim the fire man looks for
    char _310[4];
    Vec3f m_posFighter;             // 0x314 where the fireman is
    u8 m_indexFighter;              // 0x320 the fireman that saves a victim
    char _321[3];
    Vec3f m_posDiver[3];            // 0x324 the places of the divers
    u8 m_typeDiver;                 // 0x348 the diver that jumps (0x10 = none)
    char _349[3];
    Vec3f m_posKeeperL[3];          // 0x34C the places of the keepers of the left side
    Vec3f m_posKeeperR[3];          // 0x370 the places of the keepers of the right side
    grGWGuestData m_dataGuest[2];   // 0x394 the places of the guests of the oil scene (man, woman)
    stDataMultiContainer* m_tblStep; // 0x45C the table of the steps of the scenes
    char _460[4];
    grGimmickSpringData* m_springData; // 0x464 the trampoline of the fire scene

public:
    stGW();
    static stGW* create();

    virtual ~stGW();
    virtual bool loading();
    virtual void createObj();
    virtual void update(float deltaFrame);
    virtual bool isBamperVector() { return true; }
    virtual void createObjBg(int index);
    virtual void createObjScene(int index);
    virtual void createObjAlarm(int index);
    virtual void createObjChef();
    virtual void createObjChefEtc(int index);
    virtual void createObjFire();
    virtual void createObjFireSub();
    virtual void createObjFireStep();
    virtual void createObjFireManhore(int index);
    virtual void createObjFireSmoke(int index);
    virtual void createObjFireEtc(int index);
    virtual void createObjFireFighter(int index);
    virtual void createObjFireMistake(int index);
    virtual void createObjFireSpring();
    virtual void createObjFireDiver(int index);
    virtual void createObjLion();
    virtual void createObjLionKeeper(int index);
    virtual void createObjOil();
    virtual void createObjOilGuest(int index);
    virtual void updateLimit(float deltaFrame);
    virtual void updateScene(float deltaFrame);
    virtual void changeScene(u32 scene);
    virtual void selectScene();
    virtual void initStageDataTbl();

    void initCollDatafromCollWorkStep(grCollData* data, stCollisionWork* work, Ground* ground);

    static stClassInfoImpl<Stages::GW, stGW> bss_loc_14;
};
static_assert(sizeof(stGW) == 0x468, "Class is wrong size!");
