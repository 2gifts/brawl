#pragma once

#include <so/status/so_status_module_impl.h>
#include <types.h>

class soModuleAccesser;

// The lasso article's reach status. The Pikmin Chain (Olimar's up special) derives from it and extends initStatus.
class wnLassoStatusUniqProcessReach : public soStatusUniqProcess {
public:
    virtual ~wnLassoStatusUniqProcessReach() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
    virtual void execFixPos(soModuleAccesser* moduleAccesser);
};
