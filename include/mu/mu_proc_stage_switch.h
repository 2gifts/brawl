#pragma once

#include <StaticAssert.h>
#include <mu/mu_msg.h>
#include <mu/mu_proc_menu.h>
#include <nw4r/g3d/g3d_obj.h>
#include <nw4r/g3d/g3d_scngroup.h>
#include <types.h>

// The screen where the player picks the stages that can be played: a grid of the stages of the first kind (5 columns), a
// list of the others (2 columns), and the random stage under it. Each of the two groups has a header (row 0) that turns the
// whole group on and off. The cursor is on a "row" and a "column": the columns 0 ... 4 are the grid, 5 and 6 the list.
// HYPOTHESIS: the names follow how the menu uses the fields; the switches are in the save data of the game
// (GameGlobal::getGlobalRecordMenuDatap()) as one bit for each stage (bit 50: the random stage).
class muProcStageSwitch : public muProcMenu {
public:
    enum {
        GridStageCount = 31,    // the most stages the grid can show
        ListStageCount = 10,    // the most stages the list can show
        RandomBit = 50,
    };

    MuMsg* m_msg;                       // 0x66C the text of the stage the cursor is on
    u32 m_col;                          // 0x670 the column of the cursor
    u32 m_row;                          // 0x674 the row of the cursor
    u32 m_gridRows;                     // 0x678 how many rows the grid has
    u32 m_listRows;                     // 0x67C how many rows the list has
    u32 m_gridLastRow;                  // 0x680 how many stages are in the last row of the grid (0: it is full)
    u32 m_listLastRow;                  // 0x684 how many stages are in the last row of the list (0 or 1)
    u32 m_gridStages[GridStageCount];   // 0x688 the number of the stages that the grid shows
    u32 m_gridCount;                    // 0x704
    u32 m_listStages[ListStageCount];   // 0x708 the number of the stages that the list shows
    u32 m_listCount;                    // 0x730
    u32 m_unk734;
    u64 m_mask;                         // 0x738 the stages that are on (bit n: stage n)
    u64 m_gridMask;                     // 0x740 the bits of the stages of the grid
    u64 m_notGridMask;                  // 0x748 the bits of the list and of the random stage
    u64 m_listMask;                     // 0x750 the bits of the stages of the list
    u64 m_notListMask;                  // 0x758 the bits of the grid and of the random stage
    u8 m_showRandom;                    // 0x760 the random stage is shown (it is not when there is a Wi-Fi connection)
    u8 m_startIsCancel;                 // 0x761 the Start button works as the Cancel button
    u8 m_changed;                       // 0x762
    u8 m_unk763[5];

    muProcStageSwitch(MuObject** objs, nw4r::g3d::G3dObj** mdls, void* msgData);
    virtual ~muProcStageSwitch();
    virtual void init(nw4r::g3d::ScnGroup* group, int from);
    virtual int proc();
    virtual void exit();
    virtual bool selectProc(int dir);
    virtual void playSE_CursorMove();

    // The number of the entry that the cursor is on: 0 is the header of the grid, 1 the header of the list, 2 ... 32 the
    // stages of the grid, 33 ... 42 the stages of the list, 43 the random stage.
    u32 getIndex() const {
        if (m_col < 5) {
            if (m_row == 0) {
                return 0;
            }
            return (m_row - 1) * 5 + m_col + 2;
        }
        if (m_row == 0) {
            return 1;
        }
        u32 index = 0x2B;
        if (m_row <= m_listRows) {
            index = (m_row - 1) * 2 + m_col + 0x1C;
        }
        return index;
    }
    void setCursor();
};
static_assert(sizeof(muProcStageSwitch) == 0x768, "Class is wrong size!");
