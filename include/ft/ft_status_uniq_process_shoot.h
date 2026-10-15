#pragma once

#include <so/status/so_status_module_impl.h>
#include <types.h>

class soModuleAccesser;

// Shared base of the shooting statuses (item shoot, grenade throw). Only the static entry points the sora_melee
// callers use are declared; the class' own virtual overrides are not reconstructed yet.
class ftStatusUniqProcessShoot : public soStatusUniqProcess {
public:
    virtual ~ftStatusUniqProcessShoot() { }
    virtual void execStatus(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int nextStatusKind);

    // HYPOTHESIS: the two ints are the motion kinds for the ground and the air variant of the shoot status.
    static void initShoot_Common(soModuleAccesser* moduleAccesser, int groundMotionKind, int airMotionKind);
    static void initShoot_CommonAirUpper(soModuleAccesser* moduleAccesser, int groundMotionKind, int airMotionKind);
    static void execShoot_Common(soModuleAccesser* moduleAccesser);
    // HYPOTHESIS: the int is the status kind the fighter is switching to.
    static void exitShoot_Common(soModuleAccesser* moduleAccesser, int nextStatusKind);
    static void exitShoot_CommonAirUpper(soModuleAccesser* moduleAccesser, int nextStatusKind);
};
