#pragma once

#include <so/status/so_status_module_impl.h>
#include <so/so_module_accesser.h>
#include <types.h>

// Ganondorf's down special (Wizard's Foot). The class names are the game's own RTTI strings.
class ftGanonStatusUniqProcessGanonKick : public soStatusUniqProcess {
public:
    virtual ~ftGanonStatusUniqProcessGanonKick() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
    // Called from ftGanon::notifyEventCollisionAttackFighter while the kick is the current status.
    void onHit(soModuleAccesser* moduleAccesser);
};

class ftGanonStatusUniqProcessGanonKickEnd : public soStatusUniqProcess {
public:
    virtual ~ftGanonStatusUniqProcessGanonKickEnd() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int nextStatus);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
};

extern ftGanonStatusUniqProcessGanonKick g_ftGanonStatusUniqProcessGanonKick;
extern ftGanonStatusUniqProcessGanonKickEnd g_ftGanonStatusUniqProcessGanonKickEnd;
