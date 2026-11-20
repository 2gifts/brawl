#include <gf/gf_camera.h>
#include <gf/gf_model.h>
#include <revolution/OS/OSError.h>
#include <sora_adv_stage/gr_adventure_door.h>
#include <sora_adv_stage/gr_adventure_gimmick_id.h>
#include <sora_adv_stage/st_adventure2.h>
#include <st/stage.h>
#include <st/st_trigger.h>
#include <types.h>

extern stAdventure2* lbl_40_bss_15C;

// stAdventure2 members of sora_adv_stage that have no names yet (the camera of the boss door, the camera position).
// TODO: clean up - name them.
extern "C" void fn_40_54F0(stAdventure2* stage, int unk);
extern "C" void fn_40_55BC(stAdventure2* stage, Vec3f* pos);

// The data of the boss door: the 40 triggers that open it one step each.
// HYPOTHESIS: the names; the rest of the data is the one of grAdventureDoor.
struct grGimmickDoorBossData {
    u8 m_unk0[0x90];
    stTriggerData m_triggers[40];       // 0x90
};

// The frame the door animation stopped at (the doors of the stage share it).
static float s_doorFrame;

// The door to a boss of the adventure mode: it opens a step for every trigger of its data that is set, and when all are
// set the camera moves into it.
class grAdventureDoorBoss : public grAdventureDoor {
public:
    u8 m_unk1A0[4];
    grGimmickDoorBossData* m_bossData;  // 0x1A4
    float m_speedX;                     // 0x1A8 how fast the camera moves to the door
    float m_speedZ;                     // 0x1AC
    u8 m_isAnimating;                   // 0x1B0 the door animation runs to the next frame
    u8 m_isAllSet;                      // 0x1B1 every trigger is set
    float m_nextFrame;                  // 0x1B4 the frame of the door animation for the triggers that are set
    Vec3f m_cameraPos;                  // 0x1B8

    grAdventureDoorBoss(const char* taskName) : grAdventureDoor(taskName) {
        m_bossData = NULL;
        m_speedX = 0.2f;
        m_speedZ = 0.2f;
        m_isAnimating = 0;
        m_isAllSet = 0;
        m_nextFrame = 0.0f;
        m_transparencySettings = 0;
    }
    static grAdventureDoorBoss* create(int mdlIndex, u32 jumpData, const char* taskName);
    virtual ~grAdventureDoorBoss();
    virtual void startup(gfArchive* data, u32 unk1, gfSceneRoot::LayerType layerType);
    virtual void processAnim();
    virtual void setInitializeFlag();
    virtual void update(float deltaFrame);
    virtual void openTheDoor();
    void getNextDoorFrame();
};
static_assert(sizeof(grAdventureDoorBoss) == 0x1C4, "Class is wrong size!");

grAdventureDoorBoss::~grAdventureDoorBoss() { }

grAdventureDoorBoss* grAdventureDoorBoss::create(int mdlIndex, u32 jumpData, const char* taskName) {
    grAdventureDoorBoss* door = new (Heaps::StageInstance) grAdventureDoorBoss(taskName);
    if (door != NULL) {
        door->setJumpData(jumpData);
        door->setMdlIndex(mdlIndex);
    }
    return door;
}

void grAdventureDoorBoss::startup(gfArchive* data, u32 unk1, gfSceneRoot::LayerType layerType) {
    grAdventureDoor::startup(data, unk1, layerType);
    m_bossData = static_cast<grGimmickDoorBossData*>(m_gimmickData);
    OSReport("modelNum = %d\n", m_resFile.GetResMdlNumEntries());
    changeNodeAnim(0, 0);
    changeTexSrtAnim(0, 0);
    changeVisibleAnim(0, 0);
    m_modelAnims[0]->setUpdateRate(0.0f);
    changeNodeAnim(1, 1);
    changeTexSrtAnim(1, 1);
    m_modelAnims[1]->m_anmObjChrRes->SetUpdateRate(0.0f);
    float rate = g_stGameFrame.m_frameDelta;
    m_modelAnims[1]->m_anmObjTexSrtRes->SetUpdateRate(rate);
    unk15C = 180.0f;
    for (int i = 0; i < 40; i++) {
        g_stTriggerMng->createTrigger(static_cast<Gimmick::AreaKind>(0x28), &m_bossData->m_triggers[i]);
    }
    disableArea();
}

