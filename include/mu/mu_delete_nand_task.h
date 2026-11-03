#pragma once

#include <StaticAssert.h>
#include <gf/gf_task.h>
#include <memory.h>
#include <types.h>

// The task that deletes save data for the Delete Data option (muProcOptDel): it resets the data in memory, bans the
// HOME menu, has gfSysRecorder2 rewrite the save data on the Wii system memory, waits for it and lifts the ban again.
// There is only one: its state is kept in statics of the unit (mu_delete_nand_task.cpp), the task adds no members.
// Function names come from the sora_menu_main map; the gfTask name ("mlDebVsMCond") is the one the original passes.
class muDeleteNandTask : public gfTask {
public:
    // HYPOTHESIS: the names follow what each step does in processDefault.
    enum Step {
        Step_Idle = 0,              // nothing to do
        Step_Start = 1,             // init: clears the frame counter
        Step_Reset = 2,             // resets the data in memory
        Step_BanHome = 3,           // waits until the HOME menu is banned
        Step_Request = 4,           // starts the write of the save data
        Step_Wait = 5,              // waits until gfSysRecorder2 is done
        Step_Result = 6,            // turns the result of gfSysRecorder2 into an error number
        Step_Finish = 7,            // drops gfSysRecorder2 and lifts the ban
        Step_Done = 8,              // the data is deleted (muProcOptDel waits for it)
    };

    // The entries of muProcOptDel.
    // HYPOTHESIS: kinds 2 and 3 are the data that gfSysRecorder2 renews as kinds 1 and 3 (only 2 is known: adventure).
    enum Kind {
        Kind_HiScore = 0,           // GameGlobal::InitHiScore
        Kind_BattleRecord = 1,      // GameGlobal::InitBattleRecord
        Kind_Adventure = 2,
        Kind_Unk3 = 3,
        Kind_All = 4,               // GameGlobal::InitAllRecord, then all of the save data is renewed
    };

    // Makes the task (once) and starts deleting the data of the given kind (the entry of muProcOptDel).
    static muDeleteNandTask* create(HeapType heap, int kind);
    virtual void processDefault();
    virtual void init();
    virtual ~muDeleteNandTask();
    // Writes the reset record of the game (system data) to the save data; false if that failed (see the error).
    // Not static: processDefault keeps the task in r3 for the call, though it never uses it.
    bool setupSysSave();
    // HYPOTHESIS: not static: muProcOptDel loads the task before calling it (the step itself is a static of the unit)
    int getStep();

private:
    muDeleteNandTask() : gfTask("mlDebVsMCond", Category_None, 0xE, 6, true) { }
};
static_assert(sizeof(muDeleteNandTask) == 0x40, "Class is wrong size!");
