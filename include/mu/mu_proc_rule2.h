#pragma once

#include <StaticAssert.h>
#include <mu/mu_proc_menu.h>
#include <nw4r/g3d/g3d_obj.h>
#include <nw4r/g3d/g3d_scngroup.h>
#include <types.h>

// The second screen of the menu of the rules: the stock time limit, and the four switches (team attack, pause, score
// display and damage gauge), and the button that opens the screen of the stages.
// HYPOTHESIS: the names follow the setting of the rules of the game that the screen changes (GameGlobal::m_setRule).
class muProcRule2 : public muProcMenu {
public:
    enum Entry {
        Entry_StockTime = 0,
        Entry_TeamAttack = 1,
        Entry_Pause = 2,
        Entry_ScoreDisplay = 3,
        Entry_DamageGauge = 4,
        Entry_Button = 5,
        Entry_Count = 6,
    };

    union {
        u8 m_values[5];                 // 0x66C the value that each entry shows
        struct {
            u8 m_stockTime;             // 0x66C the time in minutes (0: no limit)
            u8 m_teamAttack;            // 0x66D
            u8 m_pause;                 // 0x66E
            u8 m_scoreDisplay;          // 0x66F
            u8 m_damageGauge;           // 0x670
        };
    };
    u8 m_unk671[7];                     // 0x671
    float m_frames[5];                  // 0x678 the frame where the animation of the icon of a switch stops
    int m_priority;                     // 0x68C the draw priority of the screen
    int m_mode;                         // 0x690 how the scene started the menu (0 ... 6)
    u8 m_enabled[Entry_Count];          // 0x694 the entries that the cursor can go to
    u8 m_hideRandomStage;               // 0x69A
    u8 m_startIsCancel;                 // 0x69B the Start button works as the Cancel button
    int m_repeat;                       // 0x69C the frames the direction has been held

    muProcRule2(MuObject** objs, nw4r::g3d::G3dObj** mdls, int mode, int priority);
    virtual ~muProcRule2() { }
    virtual void init(nw4r::g3d::ScnGroup* group, int from);
    virtual int proc();
    virtual void exit();
    virtual bool selectProc(int dir);
    virtual void playSE_CursorMove();

    // Whether the cursor can stay on its entry (the button is not shown when the random stage switch is hidden).
    bool canSelect() const {
        if (m_hideRandomStage == 0 && m_cursor == Entry_Button) {
            return false;
        }
        return m_enabled[m_cursor];
    }
    void setMessage();
    void setRule();
};
static_assert(sizeof(muProcRule2) == 0x6A0, "Class is wrong size!");
