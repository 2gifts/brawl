#pragma once

#include <StaticAssert.h>
#include <types.h>

class MuObject;
class MuMsg;
namespace nw4r { namespace g3d { class ResFile; class ScnGroup; } }

class IfWifiprCenter {
public:
    MuObject* m_object; // 0x00
    MuMsg* m_msg;       // 0x04
    void* m_msgData;    // 0x08 HYPOTHESIS: the message file printIndex is given
    int m_msgId;        // 0x0C HYPOTHESIS: the line dispMsg last showed

    IfWifiprCenter();
    virtual ~IfWifiprCenter();

    void createModel(nw4r::g3d::ResFile* res, void* msgData);
    void startMelee(nw4r::g3d::ScnGroup* group);
    void dispMsg(int msgId);
    void startBeginMeleeAnim();
};
static_assert(sizeof(IfWifiprCenter) == 0x14, "Class is the wrong size!");
