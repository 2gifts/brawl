#pragma once

// HYPOTHESIS: layout from the constructor, init and the menu functions; names from the map (if_minigame_training.o).

#include <StaticAssert.h>
#include <mu/mu_menu_controller.h>
#include <types.h>

class MuObject;
class MuMsg;
namespace nw4r { namespace g3d { class ResFile; class ScnGroup; } }

class IfMinigameTraining {
public:
    // HYPOTHESIS: the names of the camera setups of setCameraType (0 match camera, 1 menu open, 2 the camera option).
    enum CameraType {
        CameraType_Normal = 0,
        CameraType_Menu = 1,
        CameraType_Follow = 2,
    };
    // HYPOTHESIS: the menu lines that show the status display as on and off.
    enum StatusVisibleMsg {
        StatusVisibleMsg_On = 0xF,
        StatusVisibleMsg_Off = 0x10,
    };

    nw4r::g3d::ScnGroup* m_group;     // 0x00
    MuObject* m_models[2];            // 0x04
    MuMsg* m_msg;                     // 0x0C
    void* m_itemMsgData;              // 0x10 HYPOTHESIS: message data of the item names
    s32 m_cursor;                     // 0x14
    s32 m_speed;                      // 0x18
    s32 m_item;                       // 0x1C
    s32 m_numCP;                      // 0x20
    s32 m_cpAI;                       // 0x24
    s32 m_cpDamage;                   // 0x28
    s32 m_camera;                     // 0x2C
    bool m_statusVisible;             // 0x30
    u8 unk31[3];
    muMenuController m_controller;    // 0x34
    s32 m_step;                       // 0xDC
    s32 m_damage;                     // 0xE0
    s32 m_combo;                      // 0xE4
    s32 m_totalDamage;                // 0xE8
    s32 m_itemTbl[128];               // 0xEC
    s32 m_numItems;                   // 0x2EC
    s32 m_generateItemKind;           // 0x2F0
    bool m_quit;                      // 0x2F4
    u8 unk2F5[3];
    s32 unk2F8;                       // 0x2F8 HYPOTHESIS: the player the demo camera follows (openMenu)
    bool unk2FC;                      // 0x2FC HYPOTHESIS: closeMenu keeps the demo camera when set

    IfMinigameTraining();
    void init();
    virtual ~IfMinigameTraining();
    // HYPOTHESIS: parameter types below come from register use and the IfMngr caller; the map has the names only.
    void createModel(nw4r::g3d::ResFile* resFile, void* msgData, void* itemMsgData, u8* itemTblData, u32 itemTblSize);
    void createObjResFile(const void* nodeInfo, s32 nodeCount, nw4r::g3d::ResFile* resFile, s32 prio);
    void initMsg(void* msgData);
    void startMelee(nw4r::g3d::ScnGroup* group);
    void printStatusDamage(int damage);
    void printStatusCombo(int combo);
    void printStatusTotalDamage(int damage);
    void openMenu(int padNo, int playerNo);
    void setCameraType(int type);
    void closeMenu();
    void main();

    // HYPOTHESIS: the step and cursor functions are static members that take the object (the tables hold plain
    // function pointers and the callers pass the object as the only argument); a main function returns the next step.
    static void initStepAppear(IfMinigameTraining* training);
    static s32 mainStepAppear(IfMinigameTraining* training);
    static void initStepMenu(IfMinigameTraining* training);
    static s32 mainStepMenu(IfMinigameTraining* training);
    static void initStepLeave(IfMinigameTraining* training);
    static s32 mainStepLeave(IfMinigameTraining* training);
    static void curPosProcSpeed(IfMinigameTraining* training);
    static void curPosProcItem(IfMinigameTraining* training);
    static void curPosProcNumCP(IfMinigameTraining* training);
    static void curPosProcCPAI(IfMinigameTraining* training);
    static void curPosProcCPDamage(IfMinigameTraining* training);
    static void curPosProcCamera(IfMinigameTraining* training);
    static void curPosProcStatusVisible(IfMinigameTraining* training);
    static void curPosProcQuit(IfMinigameTraining* training);
};
static_assert(sizeof(IfMinigameTraining) == 0x304, "Class is wrong size!");
