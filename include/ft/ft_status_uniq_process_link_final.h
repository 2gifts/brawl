#pragma once

#include <so/link/so_link_event_presenter.h>
#include <so/status/so_status_module_impl.h>
#include <types.h>

class StageObject;

// HYPOTHESIS: message the Link final smash sends to the fighters it captures (kind in m_message, one value, and for message 6
// an output position). Layout from linkMessageProcedure; the first 8 bytes are the generic link event header.
struct ftLinkFinalMessage : public soLinkEventArgs {
    int m_message;  // +8
    int m_value;    // +0xC
    Vec3f m_pos;    // +0x10 (output of message 6)
    ftLinkFinalMessage(int eventKind) : soLinkEventArgs(eventKind) { }
};

// Link final status (the fighter is held by Link's final smash). Names from the module map.
class ftStatusUniqProcessLinkFinal : public soStatusUniqProcess {
public:
    virtual ~ftStatusUniqProcessLinkFinal() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execFixPos(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int);
    virtual bool checkDamage(soModuleAccesser* moduleAccesser, void*);

    static void linkMessageProcedure(ftLinkFinalMessage* message, soModuleAccesser* moduleAccesser);
};

extern ftStatusUniqProcessLinkFinal g_ftStatusUniqProcessLinkFinal;
