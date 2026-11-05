#pragma once

#include <mu/mu_menu_controller.h>
#include <nw4r/g3d/g3d_resfile.h>
#include <types.h>

class IfSnapSaveTask;
class MuObject;
namespace nw4r { namespace g3d { class ScnGroup; } }

class IfAdvPause {
public:
    u8 unk0;
    s32 unk4;
    s32 unk8[3];
    s32 unk14;
    s32 unk18;
    u8 unk1C;
    muMenuController unk20;
    u8 unkC8;
    u8 unkC9[0x1F];
    MuObject* unkE8[7];
    nw4r::g3d::ResFile unk104;
    void* unk108;
    nw4r::g3d::ResFile unk10C;
    nw4r::g3d::ScnGroup* unk110;
    s32 unk114;
    s32 unk118;
    u32 unk11C;
    s32 unk120;
    u32 unk124;
    u8 unk128;
    IfSnapSaveTask* unk12C;
    s32 unk130;
    u8 unk134;
    s32 unk138;
    void* unk13C;
    s32 unk140;
    u8 unk144;
    u8 unk145;
    u8 unk146;
    u8 unk147;

    // HYPOTHESIS: parameter types follow the manager's create call and register use.
    IfAdvPause(int arg1, int arg2);
    static IfAdvPause* create(int arg1, int arg2);
    void exitPause();
    void destroyModel();
    void drawCameraGuide(int visible);
    void createObjResFile(const void* nodeInfo, s32 nodeCount, nw4r::g3d::ResFile* resFile);
    void photoStepNoneInit();
    void photoStepCameraMoveInit();
    void photoStepSnapSaveInit();
    s32 photoStepSnapSaveMain();
};
