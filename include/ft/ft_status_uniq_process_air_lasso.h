#pragma once

#include <so/status/so_status_module_impl.h>
#include <types.h>

// Air lasso family (Lasso status, Hang / Rewind / Reach / Failure). Class shape is HYPOTHESIS: the
// base process owns initStatus/exitStatus, the common process adds execFixPos (defined in its own unit).
class ftStatusUniqProcessAirLasso : public soStatusUniqProcess {
public:
    virtual ~ftStatusUniqProcessAirLasso() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int);
};

extern ftStatusUniqProcessAirLasso g_ftStatusUniqProcessAirLasso;

class ftStatusUniqProcessAirLassoCommon : public ftStatusUniqProcessAirLasso {
public:
    virtual ~ftStatusUniqProcessAirLassoCommon() { }
    virtual void execFixPos(soModuleAccesser* moduleAccesser);
};
