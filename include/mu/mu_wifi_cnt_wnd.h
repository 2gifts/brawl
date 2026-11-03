#pragma once

#include <gf/gf_task.h>
#include <sr/sr_common.h>

class MuObject;
namespace nw4r { namespace g3d {
class ResFile;
class ScnMdlExpand;
} }

class muWifiCntWndTask : public gfTask {
public:
    MuObject* unk40[5];
    nw4r::g3d::ScnMdlExpand* unk54;
    nw4r::g3d::ScnMdlExpand* unk58;
    u32 unk5C;
    int unk60;
    int unk64;

    muWifiCntWndTask();
    virtual ~muWifiCntWndTask();
    void createObjResFile(const void* nodeInfo, int count, nw4r::g3d::ResFile* resFile, int priority, HeapType heap);
    void setWndStyle(int style, int digits);
    int dispNum(int value, int digits, int trim, int position, int object);
    void setVisible(bool visible);
};
