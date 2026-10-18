#pragma once

#include <so/status/so_status_module_impl.h>
#include <types.h>

class soModuleAccesser;

// Smash attack charge handling shared by the fighters (also used by the item swing statuses). The hold statuses
// (S4/Lw4/Hi4 hold and the item swing hold) apply a reaction multiplier and start the charge effect; the release
// statuses (S4/Lw4/Hi4 and item swing) scale the attack power by the charge stored in work float 0x21000000.
class ftStatusUniqProcessSmashAttack : public soStatusUniqProcess {
public:
    virtual ~ftStatusUniqProcessSmashAttack() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int nextStatusKind);
};

extern ftStatusUniqProcessSmashAttack g_ftStatusUniqProcessSmashAttack;
