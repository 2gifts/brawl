// Local BrawlHeaders shadow: utility function and class declarations.
#pragma once

#include <gf/gf_file_io_handle.h>
#include <gf/gf_task.h>
#include <mu/mu_object.h>
#include <nw4r/g3d/g3d_resfile.h>
#include <nw4r/g3d/g3d_scnobjcallback.h>
#include <ut/ut_list.h>

namespace utility {

// HYPOTHESIS: Loader owns an intrusive list node and a loading callback.
class Loader : public utListNode {
public:
    utList* unk8;
    virtual bool loading();
    static bool isLoadFinish();
};

// HYPOTHESIS: resource storage precedes the loader node and file handle.
class AutoResFile : public nw4r::g3d::ResFile, public Loader {
public:
    unsigned char unk14[0x10];
    gfFileIOHandle unk24;
    u32 unk28;
    bool unk2C;
    virtual bool loading();
};

class StandardTask : public gfTask {
public:
    virtual void processDefault();
    virtual ~StandardTask();

    virtual bool preLoad();
    virtual void loadResource() = 0;
    virtual bool checkLoadFinish() const;
    virtual void setupScene();
    virtual void createModel() = 0;
    virtual void update() = 0;
};

class StandardMenu : public StandardTask {
public:
    virtual void processDefault();
    virtual ~StandardMenu();
};

class DepthScaler : public nw4r::g3d::IScnObjCallback {
public:
    float unk4;
    float unk8;
    DepthScaler(MuObject& object, float first, float second);
    virtual ~DepthScaler();
    virtual void ExecCallback_CALC_WORLD(nw4r::g3d::ScnObj::Timing, nw4r::g3d::ScnObj*, u32, void*);
    virtual void ExecCallback_CALC_VIEW(nw4r::g3d::ScnObj::Timing, nw4r::g3d::ScnObj*, u32, void*);
};

}

