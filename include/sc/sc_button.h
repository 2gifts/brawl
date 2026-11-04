#pragma once

#include <StaticAssert.h>
#include <sc/sc_melee.h>
#include <types.h>

class MuObject;
class muButtonTask;
class muObjProcessTask;
namespace nw4r {
namespace g3d {
class ResAnmScn;
class ScnObj;
} // namespace g3d
} // namespace nw4r

// The scene of the button settings ("Controls" in the options): a melee scene (the player can try the buttons in a
// practice fight) with the button task on top.
class scButton : public scMelee {
public:
    // What scButton::start makes: the object process of the menu, the button task and the state of the practice.
    struct Work {
        muObjProcessTask* m_objProcess; // 0x00
        muButtonTask* m_task;      // 0x04
        u8 m_isPractice;           // 0x08 HYPOTHESIS: the task is in its practice step (process compares it)
        u8 m_unk9;                 // 0x09 HYPOTHESIS: the screen fill of start is still to be released
        u8 m_fill;                 // 0x0A HYPOTHESIS: from the result of efScreen::requestFill (start keeps its top byte)
        int m_contNo;              // 0x0C the controller of the practice fighter
    };

    int m_padNo;                   // 0x100 the pad of the player who set the buttons
    Work* m_work;                  // 0x104

    // Gives the object process of the menu the objects it updates.
    void setMenuObjs(MuObject** objs, int num);
    // Shows / hides a model (or a group) of the menu (in the menu layer of the scene root).
    void addObj(nw4r::g3d::ScnObj* obj);
    void removeObj(nw4r::g3d::ScnObj* obj);
    void removeAllObj();
    // The camera animation of the menu scene.
    void setCamAnm(nw4r::g3d::ResAnmScn* anm);
    void updateFrame();
    // Gives the practice fighter a pad (false while the fight cannot take it yet).
    bool setContNo(int contNo);
};
static_assert(sizeof(scButton) == 0x108, "Class is wrong size!");
