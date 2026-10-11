#pragma once

#include <StaticAssert.h>
#include <gf/gf_file_io_handle.h>
#include <gf/gf_task.h>
#include <memory.h>
#include <mu/mu_menu_controller.h>
#include <mu/mu_msg.h>
#include <mu/mu_object.h>
#include <nw4r/g3d/g3d_obj.h>
#include <nw4r/g3d/g3d_resfile.h>
#include <types.h>

// What the scene gives to the difficulty task.
struct muAdvDifficultyTaskParam {
    int m_difficulty;       // 0x00 the difficulty that the cursor starts on
    int m_pad0;             // 0x04 the pad of the first player
    int m_pad1;             // 0x08 the pad of the second player (-1: there is none)
};

// The adventure mode menu that asks for the difficulty (five levels): the player moves the cursor with up and down
// and decides with A (the level is taken) or B (back).
class muAdvDifficultyTask : public gfTask {
public:
    enum Step {
        Step_Load = 0,
        Step_Appear = 1,
        Step_Select = 2,
        Step_Selected = 3,
        Step_Count = 4,
    };

    // One step of the menu: what runs when the step starts, and what runs every frame (it returns the next step).
    struct StepFunc {
        void (*m_init)(muAdvDifficultyTask* task);
        int (*m_main)(muAdvDifficultyTask* task);
    };

    // The data of one player. HYPOTHESIS: the controller is the first field; the module never uses the array.
    struct PlayerWork {
        muMenuController m_controller;
        int m_unkA8;
    };

    int m_result;                               // 0x40 0: nothing yet, 1: a difficulty was taken, 2: the player went back
    int m_pad0;                                 // 0x44
    int m_pad1;                                 // 0x48
    int m_difficulty;                           // 0x4C
    int m_inputWait;                            // 0x50 frames in which the pads are not read
    muMenuController m_controllers[2];          // 0x54
    MuMsg* m_msg;                               // 0x1A4 the texts of the levels
    u8 m_unk1A8[8];
    MuObject* m_objects[2];                     // 0x1B0 the levels, the cursor
    nw4r::g3d::G3dObj* m_scnMdls[2];            // 0x1B8
    nw4r::g3d::ResFile m_resFile;               // 0x1C0
    int m_unk1C4;
    int m_step;                                 // 0x1C8
    int m_stepPrev;                             // 0x1CC
    PlayerWork m_players[2];                    // 0x1D0
    u8 m_unk328[0x14];
    bool m_isSelected;                          // 0x33C A was pressed (B otherwise)
    gfFileIOHandle m_handles[3];                // 0x340 the model, the camera, the texts
    bool m_loaded[3];                           // 0x34C

    muAdvDifficultyTask();
    static muAdvDifficultyTask* create(const muAdvDifficultyTaskParam* param);

    virtual void processDefault();
    virtual ~muAdvDifficultyTask();

    void controllProc();
    void createObjResFile(const struct muAdvDifficultyModel* models, int count, nw4r::g3d::ResFile* resFile);

    static void mainStepLoadInit(muAdvDifficultyTask* task);
    static int mainStepLoadMain(muAdvDifficultyTask* task);
    static void mainStepAppearInit(muAdvDifficultyTask* task);
    static int mainStepAppearMain(muAdvDifficultyTask* task);
    static void mainStepSelectInit(muAdvDifficultyTask* task);
    static int mainStepSelectMain(muAdvDifficultyTask* task);
    static void mainStepSelectedInit(muAdvDifficultyTask* task);
    static int mainStepSelectedMain(muAdvDifficultyTask* task);
};
static_assert(sizeof(muAdvDifficultyTask) == 0x350, "Class is wrong size!");
