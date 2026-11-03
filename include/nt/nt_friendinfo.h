#pragma once

#include <gf/gf_task.h>
#include <nt/nt_offline.h>

class gfSysRecorder2;

class ntFriendInfoSave : public gfTask {
public:
    gfSysRecorder2* unk40;
    bool unk44;

    ntFriendInfoSave();
    virtual ~ntFriendInfoSave();
    virtual void processDefault();
};
