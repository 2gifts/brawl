#pragma once

#include <StaticAssert.h>
#include <types.h>

namespace nw4r { namespace g3d { class ResFile; class ScnGroup; } }

class IfWifiIntrMenu {
public:
    char unk0[0x1C];
    u8 unk1C;
    char unk1D[0xAF];
    IfWifiIntrMenu();
    virtual ~IfWifiIntrMenu();
    // HYPOTHESIS: the second argument is the message data passed to MuMsg::setMsgData.
    void createModel(nw4r::g3d::ResFile* res, void* msgData);
    void startMelee(nw4r::g3d::ScnGroup* group);
};
static_assert(sizeof(IfWifiIntrMenu) == 0xD0, "Class is the wrong size!");
