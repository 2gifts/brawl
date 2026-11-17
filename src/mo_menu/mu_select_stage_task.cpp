#include <gf/gf_task.h>
#include <types.h>

#include <mu/mu_select_stage_task.h>

// The stage selection screen (module sora_menu_sel_stage). Only a part of its functions is decompiled.
/// Function names come from the sora_menu_sel_stage map.

// The task of the stage selection that lists and loads the custom stage files (it is created by the screen).
class muSelectStageFileTask : public gfTask {
public:
    void exit();
};

static muSelectStageFileTask* s_fileTask;

void muSelectStageTask::exit() {
    if (s_fileTask != NULL) {
        s_fileTask->exit();
        s_fileTask = NULL;
    }
    gfTask::exit();
}

void muSelectStageTask::processDefault() {
    if ((u32)m_mode <= 2) {
        processDefaultWithScreen();
    } else {
        processDefaultWithoutScreen();
    }
}