void grAdventureDoorBoss::processAnim() {
    if (m_resFile.ptr() != NULL) {
        u32 i = 0;
        u32 numModels = m_resFile.GetResMdlNumEntries();
        // TODO: clean up - the rate of Ground::processAnim is computed and not used (only the compare is left).
        float rate = g_stGameFrame.m_frameDelta;
        if (m_isPauseAnim == true) {
            rate = 0.0f;
        }
        for (; i != numModels; i++) {
            if (m_noUpdateAnim == true) {
                m_sceneModels[i]->G3dProc(8, 0, 0);
            } else {
                reinterpret_cast<nw4r::g3d::ScnMdlSimple*>(m_sceneModels[i])->UpdateFrame();
            }
        }
        if (m_collision != NULL) {
            updateG3dProcCalcWorld();
        }
        m_hasUpdatedG3dCalcWorld = false;
    }
}

void grAdventureDoorBoss::setInitializeFlag() {
    grAdventureDoor::setInitializeFlag();
    float frame = s_doorFrame;
    m_modelAnims[0]->setFrame(frame);
    m_modelAnims[1]->m_anmObjChrRes->SetFrame(frame);
}

void grAdventureDoorBoss::update(float deltaFrame) {
    grAdventureDoor::update(deltaFrame);
    if (unk150State == 1) {
        float absX = static_cast<float>(__fabs(m_cameraPos.m_x));
        if (absX < m_speedX) {
            m_cameraPos.m_x = 0.0f;
        } else if (m_cameraPos.m_x > m_speedX) {
            m_cameraPos.m_x -= m_speedX;
        } else {
            m_cameraPos.m_x += m_speedX;
        }
        if (static_cast<float>(__fabs(m_cameraPos.m_y)) - 20.0f < m_speedX) {
            m_cameraPos.m_y = 20.0f;
        } else if (m_cameraPos.m_y > m_speedX) {
            m_cameraPos.m_y -= m_speedX;
        } else {
            m_cameraPos.m_y += m_speedX;
        }
        m_cameraPos.m_z -= m_speedZ;
        fn_40_55BC(lbl_40_bss_15C, &m_cameraPos);
        m_speedX += 0.01f;
        m_speedZ += 0.02f;
    }
    getNextDoorFrame();
    if (m_nextFrame > s_doorFrame && !m_isAnimating) {
        m_modelAnims[0]->setUpdateRate(g_stGameFrame.m_frameDelta);
        m_modelAnims[1]->m_anmObjChrRes->SetUpdateRate(g_stGameFrame.m_frameDelta);
        m_isAnimating = 1;
    }
    if (m_isAnimating == 1) {
        s_doorFrame = m_modelAnims[0]->getFrame();
        if (m_nextFrame <= s_doorFrame) {
            m_modelAnims[0]->setUpdateRate(0.0f);
            m_modelAnims[1]->m_anmObjChrRes->SetUpdateRate(0.0f);
            float frame = m_nextFrame;
            s_doorFrame = frame;
            m_modelAnims[0]->setFrame(frame);
            m_modelAnims[1]->m_anmObjChrRes->SetFrame(frame);
            m_isAnimating = 0;
        }
    }
}

void grAdventureDoorBoss::getNextDoorFrame() {
    m_isAllSet = 1;
    m_nextFrame = 0.0f;
    for (int i = 0; i < 40; i++) {
        if (!g_stTriggerMng->getTriggerFlag(&m_bossData->m_triggers[i], true)) {
            m_isAllSet = 0;
            break;
        }
    }
    if (m_isAllSet == 1) {
        m_nextFrame = 800.0f;
        enableArea();
    } else {
        for (int i = 0; i < 40; i++) {
            if (g_stTriggerMng->getTriggerFlag(&m_bossData->m_triggers[i], false) == true) {
                m_nextFrame += 20.0f;
            }
        }
    }
}

void grAdventureDoorBoss::openTheDoor() {
    fn_40_54F0(lbl_40_bss_15C, 0);
    gfCamera* camera = gfCameraManager::getManager()->getCamera(0);
    m_cameraPos.m_x = camera->m_targetPos.m_x;
    m_cameraPos.m_y = camera->m_targetPos.m_y;
    m_cameraPos.m_z = camera->m_targetPos.m_z;
    EachDoorTypeEffect();
}
