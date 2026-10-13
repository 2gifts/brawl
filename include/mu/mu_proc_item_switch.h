#pragma once

#include <StaticAssert.h>
#include <mu/mu_msg.h>
#include <mu/mu_proc_menu.h>
#include <nw4r/g3d/g3d_obj.h>
#include <nw4r/g3d/g3d_scngroup.h>
#include <types.h>

// The screen where the player picks the items that can appear in the match: the frequency of the items and one switch for
// each of the 49 items. The cursor is on a "row": the first row has the frequency (four values), the next ones the
// switch that turns all the items on and off and the items, 7 in each row.
// HYPOTHESIS: the names follow how the menu uses the fields; the switches are in the save data of the game
// (GameGlobal::getGlobalRecordMenuDatap()).
class muProcItemSwitch : public muProcMenu {
public:
    enum {
        ItemCount = 49,
    };

    MuMsg* m_msg;                       // 0x66C the texts of the help (the name of the item, what it is, how it is found)
    u32 m_col;                          // 0x670 the column of the cursor
    u32 m_row;                          // 0x674 the row of the cursor
    s8 m_frequency;                     // 0x678 the frequency of the items (0 ... 3)
    u8 m_unk679[7];
    u64 m_mask;                         // 0x680 the items that are on (bit n: item n)
    u8 m_startIsCancel;                 // 0x688 the Start button works as the Cancel button
    u8 m_unk689[7];

    muProcItemSwitch(MuObject** objs, nw4r::g3d::G3dObj** mdls, void* msgData);
    virtual ~muProcItemSwitch();
    virtual void init(nw4r::g3d::ScnGroup* group, int from);
    virtual int proc();
    virtual void exit();
    virtual bool selectProc(int dir);
    virtual void playSE_CursorMove();

    // The number of the entry that the cursor is on: 0 is the frequency, 1 the switch of all the items, 2 ... the items
    u32 getIndex() const {
        u32 index;
        if (m_row == 0) {
            index = 0;
        } else if (m_row == 1) {
            index = m_col;
            if (index == 0) {
                index = 1;
            }
        } else {
            index = (m_row - 1) * 7 + m_col;
        }
        return index;
    }
    void printHelp();
    void setCursor(bool changed);
};
static_assert(sizeof(muProcItemSwitch) == 0x690, "Class is wrong size!");
