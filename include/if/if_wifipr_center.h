#pragma once

#include <StaticAssert.h>
#include <types.h>

namespace nw4r { namespace g3d { class ResFile; class ScnGroup; } }

class IfWifiprCenter {
public:
    u8 unk0[0x10];
    IfWifiprCenter();
    virtual ~IfWifiprCenter();
    // HYPOTHESIS: the second argument is the message data stored at offset 8 and later passed to MuMsg::printIndex.
    void createModel(nw4r::g3d::ResFile* res, void* msgData);
    void startMelee(nw4r::g3d::ScnGroup* group);
};
static_assert(sizeof(IfWifiprCenter) == 0x14, "Class is the wrong size!");
