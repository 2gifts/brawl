#pragma once

#include <gf/gf_task.h>
#include <types.h>

class gfSysRecorder2;
class muNoticeWndTask;
namespace nw4r { namespace g3d { class ResFile; } }

class muWifiDataLoadTask : public gfTask {
public:
    nw4r::g3d::ResFile* unk40;
    void* unk44;
    int unk48;
    void* unk4C;
    int unk50;
    int unk54;
    int unk58;
    muNoticeWndTask* unk5C;
    gfSysRecorder2* unk60;
    int unk64;
    int unk68;
    bool unk6C;

    // HYPOTHESIS: parameter types follow the notice-window creation arguments.
    static muWifiDataLoadTask* create(nw4r::g3d::ResFile*, void*, void*, int);
    muWifiDataLoadTask();
    void exit();
    virtual ~muWifiDataLoadTask();
    virtual void processDefault();
    static int mainStepLoadMain(muWifiDataLoadTask*);
    static void mainStepRemakeConfirmInit(muWifiDataLoadTask*);
    static int mainStepRemakeConfirmMain(muWifiDataLoadTask*);
};
