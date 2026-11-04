#pragma once

#include <StaticAssert.h>
#include <gf/gf_sys_recorder2.h>
#include <gf/gf_task.h>
#include <nw4r/g3d/g3d_resfile.h>
#include <nw4r/g3d/g3d_scngroup.h>
#include <types.h>

// The window that loads the Wi-Fi data of the save before a connection (and offers to remake it when it is broken).
// Its code (main.dol 0x800DAA30 ...) is not decompiled yet; the fields are the ones that create and the constructor
// write. Function names come from the sora map.
// HYPOTHESIS: the names of the fields follow create and muWifiCnctWndTask, which reads m_result and m_step.
class muWifiDataLoadTask : public gfTask {
public:
    nw4r::g3d::ResFile* m_resFile;      // 0x40 the models of the window
    nw4r::g3d::ScnGroup* m_scnGroup;    // 0x44
    int m_result;                       // 0x48 1: the data could not be used (the save is off)
    void* m_msgData;                    // 0x4C the texts of the window
    int m_step;                         // 0x50 5 once the window is done
    int m_prevStep;                     // 0x54
    int m_priority;                     // 0x58
    int m_unk5C;
    gfSysRecorder2* m_recorder;         // 0x60
    int m_unk64;
    int m_unk68;
    bool m_unk6C;

    muWifiDataLoadTask();
    static int mainStepLoadMain(muWifiDataLoadTask* task);

    virtual void processDefault();
    virtual ~muWifiDataLoadTask();
    virtual void exit();

    static muWifiDataLoadTask* create(nw4r::g3d::ResFile* resFile, nw4r::g3d::ScnGroup* group, void* msgData, int priority);
};
static_assert(sizeof(muWifiDataLoadTask) == 0x70, "Class is wrong size!");
