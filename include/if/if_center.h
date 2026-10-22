#pragma once

// HYPOTHESIS: layout from the constructor, init, destroyModel and the state functions; names from the map (if_center.o).

#include <StaticAssert.h>
#include <types.h>

class MuObject;
class MuMsg;
namespace nw4r { namespace g3d { class ScnGroup; } }

class IfCenter {
public:
    u8 m_timeDisp : 1;  // 0x00 HYPOTHESIS: set by startMelee with a time mode
    u8 unk0_40 : 1;  // 0x00
    u8 unk0_20 : 1; // 0x00
    u8 unk0 : 5;
    u8 unk1[3];
    s32 m_state;                      // 0x04
    nw4r::g3d::ScnGroup* m_group;     // 0x08
    nw4r::g3d::ScnGroup* unkC; // 0x0C HYPOTHESIS: a second scene group
    MuObject* m_models[81];           // 0x10
    nw4r::g3d::ScnGroup* m_expands[4]; // 0x154 HYPOTHESIS: ScnMdlExpand objects (destroyed with G3dObj::Destroy)
    MuMsg* m_msg;                     // 0x164
    s32 unk168;                       // 0x168 HYPOTHESIS: BGM id of the title shown with Ready, -1 for none
    u8 unk16C;                        // 0x16C
    u8 unk16D[3];
    float m_updateRate;               // 0x170
    u8 unk174;                        // 0x174
    u8 unk175[3];
    s32 unk178;                       // 0x178
    s32 unk17C;                       // 0x17C
    u8 unk180[0x60];

    IfCenter();
    void init();
    virtual ~IfCenter();
    void setTimePos(int index);
    void removeCenterTime();
    void destroyModel();
    void setCenterState(int state);
    void removeAllCenterObj();

    // HYPOTHESIS: the state functions are static members that take the object (the state table holds plain
    // function pointers and main passes the object as the only argument); a main function returns the next state.
    static void centerReadyInit(IfCenter* center);
    static s32 centerReadyMain(IfCenter* center);
    static void centerCountReadyInit(IfCenter* center);
    static s32 centerCountReadyMain(IfCenter* center);
    static void centerGoInit(IfCenter* center);
    static s32 centerGoMain(IfCenter* center);
    static void centerTimeUpInit(IfCenter* center);
    static s32 centerTimeUpMain(IfCenter* center);
    static void centerCompleteInit(IfCenter* center);
    static s32 centerCompleteMain(IfCenter* center);
    static void centerSuddenDeathInit(IfCenter* center);
    static s32 centerSuddenDeathMain(IfCenter* center);
    static void centerGameSetInit(IfCenter* center);
    static s32 centerGameSetMain(IfCenter* center);
    static void centerFailureInit(IfCenter* center);
    static s32 centerFailureMain(IfCenter* center);
    static void centerSuccessInit(IfCenter* center);
    static s32 centerSuccessMain(IfCenter* center);
    static void centerGameStartInit(IfCenter* center);
    static s32 centerGameStartMain(IfCenter* center);
    static void centerGoalInit(IfCenter* center);
    static s32 centerGoalMain(IfCenter* center);

    void eraseCorps();
};
static_assert(sizeof(IfCenter) == 0x1E4, "Class is wrong size!");
