#pragma once

#include <StaticAssert.h>
#include <gf/gf_archive.h>
#include <gf/gf_scene.h>
#include <gf/gf_task.h>
#include <memory.h>
#include <mu/mu_msg.h>
#include <mu/mu_object.h>
#include <nw4r/g3d/g3d_resfile.h>
#include <types.h>

// The scene that shows the game over screen (module sora_scene; only what this menu needs is declared here).
// HYPOTHESIS: the class hierarchy gfScene <- muMenu <- scGameover follows from the run time type information of the module.
class muMenu : public gfScene {
public:
    muMenu(const char* name) : gfScene(name) { }
    static int loadMenuSound();
    static bool isLoadFinishMenuSound();
};
class scGameover : public muMenu {
public:
    scGameover(const char* name) : muMenu(name) { }
    nw4r::g3d::ScnGroup* getScnRoot();
};

// What the scene gives to the game over task.
struct muGameOverParam {
    int m_score;                    // 0x00 the points of the player
    int m_scoreAfter;               // 0x04 the points after a continue
    int m_coins;                    // 0x08 the coins of the player (shown divided by 10)
    int m_cost;                     // 0x0C what a continue costs (in coins, shown divided by 10)
    u8 m_noContinue;                // 0x10 the player cannot continue
    int m_unk14;                    // 0x14 more than 0 shows an extra message line
};

// The game over screen: "Continue?" with the cost in coins; when the player continues the numbers count to their new
// values and the game goes on, when the player gives up the "Game Over" text appears and a button leaves the screen.
class muGameOverTask : public gfTask {
public:
    enum Step {
        Step_Loading = 0,
        Step_Appear = 1,
        Step_SelectContinue = 2,
        Step_Continue = 3,
        Step_GameOver = 4,
        Step_GameOverStart = 5,
        Step_GameOverButton = 6,
        Step_Count = 7,
    };

    // One step of the screen: what runs when the step starts, and what runs every frame (it returns the next step).
    struct StepFunc {
        void (*m_init)(muGameOverTask* task);
        int (*m_main)(muGameOverTask* task);
    };

    MuObject* m_object;                 // 0x40 the model of the screen
    int m_step;                         // 0x44
    int m_stepPrev;                     // 0x48 the step whose init function ran last
    nw4r::g3d::ResFile m_resFile;       // 0x4C
    MuMsg* m_msg;                       // 0x50 the texts
    void* m_msgData;                    // 0x54
    int m_score;                        // 0x58
    int m_scoreAfter;                   // 0x5C
    int m_coins;                        // 0x60
    int m_cost;                         // 0x64
    u8 m_noContinue;                    // 0x68
    int m_unk6C;                        // 0x6C
    // The data of a muMenuController. HYPOTHESIS: the original does not run its constructor or destructor in this class.
    u8 m_controller[0xA8];              // 0x70
    int m_coinsAfter;                   // 0x118
    int m_coinStep;                     // 0x11C
    int m_scoreStep;                    // 0x120
    u8 m_isContinue;                    // 0x124 the "Continue" entry is selected
    int m_endWait;                      // 0x128
    gfArchive* m_archive;               // 0x12C

    muGameOverTask();
    static muGameOverTask* create(const muGameOverParam* param, gfArchive* archive);

    virtual void processDefault();
    virtual ~muGameOverTask();

    void createObjResFile(const struct muGameOverModel* models, int count, nw4r::g3d::ResFile* resFile);
    void initMsg(void* msgData);
    void setGameOverObjAction(int action);

    static void mainStepLoadingInit(muGameOverTask* task);
    static int mainStepLoadingMain(muGameOverTask* task);
    static void mainStepAppearInit(muGameOverTask* task);
    static int mainStepAppearMain(muGameOverTask* task);
    static void mainStepSelectContinueInit(muGameOverTask* task);
    static int mainStepSelectContinueMain(muGameOverTask* task);
    static void mainStepContinueInit(muGameOverTask* task);
    static int mainStepContinueMain(muGameOverTask* task);
    static void mainStepGameOverInit(muGameOverTask* task);
    static int mainStepGameOverMain(muGameOverTask* task);
    static void mainStepGameOverStartInit(muGameOverTask* task);
    static int mainStepGameOverStartMain(muGameOverTask* task);
    static void mainStepGameOverButtonInit(muGameOverTask* task);
    static int mainStepGameOverButtonMain(muGameOverTask* task);
};
static_assert(sizeof(muGameOverTask) == 0x130, "Class is wrong size!");
