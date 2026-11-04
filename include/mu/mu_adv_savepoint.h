#pragma once

#include <gf/gf_task.h>
#include <mu/mu_menu_controller.h>
#include <mu/mu_vc_trial.h>

class muAdvSavePoint : public gfTask {
public:
    virtual ~muAdvSavePoint();
    void moveCursor();
    int searchCursor(int cursor, int direction);
    int checkState();

    u32 unk40;
    u32 unk44;
    u32 unk48;
    u32 unk4C;
    int unk50;
    // HYPOTHESIS: each entry has an enabled byte and two neighbors per direction.
    struct {
        bool unk0;
        int unk4[4][2];
    } unk54[7];
    muMenuController unk150;
    u32 unk1F8;
    muTaskHolder<gfTask> unk1FC;
    muTaskHolder<gfTask> unk200;
    u32 unk204[7];
    u32 unk220;
    muAutoPtr<MuObject> unk224;
    muG3dObjHolder<nw4r::g3d::G3dObj> unk228;
};
static_assert(sizeof(muAdvSavePoint) == 0x22C, "Class is the wrong size!");
