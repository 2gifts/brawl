#pragma once

#include <StaticAssert.h>
#include <gf/gf_archive.h>
#include <gf/gf_file_io_handle.h>
#include <gf/gf_task.h>
#include <memory.h>
#include <mu/mu_object.h>
#include <nw4r/g3d/g3d_obj.h>
#include <nw4r/g3d/g3d_resfile.h>
#include <ty/ty_fig_listmng.h>
#include <types.h>

// The list of the trophies that the player got, which the scene gives to the task. HYPOTHESIS: the names and the order of
// the virtual functions follow how the task walks over the list (rewind, count, take the id, go to the next).
class muFigureGetProvider {
public:
    virtual int getCount();             // 0x08
    virtual void rewind();              // 0x0C
    virtual void vf10();
    virtual void vf14();
    virtual u32 getFigId();             // 0x18
    virtual void vf1C();
    virtual void next();                // 0x20
};

// What the scene gives to the task.
struct muFigureGetTaskParam {
    int m_unk00;                        // 0x00
    int m_figId;                        // 0x04 the trophy that is shown
    muFigureGetProvider* m_provider;    // 0x08 the trophies that were got in the match
};

// The demo of a trophy that the player got: the trophy comes down on its stand and a button leaves the demo.
class muFigureGetTask : public gfTask {
public:
    enum Step {
        Step_MngDataLoad = 0,
        Step_Request = 1,
        Step_GetResData = 2,
        Step_DataSet = 3,
        Step_InputWait = 4,
        Step_Count = 5,
    };

    // One step of the demo: what runs when the step starts, and what runs every frame (it returns the next step).
    struct StepFunc {
        void (*m_init)(muFigureGetTask* task);
        int (*m_main)(muFigureGetTask* task);
    };

    static const int FigMax = 100;
    static const int ObjNum = 204;
    static const int HandleNum = 600;

    int m_done;                                 // 0x40 set when the demo is over
    int m_unk44;                                // 0x44
    int m_figId;                                // 0x48 the trophy that is shown
    u32 m_figIds[FigMax];                       // 0x4C the trophies of the list
    int m_figNum;                               // 0x1DC
    muFigureGetProvider* m_provider;            // 0x1E0
    int m_unk1E4;                               // 0x1E4
    MuObject* m_standObjs[2];                   // 0x1E8
    tyFigListDataManager* m_figMng;             // 0x1F0
    nw4r::g3d::G3dObj* m_scnMdls[3];            // 0x1F4
    nw4r::g3d::G3dObj* m_scnMdlStand;           // 0x200
    nw4r::g3d::G3dObj* m_scnMdlFigs[FigMax];    // 0x204
    nw4r::g3d::ResFile m_resFileFigs[FigMax];   // 0x394 the card of each trophy
    nw4r::g3d::ResFile m_resFileFigGet;         // 0x524 "fig_get.brres"
    nw4r::g3d::ResFile m_resFileFig;            // 0x528 the file of the trophy
    nw4r::g3d::ResFile m_resFileStand;          // 0x52C "Stand.brres"
    nw4r::g3d::ResFile m_resFileCamera;         // 0x530
    u32 m_unk534[2];
    nw4r::g3d::ResFile m_resFileUnk[FigMax];    // 0x53C
    MuObject* m_objects[ObjNum];                // 0x6CC
    gfFileIOHandle m_handles[HandleNum];        // 0x9FC
    gfArchive* m_archive;                       // 0x135C
    int m_unk1360;
    int m_unk1364;
    int m_step;                                 // 0x1368
    int m_stepPrev;                             // 0x136C
    bool m_isLeaving;                           // 0x1370 a button was pressed

    muFigureGetTask();
    static muFigureGetTask* create(const muFigureGetTaskParam* param);

    virtual void processDefault();
    virtual ~muFigureGetTask();

    void createObjResFile(const struct muFigureGetModel* models, int count, nw4r::g3d::ResFile* resFile);

    static void mainStepMngDataLoadInit(muFigureGetTask* task);
    static int mainStepMngDataLoadMain(muFigureGetTask* task);
    static void mainStepRequestInit(muFigureGetTask* task);
    static int mainStepRequestMain(muFigureGetTask* task);
    static void mainStepGetResDataInit(muFigureGetTask* task);
    static int mainStepGetResDataMain(muFigureGetTask* task);
    static void mainStepDataSetInit(muFigureGetTask* task);
    static int mainStepDataSetMain(muFigureGetTask* task);
    static void mainStepInputWaitInit(muFigureGetTask* task);
    static int mainStepInputWaitMain(muFigureGetTask* task);
};
static_assert(sizeof(muFigureGetTask) == 0x1374, "Class is wrong size!");
