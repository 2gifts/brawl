#pragma once

#include <StaticAssert.h>
#include <gf/gf_archive.h>
#include <gf/gf_task.h>
#include <memory.h>
#include <mu/mu_object.h>
#include <nw4r/g3d/g3d_resfile.h>
#include <types.h>

// The menu task of the challenger that approaches: a new challenger appears during a match and the screen shows the
// panel "MenChallengerNNNN" (one for each of the 14 challengers) sliding in. The task plays the panel animation (the
// base panel and the one of the challenger), waits for a button press, and plays the animation that takes it away.
class muChallengerApproachTask : public gfTask {
public:
    enum Step {
        Step_Idle = 0,
        Step_Start = 1,
        Step_Wait = 2,
        Step_AppearPlay = 3,   // the panel slides in (and a sound plays)
        Step_AppearWait = 4,
        Step_AppearEnd = 5,
        Step_WaitButton = 6,   // until the player presses A, B, Start or the corresponding button of the Wii remote
        Step_Pressed = 7,
        Step_ExitStart = 8,
        Step_ExitPlay = 9,     // the panel slides out
        Step_ExitEnd = 10,
        Step_Done = 11,
    };

    nw4r::g3d::ResFile m_resFileBase;           // 0x40 the base panel (file 0 of the menu archive)
    nw4r::g3d::ResFile m_resFileChallenger;     // 0x44 the panels of the challengers (file 1)
    gfTask* m_cameraTask;                       // 0x48 the camera task that draws the panels
    MuObject* m_objBase;                        // 0x4C the base panel
    MuObject* m_objChallenger;                  // 0x50 the panel of the challenger
    int m_challengerNo;                         // 0x54 which challenger approaches
    s32 m_seHandle;                             // 0x58 the sound of the approach
    char m_nodeNames[14][0x40];                 // 0x5C the names of the panels ("MenChallenger0001_TopN" ...)
    int m_step;                                 // 0x3DC
    int m_frame;                                // 0x3E0

    muChallengerApproachTask();
    static muChallengerApproachTask* create();

    virtual void processDefault();
    virtual ~muChallengerApproachTask();

    void initialize(int challengerNo);
    void release();
    void createData(gfArchive* archive);
};
static_assert(sizeof(muChallengerApproachTask) == 0x3E4, "Class is wrong size!");
