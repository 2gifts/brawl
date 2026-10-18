#pragma once

#include <so/status/so_status_module_impl.h>
#include <types.h>

class soModuleAccesser;

// Aerial attack status process. Derived processes (the jump-inherited aerial motion) build on it.
class ftStatusUniqProcessAttackAir : public soStatusUniqProcess {
public:
    virtual ~ftStatusUniqProcessAttackAir() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int nextStatusKind);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
    virtual void execFixPos(soModuleAccesser* moduleAccesser);
};
