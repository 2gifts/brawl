#pragma once

#include <StaticAssert.h>
#include <mu/mu_proc_menu.h>
#include <nw4r/g3d/g3d_obj.h>
#include <nw4r/g3d/g3d_scngroup.h>
#include <types.h>

// The first screen of the menu of the rules: the rule (time, stock or coin), the time, the stock, the handicap, the damage
// ratio, the stage choice and the two buttons that open the screens of the items and of the stages.
// HYPOTHESIS: the names follow how the menu uses the fields; the values are the ones of the setting of the rules of the
// game (GameGlobal::m_setRule) as the menu shows them.
class muProcRule1 : public muProcMenu {
public:
    // The cursor goes through 7 entries: the numbers of the first 5 are the ones of the entries of m_values.
    enum Entry {
        Entry_Rule = 0,
        Entry_Time = 1,
        Entry_Handicap = 2,
        Entry_Damage = 3,
        Entry_Stage = 4,
        Entry_ButtonA = 5,
        Entry_ButtonB = 6,
        Entry_Count = 7,
    };

    union {
        u8 m_values[8];                 // 0x66C the value that each entry shows
        struct {
            u8 m_rule;                  // 0x66C 0: time, 1: stock, 2: coin
            u8 m_time;                  // 0x66D the time in minutes (0: no limit)
            u8 m_handicap;              // 0x66E 0: off, 1: auto, 2: on (the order of the icons)
            u8 m_damage;                // 0x66F the damage ratio, multiplied by 10
            u8 m_stage;                 // 0x670 the choice of the stage
            u8 m_side;                  // 0x671 which of the two buttons the cursor goes to
            u8 m_unk672;                // 0x672
            u8 m_stock;                 // 0x673 the stock
        };
    };
    u8 m_unk674[4];                     // 0x674
    float m_frames[8];                  // 0x678 the frame where the animation of the icon of an entry stops
    int m_priority;                     // 0x698 the draw priority of the screen
    int m_mode;                         // 0x69C how the scene started the menu (0 ... 6)
    u8 m_enabled[Entry_Count];          // 0x6A0 the entries that the cursor can go to
    u8 m_hideAdditionalRule;            // 0x6A7
    u8 m_dirty;                         // 0x6A8 the animations of the entries have to be set again
    u8 m_startIsCancel;                 // 0x6A9 the Start button works as the Cancel button
    int m_repeat;                       // 0x6AC the frames the direction has been held

    muProcRule1(MuObject** objs, nw4r::g3d::G3dObj** mdls, int mode, int priority);
    virtual ~muProcRule1() { }
    virtual void init(nw4r::g3d::ScnGroup* group, int from);
    virtual int proc();
    virtual void exit();
    virtual bool selectProc(int dir);
    virtual void playSE_CursorMove();

    void setMessage();
    void setRule();
};
static_assert(sizeof(muProcRule1) == 0x6B0, "Class is wrong size!");
