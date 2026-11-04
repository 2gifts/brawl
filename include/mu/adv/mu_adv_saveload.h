// Local BrawlHeaders shadow: muAdvSaveLoadTask and SaveBox declarations.
#pragma once

#include <StaticAssert.h>
#include <ad/ad_list.h>
#include <gf/gf_task.h>
#include <sr/sr_common.h>
#include <types.h>

struct muAdvSaveTaskParam {
    u8 unk0;
    u32 unk4;
};
static_assert(sizeof(muAdvSaveTaskParam) == 0x8, "Size is the wrong size!");

struct muAdvSaveTask : public gfTask {
    u32 unk40;
    u8 unk44[0x174];
    s32 unk1B8;
    u8 unk1BC[0x3E8];
    bool unk5A4;

    // TODO: type of p3
    static muAdvSaveTask* create(const muAdvSaveTaskParam* param, Heaps::HeapType p2, u32 p3);
};
// TODO size

struct muAdvLoadTaskParam {
    u8 unk0;
    u32 unk4;
};
static_assert(sizeof(muAdvLoadTaskParam) == 0x8, "Size is the wrong size!");

struct muAdvLoadTask : public gfTask {
    u32 unk40;
    u8 unk44[0x174];
    s32 unk1B8;
    u8 unk1BC[0x3E8];
    bool unk5A4;

    s32 isLoaded() const;
    static muAdvLoadTask* create(const muAdvLoadTaskParam* param);
};
// TODO size


#include <if/if_utility.h>
#include <mu/mu_vc_trial.h>
#include <mu/mu_recorder.h>
#include <std/auto_ptr.h>

class muAdvSaveLoadTask : public utility::StandardMenu {
public:
    class SaveBox {
    public:
        std::auto_ptr<MuObject> unk0;
        std::auto_ptr<MuObject> unk4;
        std::auto_ptr<MuObject> unk8;
        std::auto_ptr<MuObject> unkC;
        std::auto_ptr<MuObject> unk10[42];
        // HYPOTHESIS: these holders own objects through the G3dObj interface.
        muG3dObjHolder<nw4r::g3d::G3dObj> unkB8;
        muG3dObjHolder<nw4r::g3d::G3dObj> unkBC;
        SaveBox();
        ~SaveBox();
    };

    Heaps::HeapType unk1AC;
    u32 unk1B0;
    muRecorder* unk1B4;
    s32 unk1B8;
    s32 unk1BC;
    u8 unk1C0[0x1348 - 0x1C0];
    muRecorder::Data unk1348[51];
    s32 unk1414;
    s32 unk1418;
    s32 unk141C;
    u8 unk1420[8];
    s32 unk1428;
    u8 unk142C[0x16];
    bool unk1442;
    bool unk1443;
    u8 unk1444[0x1518 - 0x1444];
    utility::AutoFile unk1518;
    utility::AutoFile unk1530;
    utility::AutoFile unk1548;
    u8 unk1560[0x20];
    SaveBox unk1580[4];
    u8 unk1880[8];
    std::auto_ptr<MuMsg> unk1888;
    u32 unk188C;
    u32 unk1890;

    virtual ~muAdvSaveLoadTask();
    virtual void update();
    virtual void onCheckFinished() = 0;
    virtual void initSlotList(u32 slot) = 0;
    void controllProc();
    void updateScroll();
    void updatePanel();
    void moveScrollCore(int amount);
    void initMsg();
    // HYPOTHESIS: the flag controls whether to request the next slot.
    bool loading(bool advance);
};
static_assert(sizeof(muAdvSaveLoadTask::SaveBox) == 0xC0, "SaveBox size");
