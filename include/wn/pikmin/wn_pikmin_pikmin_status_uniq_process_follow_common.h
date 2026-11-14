#pragma once

#include <so/status/so_status_module_impl.h>
#include <types.h>

// Shared base of the Pikmin follow statuses (ground, air, water): counts down the reposition timer while the Pikmin
// drifts, and raises the "far from the follow point" flag once it strays outside the allowed radius.
class wnPikminPikminStatusUniqProcessFollowCommon : public soStatusUniqProcess {
public:
    virtual ~wnPikminPikminStatusUniqProcessFollowCommon() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execFixPosCounter(soModuleAccesser* moduleAccesser);
};
