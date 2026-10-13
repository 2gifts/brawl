#pragma once

#include <StaticAssert.h>
#include <gf/gf_archive.h>
#include <gf/gf_heap_manager.h>
#include <gf/gf_task.h>
#include <memory.h>
#include <mu/menu.h>
#include <mu/mu_menu_main.h>
#include <mu/mu_object.h>
#include <mu/mu_panel_ctrl.h>
#include <mu/mu_proc_menu.h>
#include <mu/mu_proc_rule1.h>
#include <mu/mu_proc_rule2.h>
#include <nw4r/g3d/g3d_obj.h>
#include <nw4r/g3d/g3d_resfile.h>
#include <types.h>

// The screen where the player picks the items that can appear.
class muProcItemSwitch : public muProcMenu {
public:
    u8 m_unk66C[0x690 - 0x66C];

    muProcItemSwitch(MuObject** objs, nw4r::g3d::G3dObj** mdls, void* data);
    virtual ~muProcItemSwitch();
    virtual void init(nw4r::g3d::ScnGroup* group, int from);
    virtual int proc();
    virtual void exit();
    virtual bool selectProc(int dir);
    virtual void playSE_CursorMove();
};
static_assert(sizeof(muProcItemSwitch) == 0x690, "Class is wrong size!");

// The screen where the player picks the stages that can be played.
class muProcStageSwitch : public muProcMenu {
public:
    u8 m_unk66C[0x768 - 0x66C];

    muProcStageSwitch(MuObject** objs, nw4r::g3d::G3dObj** mdls, void* data);
    virtual ~muProcStageSwitch();
    virtual void init(nw4r::g3d::ScnGroup* group, int from);
    virtual int proc();
    virtual void exit();
    virtual bool selectProc(int dir);
    virtual void playSE_CursorMove();
};
static_assert(sizeof(muProcStageSwitch) == 0x768, "Class is wrong size!");

// The task of the menu of the rules.
class muRuleTask : public gfTask {
public:
    enum Proc {
        Proc_Rule1 = 0,
        Proc_Rule2 = 1,
        Proc_ItemSwitch = 2,
        Proc_StageSwitch = 3,
        Proc_Count = 4,
        Proc_None = 5,
    };

    nw4r::g3d::ResFile m_resFile;               // 0x40 the models of the menu
    muObjProcessTask* m_objProc;                // 0x44 updates the objects
    MuObject* m_objs[0x82];                     // 0x48
    nw4r::g3d::G3dObj* m_mdls[6];               // 0x250 the models that the objects are put in
    muProcMenu* m_procs[Proc_Count];            // 0x268
    int m_curProc;                              // 0x278 the proc that runs (Proc_None: none)
    int m_prevProc;                             // 0x27C the proc that is leaving
    nw4r::g3d::ScnGroup* m_group;               // 0x280 the group of the scene the objects are put in
    int m_timer;                                // 0x284 the frames since the menu started
    bool m_end;                                 // 0x288 the menu has finished
    int m_mode;                                 // 0x28C how the scene started the menu (0 ... 6)
    muPanelCtrlTask* m_panel;                   // 0x290
    void* m_msgData;                            // 0x294
    bool m_startIsCancel;                       // 0x298 the Start button works as the Cancel button

    muRuleTask(nw4r::g3d::ScnGroup* group, gfArchive* archive, muPanelCtrlTask* panel, int mode, int priority);
    virtual void processAnim();
    virtual ~muRuleTask();

    bool isProcessEnd();
    void removeObj();
    void setCtrlFlg(u8 mask);
    void setStartButtonIsCancel(bool value);
};
static_assert(sizeof(muRuleTask) == 0x29C, "Class is wrong size!");
