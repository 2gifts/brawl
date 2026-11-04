// Local shadow for native methods absent from BrawlHeaders.
#pragma once

#include <StaticAssert.h>
#include <cm/cm_controller_anm.h>
#include <cm/cm_quake.h>
#include <cm/cm_stage_param.h>
#include <gf/gf_task.h>
#include <types.h>

class cmSubject;

class CameraController : public gfTask {
public:
    enum Kind {
        None = 0x0,
        Animation = 0x1,
        AI = 0x2,
        Adventure = 0x3,
        AdventureFree = 0x4,
        AnimationLocked = 0x5,
        FighterTarget = 0x6,
        Photo = 0x7,
        MenuPad = 0x8,
        MenuFixed = 0x9,
        FigDisp1 = 0xa,
        Tech = 0xb,
        MeleeFixed = 0xc,
        Demo = 0xd,
        Default = 0xe
    };

    Kind m_currentControllerKind;
    union {
        char _68[4];
        struct {
            u8 unk44;
            u8 unk45;
            u8 unk46[2];
        };
    };
    gfCameraController* m_currentController;
    gfCameraController* m_defaultController;
    cmAnimationController* m_animationController;
    gfCameraController* m_fighterTargetController;
    gfCameraController* m_aiController;
    gfCameraController* m_meleeFixedController;
    gfCameraController* m_adventureController;
    gfCameraController* m_adventureFreeController;
    gfCameraController* m_photoController;
    gfCameraController* m_menuPadController;
    gfCameraController* m_menuFixedController;
    gfCameraController* m_figDisp1Controller;
    gfCameraController* m_techController;
    void* m_quake;
    gfCameraController* m_demoController;
    cmStageParam m_stageCameraParam; // 0x84
    cmStageParamPaused m_stageCameraParamPaused; // 0xC4
    cmStageParamFixed m_stageCameraParamFixed; // 0x110
    Rect2D m_stageCameraRange; // 0x128
    Rect2D m_stageDeadRange; // 0x138
    Rect2D m_stageCameraBounds; // 0x148 (HYPOTHESIS: left/right/up/down of the stage camera limit)
    float unk158;
    float unk15C;
    float unk160;
    float unk164;
    union {
        char unk168[36];
        struct {
            char unk168Pad[0x1C];
            float unk184;
            int unk188;
        };
    };
    float unk18C; // HYPOTHESIS: reset to 0 by the stage camera range setup (st_fzero)
    char unk190[20];

    virtual void processCamera();
    virtual void processDebug();
    virtual void renderDebug();
    virtual void init();
    virtual ~CameraController();

    gfCamera* getCameraInternal();
    CameraController();
    static CameraController* create();
    bool isForceEnableCamControll3D();
    unsigned int isSubjectSettingFlags();
    void changeInput(Kind cameraControllerKind);
    void setRangeOffset(Vec2f* offset);
    void setCameraRange(Rect2D*);
    void setDeadRange(Rect2D* range);
    void setStageParam(cmStageParam* param);
    gfCameraController* getCameraController(Kind kind);
    // HYPOTHESIS: clears the "this player may not be targeted" mark of the camera (mlFtEvent calls it on a new damage).
    void clearDisablePlayerNo(int playerNo);
    void setDisablePlayerNo(int playerNo);
    // HYPOTHESIS: reference qualifiers and float parameter names follow the start callee's register use.
    void startZoomIn(cmSubject* subject, u16 frame, const Vec2f& rotation, const Vec2f& targetRotation, float zoom, float distance);
    void setDisableZoomStart();
    void setDisableZoomEnd();
    bool isDisableZoom();
    static CameraController* getInstance();
};
static_assert(sizeof(CameraController) == 420, "Class is wrong size!");
